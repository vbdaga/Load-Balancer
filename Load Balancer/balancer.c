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
#define ADDRESS     "mysocket" 
#define No_of_servers     3

pthread_attr_t attr;
pthread_t thread[No_of_servers];
int idleThread[No_of_servers];
int curid, vacant, active;
bool working[No_of_servers];

char* ADDRESS_Server[No_of_servers] = {
  "myserver1","myserver2","myserver3"
};
char *strs[NSTRS] = {
  "Balancer Connection established\n",
};

int serverid[No_of_servers];
FILE* serverfile[No_of_servers];

void connectToServer(int server_no){
  printf("%d\n",server_no);
  char c;
  FILE *fp;
  register int i, s, len;
  struct sockaddr_un saun;

  if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
    perror("client: socket");
    exit(1);
  }

  saun.sun_family = AF_UNIX;
  strcpy(saun.sun_path, ADDRESS_Server[server_no]);

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

  serverid[server_no] = s;
  serverfile[server_no] = fp;
}

void *connectToClient(void* arg){
  
  FILE *fp;
  int i;
  int ns = ((int*)arg)[0];
  fp = fdopen(ns, "r");
  int curthread = ((int*)arg)[1];


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
  //printf("%d\n", curthread);
  sleep(10*curthread+5);

  while(1){
    char msg[100];
    int temp = 0;
    bool flag = false;
    char c;
    int cnt = 0;
    while ((c = fgetc(fp)) != EOF) {
      msg[temp++]=c;
      flag = true;
      if (c == '\n'){
        cnt++;
        if(cnt==2){
          send(serverid[curthread], msg, strlen(msg), 0);
          //puts("a");
          break;
        }
      }
    }
    if(!flag)break;
  }

  idleThread[vacant++] = curthread;
  vacant %= No_of_servers;
  active--;
  working[curthread] = false;

}

void initialiseConnection(int server_no){
  connectToServer(server_no);
}

int comeBackYesterday(){
	while(active>=No_of_servers){
      //puts("come back yesterday");
    }
	int curthread = idleThread[curid++];
  curid %= No_of_servers;
	return curthread;
}

int merryGoRound(){
  while(working[curid]);
  working[curid]=true;
  curid++;
  curid%=No_of_servers;
  return (curid-1+No_of_servers)%No_of_servers;
}

main()
{

  int fromlen;
  int i, s, ns, len;
  struct sockaddr_un saun, fsaun;
  
  for(i=0 ;i<3; i++)
    idleThread[i] = i;

  for(i=0; i<3;i++)
    initialiseConnection(i);

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
  pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
  
  while(1){
    
    ns = accept(s, (struct sockadd*)&fsaun, &fromlen);
    if (ns < 0) {
      perror("server: accept");
      exit(1);
    }
    
    int curthread = comeBackYesterday();
    //int curthread = merryGoRound();
    int par[2]={ns,curthread};
    active++;

    if(pthread_create(&thread[curthread],NULL,connectToClient,(void*)par)){
      perror("server: cant create thread");
      exit(1);
    }
    printf("Sent to server : %d\n",curthread);
    pthread_detach(thread[curthread]);
    
    /*while(active>=No_of_servers){
      //puts("come back yesterday");
    }*/
  }
  
  if (ns < 0) {
    perror("server: accept");
    exit(1);
  }

  close(s);
  return 0;
}
