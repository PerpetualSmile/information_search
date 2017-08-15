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
	searched_List = p;//��Ŵ��������ĵ����ַ���
	result = q;//��Ų�ѯ������ĵ����
}

bool Core_of_string_matching::precise_match(string target, string pattern) //��ȷƥ�䣬ʹ��������ʽƥ���������ַ�������Ҫ����ȫƥ����Ӵ���
{

	regex e(pattern, regex_constants::icase);
	bool match = regex_search(target, e);
	return match;
}


bool Core_of_string_matching::vague_match(string target, string query)//ģ��ƥ��
{
	list<string> single_word_of_query;
	stringstream sin(query);
	string temp;
	while (sin >> temp)
	{
		single_word_of_query.push_back(temp); //����ѯ��ÿ���ʷ������������single_word_of_query
	}
	int temp_ = single_word_of_query.size() / 2;//����������ʽƥ��Ĵʵĸ������ż���
	if (temp_ == 0) temp_ = 1;//����Ϊ0
	string t;
	stringstream  x;
	x << temp_;
	x >> t;// ����ת��Ϊ�ַ�

	string pattern;
	list<string>::iterator it;
	//д������ʽ
	pattern = "(";
	for (it = single_word_of_query.begin(); it != single_word_of_query.end(); it++)
	{
		pattern += ((*it) + "|");
	}
	pattern = pattern.substr(0, pattern.length() - 1);
	pattern += ")";
	pattern += "{" + t + ",}";//����ƥ�����
	regex e(pattern, regex_constants::icase);
	bool match = regex_search(target, e);//ƥ��
	return match;
}

void Core_of_string_matching::search(string query,set<int>inverse) //inverseΪ���������˵���ȫ����ص��ĵ������汣��������ĵ�  
{


	list<string>::iterator it;
	int k=-1;
	for (it = (*searched_List).begin(); it != (*searched_List).end(); it++)//searcher_List�����α�����һ�����ĵ���ɵ��ַ���
	{
		string temp = *it;
		k++;
		//���˲�����ĵ�
		set<int>::iterator in_it;
		in_it = inverse.find(k);
		if (in_it == inverse.end())continue;//����أ���������ǰ�ĵ�

		if (precise_match(temp, query))//��ȷƥ���Ƿ��������Ĳ�ѯ���Ӵ�
		{
			(*result).push_back(k);//�ҵ�����ĵ���ŷ�������
		}
	}

	
	 k = -1;
	if ((*result).size() == 0)//��ȷƥ����Ϊ0�������ģ��ƥ��
	{
		for (it = (*searched_List).begin(); it != (*searched_List).end(); it++)
		{
			string temp = *it;
			k++;
			//���˲�����ĵ�
			set<int>::iterator in_it;
			in_it = inverse.find(k);
			if (in_it == inverse.end())continue;
			//ģ��ƥ��
			if (vague_match(temp, query)) (*result).push_back(k);//ģ��ƥ��
		}
		
	}
	
}

Core_of_string_matching::~Core_of_string_matching(void)
{
}