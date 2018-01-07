#pragma once
#include <vector>
#include <random> // rand is biased to lower numbers
#include <algorithm>

#include "NeuralNetwork.h"

using namespace std;

struct Genome
{
	vector<double> vWeights;
	double dFitness;

	Genome() : dFitness(0) {}
	Genome(vector <double> w, double f) : vWeights(w), dFitness(f) {}

	//overload '<' used for sorting
	friend bool operator<(const Genome& lhs, const Genome& rhs)
	{
		return (lhs.dFitness < rhs.dFitness);
	}
};

class GeneticAlgorithm
{
public:

	GeneticAlgorithm(int popsize, double MutRat, double CrossRat, int numweights);
	~GeneticAlgorithm();

	//this runs the GA for one generation.

	vector<Genome> Epoch(vector<Genome> &old_pop);

	//-------------------accessor methods
	vector<Genome> GetChromos()const { return m_vecPop; }
	double AverageFitness()const { return m_dTotalFitness / m_iPopSize; }
	double BestFitness()const { return m_dBestFitness; }

private:

	//this holds the entire population of chromosomes
	vector <Genome> m_vecPop;

	//size of population
	int m_iPopSize;

	//amount of weights per chromo
	int m_iChromoLength;

	//NumOfWeights
	int numOfWeights;

	//total fitness of population
	double m_dTotalFitness;

	//best fitness this population
	double m_dBestFitness;

	//average fitness
	double m_dAverageFitness;

	//worst
	double m_dWorstFitness;

	//keeps track of the best genome
	int m_iFittestGenome;

	//probability that a chromosomes bits will mutate.

	//Try figures around 0.05 to 0.3 ish

	double m_dMutationRate;

	//probability of chromosomes crossing over bits

	//0.7 is pretty good

	double m_dCrossoverRate;

	//generation counter

	int m_cGeneration;

	void Crossover(const vector<double> &mum, const vector<double> &dad, vector<double> &baby1, vector<double> &baby2);

	void Mutate(vector<double> &chromo);

	Genome GetChromoRoulette();

	void GrabNBest(int NBest, const int NumCopies, vector<Genome> &vecPop);

	void CalculateBestWorstAvTot();

	void Reset();
};

