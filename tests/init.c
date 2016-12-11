#include "../lib/tcpv2.h"
#include "../tools/pcap.h"

int main(int argc, char* argv[])
{
  init();

  pcap_start("init.pcap");
  ping();
  handshake();
  pcap_stop();
  return 0;
}
