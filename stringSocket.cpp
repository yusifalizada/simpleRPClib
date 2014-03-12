//
//  stringSocket.cpp
//  Client
//
//  Created by Mark Hetherington on 3/3/2014.
//  Copyright (c) 2014 Mark Hetherington. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include "stringSocket.h"

using namespace std;

unsigned long sendString(int socket, string s) {
    int toSend = (int)s.size();
    
    // send string length
    long change = send(socket, to_string(toSend).c_str(), sizeof(toSend), 0);
    if (change < 0) return -1;
    // send string
    unsigned long sent = 0;
    const char * buffer = s.c_str();
    while (toSend > 0) {
        long change = send(socket,buffer + sent,toSend,0);
        if (change == -1) return -1;
        sent += change;
        toSend -= change;
        
    }
    return sent;
}

string recvString(int socket) {
    char sizeText[(sizeof(int))];
    
    // recieve string length
    long change = recv(socket, sizeText, sizeof(sizeText), 0);
    if (change < 0) {
        perror("rec length");
        return "";
    }
    int toRecv = atoi(sizeText);
    // recieve string
    unsigned long recved = 0;
    char buffer[toRecv];
    while (toRecv > 0) {
        long change = recv(socket, buffer + recved, toRecv, 0);
        if (change == -1) {
            perror("rec length");
            return "";
        }
        recved += change;
        toRecv -= change;
        if (change == 0) break;
    }
    buffer[recved] = '\0';
    return buffer;
}