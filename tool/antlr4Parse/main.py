import sys
from antlr4 import *
from CLexer import CLexer
from CParser import CParser
from CListener import CListener
from CVisitor import CVisitor
from antlr4.tree.Tree import TerminalNodeImpl, ErrorNodeImpl, TerminalNode, INVALID_INTERVAL

from io import StringIO


import codecs
import unittest
from antlr4.InputStream import InputStream
import pprint
import antlr4.xpath.XPath as XPath
import gen_code
from antlr4.tree.Tree import TerminalNodeImpl, ErrorNodeImpl, TerminalNode, INVALID_INTERVAL



#//ExternalDeclaration//

class MyFileStream(InputStream):

    def __init__(self, fileName, encoding='ascii', errors='strict'):
        self.fileName = fileName
        # read binary to avoid line ending conversion
        with open(fileName, 'rb') as file:
            lines = file.readlines()
            new_lines = []
            for line in lines:
                line = line.strip()
                if line.startswith("__attribute__"):
                    print "skip:%s"%(line)
                    continue
                new_lines.append(line)
            
            bytes = "\n".join(new_lines)
            data = codecs.decode(bytes, encoding, errors)
            super(type(self), self).__init__(data)

    
class MyVisitor(CVisitor):
    def init(self):
        # parse function declar
        self.funcs = []  # {name,ret_type,args}
        
    def fmt_func(self,func):
        names = [p['name'] for p in func['params']]
        pn = ",".join(names)
        fmt = "%s %s(%s)"%(func['ret_type']['name'],func['name'],pn)
        # print fmt
        return fmt
    
    def get_src_text(self,ctx):
        if isinstance(ctx, TerminalNode):
            return ctx.getText()+" "
        
        if ctx.getChildCount() == 0:
            return u""

        with StringIO() as builder:
            for child in ctx.getChildren():
                builder.write(self.get_src_text(child))
            return builder.getvalue()
        
    
    def search(self,ctx,path):
        ret = ctx
        for p in path:
            ret = getattr(ret,p)()
            if ret is None:
                break
        return ret
    
    def parse_decl_specs(self,decls):
        assert(type(decls)==CParser.DeclarationSpecifiers2Context or
               type(decls)==CParser.DeclarationSpecifiersContext)
        n = decls.getChildCount()
        if n == 0:
            return None, None
        ret = []
        is_pointer = False
        for i in xrange(0, n):
            delc_spec = decls.getChild(i)
            tq = delc_spec.typeQualifier()
            if tq is not None:
                ret.append(tq.getText())
                continue
            
            ts = delc_spec.typeSpecifier()
            if ts is None:
                #like extern(storageClassSpecifier)
                continue
            p = ts.pointer()
            if p is None:
                ret.append(ts.getText())
            else:
                is_pointer = True
                ret.append(ts.typeSpecifier().getText() + p.getChild(0).getText())
   
            # ret.append(ts.getText())
        return ret, is_pointer
    
    def parse_param_decl(self,ctx):
        assert(type(ctx) == CParser.ParameterDeclarationContext)
        decls = None
        if ctx.declarationSpecifiers() is not None:
            decls = ctx.declarationSpecifiers()
        else:
            decls = ctx.declarationSpecifiers2()
        # print decls
        return self.parse_decl_specs(decls)
        
    def parse_params(self,ctx):
        # print ctx, type(ctx)
        if ctx is None:
            return []
        assert(type(ctx) == CParser.ParameterTypeListContext)
        plist = ctx.parameterList()
        ps = []
        s = [plist]

        top = s[-1]
        while top.parameterList() is not None:
            s.append(top.parameterList())
            top = s[-1]
        while len(s)!=0:
            top = s.pop()
            ps.append(top.parameterDeclaration())
        
        args = []
        for t in ps:
            ts,is_p = self.parse_param_decl(t)
            args.append({"types":ts,
                         "name":" ".join(ts),
                         "is_pointer":is_p})
        if ctx.getChildCount()>1:
            var_arg = "..."

            args.append({"types":[var_arg],
                         "name":var_arg,
                         "is_pointer":False
                         }
                        )
        
        return args
            
    
    def visitExternalDeclaration(self, ctx):
        # print "visitExternalDeclaration:%s"%(ctx.getText())
        # print self.get_src_text(ctx)
        xp = ["declaration","initDeclaratorList",
              "initDeclarator","declarator","directDeclarator"
              ]
        dd = self.search(ctx,xp)
        # print type(dd)
        if dd is None:
            return
        name  = self.search(dd,["directDeclarator","Identifier"])
        if name is None:
            return
        name = name.getText()
        tl = self.search(dd,["parameterTypeList"])
        # print dd.getChild(1).getText()=="(",ctx.getText()
        if tl is None and (dd.getChild(1).getText()!='('):
            return
        args = self.parse_params(tl)
        dss = self.search(ctx,['declaration','declarationSpecifiers'])
        ret_types,is_p= self.parse_decl_specs(dss)
        decla_func = {"name":name,
                      "ret_type":{"types":ret_types,
                                 "name":" ".join(ret_types),
                                 "is_pointer":is_p},
                      "params":args,
                      }
        decla_func['fmt'] = self.fmt_func(decla_func)
        decla_func['src'] = self.get_src_text(ctx)
        # print decla_func
        # self.print_func(decla_func)
        print self.fmt_func(decla_func)
        self.funcs.append(decla_func)
        
        return
    
    
    def get_funcs(self):
        
        return self.funcs
        
        
        
            

 
def main(argv):
    if len(argv) > 1:
        name = argv[1]
    else:
        name = "2.c"
    
    if len(argv) > 2:
        out_file = argv[2]
    else:
        out_file = "2_out.c"
    if len(argv) > 3:
        start_idx = int(argv[3])
    else:
        start_idx = 100
    
    
    # input_stream = FileStream(name)
    input_stream = MyFileStream(name)
    lexer = CLexer(input_stream)
    stream = CommonTokenStream(lexer)
    parser = CParser(stream)
  
    tree = parser.compilationUnit()

    
    v = MyVisitor()
    v.init()
    v.visit(tree)
    funcs = v.get_funcs()
    # pprint.pprint(funcs)
    
    gen_code.gen_c_file(funcs,out_file,start_idx)
 
 
if __name__ == '__main__':
    main(sys.argv)