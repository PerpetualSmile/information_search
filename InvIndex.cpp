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

void InvIndex::add(vector<string>& doc) //��ÿƪ�ĵ�����Ϣ��ӵ������У�������������
{
	docs.push_back(doc);
	int curDocid = docs.size()-1;
	for (unsigned int w=0;w<doc.size();w++)  //����ÿƪ�ĵ��еĴ�
	{
		map<string, vector<int> >::iterator it;
		it = this->find(doc[w]);  //������map<string,vector<int>>������࣬���Ա��౾������ֵ�
		if (it==this->end())      //���ڴʵ��л�û�е�ǰ�ʣ�����������
		{
			vector<int> newList;
			(*this)[doc[w]] = newList;
			it = this->find(doc[w]);
		}
		it->second.push_back(curDocid);  //�����뵱ǰ����ص��ĵ����,�ڵ�ǰ�ʵ�����������ӵ�ǰ�ĵ��ı��
	}
}


void InvIndex::search(string query,set<int> &result)  //�����뵱ǰ��ѯ��ص��ĵ��ı�ţ��������result��
{
	vector<string> query_string;
	set<int> docIDs;
	depart(query, query_string);  //����ǰ��ѯ�ָ�Ϊһ������
	int termNum = query_string.size();
	result.clear();
	docIDs.clear();
	for (int t=0;t<termNum;t++)
	{
		search_single_word(query_string[t],docIDs);  //�Ե����ʲ�������ĵ�
		set<int>::iterator it;
		for (it=docIDs.begin();it!=docIDs.end();it++)
		{
			result.insert(*it);                 //����������ѯ���ܽ���У�set�Զ�ȥ��
		}
	}


}

void InvIndex::hash_search(string query, set<int>& result) //����hash���ѯ����ĵ�
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



void InvIndex::search_single_word(string query, set<int>& docIDs) //��ѯ�������뵥������ص��ĵ��ı��
{
	    docIDs.clear();
		map<string, vector<int>>::iterator it;
		if ((it = this->find(query)) != this->end())
		{
			docIDs.insert(it->second.begin(), it->second.end());
		}
}

void InvIndex::depart(string s, vector<string>&p)//����ѯ�ָ�Ϊһ�����ʴ���������
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

void InvIndex::build_secondary_index() //������������ṹ
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




void InvIndex::build_hash_index()	//����hash����
{
	map<string, vector<int> >::iterator it;
	int i = 0;
	for (it = this->begin(); it != this->end();i++, it++)
	{
		hash[it->first] = i;
	}

}

void InvIndex::build_word_index()  //�������е�����֮��Ӧ�������Ķ�ά����
{
	map<string, vector<int> >::iterator it;
	for (it=this->begin();it!=this->end();it++)
	{
		all_word.push_back(it->second);
	}
}

void InvIndex::second_index_search(string query, set<int>& result) //���������Ĳ��Һ���
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

void InvIndex::second_search_single_word(string query, set<int>& docIDs)//�����������ҵ�����
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
