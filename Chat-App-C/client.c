//
//  client.c
//  Client
//
//  Created by Nahom Molla on 30/04/2021.
//

#include <stdio.h>       // perror, snprintf
#include <stdlib.h>      // exit
#include <unistd.h>      // close, write
#include <string.h>      // strlen
#include <sys/socket.h>  // socket, AF_INET, SOCK_STREAM,
                         // bind, listen, accept
#include <netinet/in.h>  // servaddr, INADDR_ANY, htons
#include <arpa/inet.h>   // inet_pton
#include <pthread.h>

#define MAXLINE 4096    // max text line length

//create a client socket & attempts to connect on ip:port
int establishConnection(char* ip, int port);
void* handleStdin(void* arg);
void* handleServer(void* arg);

int serverfd;
//char* name;

int main(int argc, const char * argv[]) {
    int port = 13000;
    char* ip = "127.0.0.1";
    char* name = "no name";
    
    if (argc > 4){
        printf("invalid number of args\n");
        exit(2);
    }
    if (argc > 3){ //==4 Port
        port = (int)strtol(argv[3], NULL, 10);
    }
    if (argc > 2){ //==3 IP
        ip = (char*)argv[2];
    }
    if (argc > 1){ //==2 Name
        name = (char*)argv[1];
    }
        
    // =======================================================================
    
    serverfd = establishConnection(ip, port);
    printf("Welcome, %s!\n", name);
    dprintf(serverfd, "%s has joined the server.\n", name);
    
    pthread_t stdinListener;
    pthread_create(&stdinListener, NULL, handleStdin, &name);

    pthread_t serverListener;
    pthread_create(&serverListener, NULL, handleServer, &name);

    sleep(1);
    
    pthread_join(stdinListener, NULL);
    pthread_join(serverListener, NULL);
    
    close(serverfd);
    printf("connection terminated...\n");
}

void* handleStdin(void* arg){
    char* name = *(char**)arg;
    char buffer[MAXLINE+1];
    while(1) {
        memset(&buffer, 0, MAXLINE+1);
        
        if (read(STDIN_FILENO, buffer, MAXLINE) == -1){
            perror("stdin read failed()");
            continue;
        }
        
        //if (strncmp(buffer, "EXIT", 5) == 0){
        if (strstr(buffer, "EXIT") != NULL){
            dprintf(serverfd, "EXIT%s", name);
            break;
        }
        
        dprintf(serverfd, "%s: %s", name, buffer);
    }
    return NULL;
}

void* handleServer(void* arg){
    //char* name = *(char**)arg;
    char buffer[MAXLINE+1];
    ssize_t bytesRead;
    while(1) {
        //printf("here server\n");
        memset(&buffer, 0, MAXLINE+1);
        
        if ((bytesRead = read(serverfd, buffer, MAXLINE)) == -1){
            perror("server read failed()");
            continue;
        }
        else if (bytesRead == 0){
            break;
        }
        
        //if (strstr(buffer, "NAME") != NULL){
        //    dprintf(serverfd, "%s", name);
        //    continue;
        //}
        
        printf("%s", buffer);
    
    }
    return NULL;
}

//create a client socket & attempts to connect on ip:port
int establishConnection(char* ip, int port){
    int sockfd = -1;
    struct sockaddr_in servaddr;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1){
        perror("socket() failed");
        exit(1);
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0){
        fprintf(stderr, "inet_pton error for: %s\n", ip);
        exit(1);
    }
    
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
        perror("connect() failed");
        exit(1);
    }
    return sockfd;
}
