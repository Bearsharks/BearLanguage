#include <iostream>
#include <cstdarg>
#include "ast.h"

void AST::Run(SymbolTable* mst)
{
	myScopeTable = mst;
}

Type AST::getType()
{
	return type;
}

void Declaration::Run(SymbolTable* mst)
{
	Definition::Run(mst);
	void* resultObjectPtr = nullptr;	
	if (initList) {
		initList->Run(myScopeTable);
		TypeCheck();
		if (initList->getType() == T_LIST)
		{
			resultObjectPtr = ((List*)initList->getValue())->DeepCopy();
		}
		else
		{
			resultObjectPtr = initList->getValue();
		}	
	}
	myScopeTable->AddElement(type, id, resultObjectPtr);
}

Declaration::Declaration(char * id) : Definition(id)
{
}

Declaration::~Declaration()
{
	if (initList)
	{
		delete initList;
	}
}

void Declaration::setType(Type type_)
{
	type = type_;
}

void Declaration::setInitList(InitList * li)
{
	initList = li;
}

void Declaration::TypeCheck()
{
	if (type != initList->getType())
	{
		if (initList->getType() == T_NONE)
		{
			cout << "can't initialize with uninitialized value" << "(in declaration)" << endl;
			exit(-1);
		}
		cout << "type error can't assign " << initList->getType() << " to " << getType() << "(in declaration)" << endl;
		exit(-1);
	}
}

AST::AST():myScopeTable(nullptr)
{
}

void Definition::Run(SymbolTable* mst)
{
	AST::Run(mst);
}

Definition::Definition()
{
}

Definition::Definition(char* name_) : id(name_)
{
}

void Expression::Run(SymbolTable* mst)
{
	AST::Run(mst);	
}

void* Expression::getValue()
{
	return value;
}

pair<Type*, void**> Expression::getRef()
{
	return ref;
}

Expression::Expression()
{
}

Expression::Expression(Expression * oper1, Expression * oper2)
	:operand1(oper1), operand2(oper2)
{
}

Expression::Expression(ExprKind kind_, Expression* operand1_, Expression* operand2_)
	:kind(kind_),operand1(operand1_),operand2(operand2_)
{
}

Expression::Expression(char * oper1)
{
	kind = IDEXPR;
	id = oper1;
}

Expression::Expression(int oper1)
{
	kind = CONEXPR;
	type = T_INT;
	value = (void*)oper1;
}

Expression::~Expression()
{
	if (operand1)
	{
		delete operand1;
	}
	if (operand2)
	{
		delete operand2;
	}
	 
}

void FunctionDefinition::Run(SymbolTable * mst)
{
	AST::Run(mst);
	myScopeTable->AddFunc(type, id, params, compState);
}


FunctionDefinition::FunctionDefinition(Type paramType, char * paramName)
{
	Append(paramType, paramName);
}

FunctionDefinition::FunctionDefinition(Type returnType, char * funcName, Statement * state)
{
	FuncDef(returnType, funcName, state);
}

void FunctionDefinition::Append(Type paramType, char * paramName)
{
	params.push_back(make_pair(paramType, paramName));
}

void FunctionDefinition::FuncDef(Type returnType, char * funcName, Statement* state)
{
	type = returnType;
	id = funcName;
	compState = dynamic_cast<CompoundStatement*>(state);
	if (!compState)
	{
		cout << "compounStatement does not exist" << endl;
		exit(-1);
	}
}

void CompoundStatement::Run(SymbolTable * mst)
{	
	if (myScopeTable)
	{	
		delete myScopeTable;
	}
	Statement::Run(new SymbolTable(mst));
	for (auto x : funcParams)
	{
		myScopeTable->AddElement(x.type, x.id, x.value);
	}	
	type = T_VOID;
	value = nullptr;
	for (auto x : states)
	{
		x->Run(myScopeTable);
		ReturnStatement* returnState = dynamic_cast<ReturnStatement*>(x);
		if (returnState) {
			type = returnState->getType();
			value = returnState->getValue();
			break;
		}		
	}
	funcParams.clear();
}

CompoundStatement::CompoundStatement(Statement* state) 
{
	myScopeTable = nullptr;
	Append(state);	
}

CompoundStatement::~CompoundStatement()
{
	if (myScopeTable) {
		delete myScopeTable;
	}
	for (auto x : states)
	{
		delete x;
	}
}

void CompoundStatement::Append(Statement * state)
{
	states.push_back(state);
}

void * CompoundStatement::getValue()
{
	return value;
}


void CompoundStatement::TypeCheck()
{
}

void Statement::Run(SymbolTable * mst)
{
	AST::Run(mst);
}

Statement::Statement():AST()
{
}

void ReturnStatement::Run(SymbolTable * mst)
{
	Statement::Run(mst);
	type = T_VOID;
	value = nullptr;
	if (expr)
	{
		expr->Run(myScopeTable);
		type = expr->getType();
		value = expr->getValue();
		if (type == T_LIST)
		{
			value = ((List*)value)->DeepCopy();
		}		
	}
}

ReturnStatement::ReturnStatement(Expression* expr_):Statement(), expr(expr_)
{
}

ReturnStatement::~ReturnStatement()
{//값이 리스트라면 딥카피된 값이므로 삭제한다. 
	if (expr)
	{
		delete expr;
	}
	if (type == T_LIST)
	{
		delete (List*)value;
	}
}

void * ReturnStatement::getValue()
{
	return value;
}

void ReturnStatement::TypeCheck()
{
}

void SelectionStatement::Run(SymbolTable * mst)
{
	Statement::Run(mst);
	if (compare_expression) {
		compare_expression->Run(myScopeTable);
		Type compExprType = compare_expression->getType();
		if (compExprType == T_INT) 
		{
			Selection(compare_expression->getValue());
		}
		else 
		{
			cout << "error compare_expression type is not 'int'(in if statement)" << endl;
			exit(-1);
		}
	}
	else {
		cout << "error compare_expression is not exist (in if statement)" << endl;
		exit(-1);
	}
}

SelectionStatement::SelectionStatement
	(Expression* expr, Statement* trueState, Statement* falseState)
	: compare_expression(expr), true_statement(trueState), false_statement(falseState)
{
}

SelectionStatement::~SelectionStatement()
{
	if (compare_expression)
	{
		delete compare_expression;
	}
	if (true_statement)
	{
		delete true_statement;
	}
	if (false_statement)
	{
		delete false_statement;
	}
}

void SelectionStatement::Selection(void* compareValue)
{
	if (compareValue) {
		true_statement->Run(myScopeTable);
	}
	else if (false_statement)
	{
		false_statement->Run(myScopeTable);
	}
}

void SelectionStatement::TypeCheck()
{
}

void LoopStatement::Run(SymbolTable * mst)
{
	Statement::Run(mst);
	compare_expression->Run(myScopeTable);
	Type type = compare_expression->getType();
	if (type == T_INT)
	{
		int compValue = (int)compare_expression->getValue();
		while (compValue) {			
			statement->Run(myScopeTable);
			compare_expression->Run(myScopeTable);
			compValue = (int)compare_expression->getValue();
		}
	}
	else {
		cout << "error compare expression value is not 'int' (in while)" << endl;
	}
}

LoopStatement::LoopStatement(Expression * expr, Statement * state)
	:compare_expression(expr), statement(state)
{
}

LoopStatement::~LoopStatement()
{
	if (compare_expression)
	{
		delete compare_expression;
	}
	if (statement)
	{
		delete statement;
	}
}

void LoopStatement::TypeCheck()
{
}

void Out_statement::Run(SymbolTable * mst)
{
	Statement::Run(mst);
	expr->Run(myScopeTable);
	Type type = expr->getType();
	if (type == T_LIST)
	{
		((List*)(expr->getValue()))->Print();
		cout << endl;
	}
	else if (type == T_INT)
	{
		cout << (int)(expr->getValue()) << endl;
	}
	else
	{
		cout << "error can't Write array (in write)" << endl;
	}	
}

Out_statement::Out_statement(Expression * expr_):expr(expr_)
{
}

Out_statement::~Out_statement()
{
	delete expr;
}

void Out_statement::TypeCheck()
{
}

void Begin::Run(SymbolTable* mst)
{
	AST::Run(mst);
	for (auto x : childs)
	{
		x->Run(myScopeTable);
	}
}

Begin::Begin(AST * c)
{
	Append(c);
}

Begin::~Begin()
{
	for (auto x : childs)
	{
		delete x;
	}	 
}

void Begin::Append(AST * c)
{
	childs.push_back(c);
}

void Root::Run(SymbolTable* mst)
{
	AST::Run(new SymbolTable(nullptr));
	begin->Run(myScopeTable);
	myScopeTable->MainFuncCheck();
	FuncCall* mainFuncCall = new FuncCall((char*)"main");
	mainFuncCall->Run(myScopeTable);
	if (mainFuncCall->getType() != T_INT)
	{
		cout << "error main function Type is not 'int'" << endl;
		exit(-1);
	}
	cout << "main function ended... return value : " << (int)(mainFuncCall->getValue()) << endl;
	delete mainFuncCall;
}

Root::Root(AST * child) : begin(child)
{
}

Root::~Root()
{
	delete myScopeTable;
	delete begin;
}

void ExpressionStatement::Run(SymbolTable * mst)
{
	Statement::Run(mst);
	expr->Run(myScopeTable);
}

ExpressionStatement::ExpressionStatement(Expression * expr_) : expr(expr_)
{
}

void ExpressionStatement::TypeCheck()
{
}

void InitList::Run(SymbolTable* mst)
{
	AST::Run(mst);	
	if (kind == EXPRESSION)
	{
		ExprRun();
		return;
	}
	else {
		InitListRun();
		return;
	}	
}

InitList::InitList(Expression * expr) : curIdx(0)
{
	kind = EXPRESSION;
	Append(expr);
}

InitList::InitList(InitList * expr) : curIdx(0)
{
	kind = INITLIST;
	Append(expr);
}

InitList::~InitList()
{
	for (auto x : childs)
	{
		delete x;
	}
}

void InitList::Append(AST* child)
{
	childs.push_back(child);
}

List* InitList::getValue()
{
	return value;
}

void InitList::ExprRun()
{
	Expression* expr = dynamic_cast<Expression*>(childs[0]);
	if (!expr) {
		cout << "error child isn't Expression (in initlist)" << endl;
		exit(-1);
	}
	childs[0]->Run(myScopeTable);
	type = childs[0]->getType();
	value = (List*)expr->getValue();
}

void InitList::InitListRun()
{
	value = new List();
	type = T_LIST;
	curIdx = 0;
	for (auto x : childs)
	{
		x->Run(myScopeTable);
		ListElement newElement;
		newElement.type = x->getType();
		Expression* expr = dynamic_cast<Expression*>(x);
		if (expr) {
			newElement.value = expr->getValue();
		}
		else
		{
			InitList* li = dynamic_cast<InitList*>(x);
			if (li)
			{
				newElement.value = li->getValue();
			}
			else
			{
				exit(-1);
			}
		}
		value->getElement(curIdx) = newElement;
		curIdx++;
	}
}

void DeclarationStatement::Run(SymbolTable * mst)
{
	Statement::Run(mst);
	decl->Run(myScopeTable);
}

DeclarationStatement::DeclarationStatement(Declaration * decl_) : decl(decl_)
{
}

DeclarationStatement::~DeclarationStatement()
{
	delete decl;
}

void DeclarationStatement::TypeCheck()
{
}

//1.전역테이블까지 타고 올라가서 함수심볼을 찾는다.
//2.타입체크, 함수심볼의 인자와 funcall의 인자가 동일한지 확인한다.
//3.새로운 심볼테이블을 전역테이블을 부모로 해서 만들고, 인자를 테이블에 등록한다.
//4.compound의 심볼테이블을 새로 생성하여 그 테이블을 참조해서 compound의 자식 statement를 실행시킨다.
//5.compound내에서 자식 statement들의 심볼테이블을 compound의 심볼테이블로 해서 하나씩 실행시킨다.
//6.compound내에서 만약 returnstatement를 만나면 실행시키고, 리턴값과 타입을 compound의 값과 타입으로 한다.
//7.타입체크, compound의 값과 타입이 funcall의 리턴값과 동일한지 확인한다.
void FuncCall::Run(SymbolTable* mst)
{
	AST::Run(mst);
	SymbolElement func = myScopeTable->getFunction(id);
	vector<pair<Type, string>> funcParams = func.getParams();
	int numOfParams = funcParams.size();
	if (numOfParams != paramExprs.size())
	{
		cout << "error num of Params is different(in funcCall)" << endl;
		exit(-1);
	}
	CompoundStatement* funcCompState = (CompoundStatement*)func.getValue();
	//4.compound의 심볼테이블을 새로 생성하여 그 테이블을 참조해서 compound의 자식 statement를 실행시킨다.
	for (int i = 0 ; i < numOfParams ; i++)
	{
		paramExprs[i]->Run(myScopeTable);
		if (funcParams[i].first != paramExprs[i]->getType()) 
		{
			cout << "error Param Type is different(in funcCall)" << endl;
			exit(-1);
		}
		funcCompState->addParams(funcParams[i].first, funcParams[i].second, paramExprs[i]->getValue());
	}
	funcCompState->Run(myScopeTable->getGlobalScopeTable());
	type = funcCompState->getType();
	if (type != func.getType())
	{
		cout << "error Function return type is different(in funcCall)" << endl;
		exit(-1);
	}
	value = funcCompState->getValue();
}

FuncCall::FuncCall(char* id_)
{
	id = id_;
}

FuncCall::FuncCall(Expression * expr)
{
	Append(expr);
}

void FuncCall::Append(Expression * expr)
{
	paramExprs.push_back(expr);
}

void FuncCall::setId(char * id_)
{
	id = id_;
}

void CompoundStatement::addParams(Type type, string id, void * value)
{
	Param param;
	param.type = type;
	param.id = id;
	param.value = value;
	funcParams.push_back(param);
}

void BracketExpression::Run(SymbolTable * mst)
{
	AST::Run(mst);
	operand1->Run(myScopeTable);
	operand2->Run(myScopeTable);
	TypeCheck();
	ListElement& tmp = (createListElement) ?
		((List*)operand1->getValue())->getElement((int)operand2->getValue()) :
		(*(List*)operand1->getValue())[(int)operand2->getValue()];
	createListElement = false;
	ref = make_pair(&tmp.type, &tmp.value);
	type = tmp.type;
	value = tmp.value;
}

BracketExpression::BracketExpression(Expression * oper1, Expression * oper2)
	:Expression::Expression(oper1, oper2)
{	
}

void BracketExpression::SetCreateListElement()
{
	createListElement = true;
}

void BracketExpression::TypeCheck()
{
	Type oper1type = operand1->getType();
	Type oper2type = operand2->getType();
	if (oper1type != T_LIST || oper2type != T_INT)
	{
		cout << "Type error : " << oper1type << "[" << oper2type << "]" << endl;
		exit(-1);
	}
}

void AssignExpression::Run(SymbolTable * mst)
{
	BracketExpression* tmp = dynamic_cast<BracketExpression*>(operand1);
	if (tmp) {
		tmp->SetCreateListElement();
	}
	operand1->Run(mst);
	operand2->Run(mst);
	TypeCheck();
	if (operand2->getType() == T_LIST)
	{
		//모두 deepcopy로 이뤄지기 때문에 같은 리스트를 복사하면
		//deepcopy된 값이 리스트의 값이 되버리고 원래 값이 누수됨
		if (operand1->getRef() != operand2->getRef())
		{
			if (operand1->getType() == T_LIST)
			{
				delete (List*)operand1->getRef().second;
			}
			*(operand1->getRef().second) = ((List*)operand2->getValue())->DeepCopy();
		}
	}
	else
	{
		*(operand1->getRef().second) = operand2->getValue();
	}	
}

AssignExpression::AssignExpression(Expression* oper1, Expression* oper2)
	: Expression(oper1, oper2)
{
}

void AssignExpression::TypeCheck()
{
	if (operand1->getType() != operand2->getType())
	{
		if (operand1->getType() == T_NONE) {
			*(operand1->getRef().first) = operand2->getType();
		}
		else {
			cout << "Type error : can't assign " << operand2->getType() << " to " << operand1->getType() << endl;
			exit(-1);
		}
	}
}

void UnOperExpression::Run(SymbolTable * mst)
{
	AST::Run(mst);
	operand1->Run(myScopeTable);
	TypeCheck();
	type = T_INT;
	if (kind == NOT)
	{
		value = (void*)(!(int)operand1->getValue());
	}
	else if (kind == UMINUS)
	{
		value = (void*)(-(int)operand1->getValue());
	}
}

UnOperExpression::UnOperExpression(ExprKind kind_,Expression * oper1)
	:Expression(kind_, oper1)
{}

void UnOperExpression::TypeCheck()
{
	if (operand1->getType() != T_INT)
	{
		string oper = (kind == NOT) ? "!" : "-";
		cout << "type Error : " << oper << operand1->getType() << endl;
		exit(-1);
	}
}

void PrimaryExpression::Run(SymbolTable * mst)
{
	AST::Run(mst);
	if (kind == IDEXPR)
	{
		ref = myScopeTable->getObject(id);
		type = *(ref.first);
		value = *(ref.second);
	}
}

PrimaryExpression::PrimaryExpression(int ival)
	: Expression(ival)
{
}

PrimaryExpression::PrimaryExpression(char * sval)
	: Expression(sval)
{
}

void BiOperExpression::Run(SymbolTable * mst)
{
	Expression::Run(mst);
	operand1->Run(myScopeTable);
	operand2->Run(myScopeTable);
	TypeCheck();
	switch (kind) {
	case EQ:
		value = (void*)((int)operand1->getValue() == (int)operand2->getValue());
		break;
	case NE:
		value = (void*)((int)operand1->getValue() != (int)operand2->getValue());
		break;
	case LE:
		value = (void*)((int)operand1->getValue() <= (int)operand2->getValue());
		break;
	case GE:
		value = (void*)((int)operand1->getValue() >= (int)operand2->getValue());
		break;
	case L:
		value = (void*)((int)operand1->getValue() < (int)operand2->getValue());
		break;
	case G:
		value = (void*)((int)operand1->getValue() > (int)operand2->getValue());
		break;
	case PLUS:
		value = (void*)((int)operand1->getValue() + (int)operand2->getValue());
		break;
	case MINUS:
		value = (void*)((int)operand1->getValue() - (int)operand2->getValue());
		break;
	case MUL:
		value = (void*)((int)operand1->getValue() * (int)operand2->getValue());
		break;
	case DIV:
		value = (void*)((int)operand1->getValue() / (int)operand2->getValue());
		break;
	}
	type = T_INT;
}

BiOperExpression::BiOperExpression(ExprKind kind_, Expression * oper1, Expression * oper2)
	: Expression(kind_, oper1, oper2)
{}

void BiOperExpression::TypeCheck()
{	
	Type oper1type = operand1->getType();
	Type oper2type = operand2->getType();
	if (oper1type != oper2type || oper1type != T_INT)
	{
		cout << "Type error : " << oper1type << "oper" << oper2type << "is not allowed" << endl;
		exit(-1);
	}	
}
