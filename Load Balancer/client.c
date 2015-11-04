#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>

#define NSTRS       1           
#define ADDRESS     "mysocket"  

char *strs[NSTRS] = {
  "Client Connection Established\n",
};

main()
{
  char c;
  FILE *fp;
  register int i, s, len;
  struct sockaddr_un saun;

  if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
    perror("client: socket");
    exit(1);
  }

  saun.sun_family = AF_UNIX;
  strcpy(saun.sun_path, ADDRESS);

  len = sizeof(saun.sun_family) + strlen(saun.sun_path);

  if (connect(s, &saun, len) < 0) {
    perror("client: connect");
    exit(1);
  }

  fp = fdopen(s, "r");

  for (i = 0; i < NSTRS; i++) {
    while ((c = fgetc(fp)) != EOF) {
      putchar(c);

      if (c == '\n')
        break;
    }
  }

  for (i = 0; i < NSTRS; i++)
    send(s, strs[i], strlen(strs[i]), 0);

  //send msg to balancer
  time_t t;
  srand((unsigned) time(&t));
  char msg[10];
  sprintf(msg,"%d\n%d\n",rand()%100,rand()%100);
  send(s,msg,strlen(msg),0);  

  close(s);

  return 0;
}