#include "stub.h"

// copied from kern/uipc_syscalls.c
int
sockargs(mp, buf, buflen, type)
	struct mbuf **mp;
	caddr_t buf;
	int buflen, type;
{
	register struct sockaddr *sa;
	register struct mbuf *m;
	int error;

	if ((u_int)buflen > MLEN) {
#ifdef COMPAT_OLDSOCK
		if (type == MT_SONAME && (u_int)buflen <= 112)
			buflen = MLEN;		/* unix domain compat. hack */
		else
#endif
		return (EINVAL);
	}
	m = m_get(M_WAIT, type);
	if (m == NULL)
		return (ENOBUFS);
	m->m_len = buflen;
	error = copyin(buf, mtod(m, caddr_t), (u_int)buflen);
	if (error) {
		(void) m_free(m);
		return (error);
	}
	*mp = m;
	if (type == MT_SONAME) {
		sa = mtod(m, struct sockaddr *);

#if defined(COMPAT_OLDSOCK) && BYTE_ORDER != BIG_ENDIAN
		if (sa->sa_family == 0 && sa->sa_len < AF_MAX)
			sa->sa_family = sa->sa_len;
#endif
		sa->sa_len = buflen;
	}
	return (0);
}

// util for setup a server socket
struct socket* listenon(unsigned short port)
{
	// socket()
	struct socket* so = NULL;
	socreate(AF_INET, &so, SOCK_STREAM, 0);
	// bind()
	struct sockaddr_in addr;
	bzero(&addr, sizeof addr);
	addr.sin_len = sizeof addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	struct mbuf* nam;
	sockargs(&nam, (caddr_t)&addr, sizeof addr, MT_SONAME);
	sobind(so, nam);
	m_freem(nam);
	// listen()
	solisten(so, 5);
	return so;
}

struct socket* acceptfrom(struct socket* server)
{
//	if ((so->so_options & SO_ACCEPTCONN) == 0)
//		return (EINVAL);

//	if ((so->so_state & SS_NBIO) && so->so_qlen == 0)
//		return (EWOULDBLOCK);

//	if (so->so_error):
//		error = so->so_error;
//		so->so_error = 0;
//		return (error);
	int s = splnet();
	struct socket *so = server->so_q;
	if (!so)
		goto done;
	if (soqremque(so, 1) == 0)
		panic("accept");
	struct mbuf *nam = m_get(M_WAIT, MT_SONAME);
	(void) soaccept(so, nam);
	m_freem(nam);
done:
	splx(s);
	return so;
}

void handshake()
{
	int port = 1234;
	struct socket* listenso = listenon(port);

	// client
	struct socket* clientso = NULL;
	socreate(AF_INET, &clientso, SOCK_STREAM, 0);
	struct sockaddr_in addr;
	bzero(&addr, sizeof addr);
	addr.sin_len = sizeof addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(0x7f000001);
	struct mbuf* nam;
	sockargs(&nam, (caddr_t)&addr, sizeof addr, MT_SONAME);
	soconnect(clientso, nam);
	m_freem(nam);

	printf("listenso readable=%d, writable=%d\n", soreadable(listenso), sowriteable(listenso));
	printf("clientso readable=%d, writable=%d\n", soreadable(clientso), sowriteable(clientso));
	puts("");

	// handshaking
	ipintr();

	printf("listenso readable=%d, writable=%d\n", soreadable(listenso), sowriteable(listenso));
	printf("clientso readable=%d, writable=%d\n", soreadable(clientso), sowriteable(clientso));
	puts("");

	// accept
	struct socket* serverso = acceptfrom(listenso);

	printf("listenso readable=%d, writable=%d\n", soreadable(listenso), sowriteable(listenso));
	printf("clientso readable=%d, writable=%d\n", soreadable(clientso), sowriteable(clientso));
	printf("serverso readable=%d, writable=%d\n", soreadable(serverso), sowriteable(serverso));
	puts("");

	// close and terminate connection
	soclose(clientso);
	// clientso is FIN_WAIT_1
	ipintr();
	// clientso is FIN_WAIT_2, serverso is CLOSE_WAIT

	printf("listenso readable=%d, writable=%d\n", soreadable(listenso), sowriteable(listenso));
	printf("clientso readable=%d, writable=%d\n", soreadable(clientso), sowriteable(clientso));
	printf("serverso readable=%d, writable=%d\n", soreadable(serverso), sowriteable(serverso));

	soclose(serverso);
	// serverso is LAST_ACK
	ipintr();
	// clientso is TIME_WAIT

	soclose(listenso);
	// ipintr();  // no need
}

