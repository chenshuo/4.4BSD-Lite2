#include <stdio.h>

#include "../lib/tcpv2.h"
#include "../tools/pcap.h"

extern int tcp_do_rfc1323;
void tcp_fasttimo();

int main(int argc, char* argv[])
{
  init();

  pcap_start("self.pcap");
  tcp_do_rfc1323 = 0;
  struct socket* clientso = connectto(0x7f000001, 1024);
  ipintr();  // self connection made

  int nw = writeso(clientso, "hello", 5);
  printf("nw = %d\n", nw);
  ipintr();
  tcp_fasttimo();  // send delayed ACK

  char buf[1024] = { 0 };
  int nr = readso(clientso, buf, sizeof buf);
  printf("nr = %d, buf='%s'\n", nr, buf);

  soclose(clientso);
  ipintr();
  pcap_stop();
  return 0;
}
