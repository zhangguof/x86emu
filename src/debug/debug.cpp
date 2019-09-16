//
//  debug.cpp
//  x86emu
//
//  Created by tony on 2019/9/16.
//  Copyright © 2019 tony. All rights reserved.
//

#include "debug.hpp"
#include "logger.hpp"
#include "stdlib.h"
#include "string.h"

#include <unordered_map>
std::unordered_map<uint64_t, std::shared_ptr<DEBUG_INFO>> global_debug_info;

#define printf(val,...)

struct srcfilesdata {
    char ** srcfiles;
    Dwarf_Signed srcfilescount;
    int srcfilesres;
};

#define TRUE 1
#define FALSE 0

static void read_cu_list(Dwarf_Debug dbg);
static void print_die_data(Dwarf_Debug dbg, Dwarf_Die print_me,
                           int level,
                           struct srcfilesdata *sf);
static void get_die_and_siblings(Dwarf_Debug dbg, Dwarf_Die in_die,
                                 int is_info, int in_level,
                                 struct srcfilesdata *sf);
static void resetsrcfiles(Dwarf_Debug dbg,struct srcfilesdata *sf);

/*  Use a generic call to open the file, due to issues with Windows */

static int namesoptionon = 0;
static int checkoptionon = 0;
static int dumpallnames = 0;
FILE *dumpallnamesfile = 0;
static const  char * dumpallnamespath = 0;
#if 0
DW_UT_compile                   0x01  /* DWARF5 */
DW_UT_type                      0x02  /* DWARF5 */
DW_UT_partial                   0x03  /* DWARF5 */
#endif

static int stdrun = TRUE;

static int unittype      = DW_UT_compile;
static Dwarf_Bool g_is_info = TRUE;

int cu_version_stamp = 0;
int cu_offset_size = 0;

/*  dienumberr is used to count DIEs.
 The point is to match fissionfordie. */
static int dienumber = 0;
static int fissionfordie = -1;
static int passnullerror = 0;
/*  These hash representations have to be converted to Dwarf_Sig8
 before use. */
static const  char * cuhash = 0;
static const  char * tuhash = 0;
static const  char * cufissionhash = 0;
static const  char * tufissionhash = 0;

/*  So we get clean reports from valgrind and other tools
 we clean up strdup strings.
 With a primitive technique as we need nothing fancy. */
#define DUPSTRARRAYSIZE 100
static const char *dupstrarray[DUPSTRARRAYSIZE];
static unsigned    dupstrused;

static void
cleanupstr(void)
{
    unsigned i = 0;
    for (i = 0; i < dupstrused; ++i) {
        free((char *)dupstrarray[i]);
        dupstrarray[i] = 0;
    }
    dupstrused = 0;
}

static unsigned  char_to_uns4bit(unsigned char c)
{
    unsigned v;
    if( c >= '0' && c <= '9') {
        v =  c - '0';
    }
    else if( c >= 'a' && c <= 'f') {
        v =  c - 'a' + 10;
    }
    else if( c >= 'A' && c <= 'F') {
        v =  c - 'A' + 10;
    } else {
        printf("Garbage hex char in %c 0x%x\n",c,c);
        exit(1);
    }
    return v;
}

static void
xfrm_to_sig8(const char *cuhash_in, Dwarf_Sig8 *hash_out)
{
    char localhash[16];
    unsigned hashin_len = strlen(cuhash_in);
    unsigned fixed_size = sizeof(localhash);
    unsigned init_byte = 0;
    unsigned i;
    
    memset(localhash,0,fixed_size);
    if (hashin_len > fixed_size) {
        printf("FAIL: argument hash too long, len %u val:\"%s\"\n",hashin_len,
               cuhash_in);
        exit(1);
    }
    if (hashin_len  < fixed_size) {
        unsigned add_zeros = fixed_size - hashin_len;
        for ( ; add_zeros > 0; add_zeros--) {
            localhash[init_byte] = '0';
            init_byte++;
        }
    }
    for (i = 0; i < hashin_len; ++i,++init_byte) {
        localhash[init_byte] = cuhash_in[i];
    }
    
    /*  So now local hash as a full 16 bytes of hex characters with
     any needed leading zeros.
     transform it to 8 byte hex signature */
    
    for (i = 0; i < sizeof(Dwarf_Sig8) ; ++i) {
        unsigned char hichar = localhash[2*i];
        unsigned char lochar = localhash[2*i+1];
        hash_out->signature[i] =
        (char_to_uns4bit(hichar) << 4)  |
        char_to_uns4bit(lochar);
    }
    printf("Hex key = 0x");
    for (i = 0; i < sizeof(Dwarf_Sig8) ; ++i) {
        unsigned char c = hash_out->signature[i];
        printf("%02x",c);
    }
    printf("\n");
}

static void
format_sig8_string(Dwarf_Sig8*data, char* str_buf,unsigned
                   buf_size)
{
    unsigned i = 0;
    char *cp = str_buf;
    if (buf_size <  19) {
        LOG_ERROR("FAIL: internal coding error in test.\n");
        exit(1);
    }
    strcpy(cp,"0x");
    cp += 2;
    buf_size -= 2;
    for (; i < sizeof(data->signature); ++i,cp+=2,buf_size--) {
        snprintf(cp, buf_size, "%02x",
                 (unsigned char)(data->signature[i]));
    }
    return;
}

static void
print_debug_fission_header(struct Dwarf_Debug_Fission_Per_CU_s *fsd)
{
    const char * fissionsec = ".debug_cu_index";
    unsigned i  = 0;
    char str_buf[30];
    
    if (!fsd || !fsd->pcu_type) {
        /* No fission data. */
        return;
    }
    printf("\n");
    if (!strcmp(fsd->pcu_type,"tu")) {
        fissionsec = ".debug_tu_index";
    }
    printf("  %-19s = %s\n","Fission section",fissionsec);
    printf("  %-19s = 0x%"  DW_PR_XZEROS DW_PR_DUx "\n","Fission index ",
           fsd->pcu_index);
    format_sig8_string(&fsd->pcu_hash,str_buf,sizeof(str_buf));
    printf("  %-19s = %s\n","Fission hash",str_buf);
    /* 0 is always unused. Skip it. */
    printf("  %-19s = %s\n","Fission entries","offset     size        DW_SECTn");
    for( i = 1; i < DW_FISSION_SECT_COUNT; ++i)  {
        const char *nstring = 0;
        Dwarf_Unsigned off = 0;
        Dwarf_Unsigned size = fsd->pcu_size[i];
        int res = 0;
        if (size == 0) {
            continue;
        }
        res = dwarf_get_SECT_name(i,&nstring);
        if (res != DW_DLV_OK) {
            nstring = "Unknown SECT";
        }
        off = fsd->pcu_offset[i];
        printf("  %-19s = 0x%08llx 0x%08llx %2u\n",
               nstring,
               off,size,i);
    }
}

/*  If there is no 'error' passed into a dwarf function
 and there is an error, and an error-handler like this
 is passed.  This example simply returns so we
 test how well that action works.  */
static void
simple_error_handler(Dwarf_Error error, Dwarf_Ptr errarg)
{
    Dwarf_Unsigned unused =  (Dwarf_Unsigned)(uintptr_t)errarg;
    LOG_ERROR("\nlibdwarf error detected: 0x%" DW_PR_DUx " %s\n",
           dwarf_errno(error),dwarf_errmsg(error));
    LOG_ERROR("libdwarf errarg. Not really used here %" DW_PR_DUu "\n",
           unused);
    return;
}

int test_debug()
{
    namesoptionon = 1;
    checkoptionon = 0;
    Dwarf_Debug dbg = 0;
    const char *filepath = "/Users/tony/workspace/github/x86emu/TiniyOs/crtdll/win32/libs/lua53_1.dll";
//    int use_init_fd = FALSE;
//    int my_init_fd = 0;
    bool stdrun = true;
    int res = DW_DLV_ERROR;
    Dwarf_Error error;
    Dwarf_Handler errhand = 0;
    Dwarf_Ptr errarg = 0;
    Dwarf_Sig8 hash8;
    Dwarf_Error *errp  = 0;
    int simpleerrhand = 0;
    int i = 0;
#define MACHO_PATH_LEN 2000
    char macho_real_path[MACHO_PATH_LEN];
    res = dwarf_init_path(filepath,
                          macho_real_path,
                          MACHO_PATH_LEN,
                          DW_DLC_READ,
                          DW_GROUPNUMBER_ANY,errhand,errarg,&dbg,
                          0,0,0,errp);
    if(res != DW_DLV_OK) {
        LOG_ERROR("Giving up, cannot do DWARF processing\n");
//        cleanupstr();
        exit(1);
    }
    if (stdrun) {
        read_cu_list(dbg);
    }
    res = dwarf_finish(dbg,errp);
    if(res != DW_DLV_OK) {
        LOG_ERROR("dwarf_finish failed!\n");
    }
    
//    printf("=====debug info:%d\n",global_debug_info.size());
//    for(auto it = global_debug_info.begin();it!=global_debug_info.end();++it)
//    {
//        printf("======%s:0x%0lx\n",it->second->name.c_str(),it->second->low_pc);
//    }
    
    return 0;
}

static void
read_cu_list(Dwarf_Debug dbg)
{
    Dwarf_Unsigned cu_header_length = 0;
    Dwarf_Unsigned abbrev_offset = 0;
    Dwarf_Half     address_size = 0;
    Dwarf_Half     version_stamp = 0;
    Dwarf_Half     offset_size = 0;
    Dwarf_Half     extension_size = 0;
    Dwarf_Sig8     signature;
    Dwarf_Unsigned typeoffset = 0;
    Dwarf_Unsigned next_cu_header = 0;
    Dwarf_Half     header_cu_type = unittype;
    Dwarf_Bool     is_info = g_is_info;
    Dwarf_Error error;
    int cu_number = 0;
    Dwarf_Error *errp  = 0;
    
    
    for(;;++cu_number) {
        Dwarf_Die no_die = 0;
        Dwarf_Die cu_die = 0;
        int res = DW_DLV_ERROR;
        struct srcfilesdata sf;
        sf.srcfilesres = DW_DLV_ERROR;
        sf.srcfiles = 0;
        sf.srcfilescount = 0;
        memset(&signature,0, sizeof(signature));
        
        if(passnullerror) {
            errp = 0;
        } else {
            errp = &error;
        }
        res = dwarf_next_cu_header_d(dbg,is_info,&cu_header_length,
                                     &version_stamp, &abbrev_offset,
                                     &address_size, &offset_size,
                                     &extension_size,&signature,
                                     &typeoffset, &next_cu_header,
                                     &header_cu_type,errp);
        if(res == DW_DLV_ERROR) {
            const char *em = errp?dwarf_errmsg(error):"An error next cu her";
            LOG_ERROR("Error in dwarf_next_cu_header: %s\n",em);
            exit(1);
        }
        if(res == DW_DLV_NO_ENTRY) {
            /* Done. */
            return;
        }
        cu_version_stamp = version_stamp;
        cu_offset_size   = offset_size;
        /* The CU will have a single sibling, a cu_die. */
        res = dwarf_siblingof_b(dbg,no_die,is_info,
                                &cu_die,errp);
        if(res == DW_DLV_ERROR) {
            const char *em = errp?dwarf_errmsg(error):"An error";
            LOG_ERROR("Error in dwarf_siblingof_b on CU die: %s\n",em);
            exit(1);
        }
        if(res == DW_DLV_NO_ENTRY) {
            /* Impossible case. */
            printf("no entry! in dwarf_siblingof on CU die \n");
            exit(1);
        }
        get_die_and_siblings(dbg,cu_die,is_info,0,&sf);
        dwarf_dealloc(dbg,cu_die,DW_DLA_DIE);
        resetsrcfiles(dbg,&sf);
    }
}

static void
get_die_and_siblings(Dwarf_Debug dbg, Dwarf_Die in_die,
                     int is_info,int in_level,
                     struct srcfilesdata *sf)
{
    int res = DW_DLV_ERROR;
    Dwarf_Die cur_die=in_die;
    Dwarf_Die child = 0;
    Dwarf_Error error = 0;
    Dwarf_Error *errp = 0;
    
    if(passnullerror) {
        errp = 0;
    } else {
        errp = &error;
    }
    print_die_data(dbg,in_die,in_level,sf);
    
    for(;;) {
        Dwarf_Die sib_die = 0;
        res = dwarf_child(cur_die,&child,errp);
        if(res == DW_DLV_ERROR) {
            LOG_ERROR("Error in dwarf_child , level %d \n",in_level);
            exit(1);
        }
        if(res == DW_DLV_OK) {
            get_die_and_siblings(dbg,child,is_info,
                                 in_level+1,sf);
            /* No longer need 'child' die. */
            dwarf_dealloc(dbg,child,DW_DLA_DIE);
            child = 0;
        }
        /* res == DW_DLV_NO_ENTRY or DW_DLV_OK */
        res = dwarf_siblingof_b(dbg,cur_die,is_info,&sib_die,errp);
        if(res == DW_DLV_ERROR) {
            const char *em = errp?dwarf_errmsg(error):"Error siblingof_b";
            LOG_ERROR("Error in dwarf_siblingof_b , level %d :%s \n",
                   in_level,em);
            exit(1);
        }
        if(res == DW_DLV_NO_ENTRY) {
            /* Done at this level. */
            break;
        }
        /* res == DW_DLV_OK */
        if(cur_die != in_die) {
            dwarf_dealloc(dbg,cur_die,DW_DLA_DIE);
            cur_die = 0;
        }
        cur_die = sib_die;
        print_die_data(dbg,cur_die,in_level,sf);
    }
    return;
}
static void
get_addr(Dwarf_Attribute attr,Dwarf_Addr *val)
{
    Dwarf_Error error = 0;
    int res;
    Dwarf_Addr uval = 0;
    Dwarf_Error *errp  = 0;
    
    if(passnullerror) {
        errp = 0;
    } else {
        errp = &error;
    }
    
    res = dwarf_formaddr(attr,&uval,errp);
    if(res == DW_DLV_OK) {
        *val = uval;
        return;
    }
    return;
}
static void
get_number(Dwarf_Attribute attr,Dwarf_Unsigned *val)
{
    Dwarf_Error error = 0;
    int res;
    Dwarf_Signed sval = 0;
    Dwarf_Unsigned uval = 0;
    Dwarf_Error *errp  = 0;
    
    if(passnullerror) {
        errp = 0;
    } else {
        errp = &error;
    }
    res = dwarf_formudata(attr,&uval,errp);
    if(res == DW_DLV_OK) {
        *val = uval;
        return;
    }
    res = dwarf_formsdata(attr,&sval,errp);
    if(res == DW_DLV_OK) {
        *val = sval;
        return;
    }
    return;
}

static void
get_flag(Dwarf_Attribute attr,Dwarf_Bool *val)
{
    Dwarf_Error error = 0;
    int res;
//    Dwarf_Signed sval = 0;
//    Dwarf_Unsigned uval = 0;
    Dwarf_Bool sval;
    Dwarf_Error *errp  = 0;
    
    if(passnullerror) {
        errp = 0;
    } else {
        errp = &error;
    }
//    res = dwarf_formudata(attr,&uval,errp);
    res = dwarf_formflag(attr, &sval, errp);
    if(res == DW_DLV_OK) {
        *val = sval;
        return;
    }

    return;
}

static void
print_subprog(Dwarf_Debug dbg,Dwarf_Die die,
              int level,
              struct srcfilesdata *sf,
              const char *name)
{
    int res;
    Dwarf_Error error = 0;
    Dwarf_Attribute *attrbuf = 0;
    Dwarf_Addr lowpc = 0;
    Dwarf_Addr highpc = 0;
    Dwarf_Signed attrcount = 0;
    Dwarf_Signed i;
    Dwarf_Unsigned filenum = 0;
    Dwarf_Unsigned linenum = 0;
//    Dwarf_Unsigned external = 0;
    Dwarf_Bool external = 0;
    char *filename = 0;
    Dwarf_Error *errp = 0;
    
    if(passnullerror) {
        errp = 0;
    } else {
        errp = &error;
    }
    res = dwarf_attrlist(die,&attrbuf,&attrcount,errp);
    if(res != DW_DLV_OK) {
        return;
    }
//    res = dwarf_whatattr(attr,&attrnum,&error);
//    if(res != DW_DLV_OK) {
//        printf("Unable to get attr number");
//        exit(1);
//    }
    
    
    for(i = 0; i < attrcount ; ++i) {
        Dwarf_Half aform;
//        Dwarf_Half finalform;
        
//        res = dwarf_whatform(attrbuf[i],&finalform,&error);
//        if(res != DW_DLV_OK) {
//            printf("Unable to get attr form");
//            exit(1);
//        }
        
        res = dwarf_whatattr(attrbuf[i],&aform,errp);
        if(res == DW_DLV_OK) {
//            auto cl = dwarf_get_form_class(cu_version_stamp,
//                                      aform,cu_offset_size,finalform);
            if(aform == DW_AT_decl_file) {
                Dwarf_Signed filenum_s = 0;
                
                get_number(attrbuf[i],&filenum);
                filenum_s = filenum;
                /*  Would be good to evaluate filenum_s
                 sanity here, ensuring filenum_s-1 is sensible. */
                if((filenum > 0) && (sf->srcfilescount > (filenum_s-1))) {
                    filename = sf->srcfiles[filenum_s-1];
                }
            }
            if(aform == DW_AT_decl_line) {
                get_number(attrbuf[i],&linenum);
            }
            if(aform == DW_AT_low_pc) {
                get_addr(attrbuf[i],&lowpc);
            }
            if(aform == DW_AT_high_pc) {
                /*  This will FAIL with DWARF4 highpc form
                 of 'class constant'.  */
                get_addr(attrbuf[i],&highpc);
            }
//            if(aform ==  DW_AT_external)
//            {
//                get_flag(attrbuf[i],&external);
//            }
        }
        dwarf_dealloc(dbg,attrbuf[i],DW_DLA_ATTR);
    }
    /*  Here let's test some alternative interfaces for high and low pc.
     We only do both dwarf_highpc and dwarf_highpcb_b as
     an error check. Do not do both yourself. */
    if(checkoptionon){
        int hres = 0;
        int hresb = 0;
        int lres = 0;
        Dwarf_Addr althipc = 0;
        Dwarf_Addr hipcoffset = 0;
        Dwarf_Addr althipcb = 0;
        Dwarf_Addr altlopc = 0;
        Dwarf_Half highform = 0;
        enum Dwarf_Form_Class highclass = DW_FORM_CLASS_UNKNOWN;
        
        /*  Reusing errp before checking for err here is
         bogus. FIXME. */
        /*  Should work for DWARF 2/3 DW_AT_high_pc, and
         all high_pc where the FORM is DW_FORM_addr
         Avoid using this interface as of 2013. */
        hres  = dwarf_highpc(die,&althipc,errp);
        
        /* Should work for all DWARF DW_AT_high_pc.  */
        hresb = dwarf_highpc_b(die,&althipcb,&highform,&highclass,errp);
        
        lres = dwarf_lowpc(die,&altlopc,errp);
        printf("high_pc checking %s ",name);
        
        if (hres == DW_DLV_OK) {
            /* present, FORM addr */
            printf("highpc   0x%" DW_PR_XZEROS DW_PR_DUx " ",
                   althipc);
        } else if (hres == DW_DLV_ERROR) {
            LOG_ERROR("dwarf_highpc() error not class address ");
        } else {
            /* absent */
        }
        if(hresb == DW_DLV_OK) {
            /* present, FORM addr or const. */
            if(highform == DW_FORM_addr) {
                printf("highpcb  0x%" DW_PR_XZEROS DW_PR_DUx " ",
                       althipcb);
            } else {
                if(lres == DW_DLV_OK) {
                    hipcoffset = althipcb;
                    althipcb = altlopc + hipcoffset;
                    printf("highpcb  0x%" DW_PR_XZEROS DW_PR_DUx " "
                           "highoff  0x%" DW_PR_XZEROS DW_PR_DUx " ",
                           althipcb,hipcoffset);
                } else {
                    printf("highoff  0x%" DW_PR_XZEROS DW_PR_DUx " ",
                           althipcb);
                }
            }
        } else if (hresb == DW_DLV_ERROR) {
            LOG_ERROR("dwarf_highpc_b() error!");
        } else {
            /* absent */
        }
        
        /* Should work for all DWARF DW_AT_low_pc */
        if (lres == DW_DLV_OK) {
            /* present, FORM addr. */
            printf("lowpc    0x%" DW_PR_XZEROS DW_PR_DUx " ",
                   altlopc);
        } else if (lres == DW_DLV_ERROR) {
            LOG_ERROR("dwarf_lowpc() error!");
        } else {
            /* absent. */
        }
        printf("\n");
        
        
        
    }
    if(namesoptionon && (filenum || linenum)) {
        printf("<%3d> file: %" DW_PR_DUu " %s  line %"
               DW_PR_DUu "\n",level,filenum,filename?filename:"",linenum);
    }
    if(namesoptionon && lowpc) {
        printf("<%3d> low_pc : 0x%" DW_PR_DUx  "\n",
               level, (Dwarf_Unsigned)lowpc);
    }
    if(namesoptionon && highpc) {
        printf("<%3d> high_pc: 0x%" DW_PR_DUx  "\n",
               level, (Dwarf_Unsigned)highpc);
    }
    if(lowpc)
    {
        global_debug_info[lowpc] = std::make_shared<DEBUG_INFO>(name,lowpc,highpc);
    }
    dwarf_dealloc(dbg,attrbuf,DW_DLA_LIST);
    
}

static void
print_comp_dir(Dwarf_Debug dbg,Dwarf_Die die,
               int level, struct srcfilesdata *sf)
{
    int res;
    Dwarf_Error error = 0;
    Dwarf_Attribute *attrbuf = 0;
    Dwarf_Signed attrcount = 0;
    Dwarf_Signed i;
    Dwarf_Error *errp = 0;
    
    if(passnullerror) {
        errp = 0;
    } else {
        errp = &error;
    }
    
    res = dwarf_attrlist(die,&attrbuf,&attrcount,errp);
    if(res != DW_DLV_OK) {
        return;
    }
    sf->srcfilesres = dwarf_srcfiles(die,&sf->srcfiles,&sf->srcfilescount,
                                     &error);
    for(i = 0; i < attrcount ; ++i) {
        Dwarf_Half aform;
        res = dwarf_whatattr(attrbuf[i],&aform,errp);
        if(res == DW_DLV_OK) {
            if(aform == DW_AT_comp_dir) {
                char *name = 0;
                res = dwarf_formstring(attrbuf[i],&name,errp);
                if(res == DW_DLV_OK) {
                    printf(    "<%3d> compilation directory : \"%s\"\n",
                           level,name);
                }
            }
            if(aform == DW_AT_stmt_list) {
                /* Offset of stmt list for this CU in .debug_line */
            }
        }
        dwarf_dealloc(dbg,attrbuf[i],DW_DLA_ATTR);
    }
    dwarf_dealloc(dbg,attrbuf,DW_DLA_LIST);
}

static void
resetsrcfiles(Dwarf_Debug dbg,struct srcfilesdata *sf)
{
    Dwarf_Signed sri = 0;
    for (sri = 0; sri < sf->srcfilescount; ++sri) {
        dwarf_dealloc(dbg, sf->srcfiles[sri], DW_DLA_STRING);
    }
    dwarf_dealloc(dbg, sf->srcfiles, DW_DLA_LIST);
    sf->srcfilesres = DW_DLV_ERROR;
    sf->srcfiles = 0;
    sf->srcfilescount = 0;
}

static void
print_single_string(Dwarf_Debug dbg, Dwarf_Die die,Dwarf_Half attrnum)
{
    int res = 0;
    Dwarf_Error error = 0;
    char * stringval = 0;
    
    res = dwarf_die_text(die,attrnum, &stringval,&error);
    if (res == DW_DLV_OK) {
        fprintf(dumpallnamesfile,"%s\n",stringval);
        dwarf_dealloc(dbg,stringval, DW_DLA_STRING);
    }
    return;
}


static void
print_name_strings_attr(Dwarf_Debug dbg, Dwarf_Die die,
                        Dwarf_Attribute attr)
{
    int res = 0;
    Dwarf_Half attrnum = 0;
    Dwarf_Half finalform = 0;
    enum Dwarf_Form_Class cl = DW_FORM_CLASS_UNKNOWN;
    Dwarf_Error error = 0;
    
    res = dwarf_whatattr(attr,&attrnum,&error);
    if(res != DW_DLV_OK) {
        printf("Unable to get attr number");
        exit(1);
    }
    
    res = dwarf_whatform(attr,&finalform,&error);
    if(res != DW_DLV_OK) {
        printf("Unable to get attr form");
        exit(1);
    }
    
    cl = dwarf_get_form_class(cu_version_stamp,
                              attrnum,cu_offset_size,finalform);
    
    if (cl != DW_FORM_CLASS_STRING) {
        return;
    }
    print_single_string(dbg,die,attrnum);
}

static void
printnamestrings(Dwarf_Debug dbg, Dwarf_Die die)
{
    Dwarf_Error error =0;
    Dwarf_Attribute *atlist = 0;
    Dwarf_Signed atcount = 0;
    Dwarf_Signed i = 0;
    int res = 0;
    
    res = dwarf_attrlist(die,&atlist, &atcount,&error);
    if (res != DW_DLV_OK) {
        return;
    }
    for (i = 0; i < atcount; ++i) {
        Dwarf_Attribute attr = atlist[i];
        /*  Use an empty attr to get a placeholder on
         the attr list for this IRDie. */
        print_name_strings_attr(dbg,die,attr);
    }
    dwarf_dealloc(dbg,atlist, DW_DLA_LIST);
    
}



static void
print_die_data_i(Dwarf_Debug dbg, Dwarf_Die print_me,
                 int level,
                 struct srcfilesdata *sf)
{
    char *name = 0;
    Dwarf_Error error = 0;
    Dwarf_Half tag = 0;
    const char *tagname = 0;
    int localname = 0;
    int res = 0;
    Dwarf_Error *errp = 0;
    Dwarf_Attribute attr = 0;
    Dwarf_Half formnum = 0;
    const char *formname = 0;
    
    if (passnullerror) {
        errp = 0;
    } else {
        errp = &error;
    }
    res = dwarf_diename(print_me,&name,errp);
    if(res == DW_DLV_ERROR) {
        LOG_ERROR("Error in dwarf_diename , level %d \n",level);
        exit(1);
    }
    if(res == DW_DLV_NO_ENTRY) {
        name = "<no DW_AT_name attr>";
        localname = 1;
    }
    res = dwarf_tag(print_me,&tag,errp);
    if(res != DW_DLV_OK) {
        printf("Error in dwarf_tag , level %d \n",level);
        exit(1);
    }
    res = dwarf_get_TAG_name(tag,&tagname);
    if(res != DW_DLV_OK) {
        printf("Error in dwarf_get_TAG_name , level %d \n",level);
        exit(1);
    }
    if (dumpallnames) {
        printnamestrings(dbg,print_me);
    }
    res = dwarf_attr(print_me,DW_AT_name,&attr, errp);
    if (res != DW_DLV_OK) {
        /* do nothing */
    } else {
        res = dwarf_whatform(attr,&formnum,errp);
        if (res != DW_DLV_OK) {
            printf("Error in dwarf_whatform , level %d \n",level);
            exit(1);
        }
        formname = "form-name-unavailable";
        res = dwarf_get_FORM_name(formnum,&formname);
        if (res != DW_DLV_OK) {
            formname = "UNKNoWn FORM!";
        }
        dwarf_dealloc(dbg,attr,DW_DLA_ATTR);
    }
    
    if(namesoptionon ||checkoptionon) {
        if( tag == DW_TAG_subprogram) {
            if(namesoptionon) {
                printf(    "<%3d> subprogram            : \"%s\"\n",level,name);
            }
            print_subprog(dbg,print_me,level,sf,name);
        }
        if( (namesoptionon) && (tag == DW_TAG_compile_unit ||
                                tag == DW_TAG_partial_unit ||
                                tag == DW_TAG_type_unit)) {
            
            resetsrcfiles(dbg,sf);
            printf(    "<%3d> source file           : \"%s\"\n",level,name);
            print_comp_dir(dbg,print_me,level,sf);
        }
    } else {
        printf("<%d> tag: %d %s  name: \"%s\"",level,tag,tagname,name);
        if (formname) {
            printf(" FORM 0x%x \"%s\"",formnum, formname);
        }
        printf("\n");
    }
    if(!localname) {
        dwarf_dealloc(dbg,name,DW_DLA_STRING);
    }
}

static void
print_die_data(Dwarf_Debug dbg, Dwarf_Die print_me,
               int level,
               struct srcfilesdata *sf)
{
    
    if (fissionfordie != -1) {
        Dwarf_Debug_Fission_Per_CU percu;
        memset(&percu,0,sizeof(percu));
        if (fissionfordie == dienumber) {
            int res = 0;
            Dwarf_Error error = 0;
            Dwarf_Error *errp = 0;
            
            if (passnullerror) {
                errp = 0;
            } else {
                errp = &error;
            }
            res = dwarf_get_debugfission_for_die(print_me,
                                                 &percu,errp);
            if(res == DW_DLV_ERROR) {
                LOG_ERROR("FAIL: Error in dwarf_diename  on fissionfordie %d\n",
                       fissionfordie);
                exit(1);
            }
            if(res == DW_DLV_NO_ENTRY) {
                printf("FAIL: no-entry in dwarf_diename  on fissionfordie %d\n",
                       fissionfordie);
                exit(1);
            }
            print_die_data_i(dbg,print_me,level,sf);
            print_debug_fission_header(&percu);
            exit(0);
        }
        dienumber++;
        return;
    }
    print_die_data_i(dbg,print_me,level,sf);
    dienumber++;
}
