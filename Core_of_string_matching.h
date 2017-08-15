#pragma once
#include<list>
#include<set>
using namespace std;
class Core_of_string_matching
{
	list<string>*searched_List;
	list<int>*result;
	bool vague_match(string, string);
	bool precise_match(string, string);
public:
	Core_of_string_matching(list<string>*, list<int>*);
	void search(string,set<int>);
	~Core_of_string_matching(void);
};

