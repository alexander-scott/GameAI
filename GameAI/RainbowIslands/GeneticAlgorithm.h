#pragma once
#include <vector>
#include <random> // rand is biased to lower numbers
#include <algorithm>

#include "NeuralNetwork.h"

using namespace std;

struct Genome
{
	vector<double>		Weights;
	double				Fitness;

	Genome() : Fitness(0) {}
	Genome(vector <double> w, double f) : Weights(w), Fitness(f) {}

	//overload '<' used for sorting
	friend bool operator<(const Genome& lhs, const Genome& rhs)
	{
		return (lhs.Fitness < rhs.Fitness);
	}
};

class GeneticAlgorithm
{
public:

	GeneticAlgorithm(int popsize, double MutRat, double CrossRat, int numweights);
	~GeneticAlgorithm();

	//this runs the GA for one generation.
	vector<Genome> Epoch(vector<Genome> &old_pop);

	vector<Genome> GetChromos()const { return mPopulation; }
	double AverageFitness()const { return mTotalFitness / mPopulationSize; }
	double BestFitness()const { return mBestFitness; }

private:
	void Crossover(const vector<double> &mum, const vector<double> &dad, vector<double> &baby1, vector<double> &baby2);

	void Mutate(vector<double> &chromo);

	Genome GetChromoRoulette();

	void GrabNBest(int NBest, const int NumCopies, vector<Genome> &vecPop);

	void CalculateTotals();

	void Reset();
	
	vector<Genome>				mPopulation; // This holds the entire population of chromosomes
	
	int							mPopulationSize; // Size of population
	int							mChromoLength; // Amount of weights per chromo
	int							mNumWeights; // NumOfWeights

	int							mGeneration; // Generation counter
	
	double						mTotalFitness; // Total fitness of population
	double						mBestFitness; // Best fitness this population
	double						mAverageFitness; // Average fitness
	double						mWorstFitness; // Worst
	int							mFittestGenome; // Keeps track of the best genome

	double						mMutationRate; // Probability that a chromosomes bits will mutate.
	double						mCrossoverRate; // Probability of chromosomes crossing over bits

};

