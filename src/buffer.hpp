//
//  buffer.hpp
//  x86emu
//
//  Created by tony on 2019/8/25.
//  Copyright © 2019 tony. All rights reserved.
//

#ifndef buffer_hpp
#define buffer_hpp

#include <stdio.h>

#include <memory>

struct Buffer
{
    uint8_t* data;
    uint32_t size;
    Buffer():data(nullptr),size(0){}
    Buffer(uint32_t _size);
    ~Buffer();
    uint8_t* get_data(){return data;}
};

typedef std::shared_ptr<Buffer> BufPtr;

BufPtr new_buffer(uint32_t size);



#endif /* buffer_hpp */
