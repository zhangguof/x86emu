#-*- coding:utf-8 -*-

from jinja2 import Template

# char* strncpy(char*,const char*,size_t)
# [{'fmt': u'char* strncpy(char*,const char*,size_t)',
#   'name': u'strncpy',
#   'params': [{'is_pointer': True, 'name': u'char*', 'types': [u'char*']},
#              {'is_pointer': True,
#               'name': u'const char*',
#               'types': [u'const', u'char*']},
#              {'is_pointer': False, 'name': u'size_t', 'types': [u'size_t']}],
#   'ret_type': {'is_pointer': True, 'name': u'char*', 'types': [u'char*']}}]

c_f_tmp = '''
// file auto gen by antlr4 parser
#define NEED_CPU_REG_SHORTCUTS 1
#include "cpu/x86.h"
#include "param_names.h"
#include "elf.h"
#include <stdio.h>
#include "mmu.hpp"

#include <string.h>
#include <functional>
#include "engine.hpp"
#include "wrap_host_call.hpp"

'''

c_tmplate = '''
//src:{{src}}
//fmt:{{fmt}}
uint64_t wrap_{{name}}(uint64_t* args)
{
if(is_cpu_mode32())
{
    WIN32_ARGS w32_args = {(void*)args};
    typedef uint32_t size_t; //redef size_t;
    
    {%- set argn =  params|length -%}
    {% for param in params %}
    {% set idx = loop.index0 -%}
    typedef {{param.name}} _T_{{idx}};
    _T_{{idx}} arg_{{idx}};
    {% if param.is_pointer -%}
    arg_{{idx}} = (_T_{{idx}})getMemAddr((bx_phy_address)w32_args.next<WIN32_PTR>());
    {% else %}
    arg_{{idx}} = (_T_{{idx}})(w32_args.next<_T_{{idx}}>());
    {%- endif -%}
    {% endfor %}
    typedef {{ret_type.name}} _T_ret;

    {% if ret_type.name!='void' -%}
    auto ret = {{name}}({% for idx in range(0,argn) -%}
    {% if loop.last -%} arg_{{idx}} {% else %} arg_{{idx}}, {% endif %}{% endfor %});
    {% if ret_type.is_pointer -%} ret = (_T_ret)getGuestAddr(bx_phy_address(ret));{% endif %}
    {% else %}
    uint64_t ret = 0;
    {{name}}({% for idx in range(0,argn) -%}
    {% if loop.last -%} arg_{{idx}} {% else %} arg_{{idx}}, {% endif %}
    {%- endfor -%}
    );
    {% endif %}
    return (uint64_t)ret;
}
else
{
    {%- set argn =  params|length -%}
    {% for param in params %}
    {% set idx = loop.index0 -%}
    typedef {{param.name}} _T_{{idx}};
    auto arg_{{idx}} = (_T_{{idx}})(args[{{idx}}]);
    {% if param.is_pointer -%}
    arg_{{idx}} = (_T_{{idx}})getMemAddr((bx_phy_address)arg_{{idx}});
    {%- endif -%}
    {% endfor %}
    typedef {{ret_type.name}} _T_ret;

    {% if ret_type.name!='void' -%}
    auto ret = {{name}}({% for idx in range(0,argn) -%}
    {% if loop.last -%} arg_{{idx}} {% else %} arg_{{idx}}, {% endif %}{% endfor %});
    {% if ret_type.is_pointer -%} ret = (_T_ret)getGuestAddr(bx_phy_address(ret));{% endif %}
    {% else %}
    uint64_t ret = 0;
    {{name}}({% for idx in range(0,argn) -%}
    {% if loop.last -%} arg_{{idx}} {% else %} arg_{{idx}}, {% endif %}
    {%- endfor -%}
    );
    {% endif %}
    return (uint64_t)ret;
}
    
}

'''
c_header_tmp = '''
{% set cur_idx = start_idx %}
{% for func in funcs -%}
DEF_HOST_FUNC({{func.name}}, {{cur_idx + loop.index0}})
{% endfor %}
'''




def gen_func_c_code(func):
    template = Template(c_tmplate)
    s = template.render(**func)
    return s

def gen_c_header(funcs,filename,start_idx):
    template = Template(c_header_tmp)
    s = template.render(funcs=funcs,start_idx=start_idx)
    with open(filename,"w") as f:
        f.write(s)
    # return s
    
def gen_c_file(funcs,filename,start_idx):
    name = filename.split(".")[:-1]
    name = "".join(name)
    
    with open(filename,"w") as f:
        f.write(c_f_tmp)
        for func in funcs:
            s = gen_func_c_code(func)
            f.write(s)
    
    gen_c_header(funcs,name+".h",start_idx)
            



if __name__ == "__main__":
    print gen_func_c_code({'fmt': u'char* strncpy(char*,const char*,size_t)',
              'name': u'strncpy',
              'params': [{'is_pointer': True, 'name': u'char*', 'types': [u'char*']},
                         {'is_pointer': True,'name': u'const char*','types': [u'const', u'char*']},
                         {'is_pointer': False, 'name': u'size_t', 'types': [u'size_t']}],
              'ret_type': {'is_pointer': True, 'name': u'char*', 'types': [u'char*']}})