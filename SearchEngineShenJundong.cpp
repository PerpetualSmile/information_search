// SearchEngineShenJundong.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include"EvaluatorShenJundong.h"
#include"Preprocession.h"
#include"OutputModel.h"
#include"InvIndex.h"
#include<iostream>
#include<string>
#include<fstream>
#include<sstream>
#include<vector>
#include<map>
#include<iomanip>
#include"Core_of_string_matching.h"
#include<cstring>
#include<set>
#include <windows.h>
#include <time.h>                   //time_t time()  clock_t clock()    
#include <Mmsystem.h>               //timeGetTime()    
#pragma comment(lib, "Winmm.lib")   //timeGetTime()   

using namespace std;
typedef map<string, double> UDT_MAP_INT_CSTRING;
UDT_MAP_INT_CSTRING index;



int main()
{
	string query;
	list<string> songs;
	
	list<int> search_result;
	vector<string>  txt_name;
	vector< vector<string> >txt_content;




	vector<ID_sim>doc_index;
	ofstream fout("result.txt");
	ifstream query_file("query.txt");
	ofstream LM("1527405090沈俊东Rmethod1.txt");
	ofstream VM("1527405090沈俊东Rmethod2.txt");
	//ofstream StringMatch("1527405090沈俊东Rmethod4.txt");
	ofstream BM25("1527405090沈俊东Rmethod3.txt");
	ofstream Imethod1("1527405090沈俊东Imethod1.txt");
	//ofstream Imehtod2("1527405090沈俊东Imethod2.txt");


	vector<string> query_data;
	vector<string> random_query;



	Preprocession process(&songs);

	//定义运行时间统计变量
	clock_t timeBegin_all, timeEnd_all;
	clock_t timeBegin_preprocess, timeEnd_preprocess, timeBegin_vm, timeEnd_vm, timeBegin_lm, timeEnd_lm, timeBegin_bm25, timeEnd_bm25, timeBegin_string, timeEnd_string;
	clock_t timeBegin_inverse, timeEnd_inverse;
	timeBegin_all = clock();


	timeBegin_preprocess = clock();
	cout << "Readfile..." << endl;
	process.readfile("dest");
	//process.readData(txt_name, 0); //全文本读入
	process.readData_pagerank(txt_name,15);                              //关键词抽取后的文本读入


	//process.write_textRank(txt_name);                               //写出关键词抽取后的文件
	
	cout << "Preprocess..." << endl;
	process.count_weight();

	cout << "Build Index..." << endl;
	timeEnd_preprocess = clock();
	
	txt_content = process.getText();
	InvIndex inverse_index;
	set<int> inverse;
	timeBegin_inverse = clock();
	for (unsigned int i=0;i<txt_content.size();i++)//文档读入倒排索引中
	{
		inverse_index.add(txt_content[i]);
	}
	inverse_index.build_word_index();
	inverse_index.build_secondary_index();//建立第二个多层索引
	inverse_index.build_hash_index();
	timeEnd_inverse = clock();


	string q = "Query";
	string p;
	int r = 0;
	stringstream rank;
	//Vector Space Model
	timeBegin_vm = clock();
	while (getline(query_file, query))
	{
		double standardSim;
		r++;
		rank.clear();
		rank << r;
		rank >> p;
		VM << q + p << endl;
		inverse_index.search(query, inverse);
		//inverse_index.second_index_search(query,inverse);
		//inverse_index.hash_search(query, inverse);
		//VM << query << endl;
		doc_index.clear();
		process.count_sim(query, doc_index,inverse);
		OutputModel vmModel(doc_index);
		//standardSim=vmModel.averageDeviation();
		//standardSim=vmModel.pairDeviation();
		standardSim = vmModel.windowDeviation(3);
		//standardSim=vmModel.sigmoid();
		for (int k = 0;; k++)
		{
			int temp = doc_index[k].ID;
			if (doc_index[k].sim < standardSim) break;
			VM << txt_name[temp]  << endl;
		}

	}
	timeEnd_vm = clock();


	//Language Model
	query_file.clear();
	query_file.seekg(0);
	r = 0;
	timeBegin_lm = clock();
	while (getline(query_file, query))
	{
		double standardSim;
		r++;
		rank.clear();
		rank << r;
		rank >> p;
		LM << q + p << endl;
		//inverse_index.search(query, inverse);
		//inverse_index.second_index_search(query, inverse);
		inverse_index.hash_search(query, inverse);
		//LM << query << endl ;
		doc_index.clear();
		process.count_probability(0.7, query, doc_index,inverse);
		OutputModel lmModel(doc_index);
		//standardSim = lmModel.averageDeviation();
		//standardSim = lmModel.pairDeviation();
		standardSim = lmModel.windowDeviation(6);
		//standardSim = lmModel.sigmoid();


		for (int k = 0;; k++)
		{
			int temp = doc_index[k].ID;
			if (doc_index[k].sim <standardSim) break;
			LM << txt_name[temp] << endl;
		}

	}
	timeEnd_lm = clock();

	/*
	//character match model
	
	query_file.clear();
	query_file.seekg(0);
	songs.clear();
	for (unsigned int i = 0; i<txt_content.size(); i++)//文档读入倒排索引中
	{
		string temp_x="";
		for (unsigned int j=0;j<txt_content[i].size();j++)
		{
			temp_x += (txt_content[i][j] + " ");
		}
		songs.push_back(temp_x);
	}

	timeBegin_string = clock();
	while (getline(query_file, query))
	{
	StringMatch << query << endl << endl;
	//inverse_index.search(query, inverse);
	//inverse_index.second_index_search(query, inverse);
	inverse_index.hash_search(query, inverse);
	Core_of_string_matching Search_engine(&songs, &search_result);

	search_result.clear();
	Search_engine.search(query,inverse);
	list<int>::iterator it;
	int xyz = 0;
	for (it = search_result.begin(); it != search_result.end(); it++)
	{
		xyz++;
		if (xyz == 10)break;
		StringMatch << txt_name[*it] << endl;
	}
	StringMatch << "@@@" << endl;
	}
	timeEnd_string =clock();
	*/
	
	//BM25
	query_file.clear();
	query_file.seekg(0);
	r = 0;
	timeBegin_bm25 = clock();
	while (getline(query_file, query))
	{
		double standardSim;
		r++;
		rank.clear();
		rank << r;
		rank >> p;
		BM25 << q + p << endl;
		//inverse_index.search(query, inverse);
		//inverse_index.second_index_search(query, inverse);
		inverse_index.hash_search(query, inverse);
		//BM25 << query << endl ;
		doc_index.clear();
		process.count_score(query,doc_index,inverse);
		OutputModel bmModel(doc_index);
		//standardSim = bmModel.averageDeviation();
		//standardSim = bmModel.pairDeviation();
		standardSim = bmModel.windowDeviation(3);
		//standardSim = bmModel.sigmoid();
		for (int k = 0;; k++)
		{
			int temp = doc_index[k].ID;
		    if (doc_index[k].sim < standardSim) break;
			BM25 << txt_name[temp] << endl;
			
		}

		//cout << query << endl;
	}
	timeEnd_bm25 = clock();


	cout << "Done!" << endl;

	

	fout.close();

	timeEnd_all = clock();
	cout << "preprocess:" << timeEnd_preprocess - timeBegin_preprocess<<endl;
	cout << "inverse_index:" << timeEnd_inverse - timeBegin_inverse << endl;
	cout << "vm:" << timeEnd_vm - timeBegin_vm << endl;
	cout << "bm25:" << timeEnd_bm25 - timeBegin_bm25 << endl;
	cout << "lm:" << timeEnd_lm - timeBegin_lm << endl;
	//cout << "matching:" << timeEnd_string - timeBegin_string << endl;
	cout << "all:" << timeEnd_all - timeBegin_all << endl;

	Imethod1 << "Imethod1" << endl << endl << endl << timeEnd_vm - timeBegin_vm <<"ms";
	return 0;
}

