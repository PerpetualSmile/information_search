#pragma once
#include<vector>
#include"Preprocession.h"
class sim_deviation					
{
public:
	double deviation;
	double similarity;
};
class OutputModel
{
	double fsigmoid(double a,double b,double c,double d,double x);
	vector<double>sim;
public:
	OutputModel(vector<ID_sim>);
	double averageDeviation();
	double pairDeviation();
	double windowDeviation(int i);
	double sigmoid();
	~OutputModel();
};

