#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include "pthread.h"

int nclient=3;
char sendBuff[1024];
int cclients=0;
int clients[10];
int conn[10];

pthread_mutex_t mut1=PTHREAD_MUTEX_INITIALIZER;

/*void * fsend(void *arg){
    int *connfd=(int *)arg;
    char s[1024];
    while(1){
        //printf("Enter a string\n");
        gets(s);
        sprintf(sendBuff,"%s",s);
        send(*connfd,sendBuff,sizeof(sendBuff),0);
    }
    close(*connfd);
    exit(0);
}*/

void * func(void *arg){
    pthread_t sid;
    pthread_detach(pthread_self());
    int connfd=cclients-1;
    printf("Receiving from client %d...\n",cclients);
    //pthread_create(&sid, NULL, &fsend,(void *)connfd);
    while(1){
        //pthread_mutex_lock(&mut1);
        char recvBuff[1024];
        int p=recv(clients[connfd], recvBuff, sizeof(recvBuff),0);
        if(p==0){
            printf("Client disconnected\n");
            break;}
        char s[1024];
        sprintf(s,"%s",recvBuff);
        printf("Message received to %d: %s",connfd,s);
        int d=recvBuff[0]-48;
        if(conn[d]){
            printf("Sending message to %d\n",d);
            send(clients[d],s, sizeof(s),0);
        }
        else{
            sprintf(sendBuff,"This client is not connected anymore\n");
            send(clients[connfd],sendBuff,sizeof(sendBuff),0);
        }
        //pthread_mutex_unlock(&mut1);
    }
    close(clients[connfd]);
    conn[connfd]=0;
    printf("Client %d left. Clossed connections.\n",connfd);
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    int sockfd,connfd;
    if(argc<2){
    printf("Enter the port to connect\n");
    exit(1);
    }
    int port=atoi(argv[1]);
    struct sockaddr_in serv_addr,cli_addr;

    int numrv;  

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd==-1){
    printf("Socket creation failed\n");
    exit(1);
    }
    else
    printf("Socket creation success\n");

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '\0', sizeof(sendBuff));
 
    serv_addr.sin_family = AF_INET;    
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serv_addr.sin_port = htons(port);
    
    for(int i=0;i<10;i++)
        conn[i]=0;
    
    if(bind(sockfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1){
    printf("Naming failed\n");
    exit(1);
    }
    else
    printf("Socket named\n");

    if(listen(sockfd, nclient) == -1){
      printf("Failed to listen\n");
      exit(1);
    }
    printf("Listening...\n");
    while(1){
    connfd = accept(sockfd, (struct sockaddr*)NULL ,0);
    clients[cclients]=connfd;
    conn[cclients]=1;
    cclients++;
    sprintf(sendBuff,"Current number of clients: %d\n",cclients-1);
    send(connfd,sendBuff,sizeof(sendBuff),0);
    pthread_t tid,ptid;
    pthread_create(&ptid, NULL, &func,(void *)&connfd);
    }
    pthread_exit(NULL);
    return 0;
}
