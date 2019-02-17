#pragma once
#ifndef __LIST__
#define __LIST__
#include <map>
#include <string>
#include "consts.h"
using namespace std;

//bear����� �⺻ Ÿ���� LIST�� Ÿ�� ���� ���� ���� ������ �� �ִ� �����̳��̴�.
struct ListElement {
	Type type;
	void* value;
};

class List {
public:
	List();
	~List();
	List(List &otherList);
	List* DeepCopy();
	ListElement& getElement(int idx);
	ListElement& operator[](int idx);
	void Print(int depth = 0);
private:
	map<int, ListElement> li;
};
#endif