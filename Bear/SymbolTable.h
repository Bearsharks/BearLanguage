#pragma once
#ifndef __SYMTAB__
#define __SYMTAB__
#include <vector>
#include <string>
#include "List.h"
#include "consts.h"
using namespace std;

class SymbolElement {
public:
	SymbolElement();
	SymbolElement(Type type_, string id_, void* objectPtr = nullptr);
	SymbolElement(Type type_, string id_, vector<pair<Type, string>> params_, void* compstate);
	void* getValue();
	string getId();
	Type getType();
	vector<pair<Type, string>> getParams();
private:
	void* value;
	string id;
	Type type;
	bool isVariable;
	vector<pair<Type, string>> params;
	friend class SymbolTable;
};

class SymbolTable {
	SymbolTable* parent;
	vector<SymbolElement> table;
public:
	SymbolTable(SymbolTable* _parent);
	~SymbolTable();
	void AddElement(Type type_, string id_, void* elementObjectPtr = nullptr);
	void AddFunc(Type returnType, string funcName, vector<pair<Type, string>> params, void* compstate);
	pair<Type *, void**> getObject(string id);
	SymbolTable* getGlobalScopeTable();
	SymbolElement getFunction(string id);
	void MainFuncCheck();
	void PrintTable();
private:
	bool isThere(string const id);
};
#endif