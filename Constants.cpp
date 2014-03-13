//
//  Constants.cpp
//  simpleRPClib
//
//  Created by Mark Hetherington on 2014-03-12.
//  Copyright (c) 2014 Mark Hetherington. All rights reserved.
//

#include "Constants.h"

#define CASE(code) case code: return #code

static std::string get_protocol_string(PROTOCOL_TYPE type) {
    switch (type)
    {
        CASE(REGISTER);
        CASE(REGISTER_SUCCESS);
        CASE(REGISTER_FAILURE);
        CASE(LOC_REQUEST);
        CASE(LOC_SUCCESS);
        CASE(LOC_FAILURE);
        CASE(EXECUTE);
        CASE(EXECUTE_SUCCESS);
        CASE(EXECUTE_FAILURE);
        CASE(NONE);
    }
}

static PROTOCOL_TYPE get_protocol_from_string(std::string type_string) {
    return NONE; //NOT IMPLIMENTED
}