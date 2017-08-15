#pragma once
#include<iostream>
#include<list>
#include<string>
#include<vector>
#include<map>
#include<set>
#include<unordered_map>
using namespace std;
typedef unordered_map<string, int> hash_map;
class InvIndex : public map<string,vector<int>>
{
	vector< vector<string> > docs ;
	vector<vector<int>> all_word;
	vector <map<string, vector<int> > > second_map;
	vector<map<string, vector<int> > > third_map;
	map<string, map<int, int> > second_index;
	map<string, map<string, map<int, int> > > third_index;
	hash_map hash;
public:
	InvIndex();
	void add(vector<string>&doc);
	void search_single_word(string query, set<int>& docIDs);
	void search(string query,set<int> &result);
	void hash_search(string query, set<int> &result);

	void depart(string s, vector<string>& p);
	void build_secondary_index();
	void build_hash_index();
	void build_word_index();
	void second_index_search(string query,set<int> &result);
	void second_search_single_word(string query, set<int>& docIDs);
	~InvIndex();
};

