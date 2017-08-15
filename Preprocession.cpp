#include "stdafx.h"
#include "Preprocession.h"
#include<string>
#include<fstream>
#include<iostream>
#include<iomanip>
#include<cstring>
#include<vector>
#include<set>
#include<io.h>
#include<algorithm>
#include <windows.h>
#include <time.h>                   //time_t time()  clock_t clock()    
#include <Mmsystem.h>               //timeGetTime()    
#pragma comment(lib, "Winmm.lib")   //timeGetTime()   
typedef map<string,double> UDT_MAP_INT_CSTRING;
UDT_MAP_INT_CSTRING index_tf;
using namespace std;

bool compare(ID_sim a, ID_sim b)//根据相似度对文档排序
{
	return a.sim > b.sim;
}


int Preprocession::count_probability(double lim, string query, vector<ID_sim> &doc_index,set<int> inverse)
{
	vector<string> query_string;
	depart(query, query_string);
	doc_index.clear();
	map<string, double> map_query;
	map<string, double>::iterator it;
	//统计查询中的tf，并将结果写入map中
	for (unsigned int t = 0; t < query_string.size(); t++)
	{
		 it= map_query.find(query_string[t]);
		if (it == map_query.end())
		{
			map_query[query_string[t]] = 1;
		}
		else
		{
			map_query[query_string[t]]++;
		}
	}

	for (unsigned int i = 0; i < word_index.size(); i++)
	{
		 
		if ((inverse.find(i)) == inverse.end())continue;
		ID_sim buff;
		double temp = 1;
		double p_Mc, p_Md;
		p_Mc = 0, p_Md = 0;
		int flag = 0;
		
		map<string, double>::iterator it_, it__;

		for (map<string, double>::iterator it = map_query.begin(); it != map_query.end(); it++)
		{
			it_ = every_document_index[i].find(it->first);
			it__ = Mc_index.find(it->first);
			p_Mc = 0, p_Md = 0;
			if (it_ != every_document_index[i].end())
			{
				flag = 1;
				p_Md = it_->second;

			}
			if (it__ != Mc_index.end())
			{
				p_Mc = it__->second / length;
			}
			if (it_ != every_document_index[i].end() || it__ != Mc_index.end())
			{

				temp *= (lim*p_Md + (1 - lim)*p_Mc);
			}
			else
			{
				temp *= (1.0 / length);

			}


		}

		if (temp == 1 || flag == 0) temp = 0;
		buff.ID = i;
		buff.sim = temp;
		doc_index.push_back(buff);
	}
	std::sort(doc_index.begin(), doc_index.end(), compare);
	return 0;
}

//BM25相似度计算函数
//入口参数：query：待查询的字符串
//doc_index:保存查询结果（按照文档相似度排序之后的文档编号）
//inverse:相关文档的索引，保存了所有与当前查询相关的文档编号
//vector<map<string, double> > every_document_index; //每篇文档的tf
//vector<map<string, double> > every_document_index_fre; //每篇文档的词频
//vector<vector<Fre> > word_index; //文档中所有词的信息
//map<string, double> index;//文档中所有词的文档频率
//double length;//所有文档的平均长度
//以上变量均是类的成员变量，在预处理的时候已经将结果存入
int Preprocession::count_score(string query, vector<ID_sim> &doc_index,set<int>inverse)
{
	vector<string> query_string;
	depart(query, query_string);//调用depart函数将查询字符串按空格分解为一个个词存入向量query_string中
	doc_index.clear();
	map<string, double> map_query;
	//统计查询中的每个词的tf，并将结果写入map中
	for (unsigned int t = 0; t < query_string.size(); t++)
	{
		map<string, double>::iterator it = map_query.find(query_string[t]);
		if (it == map_query.end())
		{
			map_query[query_string[t]] = 1;
		}
		else
		{
			map_query[query_string[t]]++;
		}
	}

	//计算文档平均长度
	
	double avgdl = length / word_index.size(); //其中length是类的成员变量，在预处理中已经计算完，为文档总长度

	//计算score

	ID_sim buff; //ID_sim为自定义的结构体数据类型,存放文档编号和文档相似度
	double score = 0;
	double IDF, R, K, f, qf, b, k1, k2;
	b = 0.75;
	k1 = 2;
	k2 = 1;
	double x = word_index.size();
	double y;
	map<string, double>::iterator it_;
	map<string, double>::iterator it__;

	for (unsigned int i = 0; i < word_index.size(); i++) //计算相似度，word_index为一个二维向量，一维是文档编号，保存了所有词的信息，在预处理的时候存入
	{
		if ((inverse.find(i)) == inverse.end())continue; //若当前文档完全不相关则跳过
		score = 0;
		for (map<string, double>::iterator it = map_query.begin(); it != map_query.end(); it++)
		{
		    it_ = index.find(it->first);  //index为类成员变量，保存了所有词对应的文档频率，为字典结构
			if (it_ ==index.end())
			{
				y = 0;
			}
			else
			{
				y = x / it_->second;
			}
			IDF = log10((x-y+0.5)/(y+0.5));
			K =k1* (1-b+b*(word_index[i].size()/avgdl));
			qf = it->second ;
		    it__= every_document_index_fre[i].find(it->first);//保存了每篇文档中的单词频率，类的成员变量
			if (it__==every_document_index_fre[i].end())
			{
				f = 0;
			}
			else
			{
				f = it__->second;
			}
			R = (f*(k1+1)*qf*(k2+1))/((f+K)*(qf+k2));
			score += (IDF*R);
		}
		buff.ID = i;
		buff.sim = score;//存入相似度
		doc_index.push_back(buff);//保存结果
	}

	std::sort(doc_index.begin(), doc_index.end(), compare); //按照相似度排序
	return 0;
}

//向量空间模型相似度计算
//入口参数：query:待查询的字符串
//doc_index:保存查询结果（按照文档相似度排序之后的文档编号）
//inverse:相关文档的索引，保存了所有与当前查询相关的文档编号
//vector<map<string, double> > every_document_index; //每篇文档的tf
//vector<map<string, double> > every_document_index_fre; //每篇文档的词频
//vector<double> every_document_sqrtlen;//每篇文档的向量模长
//vector<vector<Fre> > word_index; //文档中所有词的信息


//以上变量均是类的成员变量，在预处理的时候已经将结果存入
int Preprocession::count_sim(string query,  vector<ID_sim> &doc_index,set<int>inverse) 
{
	map<string, int> map_query;
	vector<string> query_string;
	depart(query, query_string);//调用depart函数将查询字符串按空格分解为一个个词存入向量query_string中
	map<string, double> query_word_weight;
	doc_index.clear();
	//统计查询中的tf，并将结果写入map中
	for (unsigned int t = 0; t<query_string.size(); t++)
	{
		map<string, int>::iterator it = map_query.find(query_string[t]);
		if (it == map_query.end())
		{
			map_query[query_string[t]] = 1;
		}
		else
		{
			map_query[query_string[t]]++;
		}
	}

	//计算查询中每个单词的idf和weight
	for (map<string, int>::iterator it = map_query.begin(); it != map_query.end(); it++)
	{
		int frequency = it->second;
		double k = 0;
		double x = (frequency*1.0) / query_string.size();
		UDT_MAP_INT_CSTRING::iterator t = index.find(it->first);
		if (t == index.end())
		{
			k = 0;
		}
		else
		{
			k = index[it->first];

		}
		query_word_weight[it->first] = x*log10(k + 1);
	}

	//计算查询向量的模长
	double query_weight_length = 0;
	for (map<string, double>::iterator it = query_word_weight.begin(); it != query_word_weight.end(); it++)
	{

		query_weight_length += (it->second*it->second);
	}
	query_weight_length = sqrt(query_weight_length);  


	//计算相似度
	double  sim ;
	double  temp ;
	ID_sim  buff;//ID_sim为自定义的结构体数据类型,存放文档编号和文档相似度
	UDT_MAP_INT_CSTRING::iterator ti;
	for (unsigned int i = 0; i < word_index.size(); i++)//word_index为类的成员变量，在预处理中已经计算好，存放了所有词的信息的二维向量
	{
		if ((inverse.find(i)) == inverse.end())continue;
		sim = 0;
		temp = 0;
		buff;
		
		for (unsigned int j = 0; j < word_index[i].size(); j++)
		{
			 ti = query_word_weight.find(word_index[i][j].name);//查询中是否存在当前文档中的当前单词
			if (ti != query_word_weight.end())
			{
				temp += (word_index[i][j].w_td*ti->second); //存在则累加到分子
			}

		}

		sim = temp / every_document_sqrtlen[i] / query_weight_length; //与当前文档的相似度
		if (temp == 0 || every_document_sqrtlen[i] == 0 || query_weight_length == 0) sim = 0;//every_document_sqrtlen为类的成员变量，保存每篇文档的长度，预处理时计算


		buff.ID = i;
		buff.sim = sim;
		doc_index.push_back(buff);//存入结果向量中
	}

	//对文档按照相似度排序
	std::sort(doc_index.begin(), doc_index.end(), compare);
	return 0;
}

void Preprocession::write_textRank(vector<string> txt_name)
{
	for (unsigned int i=0;i<all_words_in_doc.size();i++)
	{
	    ofstream f("C:\\Users\\Jackdong\\Desktop\\SearchEngineShenjundong2010\\SearchEngineShenJundong2017（5）\\SearchEngineShenJundong\\pagerank\\"+txt_name[i]+".txt");
		for (unsigned int j=0;j<all_words_in_doc[i].size();j++)
		{
			f << all_words_in_doc[i][j] << endl;
		}
		f.close();
	}
}


void Preprocession::filter(string& s)   //delete the sign of attribute after word segmentation
{
	int pos1;
	string temp;
	while (true)
	{
		pos1 = s.find('/');
		if (pos1 == string::npos) break;

		temp += s.substr(0, pos1);
		s = s.substr(pos1 + 1);
		int pos2 = s.find(" ");
		if (pos2 == string::npos) break;
		s = s.substr(pos2);
	}
	s = temp;
}

void Preprocession::depart(string s, vector<string>&p)//将每一行的歌名分词后的各个词分别取出存入vector
{
	int pos1;
	string temp;
	while (true)
	{
		pos1 = s.find(' ');
		if (pos1 == string::npos) break;
		temp = s.substr(0, pos1);
		//transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
		p.push_back(temp);
		s = s.substr(pos1 + 1);
	}
	//transform(s.begin(), s.end(), s.begin(), ::tolower);
	p.push_back(s);
}

int Preprocession::count_fre(vector<string> temp, string s)
{
	int fre = 0;
	for (unsigned int i = 0; i<temp.size(); i++)
	{
		if (temp[i] == s) fre++;
	}
	return fre;
}


Preprocession::Preprocession(list<string> *s)
{
	L = s;
	ifstream file("stopwords.txt");
	string buff;
	while (file>>buff)
	{
		stop_word[buff]=1;
	}

}
vector<vector<string>> Preprocession::getText()
{
	return all_words_in_doc;
}
bool Preprocession::contains(list<string> &result_list, string present_string)
{
	list<string>::iterator it;
	it = result_list.begin();

	if (it != result_list.end() && present_string == *it)
		return true;
	else
		return false;
}

void Preprocession::reduction()
{
	(*L).sort();
	//(*L).unique();
	list<string> result;
	list<string>::iterator i;
	for (i = (*L).begin(); i != (*L).end(); i++)
	{
		if (!contains(result, *i))
		{
			result.push_front(*i);
		}
	}
	(*L).clear();
	list<string>::iterator it;
	for (it = result.begin(); it != result.end(); it++)
	{
		(*L).push_front(*it);
	}
}

void Preprocession::count_weight()
{
	int k = 0;
	double frequency = 0;
	for (unsigned int i = 0; i < all_words_in_doc.size(); i++, k++)
	{
		vector<Fre>temp;
		vector<string> doc=all_words_in_doc[i];
		//doc.clear();
		//depart(*i, doc);
		index_tf.clear();
		for (unsigned int t = 0; t<doc.size(); t++)
		{
			UDT_MAP_INT_CSTRING::iterator it = index_tf.find(doc[t]);
			if (it == index_tf.end())
			{
				index_tf[doc[t]] = 1;
			}
			else
			{
				index_tf[doc[t]]++;
			}
		}

		for (UDT_MAP_INT_CSTRING::iterator it = index_tf.begin(); it != index_tf.end(); it++)
		{
			frequency = it->second;
			double x = (frequency*1.0) / doc.size();
			double y = frequency;
			UDT_MAP_INT_CSTRING::iterator t = index.find(it->first);
			if (t == index.end())
			{
				index[it->first] = 1;
			}
			else
			{
				index[it->first]++;
			}
			//Fre buff(it->first,k,it->second);
			Fre buff(it->first, k, x, y);
			temp.push_back(buff);
		}
		word_index.push_back(temp);
	}
	int x_y = word_index.size();
	for (unsigned int i = 0; i<word_index.size(); i++)
	{
		for (unsigned int j = 0; j<word_index[i].size(); j++)
		{
			//p[i][j].idf = index[p[i][j].name];
			word_index[i][j].idf = x_y*1.0 / index[word_index[i][j].name];

			word_index[i][j].w_td = log10(word_index[i][j].idf)*word_index[i][j].tf;
		}

	}
	for (unsigned int i = 0; i < word_index.size(); i++)
	{
		for (unsigned int j = 0; j < word_index[i].size(); j++)
		{
			index[word_index[i][j].name] = word_index[i][j].idf;
		}

	}

	
	length = 0;
	for (unsigned int i = 0; i <word_index.size(); i++)
	{
		for (unsigned int j = 0; j < word_index[i].size(); j++)
		{
			length++;
			map<string, double>::iterator it = Mc_index.find(word_index[i][j].name);
			if (it == Mc_index.end())
			{
				Mc_index[word_index[i][j].name] =word_index[i][j].frequency;
			}
			else
			{
				Mc_index[word_index[i][j].name] +=word_index[i][j].frequency;
			}
		}
	}

	for (unsigned int i=0;i<word_index.size();i++)
	{
		map<string, double> buff;
		buff.clear();
		for (unsigned int j = 0; j < word_index[i].size(); j++)
		{
			buff[word_index[i][j].name] = word_index[i][j].tf;
		}
		every_document_index.push_back(buff);
	}

	for (unsigned int i=0;i<word_index.size();i++)
	{
		map<string, double> buff;
		buff.clear();
		for (unsigned int j = 0; j < word_index[i].size(); j++)
		{
			buff[word_index[i][j].name] = word_index[i][j].frequency;
		}
		every_document_index_fre.push_back(buff);
	}


	for (unsigned int i=0;i<word_index.size();i++)
	{
		double buff = 0;
		for (unsigned int j = 0; j < word_index[i].size(); j++)
		{
			buff += (word_index[i][j].w_td*word_index[i][j].w_td);
			
			
		}
		buff = sqrt(buff);
		every_document_sqrtlen.push_back(buff);
	}
}

void Preprocession::readfile(string name)
{
	filename = name;
}

void Preprocession::getData(string fname,int n)
{
		string buff = "";
		string temp;
		ifstream fin(fname);
		while (getline(fin, temp))
		{
			buff += temp;
		}
		string_processdata(buff);
		//cout << buff << endl;
	if(n==0)
	{
		//string x;
		removeStopword(buff);//结果同时存入类的成员变量中
		//(*L).push_back(x);
	}
	else
	{
		//string x;
		textRank(buff);
		//(*L).push_back(x);
	}
}

void Preprocession::getData_pagerank(string fname,int n)
{
	ifstream fin(fname);
	string temp;
	int count = 0;
	vector<string> buff;
	while (getline(fin,temp))
	{
		buff.push_back(temp);
		//cout << temp<<" "<<count<< endl;
		count++;
		if (count == n)break;
	}
	all_words_in_doc.push_back(buff);
}

//基于pagerank的关键词抽取算法
//入口参数：buff:每篇文档的所有词组成的字符串
//以下是对一篇文档的处理
void Preprocession::textRank(string buff)								//对每篇文档进行关键字提取
{
	
	vector <string> prog;
	map<string, double> dic;
	vector<map<string, double> >map_prog;
	vector<string> doc;
	depart(buff, doc);//将每篇文档分为一个个词存入doc向量中
	vector<ID_sim> voca;
	vector<string> word;
	vector< vector<int> > weight;
	
	for (unsigned int t = 0,y=0; t<doc.size(); t++)
	{
		ID_sim x;
		UDT_MAP_INT_CSTRING::iterator it = dic.find(doc[t]);
		if (it == dic.end())
		{
			if (is_stop_word(doc[t])) continue;							//去除停用词
			dic[doc[t]] = 1;
			x.ID = y;
			x.sim = 1;
			voca.push_back(x);											//保存文章中的每一个词
			word.push_back(doc[t]);
			y++;
		}
	}
	

	
	while (true)															//按照句号取出每句话，分别存放
	{
		int pos = buff.find("。");
		if (pos == string::npos) break;
		string temp = buff.substr(0, pos);
		prog.push_back(temp);                                          //prog保存每个句子
		buff = buff.substr(pos+1);
	}



	for (unsigned int j = 0; j < prog.size(); j++)
	{
		vector<string> word_list;
		depart(prog[j], word_list);										//对每个句子进行分词，并进行统计，写入map中
		map<string, double> temp_map;
		for (unsigned int i=0;i<word_list.size();i++)
		{
			UDT_MAP_INT_CSTRING::iterator it = temp_map.find(word_list[i]);
			if (it == temp_map.end())
			{
				temp_map[word_list[i]] = 1;
			}
		}
		map_prog.push_back(temp_map);							//字典向量保存每个句子的统计结果map			
	}
	


	
	for (unsigned int i=0;i<word.size();i++)//列出词与词之间的关系矩阵,若词与词同时出现在一个句子中则二维矩阵中的对应位置数值加1
	{
		vector<int> temp_weight;
		for (unsigned int j=0;j<word.size();j++)
		{
			if (i == j)                   //二维矩阵的主对角线为0(自己和自己)
			{
				temp_weight.push_back(0);
				continue;
			}
			int w = 0;
			for (unsigned int k = 0; k < map_prog.size(); k++)
			{
				map<string,double>::iterator it1 = map_prog[k].find(word[i]);
				map<string,double>::iterator it2 = map_prog[k].find(word[j]);
				if (it1==map_prog[k].end()||it2==map_prog[k].end())
				{
					continue;
				}
				w++;
			}
			temp_weight.push_back(w);
		}
		weight.push_back(temp_weight);
	}
	
	
	vector<int> component;//与每个单词有联系的总词数
	for (unsigned int i = 0; i<weight.size(); i++)//对二维矩阵的每一列进行求和汇总
	{
		int temp = 0;
		for (unsigned int j = 0; j<weight[i].size(); j++)
		{
			temp += weight[i][j];
		}
		component.push_back(temp);
	}
	


	vector<ID_sim> voca_backup = voca;		//备份一下每次循环前一次的各个单词的权重
	double deviation=1;
	double deviation_backup=0;
	while (deviation>0.01)   //前一次与后一次的差值若小于0.01视为结束跳出
	{
		deviation = 0;
		for (unsigned int i = 0; i < voca.size(); i++)//对每个单词分配到的权重更新
		{
			double temp = 0;
			for (unsigned int j = 0; j < weight[i].size(); j++)
			{
				if (component[j] == 0)continue;
				temp += voca_backup[j].sim*weight[i][j]/(component[j]);
			}
			deviation += abs(temp-voca[i].sim);
			voca[i].sim = temp;
		};
		if (deviation_backup == deviation)break;//前一次与后裔词的值若相等则跳出循环
		deviation_backup = deviation;
		voca_backup = voca;
	}
	std::sort(voca.begin(), voca.end(), compare);//按照最终的权重排序



	vector<string> tempt;
	for (unsigned int i=0;i<100&&i<voca.size();i++)//取出排名前100的存入pagerank结果文档
	{
		tempt.push_back(word[voca[i].ID]);
	}
	all_words_in_doc.push_back(tempt);//保存结果到向量中
}

bool Preprocession::is_stop_word(string word)
{
	if (word == "")return true;
	string::size_type pos;
	pos = word.find('/w');
	if (pos != string::npos)return true;
	filter(word);
	map<string, double>::iterator it = stop_word.find(word);
	if (it != stop_word.end())return true;
	return false;
}

void Preprocession::removeStopword(string buff)
{
	vector<string> doc;
	depart(buff, doc);
	vector<string>hhhh;
	for (unsigned int t = 0; t<doc.size(); t++)
	{
			if (is_stop_word(doc[t])) continue;												//去除停用词
			
			hhhh.push_back(doc[t]);
	}
	all_words_in_doc.push_back(hhhh);

}

void Preprocession::readData(vector<string>&file_name,int n)
{
	struct _finddata_t fileinfo;
	string in_path;
	string in_name;
	in_path=filename;
	string curr = in_path + "\\*.txt";
	long handle;
	if ((handle = _findfirst(curr.c_str(), &fileinfo)) == -1L)
	{
		cout << "没有找到匹配文件!" << endl;
		return ;
	}
	else
	{
		in_name = in_path + "\\" + fileinfo.name;
		string x = fileinfo.name;
		int pos = x.find(".");
		//x = x.substr(0,pos);
		file_name.push_back(x);
		getData(in_name,n);
		while (!(_findnext(handle, &fileinfo)))
		{
			in_name = in_path + "\\" + fileinfo.name;
			string x = fileinfo.name;
			int pos = x.find(".");
			//x = x.substr(0, pos);
			file_name.push_back(x);
			//cout << x<<endl;
			getData(in_name,n);
		}
		_findclose(handle);
	}

}

void Preprocession::readData_pagerank(vector<string>&file_name,int n)
{
	struct _finddata_t fileinfo;
	string in_path;
	string in_name;
	in_path = "pagerank";
	string curr = in_path + "\\*.txt";
	long handle;
	if ((handle = _findfirst(curr.c_str(), &fileinfo)) == -1L)
	{
		cout << "没有找到匹配文件!" << endl;
		return;
	}
	else
	{
		in_name = in_path + "\\" + fileinfo.name;
		string x = fileinfo.name;
		//int pos = x.find(".");
		//x = x.substr(0, pos);
		file_name.push_back(x);
		getData_pagerank(in_name,n);
		while (!(_findnext(handle, &fileinfo)))
		{
			in_name = in_path + "\\" + fileinfo.name;
			string x = fileinfo.name;
			//int pos = x.find(".");
			//x = x.substr(0, pos);
			file_name.push_back(x);
			//cout << x<<endl;
			getData_pagerank(in_name,n);
		}
		_findclose(handle);
	}



}






void Preprocession::string_process(string &buff, int flag)
{

	if (flag == 0)
	{
		int pos1 = buff.find("\t");
		string temp = buff.substr(pos1 + 1);
		int pos2 = temp.find("\t");
		buff = buff.substr(0, pos1 + pos2 + 1);
	}
	else if (flag == 1)
	{
		int pos = buff.find("\t");
		buff = buff.substr(pos + 1);
		pos = buff.find("\t");
		buff = buff.substr(pos + 1);
	}
	else
	{
		int pos1 = buff.find("\t");
		buff = buff.substr(0, pos1);

	}
}

void Preprocession::string_processdata(string &buff)
{

		int pos1 = buff.find("title_token:");
		string temp = buff.substr(pos1 + 13);
		buff = temp;
		
}


