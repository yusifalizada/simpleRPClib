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
    // TODO: not converted to c_str yet
    return send_string(socket, "",rpc_p.type);//(string)rpc_p.message, rpc_p.type);
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


rpc_register_protocol create_register_protocol(rpc_protocol rpc_p) {
    rpc_register_protocol rpc_rp;
    rpc_rp.type = rpc_p.type;
    
    int start = 0;
    int end = HOSTNAMESIZE;
    char *si = new char[end];
    memcpy(si, rpc_p.message, end);
    rpc_rp.server_identifier = si;
    
    start += end;
    end = sizeof(int);
    memcpy(&rpc_rp.port,(char*)rpc_p.message + start, end);
    start += end;
    rpc_rp.name = strdup((char*)rpc_p.message + start);
    
    start += rpc_rp.name.length() + 1;
    rpc_rp.argTypes = intdup((int*)((char*)rpc_p.message + start));
    
    return rpc_rp;
}

rpc_protocol compile_register_protocol(rpc_register_protocol rpc_rp) {
    rpc_protocol rpc_p;
    rpc_p.type = rpc_rp.type;
    
    
    unsigned long size =
        HOSTNAMESIZE + sizeof(int) + sizeof(rpc_rp.name) +
        get_intstar_length(rpc_rp.argTypes) * sizeof(int);
    
    rpc_p.message = malloc(size); //TODO why must i malloc instead of new
    void* data = rpc_p.message;
    memset(data, 0, size);
    
    int pos = 0;
    memcpy((char*)data + pos, rpc_rp.server_identifier.c_str(),rpc_rp.server_identifier.length());
    pos += HOSTNAMESIZE;
    memcpy((char*)data + pos, &rpc_rp.port, sizeof(int));
    pos += sizeof(int);
    memcpy((char*)data + pos, rpc_rp.name.c_str(),rpc_rp.name.length());
    pos += rpc_rp.name.length() + 1;
    
    memcpy((char*)data + pos,
           rpc_rp.argTypes,
           get_intstar_length(rpc_rp.argTypes) * sizeof(int));
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