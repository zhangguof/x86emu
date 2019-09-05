import sys
from antlr4 import *
from CLexer import CLexer
from CParser import CParser
from CListener import CListener
from CVisitor import CVisitor

import pprint
import antlr4.xpath.XPath as XPath

#//ExternalDeclaration//

    
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
            ts = decls.getChild(i).typeSpecifier()
            if ts is not None:
                if ts.pointer() is not None:
                    is_pointer = True
            else:
                ts = decls.getChild(i).typeQualifier()
            if ts is None:
                continue
            ret.append(ts.getText())
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
        return args
            
    
    def visitExternalDeclaration(self, ctx):
        # print "visitExternalDeclaration:%s"%(ctx.getText())
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
        # print decla_func
        # self.print_func(decla_func)
        # print self.fmt_func(decla_func)
        self.funcs.append(decla_func)
        
        return
    
    
    def get_funcs(self):
        
        return self.funcs
        
        
        
            

 
def main(argv):
    input_stream = FileStream(argv[1])
    lexer = CLexer(input_stream)
    stream = CommonTokenStream(lexer)
    parser = CParser(stream)
  
    tree = parser.compilationUnit()
    # p = "//ExternalDeclaration"
    # xp = XPath.XPath(parser,"2.c")
    # print xp.findAll(tree,p,parser)
    # print xp
    
    v = MyVisitor()
    v.init()
    v.visit(tree)
    funcs = v.get_funcs()
    pprint.pprint(funcs)
    
    # mylistener = MyGrammarListener()
    # mylistener.init()
    # xp = xpath
    # walker = ParseTreeWalker()
    # walker.walk(mylistener, tree)
 
if __name__ == '__main__':
    main(sys.argv)