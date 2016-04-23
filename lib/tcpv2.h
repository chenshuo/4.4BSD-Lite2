#pragma once

// only built-in types in this file

void init();
void ping();
void handshake();

void setipaddr(const char* name, unsigned ip);
void inject(const char* msg, int len);

struct socket;
struct socket* listenon(unsigned short port);
struct socket* acceptfrom(struct socket*);

void pigeonattach(int);
int pigeon_dequeue(char *buf, int len);

void tunattach(int);
