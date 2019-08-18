//
//  mmu.hpp
//  x86emu
//
//  Created by tony on 2019/8/18.
//  Copyright © 2019 tony. All rights reserved.
//

#ifndef mmu_hpp
#define mmu_hpp

#include <stdio.h>
#include "bochs.h"

#endif /* mmu_hpp */
//in long mode 64bit
//use 4 level page table!
//ref Intel® 64 and IA-32 Architectures Software Developer’s Manual
//      Volume 3 (3A, 3B, 3C & 3D): System Programming Guide

//48bit address
/*
 47    39 38             30 29         21      12 11     0
 |PML4 |  Directory Ptr  |  Directory  | Table  | Offset |
 
 */

//64bit

const Bit64u PML4EMASK = BX_CONST64(0x000FFFFFFFFFF000);

// Accessors for all  bitfields.
#define IMPLEMENT_ACCESSORS(name, bitnum)            \
BX_CPP_INLINE bx_bool get_##name() const {               \
return 1 & (val64 >> bitnum);                          \
}                                                        \
BX_CPP_INLINE void set_##name(Bit8u val) {               \
val64 = (val64 & ~(1<<bitnum)) | ((!!val) << bitnum);  \
}

// a 4-Level PML4 Entry (PML4E) that References a Page-Directory-Pointer Table
struct PML4E
{
    Bit64u val64;

    IMPLEMENT_ACCESSORS(P,   0);
    IMPLEMENT_ACCESSORS(RW,  1);
    IMPLEMENT_ACCESSORS(US,  2);
    IMPLEMENT_ACCESSORS(PWT, 3);
    IMPLEMENT_ACCESSORS(PCD, 4);
    IMPLEMENT_ACCESSORS(A,   5);
    IMPLEMENT_ACCESSORS(PS,  7);

    Bit64u get64() const {return val64;}
    void set64(Bit64u val) {val64 = val;}
    Bit64u get_phaddr() const {
        return ((val64 & PML4EMASK));
    }
    //48bit address 4k aligment
    void set_phaddr(Bit64u val){
        val64 = (val64 & ~PML4EMASK) | (val & PML4EMASK);
    }
};

// a 4-Level Page-Directory-Pointer-Table Entry (PDPTE) that References a Page Directory

typedef PML4E PDPTE;

//a 4-Level Page-Directory Entry that References a Page Table
typedef PML4E PDE;

 //a 4-Level Page-Table Entry that Maps a 4-KByte Page
struct PTE
{
    Bit64u val64;
    
    IMPLEMENT_ACCESSORS(P,   0);
    IMPLEMENT_ACCESSORS(RW,  1);
    IMPLEMENT_ACCESSORS(US,  2);
    IMPLEMENT_ACCESSORS(PWT, 3);
    IMPLEMENT_ACCESSORS(PCD, 4);
    IMPLEMENT_ACCESSORS(A,   5);
    
    //Dirty; indicates whether software has written to the 4-KByte page referenced by this entry
    IMPLEMENT_ACCESSORS(D,   6);
    IMPLEMENT_ACCESSORS(PS,  7);
    //Global; if CR4.PGE = 1, determines whether the translation is global (see Section 4.10); ignored otherwise
    IMPLEMENT_ACCESSORS(G,   8);
    
    Bit64u get64() const {return val64;}
    void set64(Bit64u val) {val64 = val;}
    
    Bit64u get_phaddr() const {
        return ((val64 & PML4EMASK));
    }
    //48bit address 4k aligment
    void set_phaddr(Bit64u val){
        val64 = (val64 & ~PML4EMASK) | (val & PML4EMASK);
    }
};


