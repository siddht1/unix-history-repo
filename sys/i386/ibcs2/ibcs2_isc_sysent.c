/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * $FreeBSD$
 * created from FreeBSD: src/sys/i386/ibcs2/syscalls.isc,v 1.11 2006/07/14 15:11:20 jhb Exp 
 */

#include <bsm/audit_kevents.h>
#include <sys/param.h>
#include <sys/sysent.h>
#include <sys/sysproto.h>
#include <i386/ibcs2/ibcs2_types.h>
#include <i386/ibcs2/ibcs2_signal.h>
#include <i386/ibcs2/ibcs2_proto.h>
#include <i386/ibcs2/ibcs2_xenix.h>

#define AS(name) (sizeof(struct name) / sizeof(register_t))

/* The casts are bogus but will do for now. */
struct sysent isc_sysent[] = {
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 0 = nosys */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 1 = isc_setostype */
	{ SYF_MPSAFE | AS(ibcs2_rename_args), (sy_call_t *)ibcs2_rename, AUE_RENAME },	/* 2 = ibcs2_rename */
	{ SYF_MPSAFE | AS(ibcs2_sigaction_args), (sy_call_t *)ibcs2_sigaction, AUE_NULL },	/* 3 = ibcs2_sigaction */
	{ SYF_MPSAFE | AS(ibcs2_sigprocmask_args), (sy_call_t *)ibcs2_sigprocmask, AUE_NULL },	/* 4 = ibcs2_sigprocmask */
	{ SYF_MPSAFE | AS(ibcs2_sigpending_args), (sy_call_t *)ibcs2_sigpending, AUE_NULL },	/* 5 = ibcs2_sigpending */
	{ SYF_MPSAFE | AS(getgroups_args), (sy_call_t *)getgroups, AUE_GETGROUPS },	/* 6 = getgroups */
	{ SYF_MPSAFE | AS(setgroups_args), (sy_call_t *)setgroups, AUE_SETGROUPS },	/* 7 = setgroups */
	{ SYF_MPSAFE | AS(ibcs2_pathconf_args), (sy_call_t *)ibcs2_pathconf, AUE_PATHCONF },	/* 8 = ibcs2_pathconf */
	{ SYF_MPSAFE | AS(ibcs2_fpathconf_args), (sy_call_t *)ibcs2_fpathconf, AUE_FPATHCONF },	/* 9 = ibcs2_fpathconf */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 10 = nosys */
	{ SYF_MPSAFE | AS(ibcs2_wait_args), (sy_call_t *)ibcs2_wait, AUE_WAIT4 },	/* 11 = ibcs2_wait */
	{ SYF_MPSAFE | 0, (sy_call_t *)setsid, AUE_SETSID },	/* 12 = setsid */
	{ SYF_MPSAFE | 0, (sy_call_t *)getpid, AUE_GETPID },	/* 13 = getpid */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 14 = isc_adduser */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 15 = isc_setuser */
	{ SYF_MPSAFE | AS(ibcs2_sysconf_args), (sy_call_t *)ibcs2_sysconf, AUE_NULL },	/* 16 = ibcs2_sysconf */
	{ SYF_MPSAFE | AS(ibcs2_sigsuspend_args), (sy_call_t *)ibcs2_sigsuspend, AUE_NULL },	/* 17 = ibcs2_sigsuspend */
	{ SYF_MPSAFE | AS(ibcs2_symlink_args), (sy_call_t *)ibcs2_symlink, AUE_SYMLINK },	/* 18 = ibcs2_symlink */
	{ SYF_MPSAFE | AS(ibcs2_readlink_args), (sy_call_t *)ibcs2_readlink, AUE_READLINK },	/* 19 = ibcs2_readlink */
	{ 0, (sy_call_t *)nosys, AUE_NULL },			/* 20 = isc_getmajor */
};
