#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include <dchat.h>
#include <dchat/server.h>

void init_server(struct ServerStatus *srv_st, unsigned short port)
{
  srv_st->port = port;

  if ((srv_st->fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    perror("socket");
    exit(1);
  }

  memset(&srv_st->addr, 0, sizeof(srv_st->addr));
  srv_st->addr.sin_family = AF_INET;
  srv_st->addr.sin_addr.s_addr = htonl(INADDR_ANY);
  srv_st->addr.sin_port = htons(srv_st->port);

  if (bind(srv_st->fd, (struct sockaddr *)&srv_st->addr, sizeof(srv_st->addr)) < 0) {
    perror("bind");
    exit(1);
  }

  if (listen(srv_st->fd, QUEUE_LIMIT) < 0) {
    perror("listen");
    exit(1);
  }
}

void wait_client(struct ServerStatus *srv_st)
{
  struct sockaddr_in cli_addr;
  int cli_addr_len;
  int cli_fd;

  while (1) {
    cli_addr_len = sizeof(cli_addr);
    if ((cli_fd = accept(srv_st->fd, (struct sockaddr *)&cli_addr, &cli_addr_len)) < 0) {
      perror("accept");
      exit(1);
    }
    printf("connected from %s\n", inet_ntoa(cli_addr.sin_addr));
    close(cli_fd);
  }
}

void srv_main(int argc, const char *argv[])
{
  if (argc < 2) {
    fprintf(stderr, "Too few arguments\n");
    usage();
    exit(1);
  }

  unsigned short port;
  if ((port = (unsigned short)atoi(argv[1])) == 0) {
    fprintf(stderr, "Invalid port number: %s\n", argv[1]);
    exit(1);
  }

  struct ServerStatus srv_st;

  init_server(&srv_st, port);
  wait_client(&srv_st);

  return;
}
