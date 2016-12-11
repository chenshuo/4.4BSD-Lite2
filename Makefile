
CC = gcc
CFLAGS = -g -Wall -O0 -m32 -Wno-parentheses

OBJDIR := objs

BINS := test_init test_pigeon test_tun

SRCS= \
     sys/kern/kern_subr.c \
     sys/kern/uipc_domain.c \
     sys/kern/uipc_mbuf.c \
     sys/kern/uipc_socket.c \
     sys/kern/uipc_socket2.c \
     sys/kern/sys_socket.c \
     sys/net/if.c \
     sys/net/if_ethersubr.c \
     sys/net/if_loop.c \
     sys/net/radix.c \
     sys/net/route.c \
     sys/netinet/if_ether.c \
     sys/netinet/igmp.c \
     sys/netinet/in.c \
     sys/netinet/in_cksum.c \
     sys/netinet/in_pcb.c \
     sys/netinet/in_proto.c \
     sys/netinet/ip_icmp.c \
     sys/netinet/ip_input.c \
     sys/netinet/ip_output.c \
     sys/netinet/raw_ip.c \
     sys/netinet/tcp_debug.c \
     sys/netinet/tcp_input.c \
     sys/netinet/tcp_output.c \
     sys/netinet/tcp_subr.c \
     sys/netinet/tcp_timer.c \
     sys/netinet/tcp_usrreq.c \
     sys/netinet/udp_usrreq.c \
     lib/handshake.c \
     lib/if_pigeon.c \
     lib/if_tun.c \
     lib/ip_intercept.c \
     lib/init.c \
     lib/ping.c \
     lib/stub.c

LIB = $(OBJDIR)/libkern.a 

all: $(addprefix $(OBJDIR)/,$(BINS))

$(OBJDIR)/test_%: tests/%.c $(LIB)
	$(CC) $(CFLAGS) $< $(LIB) -o $@

$(OBJDIR)/%.o:%.c
	$(CC) $(CFLAGS) $(KERNFLAGS) -c $< -o $@

KERNOBJS := $(addprefix $(OBJDIR)/,$(SRCS:.c=.o))

$(KERNOBJS): KERNFLAGS = -nostdinc -fno-builtin -DKERNEL -DINET -I sys

$(LIB): $(KERNOBJS) $(OBJDIR)/tools/pcap.o
	ar rcs $@ $^

$(KERNOBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)
	mkdir -p $(OBJDIR)/lib
	mkdir -p $(OBJDIR)/sys/kern
	mkdir -p $(OBJDIR)/sys/net
	mkdir -p $(OBJDIR)/sys/netinet
	mkdir -p $(OBJDIR)/tools

clean:
	rm -rf $(OBJDIR)
