#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "pthread.h"

char sendBuff[1024];
char recvBuff[1024];

void * fsend(void *arg){
    int *connfd=(int *)arg;
    while(1){
        //printf("Enter a string\n");
        //memset(s,'\0',sizeof(s));
        char s[1024];
        memset(s,'\0',sizeof(s));
        int p=read(0,s,1024);
        if(p==0){
            printf("\nClient terminated\n");
            break;}
        
        //sprintf(sendBuff,"%s",s);
        //printf("Message sent: %s\n",s);
        send(*connfd,s,sizeof(s),0);
    }
    close(*connfd);
    exit(0);
}

void * func(void *arg){
    pthread_t sid;
    pthread_detach(pthread_self());
    int *connfd=(int *)arg;
    pthread_create(&sid, NULL, &fsend,(void *)connfd);
    while(1){
        memset(recvBuff,'\0',sizeof(recvBuff));
        int p=recv(*connfd, recvBuff, sizeof(recvBuff),0);
        if(p==0){
            printf("Server has left.\n");
            break;}
        printf("Message received: %s\n",recvBuff);
    }
    close(*connfd);
    exit(0);
}

int main(int argc, char **argv)
{
    
    int sockfd = 0,n = 0;
    if(argc<3){
    printf("Enter the port and ip to connect\n");
    exit(1);
    }
    int port=atoi(argv[2]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd==-1){
    printf("Socket creation failed\n");
    exit(1);
    }
  
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    //serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
        printf("Error : Connect Failed \n");
        return 1;
    }
    else
        printf("Socket creation success\n");
        
    pthread_t tid;
    
    pthread_create(&tid, NULL, &func,(void *)&sockfd);
    
    pthread_exit(NULL);
    return 0;
}
