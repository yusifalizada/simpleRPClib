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

#endif /* defined(__simpleRPClib__socketHelper__) */
