#include "stub.h"

int sockargs(struct mbuf **mp, caddr_t buf, int buflen, int type);
void puts(const char*);

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

struct socket* acceptso(struct socket* server)
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

int writeso(struct socket* so, void* buf, int nbyte)
{
	struct uio auio;
	struct iovec aiov;
	long cnt = nbyte, error = 0;
	aiov.iov_base = (caddr_t)buf;
	aiov.iov_len = nbyte;
	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	auio.uio_resid = nbyte;
	auio.uio_rw = UIO_WRITE;
	auio.uio_segflg = UIO_USERSPACE;
	auio.uio_procp = curproc;
	if (error = sosend(so, (struct mbuf *)0,
		&auio, (struct mbuf *)0, (struct mbuf *)0, 0)) {
		if (auio.uio_resid != cnt && (error == ERESTART ||
		    error == EINTR || error == EWOULDBLOCK))
			error = 0;
		//if (error == EPIPE)
		//	psignal(p, SIGPIPE);
	}
	cnt -= auio.uio_resid;
	return cnt;
}

int readso(struct socket* so, void* buf, int nbyte)
{
	struct uio auio;
	struct iovec aiov;
	long cnt = nbyte, error = 0;
	aiov.iov_base = (caddr_t)buf;
	aiov.iov_len = nbyte;
	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	auio.uio_resid = nbyte;
	auio.uio_rw = UIO_READ;
	auio.uio_segflg = UIO_USERSPACE;
	auio.uio_procp = curproc;
	if (error = soreceive(so, (struct mbuf **)0,
		&auio, (struct mbuf **)0, (struct mbuf **)0, 0)) {
		if (auio.uio_resid != cnt && (error == ERESTART ||
		    error == EINTR || error == EWOULDBLOCK))
			error = 0;
	}
	cnt -= auio.uio_resid;
	return cnt;
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
	clientso->so_state |= SS_NBIO;
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
	struct socket* serverso = acceptso(listenso);

	printf("listenso readable=%d, writable=%d\n", soreadable(listenso), sowriteable(listenso));
	printf("clientso readable=%d, writable=%d\n", soreadable(clientso), sowriteable(clientso));
	printf("serverso readable=%d, writable=%d\n", soreadable(serverso), sowriteable(serverso));
	puts("");

	char buf[20480];
	int nw = writeso(clientso, buf, 10240);
	printf("nw = %d\n", nw);
	ipintr();
	tcp_fasttimo();
	ipintr();
	tcp_fasttimo();
	ipintr();
	tcp_fasttimo();
	ipintr();
	tcp_fasttimo();
	ipintr();

	printf("listenso readable=%d, writable=%d\n", soreadable(listenso), sowriteable(listenso));
	printf("clientso readable=%d, writable=%d\n", soreadable(clientso), sowriteable(clientso));
	printf("serverso readable=%d, writable=%d\n", soreadable(serverso), sowriteable(serverso));
	puts("");

	int nr = readso(serverso, buf, sizeof buf);
	printf("nr = %d\n", nr);

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

