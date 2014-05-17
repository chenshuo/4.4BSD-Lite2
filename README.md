User-mode 4.4BSD-Lite2 TCP/IP Stack
===================================

User-land TCP/IP protocol from 4.4BSD-Lite2, a companion of W. Richard Stevens's immortal work *TCP/IP Illustrated volume 2*, with a focus on TCP chapters.  *TCP/IP Illustrated vol.2* uses 4.4BSD-Lite as its example, a slightly early version.  TCP/IP code of 4.4BSD-Lite2 is roughly the same as FreeBSD 2.0.

This is not a fork of https://github.com/sergev/4.4BSD-Lite2, which contains the whole 4.4BSD-Lite2.tar.gz.  This repository only contains usr/src/sys of the tarball, ie. the kernel source.

This TCP/IP stack is alive, using tap/tun device on Linux.

# See also
For running recent FreeBSD TCP/IP stack in user mode, take a look at https://github.com/pkelsey/libuinet and https://github.com/cloudius-systems/osv.
