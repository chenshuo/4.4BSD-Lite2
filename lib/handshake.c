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

