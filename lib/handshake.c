#include "stub.h"

struct socket* listenon(unsigned short port)
{
  struct socket* so = NULL;
  socreate(AF_INET, &so, SOCK_STREAM, 0);
  struct sockaddr_in addr;
  bzero(&addr, sizeof addr);
  addr.sin_len = sizeof addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  struct mbuf* nam = m_devget((caddr_t)&addr, sizeof addr, 0, NULL, NULL);
  sobind(so, nam);
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
  listenon(port);
  struct socket* so = NULL;
  socreate(AF_INET, &so, SOCK_STREAM, 0);
  struct sockaddr_in addr;
  bzero(&addr, sizeof addr);
  addr.sin_len = sizeof addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(0x7f000001);
  struct mbuf* nam = m_devget((caddr_t)&addr, sizeof addr, 0, NULL, NULL);
  soconnect(so, nam);
  ipintr();
}

