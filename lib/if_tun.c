#include <sys/param.h>
#include <sys/systm.h>

#include <sys/proc.h>
#include <sys/mbuf.h>
#include <sys/buf.h>
#include <sys/dkstat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/tty.h>
#include <sys/kernel.h>
#include <sys/conf.h>

#include <net/if.h>
#include <net/if_types.h>
#include <net/netisr.h>
#include <net/route.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/in_var.h>
#include <netinet/ip.h>

struct	ifnet tunif;

extern int tun_fd;

int tun_write(const char *buf, int len);

int
tunoutput(ifp, m, dst, rt)
	struct ifnet *ifp;
	register struct mbuf *m;
	struct sockaddr *dst;
	register struct rtentry *rt;
{
	char buf[2048];
	int len = m_copydata(m, 0, sizeof buf, buf);
	if (tun_write(buf, len) == len)
		m_freem(m);
	else
		panic("short write tun");
	return 0;
}

/*
 * Process an ioctl request.
 */
int
tunioctl(ifp, cmd, data)
	register struct ifnet *ifp;
	u_long cmd;
	caddr_t data;
{
	int error = 0;
	switch (cmd) {
	case SIOCSIFADDR:
		ifp->if_flags |= IFF_UP;
		/*
		 * Everything else is done at a higher level.
		 */
		break;
	default:
		error = EINVAL;
	}
	return error;
}

void tunattach(int n)
{
	register struct ifnet *ifp = &tunif;
	ifp->if_name = "tun";
	ifp->if_mtu = 1500;
	ifp->if_flags = 0;  // IFF_POINTOPOINT;
	ifp->if_ioctl = tunioctl;
	ifp->if_output = tunoutput;
	ifp->if_type = IFT_TUN;
	ifp->if_hdrlen = 0;
	ifp->if_addrlen = 0;
	if_attach(ifp);
}

