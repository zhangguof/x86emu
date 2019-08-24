
#include <stdio.h>
#include <stdint.h>
#include "elf.h"
#include "elf-ext.hpp"
#include "buffer.hpp"


#ifdef ELFTEST
int main(int argn,const char* argv[])
{
	const char* path = "../../TiniyOs/so/libtest.so";
	if(argn>1)
	{
		path = argv[1];
	}
//    uint8_t* pdata = NULL;
//    uint32_t size = 0;
    BufPtr pdata = nullptr;
	load_elf_bin(path,&pdata);
	Elf64_Ehdr* p_elfh = (Elf64_Ehdr*) pdata->get_data();
    

	printf("load success!!\n");
    printf("section num:%d,Segment num:%d,size:%d\n",
	       p_elfh->e_shnum,p_elfh->e_phnum,pdata->size);

	return 0;
}
#endif 
