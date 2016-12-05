#include "stub.h"

// defined in tools/pcap.c

extern int pcap_enabled;

int pcap_write(const char *buf, int len, int origlen);

void ip_intercept(struct mbuf *m)
{
  if (pcap_enabled)
  {
    char buf[2048];
    int len = m_copydata(m, 0, sizeof buf, buf);
    int origlen = (m->m_flags & M_PKTHDR) ? m->m_pkthdr.len : len;
    pcap_write(buf, len, origlen);
  }
}
