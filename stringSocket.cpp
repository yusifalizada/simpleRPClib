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

// gets the int array length
int get_intstar_length(int const * istar){
    int length = 0;
    while (*(istar+length) != 0) {
        length++;
    }
    return length;
}

// creates a duplicate int array
int * intdup(int const * src, size_t len = -1)
{
    if (len == -1) len = get_intstar_length(src);
    int *p = new int[len];
    memcpy(p, src, len * sizeof(int));
    return p;
}

// copies int array
void copy_int_array(void* to, const int* from) {
    memcpy(to, from, get_intstar_length(from) * sizeof(int));
}

// copies char*
void copy_string(void* to, const char* from) {
    memcpy(to, from, strlen(from));
}

// copies int
void copy_int(void* to, const int* from) {
    *(int*)to = *from;
    //memcpy(to, from, sizeof(int));
}


unsigned long send_string(int socket, void* s, unsigned long to_send, PROTOCOL_TYPE type) {
    int int_type = type;
    
    // send string length
    long change = send(socket, to_string(to_send).c_str(), sizeof(to_send), 0);
    if (change < 0) return -1;
    
    // send type
    change = send(socket,to_string(int_type).c_str(), sizeof(int_type), 0);
    if (change < 0) return -1;
    
    // send string
    unsigned long sent = 0;
    const void * buffer = s;
    while (to_send > 0) {
        long change = send(socket,(char*)buffer + sent,to_send,0);
        if (change == -1) return -1;
        sent += change;
        to_send -= change;
    }
    return sent;
}

unsigned long send_protocol(int socket, rpc_protocol rpc_p){
    return send_string(socket, rpc_p.message, rpc_p.length, rpc_p.type);
}

rpc_protocol recv_string(int socket) {
    char sizeText[(sizeof(int))];
    char type[(sizeof(int))];
    rpc_protocol rpc_p;
    
    // recieve string length //TODO send int value
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
    unsigned long toRecv = rpc_p.length;
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
    rpc_p.message = buffer;
    return rpc_p;
}


rpc_register_protocol create_register_protocol(rpc_protocol rpc_p) {
    rpc_register_protocol rpc_rp;
    rpc_rp.type = rpc_p.type;
    int start = 0;
    
    // Copy server id
    int end = HOSTNAMESIZE;
    rpc_rp.server_identifier = strdup((char*)rpc_p.message);
    
    // Copy port
    start += end;
    end = sizeof(int);
    rpc_rp.port = *((char*)rpc_p.message + start);
    
    // Copy function name
    start += end;
    rpc_rp.name = strdup((char*)rpc_p.message + start);
    
    // Copy args
    start += rpc_rp.name.length() + 1;
    rpc_rp.argTypes = intdup((int*)((char*)rpc_p.message + start));
    
    return rpc_rp;
}

rpc_protocol compile_register_protocol(rpc_register_protocol rpc_rp) {
    rpc_protocol rpc_p;
    rpc_p.type = rpc_rp.type;
    
    // find message length
    unsigned long size =
        HOSTNAMESIZE + sizeof(int) + sizeof(rpc_rp.name) +
        get_intstar_length(rpc_rp.argTypes) * sizeof(int);
    rpc_p.length = size;
    
    // init message size
    rpc_p.message = malloc(size);
    void* data = rpc_p.message;
    memset(data, 0, size); //clears memory
    
    // Copy server id
    int pos = 0;
    copy_string((char*)data + pos, rpc_rp.server_identifier.c_str());
    
    // Copy port
    pos += HOSTNAMESIZE;
    copy_int((char*)data + pos, &rpc_rp.port);
    
    // Copy function name
    pos += sizeof(int);
    copy_string((char*)data + pos, rpc_rp.name.c_str());
    
    // Copy args
    pos += rpc_rp.name.length() + 1;
    copy_int_array((char*)data + pos, rpc_rp.argTypes);
    
    return rpc_p;
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