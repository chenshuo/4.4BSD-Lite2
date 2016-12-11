#include <stdio.h>

#include "../lib/tcpv2.h"
#include "../tools/pcap.h"

extern int tcp_do_rfc1323;

void dump(const char* buf, int len)
{
  int i;
  for (i = 0; i < len; ++i)
  {
    printf("%02x ", (unsigned char)buf[i]);
    if (i % 16 == 7)
      printf("  ");
    if (i % 16 == 15)
      printf("\n");
  }
  puts("");
}

void test_ping()
{
  // An ICMP echo request, sending from 192.168.0.1 to 192.168.0.2
  char icmp[] = "\x45\x00\x00\x54\x0\x0\x40\x00" "\x40\x01\xb9\x55"
      "\xc0\xa8\x00\x01" "\xc0\xa8\x00\x02"
      "\x08\x00\x67\x16\x3c\x9b\x00\x01\x17\xa2\x7b\x53\x00\x00\x00\x00"
      "\xfb\x84\x07\x00\x00\x00\x00\x00\x10\x11\x12\x13\x14\x15\x16\x17"
      "\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f\x20\x21\x22\x23\x24\x25\x26\x27"
      "\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f\x30\x31\x32\x33\x34\x35\x36\x37";
  inject(icmp, sizeof icmp);

  char buf[2048];
  int len = 0;
  if ( (len = pigeon_dequeue(buf, sizeof buf)) > 0)
  {
    dump(buf, len);
  }
}

int main()
{
  pigeonattach(1);
  init();
  setipaddr("pg0", 0xc0a80002);  // 192.168.0.2

  pcap_start("pigeon.pcap");
  test_ping();

  tcp_do_rfc1323 = 0;
  struct socket* clientso = connectto(0xc0a80001, 1234);

  pcap_stop();
  return 0;
}
