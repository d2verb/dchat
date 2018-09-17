#include <stdio.h>
#include <string.h>
#include <dchat/server.h>
#include <dchat/client.h>

void usage(void)
{
  const char *help = "Usage: dchat MODE ...\n"
    "<MODE>\n"
    "  srv -- dchat srv PORT\t\tStart dchat with server mode\n"
    "  cli -- dchat cli HOST PORT\tStart dchat with client mode\n";
  fprintf(stderr, "%s", help);
  return;
}

int main(int argc, const char *argv[])
{
  if (argc < 2) {
    usage();
    return 1;
  }

  if (!strcmp(argv[1], "srv")) {
    srv_main(argc-1, &argv[1]);
  } else if (!strcmp(argv[1], "cli")) {
    cli_main(argc-1, &argv[1]);
  } else {
    fprintf(stderr, "Invalid mode: %s\n", argv[1]);
    usage();
    return 1;
  }

  return 0;
}
