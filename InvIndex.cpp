#include "stdafx.h"
#include "InvIndex.h"
#include<iostream>
#include<vector>
#include<map>
#include<string>
#include<set>
#include<fstream>
#include<unordered_map>
#include<functional>
using namespace std;

InvIndex::InvIndex()
{

}

void InvIndex::add(vector<string>& doc) //将每篇文档的信息添加到索引中，建立倒排索引
{
	docs.push_back(doc);
	int curDocid = docs.size()-1;
	for (unsigned int w=0;w<doc.size();w++)  //遍历每篇文档中的词
	{
		map<string, vector<int> >::iterator it;
		it = this->find(doc[w]);  //本类是map<string,vector<int>>类的子类，所以本类本身就是字典
		if (it==this->end())      //若在词典中还没有当前词，则新增词条
		{
			vector<int> newList;
			(*this)[doc[w]] = newList;
			it = this->find(doc[w]);
		}
		it->second.push_back(curDocid);  //保存与当前词相关的文档编号,在当前词的索引表中添加当前文档的编号
	}
}


void InvIndex::search(string query,set<int> &result)  //搜索与当前查询相关的文档的编号，结果存入result中
{
	vector<string> query_string;
	set<int> docIDs;
	depart(query, query_string);  //将当前查询分割为一个个词
	int termNum = query_string.size();
	result.clear();
	docIDs.clear();
	for (int t=0;t<termNum;t++)
	{
		search_single_word(query_string[t],docIDs);  //对单个词查找相关文档
		set<int>::iterator it;
		for (it=docIDs.begin();it!=docIDs.end();it++)
		{
			result.insert(*it);                 //存入整个查询的总结果中，set自动去重
		}
	}


}

void InvIndex::hash_search(string query, set<int>& result) //利用hash表查询相关文档
{
	vector<string> query_string;
	set<int> docIDs;
	hash_map::iterator it;
	depart(query, query_string);
	int termNum = query_string.size();
	result.clear();
	for (int t = 0; t<termNum; t++)
	{
		if ((it = hash.find(query_string[t])) != hash.end())
		{
			result.insert(all_word[it->second].begin(), all_word[it->second].end());
		}
	}
}



void InvIndex::search_single_word(string query, set<int>& docIDs) //查询索引中与单个词相关的文档的编号
{
	    docIDs.clear();
		map<string, vector<int>>::iterator it;
		if ((it = this->find(query)) != this->end())
		{
			docIDs.insert(it->second.begin(), it->second.end());
		}
}

void InvIndex::depart(string s, vector<string>&p)//将查询分割为一个个词存入向量中
{
	int pos1;
	string temp;
	while (true)
	{
		pos1 = s.find(' ');
		if (pos1 == string::npos) break;
		temp = s.substr(0, pos1);
		p.push_back(temp);
		s = s.substr(pos1 + 1);
	}
	p.push_back(s);
}

void InvIndex::build_secondary_index() //建立多层索引结构
{
	map<string, vector<int> >::iterator it;
	map<int, int>::iterator its;
	map<string, vector<int> >buff;
	map<int, int> temp;
	string before, present;
	int i = 0;
	int j = 0;
	it = this->begin();
	before = it->first;
	before = before.substr(0, 2);
	for (it=this->begin();it!=this->end();it++,i++,j++)
	{
		buff.clear();
		temp.clear();
		buff[it->first] = it->second;
		second_map.push_back(buff);
		present = it->first;
		present= present.substr(0,2);
		if (present!=before)
		{
			temp[i-j] = i - 1;
			second_index[before]=temp; 
			its = temp.begin();
			j = 0;
		}
		before = present;
	
	}

	

}




void InvIndex::build_hash_index()	//建立hash索引
{
	map<string, vector<int> >::iterator it;
	int i = 0;
	for (it = this->begin(); it != this->end();i++, it++)
	{
		hash[it->first] = i;
	}

}

void InvIndex::build_word_index()  //建立所有单词与之对应的索引的二维向量
{
	map<string, vector<int> >::iterator it;
	for (it=this->begin();it!=this->end();it++)
	{
		all_word.push_back(it->second);
	}
}

void InvIndex::second_index_search(string query, set<int>& result) //二层索引的查找函数
{
	vector<string> query_string;
	set<int> docIDs;
	depart(query, query_string);
	int termNum = query_string.size();
	result.clear();
	for (int t = 0; t<termNum; t++)
	{
		second_search_single_word(query_string[t], docIDs);
		set<int>::iterator it;
		for (it = docIDs.begin(); it != docIDs.end(); it++)
		{
			//cout << *it <<" ";
			result.insert(*it);
		}
		//cout << endl;
	}
}

void InvIndex::second_search_single_word(string query, set<int>& docIDs)//二层索引查找单个词
{
	docIDs.clear();
	map<string, map<int,int> >::iterator it;
	map<int, int>::iterator its;
	map<string, vector<int> >::iterator it_;
	string two_char_begin=query.substr(0,2);
	if ((it=second_index.find(two_char_begin))!=second_index.end())
	{
		its = it->second.begin();
		for ( int i=its->first;i<=its->second;i++)
		{
			it_ = second_map[i].begin();
			if (it_->first == query) break;
		}
		docIDs.insert(it_->second.begin(),it_->second.end());
	}
}

InvIndex::~InvIndex()
{


}
