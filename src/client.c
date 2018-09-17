#include <stdio.h>
#include <stdlib.h>
#include <dchat/client.h>
#include <dchat.h>

void cli_main(int argc, const char *argv[])
{
  if (argc < 3) {
    fprintf(stderr, "Too few arguments\n");
    usage();
    exit(1);
  }
  return;
}
