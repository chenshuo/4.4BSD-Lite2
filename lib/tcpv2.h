#pragma once

// only built-in types in this file

void init();
void ping();
void handshake();

void setipaddr(const char* name, unsigned ip);
void inject(const char* msg, int len);
struct socket* listenon(unsigned short port);

void pigeonattach(int);
int pigeon_dequeue(char *buf, int len);

void tunattach(int);
