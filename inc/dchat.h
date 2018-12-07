#ifndef DCHAT_H_
#define DCHAT_H_

#include <arpa/inet.h>

struct SockInfo {
  int fd;
  unsigned short port;
  struct sockaddr_in addr;
};

void usage(void);

#endif
