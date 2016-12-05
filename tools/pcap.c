#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>

struct pcap_header {
  uint32_t magic_number;   /* magic number */
  uint16_t version_major;  /* major version number */
  uint16_t version_minor;  /* minor version number */
  int32_t  thiszone;       /* GMT to local correction */
  uint32_t sigfigs;        /* accuracy of timestamps */
  uint32_t snaplen;        /* max length of captured packets, in octets */
  uint32_t network;        /* data link type http://www.tcpdump.org/linktypes.html */
};

struct pcap_record_header {
  uint32_t ts_sec;         /* timestamp seconds */
  uint32_t ts_usec;        /* timestamp microseconds */
  uint32_t incl_len;       /* number of octets of packet saved in file */
  uint32_t orig_len;       /* actual length of packet */
};

int pcap_enabled;
FILE* pcap_fp;

void pcap_start(const char* filename)
{
  assert(!pcap_enabled && pcap_fp == NULL);

  pcap_fp = fopen(filename, "w");

  struct pcap_header header = {
    .magic_number = 0xa1b2c3d4,
    .version_major = 2,
    .version_minor = 4,
    .thiszone = 0,
    .sigfigs = 0,
    .snaplen = 65536,
    .network = 101  // Raw IP
  };

  int nw = fwrite(&header, 1, sizeof header, pcap_fp);
  assert(nw == sizeof header);

  pcap_enabled = 1;
}

void pcap_stop()
{
  fclose(pcap_fp);
  pcap_fp = NULL;
  pcap_enabled = 0;
}

void pcap_write(const char *buf, int len, int origlen)
{
  struct timeval tv = { 0, 0 };
  gettimeofday(&tv, NULL);
  struct pcap_record_header record = {
    .ts_sec = tv.tv_sec,
    .ts_usec = tv.tv_usec,
    .incl_len = len,
    .orig_len = origlen
  };

  int nw = fwrite(&record, 1, sizeof record, pcap_fp);
  assert(nw == sizeof record);
  nw = fwrite(buf, 1, len, pcap_fp);
  assert(nw == len);
}
