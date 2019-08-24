//
//  buffer.cpp
//  x86emu
//
//  Created by tony on 2019/8/25.
//  Copyright © 2019 tony. All rights reserved.
//

#include "buffer.hpp"

Buffer::Buffer(uint32_t _size){
    data = new uint8_t[_size];
    size = _size;
    
}
Buffer::~Buffer(){
    delete[] data;
}

BufPtr new_buffer(uint32_t size)
{
    auto ptr = std::make_shared<Buffer>(size);
    return ptr;
}
