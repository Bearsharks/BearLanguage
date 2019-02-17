#include <iostream>
#include <cstdarg>
#include "SymbolTable.h"
using namespace std;

SymbolTable::SymbolTable(SymbolTable* _parent) : parent(_parent) 
{

}

SymbolTable::~SymbolTable()
{
	for (auto x : table)
	{
		if (x.isVariable && x.type == T_LIST) {
			delete (List*)x.value;
		}
	}
}

void SymbolTable::AddElement(Type type_, string id_, void* elementObjectPtr)
{
	bool isSuccess;	
	if (!isThere(id_))
	{
		bool isListNotInit = type_ == T_LIST && !elementObjectPtr;
		if (isListNotInit) {
			elementObjectPtr = new List();
		}
		table.push_back(SymbolElement(type_, id_, elementObjectPtr));
	}
	else
	{
		cout << "error id " << id_ << " is duplicated" << endl;
		exit(-1);
	}
}

void SymbolTable::AddFunc(Type returnType, string funcName, vector<pair<Type, string>> params, void* compstate)
{	
	if (parent)
	{
		cout << "error : isn't Global Scope Table" << endl;
		exit(-1);
	}
	bool isSuccess = true;
	for (auto x : table)
	{
		if (x.id == funcName && x.type == returnType && x.params.size() == params.size())
		{//리턴타입과 함수 이름과 인자의 갯수까지 같은데
			int size = params.size();
			for (int i = 0 ; i < size ; i++)
			{
				if (x.params[i].first != params[i].first)
				{
					isSuccess = true;
					break;
				}
			}
			if (isSuccess == false)//모든 인자들의 타입까지 같으면
			{
				cout << "type error : function signature is duplicated" << endl;
				exit(-1);
			}
		}
	}
	table.push_back(SymbolElement(returnType, funcName, params, compstate));
}


pair<Type *, void**> SymbolTable::getObject(string id)
{
	for (int i = 0; i < table.size() ; i++)
	{
		if (table[i].id == id)
		{
			return make_pair(&(table[i].type), &(table[i].value));
		}
	}
	if (parent)
	{
		return parent->getObject(id);
	}
	else
	{
		cout << id << " not exist" << endl;
		exit(-1);
	}
}

SymbolTable * SymbolTable::getGlobalScopeTable()
{
	SymbolTable * result = this;
	while (result->parent)
	{		
		result = result->parent;
	}
	return result;
}

SymbolElement SymbolTable::getFunction(string id)
{
	bool isFound = false;
	SymbolTable* GlobalScopeTable = getGlobalScopeTable();
	SymbolElement result;
	for (auto x : GlobalScopeTable->table)
	{
		if (x.id == id) {
			result = x;
			isFound = true;
			break;
		}
	}
	if (!isFound)
	{
		cout << "Function" << id << " does not Exist" << endl;
		exit(-1);
	}
	return result;
}

void SymbolTable::MainFuncCheck()
{
	bool existMain = false;
	for (auto x : table)
	{
		if (x.id == "main")
		{
			existMain = true;
			break;
		}
	}
	if (!existMain)
	{
		cout << "error main function does not exist" << endl;
		exit(-1);
	}	
}

void SymbolTable::PrintTable()
{
	cout << "------Symboltable---------" << endl;
	for (auto x : table)
	{		
		Type type = x.type;
		cout << x.id << ' ' << type << ' ';
		if (type == T_INT)
		{
			cout << (int)x.value << endl;
		}
		else if(type == T_LIST)
		{
			cout << endl;
			((List*)x.value)->Print(1);
		}
		else
		{
			cout << endl;
		}
	}
	if (parent)
	{
		cout << "parents";
		parent->PrintTable();
	}
	else
	{
		cout << "symbol Print end" << endl << endl;
	}
}


bool SymbolTable::isThere(string id)
{
	for (auto x : table) {
		if (x.id == id) return true;
	}
	return false;
}

SymbolElement::SymbolElement()
{
}

SymbolElement::SymbolElement(Type type_, string id_, void* objectPtr)
	:type(type_), id(id_), value(objectPtr), isVariable(true)
{
}

SymbolElement::SymbolElement(Type type_, string id_, vector<pair<Type, string>> params_, void * compstate)
	: type(type_), id(id_), params(params_), value(compstate), isVariable(false)
{
}

void * SymbolElement::getValue()
{
	return value;
}

string SymbolElement::getId()
{
	return id;
}

Type SymbolElement::getType()
{
	return type;
}

vector<pair<Type, string>> SymbolElement::getParams()
{
	return params;
}