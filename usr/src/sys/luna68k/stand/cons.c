/*
 * Copyright (c) 1992 OMRON Corporation.
 * Copyright (c) 1992 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * OMRON Corporation.
 *
 * %sccs.include.redist.c%
 *
 *	@(#)cons.c	7.1 (Berkeley) %G%
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/ioctl.h>
#include <sys/tty.h>
#include <sys/file.h>
#include <sys/conf.h>
#include <luna68k/luna68k/cons.h>

#define NBMC	1
#define	NSIO	1
#define	NROM	1

/* XXX - all this could be autoconfig()ed */
#include "romvec.h"
#if NBMC > 0
int bmccnprobe(), bmccninit(), bmccngetc(), bmccnputc();
#endif
#if NSIO > 0
int siocnprobe(), siocninit(), siocngetc(), siocnputc();
#endif
#if NROM > 0
int romcnprobe(), romcninit(), romcngetc(), romcnputc();
#endif

struct	consdev constab[] = {
#if NBMC > 0
	{ bmccnprobe,	bmccninit,	bmccngetc,	bmccnputc },
#endif
#if NSIO > 0
	{ siocnprobe,	siocninit,	siocngetc,	siocnputc },
#endif
#if NROM > 0
	{ romcnprobe,	romcninit,	romcngetc,	romcnputc },
#endif
	{ 0 },
};
/* end XXX */

struct	tty *constty = 0;	/* virtual console output device */
struct	consdev *cn_tab;	/* physical console device info */
struct	tty *cn_tty;		/* XXX: console tty struct for tprintf */

cninit()
{
	register struct consdev *cp;

	/*
	 * Collect information about all possible consoles
	 * and find the one with highest priority
	 */
	for (cp = constab; cp->cn_probe; cp++) {
		(*cp->cn_probe)(cp);
		if (cp->cn_pri > CN_DEAD &&
		    (cn_tab == NULL || cp->cn_pri > cn_tab->cn_pri))
			cn_tab = cp;
	}
	/*
	 * No console, we can handle it
	 */
	if ((cp = cn_tab) == NULL)
		return;
	/*
	 * Turn on console
	 */
	cn_tty = cp->cn_tp;
	(*cp->cn_init)(cp);
}

cngetc()
{
	if (cn_tab == NULL)
		return(0);
	return((*cn_tab->cn_getc)(cn_tab->cn_dev));
}

cnputc(c)
	register int c;
{
	if (cn_tab == NULL)
		return;
	if (c) {
		(*cn_tab->cn_putc)(cn_tab->cn_dev, c);
		if (c == '\n')
			(*cn_tab->cn_putc)(cn_tab->cn_dev, '\r');
	}
}
