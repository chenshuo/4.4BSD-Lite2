#include "../lib/tcpv2.h"

int main(int argc, char* argv[])
{
  init();

  ping();
  handshake();
  return 0;
}
