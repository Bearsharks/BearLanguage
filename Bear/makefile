YACC=bison -dy
LEX=flex
BearInterpreter.exe:	lex.yy.c y.tab.c y.tab.h ast.h ast.cpp SymbolTable.h SymbolTable.cpp List.h List.cpp
	$(CXX) -o BearInterpreter lex.yy.c y.tab.c ast.cpp SymbolTable.cpp List.cpp
lex.yy.c:	bearLex.l
	$(LEX) bearLex.l
y.tab.c:	bearBison.y
	$(YACC) bearBison.y
y.tab.h:	bearBison.y
	$(YACC) bearBison.y
clean:
	$(RM) BearInterpreter.exe lex.yy.c y.tab.c y.tab.h bearBison.tab.c
