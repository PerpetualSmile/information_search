#include "stdafx.h"
#include "OutputModel.h"
#include"numeric"
#include<vector>
#include <iostream>
#include<iomanip>
#include<cmath>
OutputModel::OutputModel(vector<ID_sim>doc)
{
	for (unsigned int i=0;i<doc.size();i++)
	{
		if (doc[i].sim == 0) break;
		sim.push_back(doc[i].sim);
	}
}

double OutputModel::averageDeviation()
{
	double buff1;
	double buff2;
	double average_above;
	double average_below;
	double sum_above;
	double sum_below;
	vector<sim_deviation>result;
	sim_deviation temp;
	for (unsigned int i=0;i<sim.size()-1;i++)
	{
		sum_above = accumulate(sim.begin(),sim.begin()+i+1,0.0);
		sum_below = accumulate(sim.begin()+i+1,sim.end(),0.0);
		//cout <<fixed<<setprecision(20)<< sum_above <<" "<<sum_below<<" "<<sum_above+sum_below<< endl;
		average_above = sum_above / (i + 1);
		average_below = sum_below / (sim.size()-i-1);
		temp.similarity = sim[i];
		temp.deviation = average_above - average_below;
		//cout << average_above<<"  "<<average_below<<" "<<temp.deviation<<endl ;
		result.push_back(temp);
	}
	buff1 = result[0].deviation;
	buff2 = result[0].similarity;
	for (unsigned int i=1;i<result.size();i++)
	{
		if (result[i].deviation>=buff1)
		{
			buff1 = result[i].deviation;
			buff2 = result[i].similarity;
		}
	}
	return buff2;
}

double OutputModel::pairDeviation()
{
	double buff1;
	double buff2;

	vector<sim_deviation>result;
	sim_deviation temp;
	for (unsigned int i = 0; i < sim.size() - 1; i++)
	{
		temp.similarity = sim[i];
		temp.deviation =sim[i]-sim[i+1];
		result.push_back(temp);
	}
	buff1 = result[0].deviation;
	buff2 = result[0].similarity;
	for (unsigned int i = 1; i < result.size(); i++)
	{
		if (result[i].deviation >= buff1)
		{
			buff1 = result[i].deviation;
			buff2 = result[i].similarity;
		}
	}
	return buff2;
}

double OutputModel::windowDeviation(int n)
{
	double buff1;
	double buff2;

	vector<sim_deviation>result;
	sim_deviation temp;
	for (unsigned int i = 0; i < sim.size() - n; i++)
	{
		temp.similarity = sim[i+n];
		//temp.similarity = sim[i ];
		temp.deviation = sim[i] - sim[i + n];
		//cout << sim[i] << " " << sim[i + n] << " " << temp.deviation << endl;
		result.push_back(temp);
	}
	buff1 = result[0].deviation;
	buff2 = result[0].similarity;
	for (unsigned int i = 1; i < result.size(); i++)
	{
		if (result[i].deviation >= buff1)
		{
			buff1 = result[i].deviation;
			buff2 = result[i].similarity;
		}
	}
	return buff2;
	
}

double OutputModel::sigmoid()
{
	double max_sim = sim[0];
	double min_sim = sim[sim.size() - 1];
	double n = (max_sim - min_sim) / 2;
	unsigned int i = 0;
	for (; i < sim.size(); i++)
	{
		if (sim[i] <= n)
		{
			break;
		}
	}

	return ((n - sim[i]) > (sim[i - 1] - n)) ? sim[i - 1] : sim[i];
	//double max_sim = sim[0];
	//double min_sim = sim[sim.size()-1];
	//double a = max_sim - min_sim;
	//double d = min_sim;
	//double c;
	//for(c=1;c>0;c-=0.01)
	//{
	//	double sum=0;
	//	for(unsigned int x=0;x<sim.size();x++)
	//	{
	//		double temp;
	//		temp=sim[x]-fsigmoid(a, sim.size() / 2, c, d, x);
	//		//cout <<sim[x]<<"  "<< fsigmoid(a, sim.size() / 2, c, d, x) << endl;
	//		if (temp < 0)temp = -temp;
	//		sum += temp;
	//	}
	//	cout << sum << " " <<c <<endl;
	//}
	//return 0.5;
}


OutputModel::~OutputModel()
{
}

double OutputModel::fsigmoid(double a, double b, double c, double d, double x)
{
	return a/(1+exp(c*(x- b)))+d;
}
