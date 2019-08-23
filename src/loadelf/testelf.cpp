
#include <stdio.h>
#include <stdint.h>
#include "elf.h"


void load_elf_bin(const char* path,uint8_t **pdata,uint32_t &size);


#ifdef ELFTEST
int main(int argn,const char* argv[])
{
	const char* path = "../../TiniyOs/so/libtest.so";
	if(argn>1)
	{
		path = argv[1];
	}
	uint8_t* pdata = NULL;
	uint32_t size = 0;
	load_elf_bin(path,&pdata,size);
	Elf64_Ehdr* p_elfh = (Elf64_Ehdr*) pdata;
    

	printf("load success!!\n");
	printf("section num:%d,Segment num:%d\n",
	       p_elfh->e_shnum,p_elfh->e_phnum,size);

	return 0;
}
#endif 
