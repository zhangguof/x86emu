#include <stdint.h>
#include <stdio.h>
#include "gdt.h"


const uint32_t GDT_SIZE = 16;

u8			gdt_ptr[6];	// 0~15:Limit  16~47:Base
DESCRIPTOR	gdt[GDT_SIZE];
uint32_t used_gdt_des = 0;


//填充gdt描述符表
void init_descriptor(DESCRIPTOR *p_desc, u32 base, u32 limit, u16 attribute)
{
	// printf("base:0x%0lx,limit:0x%0lx\n", base,limit);
    p_desc->limit_low   = limit & 0x0ffff;
    p_desc->base_low    = base & 0x0ffff;
    p_desc->base_mid     = (base>>16) & 0x0ff;
    p_desc->attr1       = attribute & 0xff;
    p_desc->limit_high_attr2 = ((limit>>16) & 0x0f) | (attribute>>8) & 0xf0;
    p_desc->base_high   = (base>>24) & 0x0ff;
}

void init_gdt()
{
	u16* p_gdt_limt=(u16 *)(&gdt_ptr[0]);
    u32* p_gdt_base=(u32 *)(&gdt_ptr[2]);
	init_descriptor(gdt+0,0,0,0);
	init_descriptor(gdt+1,0,0xfffff, DA_32|DA_CR|DA_LIMIT_4K);
	// used_gdt_des = 2;
	for(int i=2;i<GDT_SIZE;++i)
	{
		init_descriptor(gdt+i,0,0xfffff,DA_32|DA_DRW|DA_LIMIT_4K);
	}
	used_gdt_des = 3;
	// init_descriptor(gdt+2,0,0xfffff, DA_32|DA_DRW|DA_LIMIT_4K);

	*p_gdt_limt = GDT_SIZE * sizeof(DESCRIPTOR) - 1;
    *p_gdt_base = (u32)&gdt;
    // printf("try load gdt!\n");
    load_gdt();

}



