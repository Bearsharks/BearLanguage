%{
    #include <iostream>
    #include <stdlib.h>
    #include "ast.h"
    int yyerror(const char *msg), yylex();
    using namespace std;

%}
%union {
    AST* pval;
    Expression* Expr;
    Begin* begin;
    Statement* state;
    InitList* list;
    CompoundStatement* comp;
    FuncCall* func;
    ExprKind exprkind;
    Declaration* decl;
    FunctionDefinition* fundef;
    int ival;
    char* sval;
    Type type;
}
%type  <pval>  Root
%type  <begin> Begin
%type  <decl>  declaration init_declarator declarator
%type  <fundef>  parameter_declaration function_definition
%type  <comp>  statement_list
%type  <func> argument_expression_list
%type  <state>  statement
%type  <list> initializer_list initializer
%type  <state>  declaration_statement return_statement
%type  <state>  compound_statement expression_statement
%type  <state>  selection_statement iteration_statement
%type  <state>  out_statement
%type  <type>  type_specifier
%type  <Expr>  assignment_expression
%type  <Expr>  relational_expression equality_expression
%type  <Expr>  primary_expression postfix_expression
%type  <Expr>  unary_expression
%type  <Expr>  multiplicative_expression additive_expression
%type  <exprkind>  unary_operator
%token IF ELSE WHILE RETURN OUT
%token LE_OP GE_OP EQ_OP NE_OP
%token LIST INT VOID
%token <sval>  ID
%token <ival>  CONSTANT
%start Root
%%
Root
  : Begin { $$ = new Root($1); $$->Run(); delete $$;}
  ;

Begin
  : Begin declaration_statement { $1->Append($2); $$ = $1;}
  | Begin function_definition { $1->Append($2); $$ = $1;}
  | declaration_statement { $$ = new Begin($1);}
  | function_definition { $$ = new Begin($1);}
  ;

primary_expression
	: ID { $$ = new PrimaryExpression($1);}
	| CONSTANT { $$ = new PrimaryExpression($1);}
	| '(' assignment_expression ')' { $$ = $2;}
	;

postfix_expression
	: primary_expression { $$ = $1;}
	| postfix_expression '[' assignment_expression ']' { $$ = new BracketExpression($1, $3);}
	| ID '(' ')' { $$ = new FuncCall($1);}
	| ID '(' argument_expression_list ')' { $3->setId($1); $$ = $3;}
	;

unary_operator
	: '-' { $$ = ExprKind::UMINUS;}
	| '!' { $$ = ExprKind::NOT;}
	;

unary_expression
	: postfix_expression { $$ = $1;}
	| unary_operator unary_expression { $$ = new UnOperExpression($1, $2);}
	;

multiplicative_expression
	: unary_expression { $$ = $1;}
	| multiplicative_expression '*' unary_expression { $$ = new BiOperExpression(ExprKind::MUL, $1, $3);}
	| multiplicative_expression '/' unary_expression { $$ = new BiOperExpression(ExprKind::DIV, $1, $3);}
	;

additive_expression
	: multiplicative_expression { $$ = $1;}
	| additive_expression '+' multiplicative_expression  { $$ = new BiOperExpression(ExprKind::PLUS, $1, $3);}
	| additive_expression '-' multiplicative_expression  { $$ = new BiOperExpression(ExprKind::MINUS, $1, $3);}
	;

relational_expression
	: additive_expression { $$ = $1;}
	| relational_expression '<' additive_expression { $$ = new BiOperExpression(ExprKind::L, $1, $3);}
	| relational_expression '>' additive_expression { $$ = new BiOperExpression(ExprKind::G, $1, $3);}
	| relational_expression LE_OP additive_expression { $$ = new BiOperExpression(ExprKind::LE, $1, $3);}
	| relational_expression GE_OP additive_expression { $$ = new BiOperExpression(ExprKind::GE, $1, $3);}
	;

equality_expression
	: relational_expression { $$ = $1;}
	| equality_expression EQ_OP relational_expression { $$ = new BiOperExpression(ExprKind::EQ, $1, $3);}
	| equality_expression NE_OP relational_expression { $$ = new BiOperExpression(ExprKind::NE, $1, $3);}
	;

assignment_expression
  : equality_expression { $$ = $1;}
	| postfix_expression '=' assignment_expression { $$ = new AssignExpression($1, $3);}
	;

argument_expression_list
	: assignment_expression { $$ = new FuncCall($1);}
	| argument_expression_list ',' assignment_expression { $1->Append($3); $$ = $1;}
	;

type_specifier
	: VOID { $$ = T_VOID;}
	| INT { $$ = T_INT;}
	| LIST { $$ = T_LIST;}
	;

declaration_statement
  : declaration ';' { $$ = new DeclarationStatement($1);}
  ;

declaration
  : type_specifier init_declarator  { $2->setType($1); $$ = $2;}
	;

init_declarator
	: declarator { $$ = $1; }
	| declarator '=' initializer { $1->setInitList($3); $$ = $1; }
	;

parameter_declaration
	: type_specifier ID  { $$ = new FunctionDefinition($1, $2);}
	| parameter_declaration ',' type_specifier ID { $1->Append($3,$4); $$ = $1;}
	;

declarator
	: ID { $$ = new Declaration($1); }
	;

initializer
	: assignment_expression { $$ = new InitList($1); }
	| '{' initializer_list '}' { $$ = $2; }
	;

initializer_list
	: initializer { $$ = new InitList($1);}
	| initializer_list ',' initializer { $1->Append($3); $$ = $1; }
	;

statement
	: compound_statement { $$ = $1;}
	| expression_statement { $$ = $1;}
	| selection_statement { $$ = $1;}
	| iteration_statement { $$ = $1;}
  | declaration_statement { $$ = $1;}
  | out_statement { $$ = $1;}
  | return_statement { $$ = $1;}
	;

out_statement
  : OUT '(' assignment_expression ')' ';' { $$ = new Out_statement($3);}
  ;

statement_list
	: statement { $$ = new CompoundStatement($1);}
	| statement_list statement { $1->Append($2); $$ = $1;}
	;

compound_statement
	: '{' statement_list '}' { $$ = $2; }
	;

expression_statement
	: ';' {$$ = new ExpressionStatement(new Expression(ExprKind::EMPTY));}
	| assignment_expression ';' { $$ = new ExpressionStatement($1); }
	;

selection_statement
	: IF '(' assignment_expression ')' statement {$$ = new SelectionStatement($3, $5);}
	| IF '(' assignment_expression ')' statement ELSE statement {$$ = new SelectionStatement($3, $5, $7);}
	;

iteration_statement
	: WHILE '(' assignment_expression ')' statement {$$ = new LoopStatement($3, $5);}
	;

return_statement
	: RETURN ';' {$$ = new ReturnStatement();}
	| RETURN assignment_expression ';' { $$ = new ReturnStatement($2);}
	;

function_definition
  : type_specifier ID '(' parameter_declaration ')' compound_statement { $4->FuncDef($1,$2,$6); $$ = $4; }
  | type_specifier ID '(' ')' compound_statement { $$ = new FunctionDefinition($1,$2,$5); }
  ;

%%
int main() { yyparse(); return 0; }
int yyerror(const char *msg) { fputs(msg, stderr); }
