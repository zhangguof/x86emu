//
//  x86loader.cpp
//  x86loader
//
//  Created by tony on 2019/10/15.
//  Copyright © 2019 tony. All rights reserved.
//

#include <iostream>
#include "x86loader.hpp"
#include "x86loaderPriv.hpp"

void x86loader::HelloWorld(const char * s)
{
    x86loaderPriv *theObj = new x86loaderPriv;
    theObj->HelloWorldPriv(s);
    delete theObj;
};

void x86loaderPriv::HelloWorldPriv(const char * s) 
{
    std::cout << s << std::endl;
};

