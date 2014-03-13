//
//  socketHelper.h
//  simpleRPClib
//
//  Created by Mark Hetherington on 2014-03-13.
//  Copyright (c) 2014 Mark Hetherington. All rights reserved.
//

#ifndef __simpleRPClib__socketHelper__
#define __simpleRPClib__socketHelper__

#include <iostream>

// Connect to a server, returns a socket
int connectToServer(const char* hostname, const char* port);

// Close connection on socket
void closeConnection(int sockfd);

// starts server and returns sockfd
int startServer();

// listens on sockfd and performs function with passed in sock_fd
void serverAcceptLoop(int sockfd, void (*f)(int));

// print server address and port
void printServerInfo(int sockfd);

#endif /* defined(__simpleRPClib__socketHelper__) */
