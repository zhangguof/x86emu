export CLASSPATH=".:/Users/tony/workspace/github/x86emu/test/antlr4/antlr-4.7.1-complete.jar:$CLASSPATH"

alias antlr4='java -Xmx500M -cp "Users/tony/workspace/github/x86emu/test/antlr4/antlr-4.7.1-complete.jar:$CLASSPATH" org.antlr.v4.Tool'

alias grun='java -Xmx500M -cp "Users/tony/workspace/github/x86emu/test/antlr4/antlr-4.7.1-complete.jar:$CLASSPATH" org.antlr.v4.gui.TestRig'

antlr4 -Dlanguage=Python2 -visitor C.g4

grun C compilationUnit -gui 2.c