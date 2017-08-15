#include "stdafx.h"
#include "EvaluatorShenJundong.h"


EvaluatorShenJundong::EvaluatorShenJundong(double  a, double  b, double  c)
{
	num_of_correct_results = a;
	num_of_all_results = b;
	num_of_all = c;
	Precision = num_of_correct_results / num_of_all_results;
	Recall = num_of_correct_results / num_of_all;
	F = (Precision*Recall * 2) / (Precision + Recall);
}


double EvaluatorShenJundong::getPrecision()
{
	return Precision;
}

double EvaluatorShenJundong::getF()
{
	return F;
}

double EvaluatorShenJundong::getRecall()
{
	return Recall;
}


EvaluatorShenJundong::~EvaluatorShenJundong()
{
}
