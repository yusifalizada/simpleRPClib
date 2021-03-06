//
//  stringSocket.h
//  Client
//
//  Created by Mark Hetherington on 3/3/2014.
//  Copyright (c) 2014 Mark Hetherington. All rights reserved.
//

#ifndef __Client__stringSocket__
#define __Client__stringSocket__

#include <iostream>
#include <string>
#include "Constants.h"

unsigned long send_string(int socket, void* s, unsigned long to_send, PROTOCOL_TYPE type);
rpc_protocol recv_string(int socket);

rpc_protocol compile_register_protocol(rpc_register_protocol rpc_rp);
rpc_register_protocol create_register_protocol(rpc_protocol rcp_p);

rpc_base recv_protocol(int socket);
unsigned long send_protocol(int socket, rpc_protocol rpc_p);

#endif /* defined(__Client__stringSocket__) */
