/*
 * authreadkeys.c - routines to support the reading of the key file
 */
#include <config.h>
#include <stdio.h>
#include <ctype.h>

#include "ntpd.h"	/* Only for DPRINTF */
#include "ntp_fp.h"
#include "ntp.h"
#include "ntp_syslog.h"
#include "ntp_stdlib.h"
#include "ntp_keyacc.h"

#ifdef OPENSSL
#include "openssl/objects.h"
#include "openssl/evp.h"
#endif	/* OPENSSL */

/* Forwards */
static char *nexttok (char **);

/*
 * nexttok - basic internal tokenizing routine
 */
static char *
nexttok(
	char	**str
	)
{
	register char *cp;
	char *starttok;

	cp = *str;

	/*
	 * Space past white space
	 */
	while (*cp == ' ' || *cp == '\t')
		cp++;
	
	/*
	 * Save this and space to end of token
	 */
	starttok = cp;
	while (*cp != '\0' && *cp != '\n' && *cp != ' '
	       && *cp != '\t' && *cp != '#')
		cp++;
	
	/*
	 * If token length is zero return an error, else set end of
	 * token to zero and return start.
	 */
	if (starttok == cp)
		return NULL;
	
	if (*cp == ' ' || *cp == '\t')
		*cp++ = '\0';
	else
		*cp = '\0';
	
	*str = cp;
	return starttok;
}


/* TALOS-CAN-0055: possibly DoS attack by setting the key file to the
 * log file. This is hard to prevent (it would need to check two files
 * to be the same on the inode level, which will not work so easily with
 * Windows or VMS) but we can avoid the self-amplification loop: We only
 * log the first 5 errors, silently ignore the next 10 errors, and give
 * up when when we have found more than 15 errors.
 *
 * This avoids the endless file iteration we will end up with otherwise,
 * and also avoids overflowing the log file.
 *
 * Nevertheless, once this happens, the keys are gone since this would
 * require a save/swap strategy that is not easy to apply due to the
 * data on global/static level.
 */

static const u_int nerr_loglimit = 5u;
static const u_int nerr_maxlimit = 15;

static void log_maybe(u_int*, const char*, ...) NTP_PRINTF(2, 3);

typedef struct keydata KeyDataT;
struct keydata {
	KeyDataT *next;		/* queue/stack link		*/
	KeyAccT  *keyacclist;	/* key access list		*/
	keyid_t   keyid;	/* stored key ID		*/
	u_short   keytype;	/* stored key type		*/
	u_short   seclen;	/* length of secret		*/
	u_char    secbuf[1];	/* begin of secret (formal only)*/
};

static void
log_maybe(
	u_int      *pnerr,
	const char *fmt  ,
	...)
{
	va_list ap;
	if (++(*pnerr) <= nerr_loglimit) {
		va_start(ap, fmt);
		mvsyslog(LOG_ERR, fmt, ap);
		va_end(ap);
	}
}

/*
 * authreadkeys - (re)read keys from a file.
 */
int
authreadkeys(
	const char *file
	)
{
	FILE	*fp;
	char	*line;
	char	*token;
	keyid_t	keyno;
	int	keytype;
	char	buf[512];		/* lots of room for line */
	u_char	keystr[32];		/* Bug 2537 */
	size_t	len;
	size_t	j;
	u_int   nerr;
	KeyDataT *list = NULL;
	KeyDataT *next = NULL;
	/*
	 * Open file.  Complain and return if it can't be opened.
	 */
	fp = fopen(file, "r");
	if (fp == NULL) {
		msyslog(LOG_ERR, "authreadkeys: file '%s': %m",
		    file);
		goto onerror;
	}
	INIT_SSL();

	/*
	 * Now read lines from the file, looking for key entries. Put
	 * the data into temporary store for later propagation to avoid
	 * two-pass processing.
	 */
	nerr = 0;
	while ((line = fgets(buf, sizeof buf, fp)) != NULL) {
		if (nerr > nerr_maxlimit)
			break;
		token = nexttok(&line);
		if (token == NULL)
			continue;
		
		/*
		 * First is key number.  See if it is okay.
		 */
		keyno = atoi(token);
		if (keyno == 0) {
			log_maybe(&nerr,
				  "authreadkeys: cannot change key %s",
				  token);
			continue;
		}

		if (keyno > NTP_MAXKEY) {
			log_maybe(&nerr,
				  "authreadkeys: key %s > %d reserved for Autokey",
				  token, NTP_MAXKEY);
			continue;
		}

		/*
		 * Next is keytype. See if that is all right.
		 */
		token = nexttok(&line);
		if (token == NULL) {
			log_maybe(&nerr,
				  "authreadkeys: no key type for key %d",
				  keyno);
			continue;
		}
#ifdef OPENSSL
		/*
		 * The key type is the NID used by the message digest 
		 * algorithm. There are a number of inconsistencies in
		 * the OpenSSL database. We attempt to discover them
		 * here and prevent use of inconsistent data later.
		 */
		keytype = keytype_from_text(token, NULL);
		if (keytype == 0) {
			log_maybe(&nerr,
				  "authreadkeys: invalid type for key %d",
				  keyno);
			continue;
		}
		if (EVP_get_digestbynid(keytype) == NULL) {
			log_maybe(&nerr,
				  "authreadkeys: no algorithm for key %d",
				  keyno);
			continue;
		}
#else	/* !OPENSSL follows */

		/*
		 * The key type is unused, but is required to be 'M' or
		 * 'm' for compatibility.
		 */
		if (!(*token == 'M' || *token == 'm')) {
			log_maybe(&nerr,
				  "authreadkeys: invalid type for key %d",
				  keyno);
			continue;
		}
		keytype = KEY_TYPE_MD5;
#endif	/* !OPENSSL */

		/*
		 * Finally, get key and insert it. If it is longer than 20
		 * characters, it is a binary string encoded in hex;
		 * otherwise, it is a text string of printable ASCII
		 * characters.
		 */
		token = nexttok(&line);
		if (token == NULL) {
			log_maybe(&nerr,
				  "authreadkeys: no key for key %d", keyno);
			continue;
		}
		next = NULL;
		len = strlen(token);
		if (len <= 20) {	/* Bug 2537 */
			next = emalloc(sizeof(KeyDataT) + len);
			next->keyacclist = NULL;
			next->keyid   = keyno;
			next->keytype = keytype;
			next->seclen  = len;
			memcpy(next->secbuf, token, len);
		} else {
			static const char hex[] = "0123456789abcdef";
			u_char	temp;
			char	*ptr;
			size_t	jlim;

			jlim = min(len, 2 * sizeof(keystr));
			for (j = 0; j < jlim; j++) {
				ptr = strchr(hex, tolower((unsigned char)token[j]));
				if (ptr == NULL)
					break;	/* abort decoding */
				temp = (u_char)(ptr - hex);
				if (j & 1)
					keystr[j / 2] |= temp;
				else
					keystr[j / 2] = temp << 4;
			}
			if (j < jlim) {
				log_maybe(&nerr,
					  "authreadkeys: invalid hex digit for key %d",
					  keyno);
				continue;
			}
			len = jlim/2; /* hmmmm.... what about odd length?!? */
			next = emalloc(sizeof(KeyDataT) + len);
			next->keyacclist = NULL;
			next->keyid   = keyno;
			next->keytype = keytype;
			next->seclen  = len;
			memcpy(next->secbuf, keystr, len);
		}

		token = nexttok(&line);
DPRINTF(0, ("authreadkeys: full access list <%s>\n", (token) ? token : "NULL"));
		if (token != NULL) {	/* A comma-separated IP access list */
			char *tp = token;

			while (tp) {
				char *i;
				KeyAccT ka;

				i = strchr(tp, (int)',');
				if (i)
					*i = '\0';
DPRINTF(0, ("authreadkeys: access list:  <%s>\n", tp));

				if (is_ip_address(tp, AF_UNSPEC, &ka.addr)) {
					KeyAccT *kap;

					kap = emalloc(sizeof(KeyAccT));
					memcpy(kap, &ka, sizeof ka);
					kap->next = next->keyacclist;
					next->keyacclist = kap;
				} else {
					log_maybe(&nerr,
						  "authreadkeys: invalid IP address <%s> for key %d",
						  tp, keyno);
				}

				if (i) {
					tp = i + 1;
				} else {
					tp = 0;
				}
			}
		}

		INSIST(NULL != next);
		next->next = list;
		list = next;
	}
	fclose(fp);
	if (nerr > nerr_maxlimit) {
		msyslog(LOG_ERR,
			"authreadkeys: rejecting file '%s' after %u errors (emergency break)",
			file, nerr);
		goto onerror;
	}
	if (nerr > 0) {
		msyslog(LOG_ERR,
			"authreadkeys: rejecting file '%s' after %u error(s)",
			file, nerr);
		goto onerror;
	}

	/* first remove old file-based keys */
	auth_delkeys();
	/* insert the new key material */
	while (NULL != (next = list)) {
		list = next->next;
		MD5auth_setkey(next->keyid, next->keytype,
			       next->secbuf, next->seclen, next->keyacclist);
		/* purge secrets from memory before free()ing it */
		memset(next, 0, sizeof(*next) + next->seclen);
		free(next);
	}
	return (1);

  onerror:
	/* Mop up temporary storage before bailing out. */
	while (NULL != (next = list)) {
		list = next->next;

		while (next->keyacclist) {
			KeyAccT *kap = next->keyacclist;

			next->keyacclist = kap->next;
			free(kap);
		}

		/* purge secrets from memory before free()ing it */
		memset(next, 0, sizeof(*next) + next->seclen);
		free(next);
	}
	return (0);
}
