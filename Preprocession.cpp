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

bool compare(ID_sim a, ID_sim b)//�������ƶȶ��ĵ�����
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
	//ͳ�Ʋ�ѯ�е�tf���������д��map��
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

//BM25���ƶȼ��㺯��
//��ڲ�����query������ѯ���ַ���
//doc_index:�����ѯ����������ĵ����ƶ�����֮����ĵ���ţ�
//inverse:����ĵ��������������������뵱ǰ��ѯ��ص��ĵ����
//vector<map<string, double> > every_document_index; //ÿƪ�ĵ���tf
//vector<map<string, double> > every_document_index_fre; //ÿƪ�ĵ��Ĵ�Ƶ
//vector<vector<Fre> > word_index; //�ĵ������дʵ���Ϣ
//map<string, double> index;//�ĵ������дʵ��ĵ�Ƶ��
//double length;//�����ĵ���ƽ������
//���ϱ���������ĳ�Ա��������Ԥ�����ʱ���Ѿ����������
int Preprocession::count_score(string query, vector<ID_sim> &doc_index,set<int>inverse)
{
	vector<string> query_string;
	depart(query, query_string);//����depart��������ѯ�ַ������ո�ֽ�Ϊһ�����ʴ�������query_string��
	doc_index.clear();
	map<string, double> map_query;
	//ͳ�Ʋ�ѯ�е�ÿ���ʵ�tf���������д��map��
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

	//�����ĵ�ƽ������
	
	double avgdl = length / word_index.size(); //����length����ĳ�Ա��������Ԥ�������Ѿ������꣬Ϊ�ĵ��ܳ���

	//����score

	ID_sim buff; //ID_simΪ�Զ���Ľṹ����������,����ĵ���ź��ĵ����ƶ�
	double score = 0;
	double IDF, R, K, f, qf, b, k1, k2;
	b = 0.75;
	k1 = 2;
	k2 = 1;
	double x = word_index.size();
	double y;
	map<string, double>::iterator it_;
	map<string, double>::iterator it__;

	for (unsigned int i = 0; i < word_index.size(); i++) //�������ƶȣ�word_indexΪһ����ά������һά���ĵ���ţ����������дʵ���Ϣ����Ԥ�����ʱ�����
	{
		if ((inverse.find(i)) == inverse.end())continue; //����ǰ�ĵ���ȫ�����������
		score = 0;
		for (map<string, double>::iterator it = map_query.begin(); it != map_query.end(); it++)
		{
		    it_ = index.find(it->first);  //indexΪ���Ա���������������дʶ�Ӧ���ĵ�Ƶ�ʣ�Ϊ�ֵ�ṹ
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
		    it__= every_document_index_fre[i].find(it->first);//������ÿƪ�ĵ��еĵ���Ƶ�ʣ���ĳ�Ա����
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
		buff.sim = score;//�������ƶ�
		doc_index.push_back(buff);//������
	}

	std::sort(doc_index.begin(), doc_index.end(), compare); //�������ƶ�����
	return 0;
}

//�����ռ�ģ�����ƶȼ���
//��ڲ�����query:����ѯ���ַ���
//doc_index:�����ѯ����������ĵ����ƶ�����֮����ĵ���ţ�
//inverse:����ĵ��������������������뵱ǰ��ѯ��ص��ĵ����
//vector<map<string, double> > every_document_index; //ÿƪ�ĵ���tf
//vector<map<string, double> > every_document_index_fre; //ÿƪ�ĵ��Ĵ�Ƶ
//vector<double> every_document_sqrtlen;//ÿƪ�ĵ�������ģ��
//vector<vector<Fre> > word_index; //�ĵ������дʵ���Ϣ


//���ϱ���������ĳ�Ա��������Ԥ�����ʱ���Ѿ����������
int Preprocession::count_sim(string query,  vector<ID_sim> &doc_index,set<int>inverse) 
{
	map<string, int> map_query;
	vector<string> query_string;
	depart(query, query_string);//����depart��������ѯ�ַ������ո�ֽ�Ϊһ�����ʴ�������query_string��
	map<string, double> query_word_weight;
	doc_index.clear();
	//ͳ�Ʋ�ѯ�е�tf���������д��map��
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

	//�����ѯ��ÿ�����ʵ�idf��weight
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

	//�����ѯ������ģ��
	double query_weight_length = 0;
	for (map<string, double>::iterator it = query_word_weight.begin(); it != query_word_weight.end(); it++)
	{

		query_weight_length += (it->second*it->second);
	}
	query_weight_length = sqrt(query_weight_length);  


	//�������ƶ�
	double  sim ;
	double  temp ;
	ID_sim  buff;//ID_simΪ�Զ���Ľṹ����������,����ĵ���ź��ĵ����ƶ�
	UDT_MAP_INT_CSTRING::iterator ti;
	for (unsigned int i = 0; i < word_index.size(); i++)//word_indexΪ��ĳ�Ա��������Ԥ�������Ѿ�����ã���������дʵ���Ϣ�Ķ�ά����
	{
		if ((inverse.find(i)) == inverse.end())continue;
		sim = 0;
		temp = 0;
		buff;
		
		for (unsigned int j = 0; j < word_index[i].size(); j++)
		{
			 ti = query_word_weight.find(word_index[i][j].name);//��ѯ���Ƿ���ڵ�ǰ�ĵ��еĵ�ǰ����
			if (ti != query_word_weight.end())
			{
				temp += (word_index[i][j].w_td*ti->second); //�������ۼӵ�����
			}

		}

		sim = temp / every_document_sqrtlen[i] / query_weight_length; //�뵱ǰ�ĵ������ƶ�
		if (temp == 0 || every_document_sqrtlen[i] == 0 || query_weight_length == 0) sim = 0;//every_document_sqrtlenΪ��ĳ�Ա����������ÿƪ�ĵ��ĳ��ȣ�Ԥ����ʱ����


		buff.ID = i;
		buff.sim = sim;
		doc_index.push_back(buff);//������������
	}

	//���ĵ��������ƶ�����
	std::sort(doc_index.begin(), doc_index.end(), compare);
	return 0;
}

void Preprocession::write_textRank(vector<string> txt_name)
{
	for (unsigned int i=0;i<all_words_in_doc.size();i++)
	{
	    ofstream f("C:\\Users\\Jackdong\\Desktop\\SearchEngineShenjundong2010\\SearchEngineShenJundong2017��5��\\SearchEngineShenJundong\\pagerank\\"+txt_name[i]+".txt");
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

void Preprocession::depart(string s, vector<string>&p)//��ÿһ�еĸ����ִʺ�ĸ����ʷֱ�ȡ������vector
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
		removeStopword(buff);//���ͬʱ������ĳ�Ա������
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

//����pagerank�Ĺؼ��ʳ�ȡ�㷨
//��ڲ�����buff:ÿƪ�ĵ������д���ɵ��ַ���
//�����Ƕ�һƪ�ĵ��Ĵ���
void Preprocession::textRank(string buff)								//��ÿƪ�ĵ����йؼ�����ȡ
{
	
	vector <string> prog;
	map<string, double> dic;
	vector<map<string, double> >map_prog;
	vector<string> doc;
	depart(buff, doc);//��ÿƪ�ĵ���Ϊһ�����ʴ���doc������
	vector<ID_sim> voca;
	vector<string> word;
	vector< vector<int> > weight;
	
	for (unsigned int t = 0,y=0; t<doc.size(); t++)
	{
		ID_sim x;
		UDT_MAP_INT_CSTRING::iterator it = dic.find(doc[t]);
		if (it == dic.end())
		{
			if (is_stop_word(doc[t])) continue;							//ȥ��ͣ�ô�
			dic[doc[t]] = 1;
			x.ID = y;
			x.sim = 1;
			voca.push_back(x);											//���������е�ÿһ����
			word.push_back(doc[t]);
			y++;
		}
	}
	

	
	while (true)															//���վ��ȡ��ÿ�仰���ֱ���
	{
		int pos = buff.find("��");
		if (pos == string::npos) break;
		string temp = buff.substr(0, pos);
		prog.push_back(temp);                                          //prog����ÿ������
		buff = buff.substr(pos+1);
	}



	for (unsigned int j = 0; j < prog.size(); j++)
	{
		vector<string> word_list;
		depart(prog[j], word_list);										//��ÿ�����ӽ��зִʣ�������ͳ�ƣ�д��map��
		map<string, double> temp_map;
		for (unsigned int i=0;i<word_list.size();i++)
		{
			UDT_MAP_INT_CSTRING::iterator it = temp_map.find(word_list[i]);
			if (it == temp_map.end())
			{
				temp_map[word_list[i]] = 1;
			}
		}
		map_prog.push_back(temp_map);							//�ֵ���������ÿ�����ӵ�ͳ�ƽ��map			
	}
	


	
	for (unsigned int i=0;i<word.size();i++)//�г������֮��Ĺ�ϵ����,�������ͬʱ������һ�����������ά�����еĶ�Ӧλ����ֵ��1
	{
		vector<int> temp_weight;
		for (unsigned int j=0;j<word.size();j++)
		{
			if (i == j)                   //��ά��������Խ���Ϊ0(�Լ����Լ�)
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
	
	
	vector<int> component;//��ÿ����������ϵ���ܴ���
	for (unsigned int i = 0; i<weight.size(); i++)//�Զ�ά�����ÿһ�н�����ͻ���
	{
		int temp = 0;
		for (unsigned int j = 0; j<weight[i].size(); j++)
		{
			temp += weight[i][j];
		}
		component.push_back(temp);
	}
	


	vector<ID_sim> voca_backup = voca;		//����һ��ÿ��ѭ��ǰһ�εĸ������ʵ�Ȩ��
	double deviation=1;
	double deviation_backup=0;
	while (deviation>0.01)   //ǰһ�����һ�εĲ�ֵ��С��0.01��Ϊ��������
	{
		deviation = 0;
		for (unsigned int i = 0; i < voca.size(); i++)//��ÿ�����ʷ��䵽��Ȩ�ظ���
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
		if (deviation_backup == deviation)break;//ǰһ�������ʵ�ֵ�����������ѭ��
		deviation_backup = deviation;
		voca_backup = voca;
	}
	std::sort(voca.begin(), voca.end(), compare);//�������յ�Ȩ������



	vector<string> tempt;
	for (unsigned int i=0;i<100&&i<voca.size();i++)//ȡ������ǰ100�Ĵ���pagerank����ĵ�
	{
		tempt.push_back(word[voca[i].ID]);
	}
	all_words_in_doc.push_back(tempt);//��������������
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
			if (is_stop_word(doc[t])) continue;												//ȥ��ͣ�ô�
			
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
		cout << "û���ҵ�ƥ���ļ�!" << endl;
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
		cout << "û���ҵ�ƥ���ļ�!" << endl;
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


