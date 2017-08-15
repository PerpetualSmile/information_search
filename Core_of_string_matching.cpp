#include "stdafx.h"
#include "Core_of_string_matching.h"
#include<regex>
#include<sstream>
#include<iostream>
#include<string>
#include<cstring>
#include<set>
#include"NLPIR.h"
#pragma comment(lib,"NLPIR.lib")

Core_of_string_matching::Core_of_string_matching(list<string>* p, list<int>*q)
{
	searched_List = p;//存放待搜索的文档的字符串
	result = q;//存放查询结果的文档编号
}

bool Core_of_string_matching::precise_match(string target, string pattern) //精确匹配，使用正则表达式匹配完整的字符串，需要有完全匹配的子串。
{

	regex e(pattern, regex_constants::icase);
	bool match = regex_search(target, e);
	return match;
}


bool Core_of_string_matching::vague_match(string target, string query)//模糊匹配
{
	list<string> single_word_of_query;
	stringstream sin(query);
	string temp;
	while (sin >> temp)
	{
		single_word_of_query.push_back(temp); //将查询的每个词分离出来，存入single_word_of_query
	}
	int temp_ = single_word_of_query.size() / 2;//设置正则表达式匹配的词的个数（门槛）
	if (temp_ == 0) temp_ = 1;//词数为0
	string t;
	stringstream  x;
	x << temp_;
	x >> t;// 数字转化为字符

	string pattern;
	list<string>::iterator it;
	//写正则表达式
	pattern = "(";
	for (it = single_word_of_query.begin(); it != single_word_of_query.end(); it++)
	{
		pattern += ((*it) + "|");
	}
	pattern = pattern.substr(0, pattern.length() - 1);
	pattern += ")";
	pattern += "{" + t + ",}";//设置匹配个数
	regex e(pattern, regex_constants::icase);
	bool match = regex_search(target, e);//匹配
	return match;
}

void Core_of_string_matching::search(string query,set<int>inverse) //inverse为索引，过滤掉完全不相关的文档，里面保存了相关文档  
{


	list<string>::iterator it;
	int k=-1;
	for (it = (*searched_List).begin(); it != (*searched_List).end(); it++)//searcher_List中依次保存了一整个文档组成的字符串
	{
		string temp = *it;
		k++;
		//过滤不相关文档
		set<int>::iterator in_it;
		in_it = inverse.find(k);
		if (in_it == inverse.end())continue;//不相关，则跳过当前文档

		if (precise_match(temp, query))//精确匹配是否含有完整的查询的子串
		{
			(*result).push_back(k);//找到则把文档编号放入结果中
		}
	}

	
	 k = -1;
	if ((*result).size() == 0)//精确匹配结果为0，则进行模糊匹配
	{
		for (it = (*searched_List).begin(); it != (*searched_List).end(); it++)
		{
			string temp = *it;
			k++;
			//过滤不相关文档
			set<int>::iterator in_it;
			in_it = inverse.find(k);
			if (in_it == inverse.end())continue;
			//模糊匹配
			if (vague_match(temp, query)) (*result).push_back(k);//模糊匹配
		}
		
	}
	
}

Core_of_string_matching::~Core_of_string_matching(void)
{
}