#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>


#define NSTRS       1           
#define No_of_servers     3
#define ADDRESS "myserver1"

pthread_attr_t attr;

char *strs[NSTRS] = {
  "Server 1 Connection established\n",
};


void *connectToClient(void* sockid){
  
  FILE *fp;
  int i;
  int ns = *(int*)sockid;
  fp = fdopen(ns, "r");

  for ( i = 0; i < NSTRS; i++)
    send(ns, strs[i], strlen(strs[i]), 0);

  for ( i = 0; i < NSTRS; i++) {
    char c;
    while ((c = fgetc(fp)) != EOF) {
      putchar(c);

      if (c == '\n')
        break;
    }
  }

  
  //read from client
  int a[2] = {0,0};
  while(1){
    bool flag = false;
    char c;
    int cnt = 0;
    while ((c = fgetc(fp)) != EOF) {
      flag = true;
      if (c == '\n'){
        cnt++;
        if(cnt==2){
          printf("Query : %d %d\n",a[0],a[1]);
          int ans = a[0] + a[1];
          char msg[10];
          sprintf(msg,"%d",ans);
          printf("Response : %s\n",msg);
          a[0] = a[1] = 0;
          break;
        }
      }
      else{
        a[cnt] = 10*a[cnt] + c-'0';
      }
    }
    if(!flag)break;
  }
}

main()
{
  
  
  int fromlen;
  int i, s, ns, len;
  struct sockaddr_un saun, fsaun;
  
  if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
    perror("server: socket");
    exit(1);
  }

  saun.sun_family = AF_UNIX;
  strcpy(saun.sun_path, ADDRESS);

  unlink(ADDRESS);
  len = sizeof(saun.sun_family) + strlen(saun.sun_path);

  if (bind(s, &saun, len) < 0) {
    perror("server: bind");
    exit(1);
  }

  if (listen(s, 5) < 0) {
    perror("server: listen");
    exit(1);
  }

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
  
  while(ns = accept(s, (struct sockadd*)&fsaun, &fromlen)){
    pthread_t thread;
    if(pthread_create(&thread,NULL,connectToClient,(void*)&ns)){
      perror("server: cant create thread");
      exit(1);
    }
    pthread_join(thread,NULL);
  }
  
  if (ns < 0) {
    perror("server: accept");
    exit(1);
  }

  //close(s);
  return 0;
}
