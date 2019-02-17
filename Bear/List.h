#pragma once
#ifndef __LIST__
#define __LIST__
#include <map>
#include <string>
#include "consts.h"
using namespace std;

//bear언어의 기본 타입인 LIST는 타입 제한 없이 값을 저장할 수 있는 컨테이너이다.
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