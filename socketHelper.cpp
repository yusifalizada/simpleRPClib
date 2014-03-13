//
//  socketHelper.cpp
//  simpleRPClib
//
//  Created by Mark Hetherington on 2014-03-13.
//  Copyright (c) 2014 Mark Hetherington. All rights reserved.
//

#include "socketHelper.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string>
#include <arpa/inet.h>
#include "stringSocket.h"
#include <queue>
#include <pthread.h>
#include <sys/wait.h>
#include <signal.h>
#include <string>
#include <sys/time.h>
#include <fcntl.h>

#define PORT "0"  // the port users will be connecting to
#define BACKLOG 10     // how many pending connections queue will hold

using namespace std;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

unsigned short int get_client_port(struct sockaddr *clientInformation)
{
	unsigned short int portNumber;
	
	if (clientInformation->sa_family == AF_INET) {
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)clientInformation;
		portNumber = ipv4->sin_port;
		return ntohs(portNumber);
	}else{
		struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)clientInformation;
		portNumber = ipv6->sin6_port;
		return ntohs(portNumber);
   	}
}

int connectToServer(const char* hostname, const char* port) {
    
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    if ((rv = getaddrinfo(hostname, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }
    
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        
        break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return -1;
    }
    
    freeaddrinfo(servinfo); // all done with this structure
    return sockfd;
}

void closeConnection(int sockfd) {
    close(sockfd);
}

/////// SERVER STUFF ///////
void printServerInfo(int sockfd) {
    
    char hostname[1024];
    gethostname(hostname, sizeof(hostname));
    printf("BINDER_ADDRESS %s\n", hostname);
    
    struct sockaddr sin;
    socklen_t len = sizeof(sin);
    getsockname(sockfd, &sin, &len);
    printf("BINDER_PORT %d\n",get_client_port(&sin));
}

int serverStart() {
    
    int sockfd;  // listen on sock_fd, new connection on new_fd
    int rv;
    int yes=1;
    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    
    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        
        if (::bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        
        break;
    }
    
    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }
    
    
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    freeaddrinfo(servinfo); // all done with this structure
    
    struct sigaction sa;
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
    
    return sockfd;
}

void serverAcceptLoop(int sockfd, void (*f)(int)) {
    int max_fd;
    fd_set read_fd_set;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    
    max_fd = sockfd;
    FD_ZERO(&read_fd_set);
    FD_SET(sockfd, &read_fd_set);
    
    while(1) {  // main accept() loop
        // get an fd
        int new_fd = -1;
        
        fd_set readSetTemp = read_fd_set;
        if (select(max_fd+1, &readSetTemp, NULL, NULL, NULL) < 0) exit(4);
        
        for (int i = 0; i < max_fd+1; i++) {
            if (FD_ISSET(i, &readSetTemp)) {
                if (i == sockfd) {
                    // accept
                    sin_size = sizeof their_addr;
                    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
                    
                    // update fd stuff
                    if (new_fd != -1) {
                        FD_SET(new_fd, &read_fd_set);
                        if (new_fd > max_fd) {
                            max_fd = new_fd;
                        }
                    }
                    goto finish;
                } else {
                    new_fd = i;
                    goto finish; //todo fix with a function
                }
            }
        }
    finish:
        
        if (new_fd == -1) {
            perror("accept");
            continue;
        }
        
        //perform action
        (*f)(new_fd);
        
        close(new_fd);
        FD_CLR(new_fd, &read_fd_set);
        
    }
}

void endServer() {
}