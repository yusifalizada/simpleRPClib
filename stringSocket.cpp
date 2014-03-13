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

int * intdup(int const * src, size_t len)
{
    int *p = new int[len];
    memcpy(p, src, len * sizeof(int));
    return p;
}

unsigned long send_string(int socket, string s, PROTOCOL_TYPE type) {
    int to_send = (int)s.size();
    int int_type = type;
    
    // send string length
    long change = send(socket, to_string(to_send).c_str(), sizeof(to_send), 0);
    if (change < 0) return -1;
    
    // send type
    change = send(socket,to_string(int_type).c_str(), sizeof(int_type), 0);
    if (change < 0) return -1;
    
    // send string
    unsigned long sent = 0;
    const char * buffer = s.c_str();
    while (to_send > 0) {
        long change = send(socket,buffer + sent,to_send,0);
        if (change == -1) return -1;
        sent += change;
        to_send -= change;
    }
    return sent;
}

unsigned long send_protocol(int socket, rpc_protocol rpc_p){
    return send_string(socket, rpc_p.message, rpc_p.type);
}

rpc_protocol recv_string(int socket) {
    char sizeText[(sizeof(int))];
    char type[(sizeof(int))];
    rpc_protocol rpc_p;
    
    // recieve string length
    long change = recv(socket, sizeText, sizeof(sizeText), 0);
    if (change < 0) {
        perror("rec length");
        return rpc_p;
    }
    rpc_p.length = atoi(sizeText);
    
    // recieve RPC type
    change = recv(socket, type, sizeof(type), 0);
    if (change < 0) {
        perror("rec type");
        return rpc_p;
    }
    rpc_p.type = (PROTOCOL_TYPE)atoi(type);
    
    // recieve string
    int toRecv = rpc_p.length;
    unsigned long recved = 0;
    char buffer[toRecv];
    while (toRecv > 0) {
        long change = recv(socket, buffer + recved, toRecv, 0);
        if (change == -1) {
            perror("rec string");
            return rpc_p;
        }
        recved += change;
        toRecv -= change;
        if (change == 0) break;
    }
    buffer[recved] = '\0';
    rpc_p.message = buffer; //TODO: confirm works
    return rpc_p;
}


rpc_register_protocol create_register_protocol(rpc_protocol rcp_p) {
    rpc_register_protocol rpc_rp;
    rpc_rp.type = rcp_p.type;
    size_t start = 0;
    size_t end = HOSTNAMESIZE;
    rpc_rp.server_identifier = rcp_p.message.substr(start, end);
    
    start += end;
    end = sizeof(int);
    rpc_rp.port = atoi(rcp_p.message.substr(start,end).c_str());
    
    start += end;
    end = rcp_p.message.find('\0',start);
    rpc_rp.name = rcp_p.message.substr(start,end+1);
    
    start += end+1;
    rpc_rp.argTypes = intdup((int*)rcp_p.message.substr(start).c_str(),start - rcp_p.message.length());
    return rpc_rp;

}

rpc_base recv_protocol(int socket) {
    rpc_protocol rcp_p = recv_string(socket);
    switch (rcp_p.type) {
        case REGISTER:
            return create_register_protocol(rcp_p);
    }
    //TODO: handle error state
    rpc_base b;
    return b;
};