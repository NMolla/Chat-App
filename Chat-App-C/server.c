//
//  server.c
//  Server
//
//  Created by Nahom Molla on 30/04/2021.
//

#include <stdio.h>       // perror, snprintf
#include <stdlib.h>
#include <unistd.h>      // close, write
#include <string.h>      // strlen
#include <sys/socket.h>  // socket, AF_INET, SOCK_STREAM,
                         // bind, listen, accept
#include <netinet/in.h>  // servaddr, INADDR_ANY, htons
#include <pthread.h>
#include "wrapper.h"

#define MAXLINE 4096    // max text line length
#define LISTENQ 1024    // 2nd argument to listen()
#define MAXCLIENTS 100

typedef struct{
    int clientfd;
    char* message;
} Client;

//typedef struct{
//    int clientfd;
//    char name[32];
//} ClientO;

// function declarations =============================================
int setup(int port); //create a server socket listening on localhost

void addClient(int clientfd);
void removeClient(int clientfd);
void displayOnlineClients(int self);
void broadcastMessage(char* msg, int self);
void* handleConnection(void* arg);
void* broadcastMsg(void* arg);

//void addClientO(ClientO clo);
//void removeClientO(ClientO clo);

//globals ============================================================
int client_count = 0;
int clients[MAXCLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
SAFE_BOUNDED messages;

//int clientO_count = 0;
//ClientO clientsO[MAXCLIENTS];
//pthread_mutex_t clientsO_mutex = PTHREAD_MUTEX_INITIALIZER;


int main(int argc, const char * argv[]) {
    int port = 13000;
    
    if (argc > 2){
        fprintf(stderr, "invalid number of args\n");
        exit(2);
    }
    if (argc > 1){
        port = (int)strtol(argv[2], NULL, 10);
    }
    
    // =====================================================================
    
    int serverfd = setup(port);
    S_bounded_init(&messages, 10);
    
    for (int i=0; i<MAXCLIENTS; ++i){
        clients[i] = -1;
    }
    
    pthread_t conn_thread, msg_thread;
    pthread_create(&msg_thread, NULL, &broadcastMsg, NULL);
    
    while(1){
        int clientfd = accept(serverfd, NULL, NULL);
        
        if (client_count + 1 == MAXCLIENTS){
            fprintf(stderr, "maximum number of clients reached.\n");
            close(clientfd);
            continue;
        }
        
        addClient(clientfd);
        //displayOnlineClients(clientfd);
        
        int* clientfd_ptr = (int*)malloc(sizeof(clientfd));
        *clientfd_ptr = clientfd;
        pthread_create(&conn_thread, NULL, &handleConnection, (void*)clientfd_ptr);
    }
}

void* broadcastMsg(void* arg){
    while(1){
        Client* cli_struct_ptr = (Client*)S_bounded_get(&messages);
        char* message = cli_struct_ptr->message;
        int self = cli_struct_ptr->clientfd;
        
        broadcastMessage(message, self);
    }
}

void* handleConnection(void* arg){
    ++client_count;
    int* clientfd_ptr = (int*)arg;
    int welcome = 1;
    
    //displayOnlineClients(*clientfd_ptr);
    
    while (1) {
        char* buffer = (char*)malloc((MAXLINE+1)*sizeof(char));
        memset(buffer, 0, MAXLINE+1);
        
        Client* cli_struct = (Client*)malloc(sizeof(Client));
        cli_struct->clientfd = *clientfd_ptr;
        
        if (read(*clientfd_ptr, buffer, MAXLINE) == -1){
            perror("read() failed");
            break;
        }
        
        if (strstr(buffer, "EXIT") != NULL){
            char* name_start = buffer + 4;
            char name[32];
            strncpy(name, name_start, 32);
            
            memset(buffer, 0, MAXLINE+1);
            //sprintf(buffer, "client #%d - %s has disconnected.\n", *clientfd_ptr, name);
            sprintf(buffer, "%s left.\n", name);
            printf("%s has disconnected.\n", name);
            cli_struct->message = buffer;
            S_bounded_put(&messages, (long long)cli_struct);
            removeClient(*clientfd_ptr);
            --client_count;
            close(*clientfd_ptr);
            break;
        }
        
        if (welcome == 1){
            printf("%s", buffer);
            welcome = 0;
        }
        
        cli_struct->message = buffer;
        S_bounded_put(&messages, (long long)cli_struct);
    }
    
    return NULL;
}

void broadcastMessage(char* msg, int self){
    pthread_mutex_lock(&clients_mutex);
    
    for (int i=0; i<MAXCLIENTS; ++i){
        if (clients[i] != -1){
            if (clients[i] != self){
                dprintf(clients[i], "%s", msg);
            }
        }
    }
    
    pthread_mutex_unlock(&clients_mutex);
}

void addClient(int clientfd){
    pthread_mutex_lock(&clients_mutex);
    
    for (int i=0; i<MAXCLIENTS; ++i){
        if (clients[i] == -1){
            clients[i] = clientfd;
            break;
        }
    }
    
    pthread_mutex_unlock(&clients_mutex);
}

void removeClient(int clientfd){
    pthread_mutex_lock(&clients_mutex);
    
    for (int i=0; i<MAXCLIENTS; ++i){
        if (clients[i] == clientfd){
            clients[i] = -1;
            break;
        }
    }
    
    pthread_mutex_unlock(&clients_mutex);
}

void displayOnlineClients(int self){
    pthread_mutex_lock(&clients_mutex);
    
    char name[32];
    for (int i=0; i<MAXCLIENTS; ++i){
        if (clients[i] != -1){
            if (clients[i] != self){
                dprintf(clients[i], "NAME");
                sleep(1);
                
                if (read(clients[i], name, 32) == -1){
                    perror("read() failed");
                    continue;
                }
                
                dprintf(self, "%s is currently online.\n", name);
            }
        }
    }
    
    pthread_mutex_unlock(&clients_mutex);
}

//create a server socket listening on localhost
int setup(int port){
    int server_socket;
    struct sockaddr_in servaddr;

    printf("setting up server...\n");

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket() failed");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); //use INADDR_ANY for 0.0.0.0 (any available)
    servaddr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr*) &servaddr, sizeof(servaddr)) == -1){
        perror("bind() failed");
        exit(1);
    }

    if (listen(server_socket, LISTENQ) == -1){
        perror("listen() failed");
        exit(1);
    }
    printf("listening on port %d...\n", port);
    
    return server_socket;
}
