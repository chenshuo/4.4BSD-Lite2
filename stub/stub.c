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
#include <sys/ioctl.h>
#include <sys/protosw.h>
#include <sys/domain.h>
#include <sys/signalvar.h>
#include <sys/socket.h>
#include <sys/socketvar.h>

#include <net/if.h>

#include <vm/vm.h>
#include <vm/vm_kern.h>

extern void exit(int) __attribute__ ((__noreturn__));
extern int gettimeofday(struct timeval *, void*);

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

int subyte(void *base, int byte)
{
	return 0;
}

int suibyte(void *base, int byte)
{
	return 0;
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
// sys/conf/param.c
//////////////////////////////////////////////////////////////////////////////
#define HZ 100
int hz = HZ;
int tick = 1000000 / HZ;
struct	proc *curproc = NULL;

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

struct vm_map mb_map_0;
vm_map_t	mb_map = &mb_map_0;

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
	if (map != mb_map)
		panic("kmem_malloc: known map");
	return malloc(size);
}

