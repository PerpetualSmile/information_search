#pragma once
#include<list>
#include<cstring>
#include<vector>
#include<map>
#include<set>
#include<string>
//#include"NLPIR.h"
//#pragma comment(lib,"NLPIR.lib")
using namespace std;
class ID_sim					//存储文档ID以及最后计算出的相似度值
{
public:
	int ID;
	double sim;
};

class Fre							//存储每个单词，按照文档名
{
public:
	string name;
	int ID;
	double tf;
	double idf;
	double w_td;
	double frequency;
	Fre(string n, int i, double t,double y)
	{
		name = n;
		ID = i;
		tf = t;
		frequency = y;
	}
};


class Preprocession
{
	list<string> *L;
	vector<vector<string> >all_words_in_doc;
	string filename;
	map<string,double>stop_word;
	vector<map<string, double> > every_document_index; //每篇文档的tf
	vector<map<string, double> > every_document_index_fre; //每篇文档的词频
	vector<double> every_document_sqrtlen;//每篇文档的向量模长
	vector<vector<Fre> > word_index; //文档中所有词的信息
	map<string, double> index;//文档中所有词的文档频率
	map<string, double> Mc_index;//文档中所有词的总词频
	double length;//所有文档的平均长度

	bool contains(list<string>&, string);
	void string_process(string&, int);
	void string_processdata(string&);
	void filter(string&);
	void depart(string, vector<string>&);
	int count_fre(vector<string>, string);
	void getData(string fname,int n);
	void getData_pagerank(string fname,int n);
	void textRank(string buff);
	bool is_stop_word(string word);
	void removeStopword(string buff);
public:
	Preprocession(list<string> *s);
	vector< vector<string>>getText();
	void reduction();
	int count_probability(double,string, vector<ID_sim> &doc_index,set<int> );
	int count_score(string query,  vector<ID_sim> &doc_index, set<int>);
	void count_weight();
	void readfile(string name);
	void readData(vector<string>&,int n);
	void readData_pagerank(vector<string>&,int n);
	int count_sim(string,vector<ID_sim>&, set<int>);
	void write_textRank(vector<string>);
};
