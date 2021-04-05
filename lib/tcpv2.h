#pragma once

// declares functions in lib/ to be used by main()

// only built-in types in this file

void init();
void ping();
void handshake();

void setipaddr(const char* name, unsigned ip);
void inject(const char* msg, int len);

struct socket;
struct socket* connectto(unsigned ip, unsigned short port);
struct socket* listenon(unsigned short port);
struct socket* acceptso(struct socket*);
int writeso(struct socket* so, void* buf, int nbyte);
int readso(struct socket* so, void* buf, int nbyte);
int issoreadable(struct socket* so);

void pigeonattach(int);
int pigeon_dequeue(char *buf, int len);

void tunattach(int);

// defined in sys/
void ipintr();
void soclose(struct socket*);
