#ifndef DCHAT_SERVER_ 
#define DCHAT_SERVER_ 

#include <arpa/inet.h>

#define QUEUE_LIMIT 5

struct ServerStatus {
  int fd;
  unsigned short port;
  struct sockaddr_in addr;
};

void srv_main(int, const char **);

#endif
