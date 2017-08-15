#pragma once
class EvaluatorShenJundong
{

	double Precision;
	double Recall;
	double F;
	double  num_of_all_results;
	double  num_of_correct_results;
	double  num_of_all;
public:
	EvaluatorShenJundong(double a, double b, double c);
	double getPrecision();
	double getRecall();
	double getF();
	~EvaluatorShenJundong();
};

