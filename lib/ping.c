#include "stub.h"

void enqueue(struct ifqueue *inq, struct mbuf *m)
{
	int s = splimp();
	if (IF_QFULL(inq)) {
		IF_DROP(inq);
		m_freem(m);
	} else
		IF_ENQUEUE(inq, m);
	splx(s);
}

struct mbuf *dequeue(struct ifqueue *inq)
{
	struct mbuf *m = NULL;
	int s = splimp();
	IF_DEQUEUE(inq, m);
	splx(s);
	return m;
}

void inject(char* msg, int len)
{
	struct mbuf *m = m_devget(msg, len, 0, NULL, NULL);
	enqueue(&ipintrq, m);
	updatetime();
	ipintr();
}

void ping()
{
	// An ICMP echo request, sending from 127.0.0.1 to 127.0.0.1
	char icmp[] = "\x45\x00\x00\x54\x3d\x8b\x40\x00\x40\x01\xff\x1b"
		"\x7f\x00\x00\x01" "\x7f\x00\x00\x01"
		"\x08\x00\x95\x0c\x13\x83\x00\x01\x65\x7e\x79\x53\x00\x00\x00\x00"
		"\xa8\xca\x09\x00\x00\x00\x00\x00\x10\x11\x12\x13\x14\x15\x16\x17"
		"\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f\x20\x21\x22\x23\x24\x25\x26\x27"
		"\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f\x30\x31\x32\x33\x34\x35\x36\x37";
	inject(icmp, sizeof icmp);
}
