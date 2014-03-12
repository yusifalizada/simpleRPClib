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

unsigned long sendString(int socket, std::string s);
std::string recvString(int socket);

#endif /* defined(__Client__stringSocket__) */
