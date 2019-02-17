#pragma once
#ifndef __AST__
#define __AST__
#include <vector>
#include <string>
#include "SymbolTable.h"
#include "consts.h"
using namespace std;

class Expression;

class AST {
public:
	virtual void Run(SymbolTable* mst = nullptr) = 0;
	Type getType();
protected:
	AST();
	SymbolTable* myScopeTable;
	Type type = T_VOID;
};

//변수 선언 후 초기화 할때, 초기 값 노드 
class InitList : public AST {
	enum myKind
	{
		INITLIST,
		EXPRESSION
	};
public:
	virtual void Run(SymbolTable* mst) override;
	InitList(Expression* expr);
	InitList(InitList* expr);
	~InitList();
	void Append(AST* expr);
	List* getValue();
private:
	void ExprRun();
	void InitListRun();
	int curIdx;
	List* value;
	myKind kind;
	vector<AST*> childs;
};


class Root : public AST {
public:
	virtual void Run(SymbolTable* mst) override;
	Root(AST* child);
	~Root();
private:
	AST* begin;
};

class Begin : public AST {
public:
	virtual void Run(SymbolTable* mst) override;
	Begin(AST* c);
	~Begin();
	void Append(AST* c);
private:
	vector<AST*> childs;
};

//모든 expression의 부모 클래스
class Expression : public AST {	
public:
	virtual void Run(SymbolTable* mst) override;
	void* getValue();
	pair<Type*, void**> getRef();
	Expression();
	Expression(Expression* oper1, Expression* oper2 = nullptr);
	Expression(ExprKind kind, Expression* oper1 = nullptr, Expression* oper2 = nullptr);
	Expression(char* oper1);
	Expression(int oper1);
	~Expression();
protected:
	ExprKind kind;
	pair<Type*, void**> ref;
	void* value;
	string id = "";
	Expression* operand1 = nullptr;
	Expression* operand2 = nullptr;		
};

//{}
class BracketExpression : public Expression {
public:
	virtual void Run(SymbolTable* mst) override;
	BracketExpression(Expression* oper1, Expression* oper2);
	void SetCreateListElement();
	void TypeCheck();
private:
	bool createListElement = false;
};

//=
class AssignExpression : public Expression {
public:
	virtual void Run(SymbolTable* mst) override;
	AssignExpression(Expression* oper1, Expression* oper2);
	void TypeCheck();
};

class UnOperExpression : public Expression {
public:
	virtual void Run(SymbolTable* mst) override;
	UnOperExpression(ExprKind kind_, Expression* oper1);
	void TypeCheck();
};

class BiOperExpression : public Expression {
public:
	virtual void Run(SymbolTable* mst) override;
	BiOperExpression(ExprKind kind_, Expression* oper1, Expression* oper2);
private:
	void TypeCheck();
};

class FuncCall : public Expression {
public:
	virtual void Run(SymbolTable* mst) override;
	FuncCall(char* id_);
	FuncCall(Expression* expr);
	void Append(Expression* expr);
	void setId(char* id_);
private:
	vector<Expression*> paramExprs;
};

class PrimaryExpression : public Expression {
public:
	virtual void Run(SymbolTable* mst) override;
	PrimaryExpression(int ival);
	PrimaryExpression(char* sval);
};


class Statement : public AST {
public:
	virtual void Run(SymbolTable* mst) override;
	Statement();
protected:
	virtual void TypeCheck() = 0;
};

//{}
class CompoundStatement : public Statement {
	struct Param {
		Type type;
		string id;
		void * value;
	};
public:
	virtual void Run(SymbolTable* mst) override;
	CompoundStatement(Statement* state);
	~CompoundStatement();
	void Append(Statement* state);
	void* getValue();
	void addParams(Type type, string id, void * value);
protected:
	vector<Statement*> states;
	void* value;
	virtual void TypeCheck() override;
	vector<Param> funcParams;
};

//리턴문
class ReturnStatement : public Statement {
public:
	virtual void Run(SymbolTable* mst) override;
	ReturnStatement(Expression* expr_ = nullptr);
	~ReturnStatement();
	void* getValue();
protected:
	void* value;
	Expression* expr;
	virtual void TypeCheck() override;
};

//if문
class SelectionStatement : public Statement {
public:
	virtual void Run(SymbolTable* mst) override;
	SelectionStatement(Expression* expr, Statement* trueState, Statement* falseState = nullptr);
	~SelectionStatement();
protected:
	Expression* compare_expression;
	Statement* true_statement;
	Statement* false_statement;
private:
	void Selection(void* compareValue);
	virtual void TypeCheck() override;
};

//반목문
class LoopStatement : public Statement {
public:
	virtual void Run(SymbolTable* mst) override;
	LoopStatement(Expression* expr, Statement* state);
	~LoopStatement();
protected:
	Expression* compare_expression;
	Statement* statement;
	virtual void TypeCheck() override;
};

//출력문
class Out_statement : public Statement {
public:
	virtual void Run(SymbolTable* mst) override;
	Out_statement(Expression* expr_);
	~Out_statement();
protected:
	Expression* expr;
	virtual void TypeCheck() override;
};

class ExpressionStatement : public Statement {
public:
	virtual void Run(SymbolTable* mst) override;
	ExpressionStatement(Expression* expr_);
protected:
	Expression* expr;
	virtual void TypeCheck() override;
};


class Definition : public AST {
public:	
	virtual void Run(SymbolTable* mst) override;
protected:
	Definition();
	Definition(char* name);
	string id;
};


class Declaration : public Definition {
public:	
	virtual void Run(SymbolTable* mst) override;
	Declaration(char* id);
	~Declaration();
	void setType(Type type_);
	void setInitList(InitList* li);
	void TypeCheck();
private:
	InitList* initList = nullptr;
};

//변수선언
class DeclarationStatement : public Statement {
public:
	virtual void Run(SymbolTable* mst) override;
	DeclarationStatement(Declaration* decl_);
	~DeclarationStatement();
protected:
	Declaration* decl;
	virtual void TypeCheck() override;
};

struct Param_info {
	Type type;
	string name;
};

//함수선언
class FunctionDefinition : public Definition {
public:
	virtual void Run(SymbolTable* mst) override;
	FunctionDefinition(Type paramType, char* paramName);
	FunctionDefinition(Type returnType, char* funcName, Statement* state);
	void Append(Type paramType, char* paramName);
	void FuncDef(Type returnType, char* funcName, Statement* state);
protected:
	vector<pair<Type, string>> params;
	CompoundStatement* compState;
};
#endif