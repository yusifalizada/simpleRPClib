//
//  Constants.h
//  simpleRPClib
//
//  Created by Mark Hetherington on 2014-03-12.
//  Copyright (c) 2014 Mark Hetherington. All rights reserved.
//

#ifndef __simpleRPClib__Constants__
#define __simpleRPClib__Constants__

#define HOSTNAMESIZE 128 //255?

#include <iostream>
#include <string>

// Type of message
enum PROTOCOL_TYPE {
    NONE = 10, // SPECIAL FOR NOW
    REGISTER = 0,
    REGISTER_SUCCESS = 1,
    REGISTER_FAILURE = 2,
    LOC_REQUEST = 3,
    LOC_SUCCESS = 4,
    LOC_FAILURE = 5,
    EXECUTE = 6,
    EXECUTE_SUCCESS = 7,
    EXECUTE_FAILURE = 8,
    };

// basic protocol
struct rpc_protocol {
    unsigned long length;
    PROTOCOL_TYPE type;
    void* message;
};

// Basic Response
//struct rpc_response {
//    
//};

struct rpc_base {
    PROTOCOL_TYPE type;
};

// Server/Binder Messages
// REGISTER, server_identifier, port, name, argTypes
struct rpc_register_protocol : rpc_base {
    std::string server_identifier;
    int port;
    std::string name;
    int* argTypes;
};

struct rpc_loc_request : rpc_base {
    std::string name;
    int* argTypes;
};

struct rpc_loc_success : rpc_base {
    std::string server_identifier;
    int port;
};

struct rpc_loc_failure : rpc_base {
    std::string server_identifier;
    int reasonCode;
};


// Client/Binder Messages
// LOC_REQUEST, name, argTypes
// LOC_SUCCESS, server_identifier, port
// LOC_FAILURE, reasonCode

// Client/Server Messages
// EXECUTE, name, argTypes, args
// EXECUTE_SUCCESS, name, argTypes, args
// EXECUTE_FAILURE, reasonCode

// Terminate Messages
// TERMINATE

std::string get_protocol_string(PROTOCOL_TYPE type);


#endif /* defined(__simpleRPClib__Constants__) */
