#!/bin/bash

set -x

CC="gcc -g3 -Wall -Wno-parentheses -O0 -m32 -nostdinc -fno-builtin "
CC="$CC -DKERNEL -DINET -DTCPDEBUG -I sys "

mkdir -p objs
rm -rf objs/*.o

$CC -c sys/kern/kern_subr.c -o objs/kern_subr.o
$CC -c sys/kern/uipc_domain.c -o objs/uipc_domain.o
$CC -c sys/kern/uipc_mbuf.c -o objs/uipc_mbuf.o
$CC -c sys/kern/uipc_socket.c -o objs/uipc_socket.o
$CC -c sys/kern/uipc_socket2.c -o objs/uipc_socket2.o
$CC -c sys/kern/uipc_syscalls.c -o objs/uipc_syscalls.o
$CC -c sys/kern/sys_socket.c -o objs/sys_socket.o

$CC -c sys/net/if.c -o objs/if.o
$CC -c sys/net/if_ethersubr.c -o objs/if_ethersubr.o
$CC -c sys/net/if_loop.c -o objs/if_loop.o
$CC -c sys/net/radix.c -o objs/radix.o
$CC -c sys/net/route.c -o objs/route.o

$CC -c sys/netinet/if_ether.c -o objs/if_ether.o
$CC -c sys/netinet/igmp.c -o objs/igmp.o
$CC -c sys/netinet/in.c -o objs/in.o
$CC -c sys/netinet/in_cksum.c -o objs/in_cksum.o
$CC -c sys/netinet/in_pcb.c -o objs/in_pcb.o
$CC -c sys/netinet/in_proto.c -o objs/in_proto.o

$CC -c sys/netinet/ip_icmp.c -o objs/ip_icmp.o
$CC -c sys/netinet/ip_input.c -o objs/ip_input.o
$CC -c sys/netinet/ip_output.c -o objs/ip_output.o
$CC -c sys/netinet/raw_ip.c -o objs/raw_ip.o

$CC -c sys/netinet/tcp_debug.c -o objs/tcp_debug.o
$CC -c sys/netinet/tcp_input.c -o objs/tcp_input.o
$CC -c sys/netinet/tcp_output.c -o objs/tcp_output.o
$CC -c sys/netinet/tcp_subr.c -o objs/tcp_subr.o
$CC -c sys/netinet/tcp_timer.c -o objs/tcp_timer.o
$CC -c sys/netinet/tcp_usrreq.c -o objs/tcp_usrreq.o

$CC -c sys/netinet/udp_usrreq.c -o objs/udp_usrreq.o

$CC -c stub/stub.c -o objs/stub.o

ar rcs objs/libnetinet.a objs/*.o

gcc -m32 -g tests/init.c -o objs/test_init objs/libnetinet.a
