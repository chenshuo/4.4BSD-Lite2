#include <sys/param.h>
#include <sys/systm.h>

#include <sys/map.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/buf.h>
#include <sys/mbuf.h>
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
#include <netinet/in.h>
#include <netinet/in_var.h>

void ping()
{
  // An ICMP echo request, sending from 127.0.0.1 to 127.0.0.1
  char icmp[] = "\x45\x00\x00\x54\x3d\x8b\x40\x00\x40\x01\xff\x1b"
      "\x7f\x00\x00\x01" "\x7f\x00\x00\x01"
      "\x08\x00\x95\x0c\x13\x83\x00\x01\x65\x7e\x79\x53\x00\x00\x00\x00"
      "\xa8\xca\x09\x00\x00\x00\x00\x00\x10\x11\x12\x13\x14\x15\x16\x17"
      "\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f\x20\x21\x22\x23\x24\x25\x26\x27"
      "\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f\x30\x31\x32\x33\x34\x35\x36\x37";

  struct mbuf *m = m_devget(icmp, sizeof(icmp), 0, NULL, NULL);
  register struct ifqueue *inq;
  inq = &ipintrq;
  int s = splimp();
  if (IF_QFULL(inq)) {
    IF_DROP(inq);
    m_freem(m);
  } else
    IF_ENQUEUE(inq, m);
  splx(s);
  ipintr();
}
