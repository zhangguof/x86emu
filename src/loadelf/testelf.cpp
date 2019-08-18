#include "elf.h"
#include <stdio.h>
#include <stdint.h>


extern void load_elf_bin(const char* path,uint8_t **pdata);


#ifdef ELFTEST
int main(int argn,const char* argv[])
{
	const char* path = "test";
	if(argn>1)
	{
		path = argv[1];
	}
	uint8_t* pdata = NULL;
	load_elf_bin(path,&pdata);
	Elf64_Ehdr* p_elfh = (Elf64_Ehdr*) pdata;

	printf("load success!!\n");
	printf("section num:%d,Segment num:%d\n",
	       p_elfh->e_shnum,p_elfh->e_phnum);

	return 0;
}
#endif 