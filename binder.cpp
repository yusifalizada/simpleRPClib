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

using namespace std;

int main(int argc, char *argv[]) {
    
    /* prepare server functions' signatures */
    int count0 = 3;
    int argTypes0[count0 + 1];

    argTypes0[0] = 2;//(1 << ARG_OUTPUT) | (ARG_INT << 16);
    argTypes0[1] = 3;//(1 << ARG_INPUT) | (ARG_INT << 16);
    argTypes0[2] = 1;//(1 << ARG_INPUT) | (ARG_INT << 16);
    argTypes0[3] = 0;
    
    rpc_register_protocol rpc;
    rpc.type = REGISTER;
    rpc.server_identifier = "getsomemoney";
    rpc.port = 13;
    rpc.name = "functionA";
    rpc.argTypes = argTypes0;
    
    rpc_protocol pp = compile_register_protocol(rpc);
    cout << get_protocol_string(pp.type) << " String: " << pp.message << endl;
    rpc_register_protocol second = create_register_protocol(pp);

    
    /* return */
    return 0;
}