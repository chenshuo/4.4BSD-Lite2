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
#include <net/route.h>
#include <netinet/in.h>

struct	pcred cred0;
struct	ucred ucred0;

void mbinit();
void domaininit();

void setipaddr(const char* name, uint ip)
{
  //int fd = socket(AF_INET, SOCK_DGRAM, 0);
  struct ifreq req;
  bzero(&req, sizeof req);
  strcpy(req.ifr_name, name);
  struct sockaddr_in loaddr;
  bzero(&loaddr, sizeof loaddr);
  loaddr.sin_len = sizeof loaddr;
  loaddr.sin_family = AF_INET;
  loaddr.sin_addr.s_addr = htonl(ip);
  bcopy(&loaddr, &req.ifr_addr, sizeof loaddr);
  struct socket* so = NULL;
  socreate(AF_INET, &so, SOCK_DGRAM, 0);
  ifioctl(so, SIOCSIFADDR, (caddr_t)&req, curproc);

  sofree(so);  // FIXME: this doesn't free memory
}

void init()
{
  curproc->p_cred = &cred0;
  curproc->p_ucred = &ucred0;

  pigeonattach(1);
  loopattach(1);
  mbinit();

  int s = splimp();
  ifinit();
  domaininit();
  route_init();
  splx(s);

  setipaddr("lo0", 0x7f000001);
  setipaddr("pg0", 0xc0a80002);
}
