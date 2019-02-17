#pragma once
#ifndef __ENUMS__
#define __ENUMS__
#include <string>

enum ExprKind
{
	ASSIGN,
	EQ,
	NE,
	LE,
	GE,
	L,
	G,
	PLUS,
	MINUS,
	MUL,
	DIV,
	NOT,
	UMINUS,
	IDEXPR,
	CONEXPR,
	EMPTY
};

enum Type {
	T_VOID,
	T_INT,
	T_LIST,
	T_NONE
};

const std::string NOTFOUND = "NOTFOUND";

#endif