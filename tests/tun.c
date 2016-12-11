#include <assert.h>
#include <fcntl.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>  // inet_ntop
#include <linux/if_tun.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include "../lib/tcpv2.h"

void pffasttimo(void *);  // run every 200ms
void pfslowtimo(void *);  // run every 500ms

int tun_fd = -1;

int tun_write(const char *buf, int len)
{
  printf("write %4d bytes\n", len);
  return write(tun_fd, buf, len);
}

int sethostaddr(const char* dev)
{
  struct ifreq ifr;
  bzero(&ifr, sizeof(ifr));
  strcpy(ifr.ifr_name, dev);
  struct sockaddr_in addr;
  bzero(&addr, sizeof addr);
  addr.sin_family = AF_INET;
  inet_pton(AF_INET, "192.168.0.1", &addr.sin_addr);
  //addr.sin_addr.s_addr = htonl(0xc0a80001);
  bcopy(&addr, &ifr.ifr_addr, sizeof addr);
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
    return sockfd;
  int err = 0;
  // ifconfig tun0 192.168.0.1
  if ((err = ioctl(sockfd, SIOCSIFADDR, (void *) &ifr)) < 0)
  {
    perror("ioctl SIOCSIFADDR");
    goto done;
  }
  // ifup tun0
  if ((err = ioctl(sockfd, SIOCGIFFLAGS, (void *) &ifr)) < 0)
  {
    perror("ioctl SIOCGIFFLAGS");
    goto done;
  }
  ifr.ifr_flags |= IFF_UP;
  if ((err = ioctl(sockfd, SIOCSIFFLAGS, (void *) &ifr)) < 0)
  {
    perror("ioctl SIOCSIFFLAGS");
    goto done;
  }
  // ifconfig tun0 192.168.0.1/24
  inet_pton(AF_INET, "255.255.255.0", &addr.sin_addr);
  bcopy(&addr, &ifr.ifr_netmask, sizeof addr);
  if ((err = ioctl(sockfd, SIOCSIFNETMASK, (void *) &ifr)) < 0)
  {
    perror("ioctl SIOCSIFNETMASK");
    goto done;
  }
done:
  close(sockfd);
  return err;
}

int tun_alloc(char dev[IFNAMSIZ])
{
  struct ifreq ifr;
  int fd, err;

  if ((fd = open("/dev/net/tun", O_RDWR)) < 0)
  {
    perror("open");
    return -1;
  }

  bzero(&ifr, sizeof(ifr));
  ifr.ifr_flags = IFF_TUN | IFF_NO_PI;

  if (*dev)
  {
    strncpy(ifr.ifr_name, dev, IFNAMSIZ);
  }

  if ((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0)
  {
    perror("ioctl TUNSETIFF");
    close(fd);
    return err;
  }
  strcpy(dev, ifr.ifr_name);
  if ((err = sethostaddr(dev)) < 0)
    return err;

  return fd;
}

int64_t slowtimer, fasttimer;

int64_t now_ms()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void do_timers()
{
  int64_t now = now_ms();
  if (now - fasttimer >= 200)
  {
    pffasttimo(NULL);
    fasttimer = now;
  }
  if (now - slowtimer >= 500)
  {
    pfslowtimo(NULL);
    slowtimer = now;
  }
}

int64_t min(int64_t a, int64_t b)
{
  return a < b ? a : b;
}

int main()
{
  tunattach(1);
  init();
  setipaddr("tun0", 0xc0a80002);  // 192.168.0.2
  do_timers();

  char ifname[IFNAMSIZ] = "tun%d";
  tun_fd = tun_alloc(ifname);

  if (tun_fd < 0)
  {
    fprintf(stderr, "tunnel interface allocation failed\n");
    exit(1);
  }

  printf("allocted tunnel interface %s\n", ifname);

  struct socket *server = listenon(1234);
  struct socket *client = NULL;

  struct pollfd pfd = {
    .fd = tun_fd,
    .events = POLLIN,
    .revents = 0,
  };

  for (;;)
  {
    char buf[2048];

    int64_t next_timeout = min(fasttimer+200, slowtimer+500);
    int64_t now = now_ms();
    int waitms = next_timeout - now;
    if (waitms < 0)
      waitms = 0;
    int nevents = poll(&pfd, 1, waitms+10);

    do_timers();

    if (nevents == 0)
      continue;

    int len = read(tun_fd, buf, sizeof(buf));
    if (len < 0)
    {
      perror("read");
      close(tun_fd);
      exit(1);
    }
    if (len == sizeof buf)
    {
      fprintf(stderr, "tun read too much\n");
      close(tun_fd);
      exit(1);
    }

    printf("read  %4d bytes from %s\n", len, ifname);
    inject(buf, len);
    struct socket* so = acceptso(server);
    if (so)
    {
      printf("accepted\n");
      if (client)
      {
        // close
      }
      client = so;
    }
    if (client)
    {
      // read
    }
  }
  return 0;
}
