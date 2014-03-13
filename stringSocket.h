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

unsigned long send_string(int socket, std::string s, PROTOCOL_TYPE type);
rpc_protocol recv_string(int socket);

#endif /* defined(__Client__stringSocket__) */
