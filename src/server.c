#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <dchat.h>
#include <dchat/server.h>

static struct SockInfo srv_info;
static struct WaitingClient {
  struct SockInfo *info;
  pthread_mutex_t mutex;
} waiting_client;

void
init_server(void)
{
  // initialize server socket
  if ((srv_info.fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    perror("socket");
    exit(1);
  }

  memset(&srv_info.addr, 0, sizeof(srv_info.addr));
  srv_info.addr.sin_family = AF_INET;
  srv_info.addr.sin_addr.s_addr = htonl(INADDR_ANY);
  srv_info.addr.sin_port = htons(srv_info.port);

  int optval = 1;
  if (setsockopt(srv_info.fd,
        SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(optval))) {
    perror("bind");
    exit(1);
  }

  if (bind(srv_info.fd, (struct sockaddr *)&srv_info.addr, sizeof(srv_info.addr)) < 0) {
    perror("bind");
    exit(1);
  }

  if (listen(srv_info.fd, QUEUE_LIMIT) < 0) {
    perror("listen");
    exit(1);
  }

  // initialize waiting client information
  waiting_client.info = NULL;
  waiting_client.mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_init(&waiting_client.mutex, NULL);

  printf("Server initialization done!\n");
}

void
start_chat(struct SockInfo *c1, struct SockInfo *c2)
{
  fd_set fds, readfds;
  int maxfd;
  char msgbuf[1024];

  if (c1->fd > c2->fd) maxfd = c1->fd;
  else maxfd = c2->fd;

  FD_ZERO(&readfds);
  FD_SET(c1->fd, &readfds);
  FD_SET(c2->fd, &readfds);

  while (1) {
    memcpy(&fds, &readfds, sizeof(fd_set));
    select(maxfd+1, &fds, NULL, NULL, NULL);
    
    // If there are data from client 1
    if (FD_ISSET(c1->fd, &fds)) {
      memset(msgbuf, 0, sizeof(msgbuf));
      recv(c1->fd, msgbuf, sizeof(msgbuf), 0);
      // send c1's message to c2
      send(c2->fd, msgbuf, strlen(msgbuf), 0);
    }

    // If there are data from client 2
    if (FD_ISSET(c2->fd, &fds)) {
      memset(msgbuf, 0, sizeof(msgbuf));
      recv(c2->fd, msgbuf, sizeof(msgbuf), 0);
      // send c2's message to c1
      send(c1->fd, msgbuf, strlen(msgbuf), 0);
    }
  }
}

void
handle_client(void *arg)
{
  struct SockInfo *cli_info = (struct SockInfo *)arg;

  send(cli_info->fd, "Start Matching...\n", 18, 0);

  pthread_mutex_lock(&waiting_client.mutex);
  if (!waiting_client.info) {
    waiting_client.info = cli_info;
    pthread_mutex_unlock(&waiting_client.mutex);
  } else {
    // Get buddy information
    struct SockInfo *buddy = waiting_client.info;

    waiting_client.info = NULL;
    pthread_mutex_unlock(&waiting_client.mutex);

    // Talk with buddy
    start_chat(cli_info, buddy);

    close(cli_info->fd);
    close(buddy->fd);

    free(cli_info);
    free(buddy);
  }
}

void
wait_client(void)
{
  struct SockInfo *cli_info;
  pthread_t thread;
  int addr_len;

  while (1) {
    cli_info = malloc(sizeof(struct SockInfo));
    if (!cli_info) {
      fprintf(stderr, "can not allocate SockInfo structure\n");
      exit(1);
    }

    addr_len = sizeof(cli_info->addr);
    cli_info->fd = accept(srv_info.fd, (struct sockaddr *)&cli_info->addr, &addr_len);

    if (cli_info->fd < 0) {
      perror("accept");
      exit(1);
    }

    pthread_create(&thread, NULL, (void *)handle_client, (void *)cli_info);
  }
}

void
fini_server(void)
{
  // destroy mutex
  pthread_mutex_destroy(&waiting_client.mutex);

  // if there is waiting client then finilize it
  if (waiting_client.info) {
    send(waiting_client.info->fd, "Server is already down", 23, 0);
    close(waiting_client.info->fd);
    free(waiting_client.info);
  }
}

void
srv_main(int argc, const char *argv[])
{
  if (argc < 2) {
    fprintf(stderr, "Too few arguments\n");
    usage();
    exit(1);
  }

  if ((srv_info.port = (unsigned short)atoi(argv[1])) == 0) {
    fprintf(stderr, "Invalid port number: %s\n", argv[1]);
    exit(1);
  }

  init_server();
  wait_client();
  fini_server();

  return;
}
