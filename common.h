#pragma once

#include <stdlib.h>

#include <arpa/inet.h>

int server_sockaddr_init(const char *proto, const char* portstr, struct sockaddr_storage *storage);
void addrtostr(const struct sockaddr *addr, char *str, size_t strsize);
int addrparse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage);
void DieWithUserMessage(const char *msg, const char *detail);
void DieWithSystemMessage(const char *msg);

