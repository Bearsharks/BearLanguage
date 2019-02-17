#include <iostream>
#include "List.h"

List::List()
{
}

List::~List()
{
	auto iter = li.begin();
	for (auto iter = li.begin(); iter != li.end(); ++iter)
	{
		if (iter->second.type == T_LIST)
		{
			delete (List*)(iter->second.value);
		}
	}
}

List::List(List & otherList)
{
	li = otherList.li;
}

List * List::DeepCopy()
{
	List* newList = new List();
	auto it = li.begin();
	while (it != li.end())
	{
		ListElement tmpElem = it->second;
		if (tmpElem.type == T_LIST)
		{
			tmpElem.value = ((List*)tmpElem.value)->DeepCopy();
		}
		(newList->li)[it->first] = tmpElem;
		++it;
	}
	return newList;
}

ListElement& List::operator[](int idx)
{
	pair<map<int, ListElement>::iterator, bool > pr;
	pr.first = li.find(idx);
	if (li.end() == pr.first) {
		cout << idx << " is List out of range (in List)" << endl;
		exit(-1);
	}
	return pr.first->second;
}

ListElement& List::getElement(int idx)
{
	pair<map<int, ListElement>::iterator, bool > pr;
	pr.first = li.find(idx);
	if (li.end() == pr.first) {
		ListElement newElement;
		newElement.type = T_NONE;
		newElement.value = nullptr;
		pr = li.insert(make_pair(idx, newElement));
		if (!pr.second) {
			cout << idx << " inserting failed (in List)" << endl;
			exit(-1);
		}
	}
	return pr.first->second;
}

void List::Print(int depth)
{
	for (auto x : li)
	{
		for (int i = 0; i < depth; i++)
		{
			cout << "  ";
		}
		Type tmp = x.second.type;
		cout << x.first << ' ' << x.second.type << ' ';
		if (tmp == T_LIST)
		{
			cout << endl;
			((List*)x.second.value)->Print(depth + 1);
			cout << endl;
		}
		else if (tmp == T_INT)
		{
			cout << (int)x.second.value << endl;
		}
	}
}