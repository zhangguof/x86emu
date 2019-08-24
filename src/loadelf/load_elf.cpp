
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <algorithm>

#include "elf.h"
#include "elf-ext.hpp"
#include "buffer.hpp"


const int MAX_FILE_BUF = 2*1024*1024; //2M;

int load_file(const char* path, BufPtr &pdata)
{
	int fd;
	fd = open(path,O_RDONLY);
	assert(fd!=0);

	struct stat stat_buf;
	int ret = fstat(fd,&stat_buf);
	if(ret)
	{
		close(fd);
		printf("Error! open file!:%s\n",path);
		return 0;
	}
	uint32_t size = stat_buf.st_size;
    pdata = new_buffer(size);
    
	uint8_t* ptr_buf = pdata->get_data();
    
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
	return ptr_buf - pdata->get_data();
}

void load_elf32(uint8_t* buf,Elf32_Ehdr* elf_h)
{

}

#define PRINT_ATTR(name,ptr) \
printf(#name":0x%0llx\n",(ptr)->name);

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

Elf64_Phdr* load_elf64ph(uint8_t* buf,Elf64_Ehdr* elf_h,int &num)
{
	int n = elf_h->e_phnum;
	num = n;
	if(n==0) return NULL;
	assert(sizeof(Elf64_Phdr) == elf_h->e_phentsize);

	Elf64_Phdr* elf_phs = new Elf64_Phdr[n];
	Elf64_Phdr* p_ph = (Elf64_Phdr*)(buf + elf_h->e_phoff);

	for(int i=0;i<n;++i)
	{
		elf_phs[i] = p_ph[i];
	}
	return elf_phs;
}

void print_elf_program_header(Elf64_Phdr* ph)
{
	PRINT_ATTR(p_type,ph);
	PRINT_ATTR(p_flags,ph);
	PRINT_ATTR(p_offset,ph);   /* Segment file offset */
	PRINT_ATTR(p_vaddr,ph);    /* Segment virtual address */
	PRINT_ATTR(p_paddr,ph);    /* Segment physical address */
	PRINT_ATTR(p_filesz,ph);   /* Segment size in file */
	PRINT_ATTR(p_memsz,ph);    /* Segment size in memory */
	PRINT_ATTR(p_align,ph);    /* Segment alignment, file & memory */
}


#ifdef ELFTEST

void print_sym_info(Elf64_Sym* sym)
{
    PRINT_ATTR(st_name,sym);        /* Symbol name, index in string tbl */
    PRINT_ATTR(st_info,sym);    /* Type and binding attributes */
    PRINT_ATTR(st_other,sym);    /* No defined meaning, 0 */
    PRINT_ATTR(st_shndx,sym);        /* Associated section index */
    PRINT_ATTR(st_value,sym);        /* Value of the symbol */
    PRINT_ATTR(st_size,sym);        /* Associated symbol size */
    
}

const char* get_tag_name(uint32_t tag)
{
	size_t n = sizeof(dtag)/sizeof(lookup);
	lookup t = {tag,"0"};
	auto ite = std::lower_bound(dtag,dtag+n,t,
		[](const lookup& a,const lookup& b){
			return a.n < b.n;
	});
	return ite->s;
}
const char* get_rtype_name(uint32_t tag)
{
	size_t n = sizeof(rtype)/sizeof(lookup);
	lookup t = {tag,"0"};
	auto ite = std::lower_bound(rtype,rtype+n,t,
		[](const lookup& a,const lookup& b){
			return a.n < b.n;
	});
	return ite->s;
}
#endif

void load_elf64_dyn(Elf64_Ehdr* e)
{
	uint8_t* buf = (uint8_t*) e;
	int num = 0;
	Elf64_Phdr* ph=load_elf64ph((uint8_t*)e,e,num);
	Elf64_Phdr* ph_dyn = NULL; //find section .dynamic
	int dn = 0;
	Elf64_Dyn* pd = NULL; //
	for(int i=0;i<num;++i)
	{
		printf("====%d:type:0x%0x\n",i,ph[i].p_type);
		if(ph[i].p_type == PT_DYNAMIC)
		{
			ph_dyn = ph+i;
			dn = ph_dyn->p_memsz / (sizeof(Elf64_Dyn));
			pd = (Elf64_Dyn*)(buf + ph_dyn->p_offset);
			break;
		}
	}
	printf("==load dyn head:%d\n",dn);
	Elf64_Sym* dy_sym = NULL;
	char* dy_strtab = NULL;
	Elf64_Rela* rela = NULL; // section .rela.dyn, rel for .got
	Elf64_Rela* plt_rela = NULL; //section .rela.plt rel for .plt
	int rnum = 0; //rel num;
	int plt_rnum = 0; //plt rel num;
	for(int i=0;i<dn;++i)
	{
		Elf64_Dyn* p = pd + i;
//        printf("==dyn:#%d,tag:%s(0x%0x),d_val:0x%0x\n",i,
//                get_tag_name(p->d_tag),p->d_tag,p->d_un.d_val);
		switch(p->d_tag)
		{
			case DT_STRTAB:
				dy_strtab = (char*)(buf + p->d_un.d_ptr);
				break;
			case DT_SYMTAB:
				dy_sym = (Elf64_Sym*)(buf + p->d_un.d_ptr);
				break;
			case DT_RELA:
				rela = (Elf64_Rela*)(buf + p->d_un.d_ptr);
				break;
			case DT_RELASZ:
				rnum = p->d_un.d_val / (sizeof(Elf64_Rela));
				break;
			case DT_PLTRELSZ:
				plt_rnum = p->d_un.d_val / (sizeof(Elf64_Rela));
				break;
			case DT_JMPREL:
				plt_rela = (Elf64_Rela*)(buf + p->d_un.d_ptr);
				break;
			case DT_NULL:
			default:
				break;
		}
		if(p->d_tag == DT_NULL) break;
	}
//    printf("===rel sym num:%d,sizeof:%d\n",rnum,sizeof(Elf64_Rela));
	for(int i=0;i<rnum;++i)
	{
		Elf64_Rela* r = rela + i;
		auto sym = dy_sym[ELF64_R_SYM(r->r_info)];
		uint32_t r_type = ELF64_R_TYPE(r->r_info);
//        printf("=====rel:#%d,addr:r_offset:%0x,r_type:%s(0x%0x),name:%s,r_addend:0x%0x\n",
//                i,r->r_offset,get_rtype_name(r_type),r_type,
//                dy_strtab+sym.st_name,r->r_addend);
//        print_sym_info(&sym);

	}
	for(int i=0;i<plt_rnum;++i)
	{
		Elf64_Rela* r = plt_rela + i;
		auto sym = dy_sym[ELF64_R_SYM(r->r_info)];
		uint32_t r_type = ELF64_R_TYPE(r->r_info);
//        printf("=====pot:#%d,addr:r_offset:%0x,r_type:%s(0x%0x),name:%s,r_addend:0x%0x\n",
//                i,r->r_offset,get_rtype_name(r_type),r_type,
//                dy_strtab+sym.st_name,r->r_addend);
//        print_sym_info(&sym);
	}


}


void load_elf64(uint8_t* buf,Elf64_Ehdr* elf_h)
{
	Elf64_Ehdr* p_elfh = (Elf64_Ehdr*)buf;
	*elf_h = *p_elfh;

	
	
	assert(sizeof(Elf64_Ehdr) == elf_h->e_ehsize);
	assert(sizeof(Elf64_Shdr) == elf_h->e_shentsize);


	int elf_shnum = elf_h->e_shnum; //section header num
	Elf64_Shdr* elf_shs = new Elf64_Shdr[elf_shnum];
	uint32_t offset = elf_h->e_shoff; //section header offset
	Elf64_Shdr* p_sh =(Elf64_Shdr*)(buf + offset);

	char* shstrtab = load_str_table_section(buf,
					    p_sh + elf_h->e_shstrndx);


	char* strtab = NULL;
	Elf64_Sym* symtab = NULL;
	int nsymtab = 0;


	for(int i=0;i<elf_shnum;++i)
	{
		elf_shs[i] = p_sh[i];
		const char* sec_name = shstrtab + elf_shs[i].sh_name;
		if(strcmp(sec_name,".strtab")==0)
		{
			strtab = load_str_table_section(buf,elf_shs + i);
		}
		else if(strcmp(sec_name,".symtab")==0)
		{
			symtab = load_symbol_table(buf,elf_shs+i,nsymtab);
		}
	}

	//load program header
//    int elf_phnum = 0;
//    Elf64_Phdr* elf_phs = load_elf64ph(buf,elf_h,elf_phnum);

#ifdef ELFTEST
	print_elf_info(p_elfh);

	for(int i=0;i<elf_shnum;++i)
	{
		const char* sec_name = shstrtab + elf_shs[i].sh_name;
		printf("===seciton #%d:%s\n",i, sec_name);
		print_elf_seciton_info(&elf_shs[i]);
	}

	for(int i=0;i<elf_phnum;++i)
	{
		printf("===program header:#%d\n",i);
		print_elf_program_header(elf_phs+i);
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
	load_elf64_dyn((Elf64_Ehdr*)buf);
#endif

}



void load_elf_bin(const char* path,std::shared_ptr<Buffer>& p_data)
{
//    uint8_t buf[MAX_FILE_BUF];
	int ret = load_file(path,p_data);
	if(ret<=0)
	{
		return;
	}
    assert(p_data != nullptr && p_data->size == ret);
    
	uint32_t filesz = ret;
    uint8_t* buf = p_data->get_data();


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
//    *pdata = new uint8_t[filesz];
//    memcpy(*pdata, buf,filesz);
//    size = filesz;
    
	if(is_bit64)
	{
		printf("loading elf64....\n");
		Elf64_Ehdr elf64;
		load_elf64(buf,&elf64);
	}
	else
	{
		printf("loading elf32....\n");
	}
}


