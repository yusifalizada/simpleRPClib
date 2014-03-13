//
//  binder.cpp
//  simpleRPClib
//
//  Created by Mark Hetherington on 2014-03-12.
//  Copyright (c) 2014 Mark Hetherington. All rights reserved.
//

#include "binder.h"
#include "stringSocket.h"
#include "Constants.h"
#include "rpc.h"
#include "server_function_skels.h"
#include "socketHelper.h"

using namespace std;

void action(int new_fd) {
    rpc_protocol buffer = recv_string(new_fd);
    if (send_protocol(new_fd, buffer) == -1) perror("send error");
};

int main(int argc, char *argv[]) {
    
    int sockfd = serverStart();
    printServerInfo(sockfd);
    serverAcceptLoop(sockfd, action);

    /* return */
    return 0;
}