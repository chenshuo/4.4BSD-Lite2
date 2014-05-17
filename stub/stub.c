#include <sys/param.h>
#include <sys/systm.h>

#include <sys/map.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/buf.h>
#include <sys/acct.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <ufs/ufs/quota.h>
#include <sys/uio.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/ioctl.h>
#include <sys/protosw.h>
#include <sys/domain.h>
#include <sys/socket.h>
#include <sys/socketvar.h>

#include <net/if.h>

#include <vm/vm_param.h>

int splnet(void)
{
	// FIXME
	return 1;
}

int splimp(void)
{
	// FIXME
	return 5;
}

void splx(int x)
{
	// FIXME
}

void microtime(tvp)
	register struct timeval *tvp;
{
	gettimeofday(tvp, NULL);
}

void ovbcopy(const void *src, void *dest, size_t n)
{
	bcopy(src, dest, n);
}

//////////////////////////////////////////////////////////////////////////////
int hz = 100;
struct  proc *curproc = NULL;

//////////////////////////////////////////////////////////////////////////////
// sys/i386/i386/machdep.c
//////////////////////////////////////////////////////////////////////////////
/*
 * insert an element into a queue 
 */
#undef insque
void _insque(element, head)
	register struct prochd *element, *head;
{
	element->ph_link = head->ph_link;
	head->ph_link = (struct proc *)element;
	element->ph_rlink = (struct proc *)head;
	((struct prochd *)(element->ph_link))->ph_rlink=(struct proc *)element;
}

/*
 * remove an element from a queue
 */
#undef remque
void _remque(element)
	register struct prochd *element;
{
	((struct prochd *)(element->ph_link))->ph_rlink = element->ph_rlink;
	((struct prochd *)(element->ph_rlink))->ph_link = element->ph_link;
	element->ph_rlink = (struct proc *)0;
}

//////////////////////////////////////////////////////////////////////////////
// sys/i386/i386/trap.c
//////////////////////////////////////////////////////////////////////////////
int
copyout(from, to, len)
	void *from;
	void *to;
	u_int len;
{
	bcopy (from, to, len);
	return 0;
}

int copyin(void* from, void* to, u_int len)
{
	bcopy (from, to, len);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////
// sys/kern/kern_clock.c
//////////////////////////////////////////////////////////////////////////////
/*
 * timeout --
 *	Execute a function after a specified length of time.
 *
 * untimeout --
 *	Cancel previous timeout function call.
 *
 *	See AT&T BCI Driver Reference Manual for specification.  This
 *	implementation differs from that one in that no identification
 *	value is returned from timeout, rather, the original arguments
 *	to timeout are used to identify entries for untimeout.
 */
void
timeout(ftn, arg, ticks)
	void (*ftn) __P((void *));
	void *arg;
	register int ticks;
{
  // FIXME
}

//////////////////////////////////////////////////////////////////////////////
// sys/kern/kern_prot.c
//////////////////////////////////////////////////////////////////////////////
/*
 * Test whether the specified credentials imply "super-user"
 * privilege; if so, and we have accounting info, set the flag
 * indicating use of super-powers.
 * Returns 0 or error.
 */
int
suser(cred, acflag)
	struct ucred *cred;
	u_short *acflag;
{
	if (cred->cr_uid == 0) {
		if (acflag)
			*acflag |= ASU;
		return (0);
	}
	return (EPERM);
}

//////////////////////////////////////////////////////////////////////////////
// sys/kern/kern_synch.c
//////////////////////////////////////////////////////////////////////////////
/*
 * General sleep call.  Suspends the current process until a wakeup is
 * performed on the specified identifier.  The process will then be made
 * runnable with the specified priority.  Sleeps at most timo/hz seconds
 * (0 means no timeout).  If pri includes PCATCH flag, signals are checked
 * before and after sleeping, else signals are not checked.  Returns 0 if
 * awakened, EWOULDBLOCK if the timeout expires.  If PCATCH is set and a
 * signal needs to be delivered, ERESTART is returned if the current system
 * call should be restarted if possible, and EINTR is returned if the system
 * call should be interrupted by the signal (return EINTR).
 */
int
tsleep(ident, priority, wmesg, timo)
	void *ident;
	int priority, timo;
	char *wmesg;
{
  // FIXME
  return 0;
}

/*
 * Make all processes sleeping on the specified identifier runnable.
 */
void
wakeup(ident)
	register void *ident;
{
	// FIXME
}

//////////////////////////////////////////////////////////////////////////////
// sys/kern/kern_sysctl.c
//////////////////////////////////////////////////////////////////////////////
/*
 * Validate parameters and get old / set new parameters
 * for an integer-valued sysctl function.
 */
int
sysctl_int(oldp, oldlenp, newp, newlen, valp)
	void *oldp;
	size_t *oldlenp;
	void *newp;
	size_t newlen;
	int *valp;
{
	int error = 0;

	if (oldp && *oldlenp < sizeof(int))
		return (ENOMEM);
	if (newp && newlen != sizeof(int))
		return (EINVAL);
	*oldlenp = sizeof(int);
	if (oldp)
		error = copyout(valp, oldp, sizeof(int));
	if (error == 0 && newp)
		error = copyin(newp, valp, sizeof(int));
	return (error);
}


//////////////////////////////////////////////////////////////////////////////
// sys/kern/subr_prf.c
//////////////////////////////////////////////////////////////////////////////
void panic(const char *fmt, ...)
{
	printf("panic: ");
	// FIXME
	exit(1);
}

/*
 * Log writes to the log buffer, and guarantees not to sleep (so can be
 * called by interrupt routines).  If there is no process reading the
 * log yet, it writes to the console also.
 */
void log(int level, const char *fmt, ...)
{
}

//////////////////////////////////////////////////////////////////////////////
// sys/kern/subr_proc.c
//////////////////////////////////////////////////////////////////////////////
/*
 * Other process lists
 */
struct pidhashhead *pidhashtbl;
u_long pidhash;

/*
 * Locate a process by number
 */
struct proc *
pfind(pid)
	register pid_t pid;
{
	register struct proc *p;

	for (p = PIDHASH(pid)->lh_first; p != 0; p = p->p_hash.le_next)
		if (p->p_pid == pid)
			return (p);
	return (NULL);
}

//////////////////////////////////////////////////////////////////////////////
// sys/kern/sys_generic.c
//////////////////////////////////////////////////////////////////////////////
/*
 * Do a wakeup when a selectable event occurs.
 */
void
selwakeup(sip)
	register struct selinfo *sip;
{
}

//////////////////////////////////////////////////////////////////////////////
// sys/net/if_loop.c
//////////////////////////////////////////////////////////////////////////////
struct	ifnet loif;

int
looutput(ifp, m, dst, rt)
	struct ifnet *ifp;
	register struct mbuf *m;
	struct sockaddr *dst;
	register struct rtentry *rt;
{
  // FIXME
  return 0;
}

//////////////////////////////////////////////////////////////////////////////
// sys/net/rtsock.c
//////////////////////////////////////////////////////////////////////////////
/*
 * This routine is called to generate a message from the routing
 * socket indicating that a redirect has occured, a routing lookup
 * has failed, or that a protocol has detected timeouts to a particular
 * destination.
 */
void
rt_missmsg(type, rtinfo, flags, error)
	int type, flags, error;
	register struct rt_addrinfo *rtinfo;
{
  // FIXME
}

/*
 * This is called to generate messages from the routing socket
 * indicating a network interface has had addresses associated with it.
 * if we ever reverse the logic and replace messages TO the routing
 * socket indicate a request to configure interfaces, then it will
 * be unnecessary as the routing socket will automatically generate
 * copies of it.
 */
void
rt_newaddrmsg(cmd, ifa, error, rt)
	int cmd, error;
	register struct ifaddr *ifa;
	register struct rtentry *rt;
{
	// FIXME
}

//////////////////////////////////////////////////////////////////////////////
// sys/vm/vm_kern.c
//////////////////////////////////////////////////////////////////////////////
typedef struct vm_map *vm_map_t;
vm_map_t        mb_map;

/*
 * Allocate wired-down memory in the kernel's address map for the higher
 * level kernel memory allocator (kern/kern_malloc.c).  We cannot use
 * kmem_alloc() because we may need to allocate memory at interrupt
 * level where we cannot block (canwait == FALSE).
 *
 * This routine has its own private kernel submap (kmem_map) and object
 * (kmem_object).  This, combined with the fact that only malloc uses
 * this routine, ensures that we will never block in map or object waits.
 *
 * Note that this still only works in a uni-processor environment and
 * when called at splhigh().
 *
 * We don't worry about expanding the map (adding entries) since entries
 * for wired maps are statically allocated.
 */
vm_offset_t
kmem_malloc(map, size, canwait)
	register vm_map_t	map;
	register vm_size_t	size;
	boolean_t		canwait;
{
	// FIXME
	return 0;
}

// TEMP kern/uipc_socket.c
// FIXME:

int
sofree(so)
	register struct socket *so;
{

	if (so->so_pcb || (so->so_state & SS_NOFDREF) == 0)
		return -1;
	if (so->so_head) {
		if (!soqremque(so, 0) && !soqremque(so, 1))
			panic("sofree dq");
		so->so_head = 0;
	}
	sbrelease(&so->so_snd);
	sorflush(so);
	FREE(so, M_SOCKET);
	return 0;
}

void
sorflush(so)
	register struct socket *so;
{
	register struct sockbuf *sb = &so->so_rcv;
	register struct protosw *pr = so->so_proto;
	register int s;
	struct sockbuf asb;

	sb->sb_flags |= SB_NOINTR;
	(void) sblock(sb, M_WAITOK);
	s = splimp();
	socantrcvmore(so);
	sbunlock(sb);
	asb = *sb;
	bzero((caddr_t)sb, sizeof (*sb));
	splx(s);
	if (pr->pr_flags & PR_RIGHTS && pr->pr_domain->dom_dispose)
		(*pr->pr_domain->dom_dispose)(asb.sb_mb);
	sbrelease(&asb);
}

/*
 * Must be called at splnet...
 */
int
soabort(so)
	struct socket *so;
{

	return (
	    (*so->so_proto->pr_usrreq)(so, PRU_ABORT,
		(struct mbuf *)0, (struct mbuf *)0, (struct mbuf *)0));
}

void
sohasoutofband(so)
	register struct socket *so;
{
	struct proc *p;

	if (so->so_pgid < 0)
		gsignal(-so->so_pgid, SIGURG);
	else if (so->so_pgid > 0 && (p = pfind(so->so_pgid)) != 0)
		psignal(p, SIGURG);
	selwakeup(&so->so_rcv.sb_sel);
}
