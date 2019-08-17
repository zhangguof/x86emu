#include "elf.h"
#include "stdio.h"
#include "string.h"
#include "assert.h"

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

const int MAX_FILE_BUF = 512*1024; //512k;

int load_file(const char* path, uint8_t* data)
{
	int fd;
	fd = open(path,O_RDONLY);
	assert(fd!=0);

	struct stat stat_buf;
	int ret = fstat(fd,&stat_buf);
	if(ret)
	{
		close(fd);
		printf("Error! open file!\n");
		return 0;
	}
	uint32_t size = stat_buf.st_size;
	uint8_t* ptr_buf = data;
	while(size > 0)
	{
		ret = read(fd,ptr_buf,size);
		if(ret<=0)
		{
			printf("Error:read file!\n");
			return 0;
		}
		size -= ret;
		ptr_buf += ret;
	}
	close(fd);
	return ptr_buf - data;

}

void load_elf32(uint8_t* buf,Elf32_Ehdr* elf_h)
{

}

#define PRINT_ATTR(name,ptr) \
printf(#name":0x%0x\n",(ptr)->name);

void print_elf_info(Elf64_Ehdr* p_elfh)
{
	// printf("e_type:%0x\n",p_elfh->e_type);
	// printf("e_machine:%0x\n",p_elfh->e_machine);
	PRINT_ATTR(e_type,p_elfh);
	PRINT_ATTR(e_machine,p_elfh);
	PRINT_ATTR(e_version,p_elfh);
	PRINT_ATTR(e_entry,p_elfh);		/* Entry point virtual address */
	PRINT_ATTR(e_phoff,p_elfh);		/* Program header table file offset */
	PRINT_ATTR(e_shoff,p_elfh);		/* Section header table file offset */
	PRINT_ATTR(e_flags,p_elfh);
	PRINT_ATTR(e_ehsize,p_elfh);
	PRINT_ATTR(e_phentsize,p_elfh);
	PRINT_ATTR(e_phnum,p_elfh);
	PRINT_ATTR(e_shentsize,p_elfh);
	PRINT_ATTR(e_shnum,p_elfh);
	PRINT_ATTR(e_shstrndx,p_elfh);

}

void print_elf_seciton_info(Elf64_Shdr* p_es)
{
	PRINT_ATTR(sh_name,p_es);		/* Section name, index in string tbl */
	PRINT_ATTR(sh_type,p_es);		/* Type of section */
	PRINT_ATTR(sh_flags,p_es);		/* Miscellaneous section attributes */
	PRINT_ATTR(sh_addr,p_es);		/* Section virtual addr at execution */
	PRINT_ATTR(sh_offset,p_es);		/* Section file offset */
	PRINT_ATTR(sh_size,p_es);		/* Size of section in bytes */
	PRINT_ATTR(sh_link,p_es);		/* Index of another section */
	PRINT_ATTR(sh_info,p_es);		/* Additional section information */
	PRINT_ATTR(sh_addralign,p_es);	/* Section alignment */
	PRINT_ATTR(sh_entsize,p_es);	/* Entry size if section holds table */	
}

char* load_str_table_section(uint8_t* buf,Elf64_Shdr* p_sh)
{
	char* strs = new char[p_sh->sh_size];
	memcpy(strs, buf+p_sh->sh_offset, p_sh->sh_size);
	return strs;
}
Elf64_Sym* load_symbol_table(uint8_t* buf,Elf64_Shdr* p_sh,int &n)
{
	assert(sizeof(Elf64_Sym) == p_sh->sh_entsize);
	int num = p_sh->sh_size / p_sh->sh_entsize;
	Elf64_Sym* symtab = new Elf64_Sym[num];

	memcpy(symtab,buf+p_sh->sh_offset,p_sh->sh_size);
	n = num;

	return symtab;


}

void load_elf64(uint8_t* buf,Elf64_Ehdr* elf_h)
{
	Elf64_Ehdr* p_elfh = (Elf64_Ehdr*)buf;
	*elf_h = *p_elfh;
	print_elf_info(p_elfh);
	
	assert(sizeof(Elf64_Ehdr) == elf_h->e_ehsize);
	assert(sizeof(Elf64_Shdr) == elf_h->e_shentsize);


	int elf_shnum = elf_h->e_shnum; //section header num
	Elf64_Shdr* elf_sh = new Elf64_Shdr[elf_shnum];
	uint32_t offset = elf_h->e_shoff; //section header offset
	Elf64_Shdr* p_sh =(Elf64_Shdr*)(buf + offset);

	char* shstrtab = load_str_table_section(buf,
					    p_sh + elf_h->e_shstrndx);


	char* strtab = NULL;
	Elf64_Sym* symtab = NULL;
	int nsymtab = 0;


	for(int i=0;i<elf_shnum;++i)
	{
		elf_sh[i] = p_sh[i];
		const char* sec_name = shstrtab + elf_sh[i].sh_name;
		printf("===seciton #%d:%s\n",i, sec_name);
		if(strcmp(sec_name,".strtab")==0)
		{
			strtab = load_str_table_section(buf,elf_sh + i);
		}
		else if(strcmp(sec_name,".symtab")==0)
		{
			symtab = load_symbol_table(buf,elf_sh+i,nsymtab);
		}
		print_elf_seciton_info(&elf_sh[i]);
	}

	printf("===Symbol table:\n");
	for(int i=0;i<nsymtab;++i)
	{
		char* name = strtab+symtab[i].st_name;
		printf("name:%s\n",name);

		PRINT_ATTR(st_name,symtab+i);
		PRINT_ATTR(st_info,symtab+i);
		PRINT_ATTR(st_other,symtab+i);
		PRINT_ATTR(st_shndx,symtab+i);
		PRINT_ATTR(st_value,symtab+i);
		PRINT_ATTR(st_size,symtab+i);
	}

	//section header table section
	// Elf64_Shdr* p_sh_str = elf_sh + elf_h->e_shstrndx;
	// char sec_strtable[p_sh_str->sh_size];
	// memcpy(sec_strtable,buf+p_sh_str->sh_offset,
	// 	   p_sh_str->sh_size);

	//load section header table section

}

void load_elf_bin(const char* path)
{
	uint8_t buf[MAX_FILE_BUF];
	int ret = load_file(path,buf);
	if(ret<=0)
	{
		return;
	}
	uint8_t* p_bytes = buf;
	uint8_t elf_ident[] = {0x7F,'E','L','F'};
	ret = memcmp(p_bytes,elf_ident,
					sizeof(elf_ident)/sizeof(uint8_t));
	if(ret!=0)
	{
		printf("No elf file found!\n");
		return;
	}
	bool is_bit64 = p_bytes[4]==2?true:false;
	if(is_bit64)
	{
		printf("load elf64!\n");
		Elf64_Ehdr elf64;
		load_elf64(buf,&elf64);
	}
	else
	{
		printf("load elf32\n");
	}

}



int main()
{
	load_elf_bin("../../test/add.o");
	return 0;
}