#include "GeneticAlgorithm.h"

std::random_device dseeder;
std::mt19937 RandSeed(dseeder());
std::uniform_real_distribution<double> GetRand(0, 1); //(min, max)

GeneticAlgorithm::GeneticAlgorithm(int popsize, double MutRat, double CrossRat, int numWeights) : mPopulationSize(popsize), mMutationRate(MutRat), mCrossoverRate(CrossRat)
{
	mTotalFitness = 0;
	mGeneration = 0;
	mFittestGenome = 0;
	mBestFitness = 0;
	mWorstFitness = 99999999;
	mAverageFitness = 0;

	// Initialise population with chromosomes consisting of random weights and all fitnesses set to zero
	for (int i = 0; i<mPopulationSize; ++i)
	{
		mPopulation.push_back(Genome());

		for (int j = 0; j < numWeights; ++j)
		{
			mPopulation[i].Weights.push_back(GetRand(RandSeed));
		}
	}

	mChromoLength = numWeights;
}

GeneticAlgorithm::~GeneticAlgorithm()
{
}

// Randomly mutate a weight 
void GeneticAlgorithm::Mutate(vector<double> &chromo)
{
	// Traverse the chromosome and mutate each weight dependent on the mutation rate
	for (int i = 0; i < chromo.size(); ++i)
	{
		// Only mutate this if our dice roll is less than the mutation rate
		if (GetRand(RandSeed) < mMutationRate)
		{
			// Add or subtract a small value to the weight
			chromo[i] += ((GetRand(RandSeed) - GetRand(RandSeed)) * kMaxPerturbation);
		}
	}
}

// Return a fit genome
Genome GeneticAlgorithm::GetChromoRoulette()
{
	// Get a random number between 0 & total fitness count
	double Slice = (double)(GetRand(RandSeed) * mTotalFitness);

	// This will be set to the chosen chromosome
	Genome TheChosenOne;

	// Go through the chromosones totalling the fitness
	double FitnessSoFar = 0;

	for (int i = 0; i<mPopulationSize; ++i)
	{
		FitnessSoFar += mPopulation[i].Fitness;

		// If the fitness so far > random number return the chromo at this point
		if (FitnessSoFar >= Slice)
		{
			TheChosenOne = mPopulation[i];
			break;
		}

	}

	return TheChosenOne;
}

// Create offspring from parents
void GeneticAlgorithm::Crossover(const vector<double> &mum,
	const vector<double> &dad,
	vector<double>       &baby1,
	vector<double>       &baby2)
{
	// just return parents as offspring dependent on the rate or if parents are the same
	if ((GetRand(RandSeed) > mCrossoverRate) || (mum == dad))
	{
		baby1 = mum;
		baby2 = dad;

		return;
	}

	// Determine a crossover point
	std::uniform_int_distribution<int> genInt(0, mChromoLength - 1); //(min, max)

	int popCount = genInt(RandSeed);

	// Create the offspring
	for (int i = 0; i < popCount; ++i)
	{
		baby1.push_back(mum[i]);
		baby2.push_back(dad[i]);
	}

	for (int i = popCount; i<mum.size(); ++i)
	{
		baby1.push_back(dad[i]);
		baby2.push_back(mum[i]);
	}

	return;
}

// Updates and returns a new population of chromosones.
vector<Genome> GeneticAlgorithm::Epoch(vector<Genome> &old_pop)
{
	// Assign the given population to the classes population
	mPopulation = old_pop;

	// Reset the appropriate variables
	Reset();

	// Sort the population (for scaling and elitism)
	sort(mPopulation.begin(), mPopulation.end());

	// Calculate best, worst, average and total fitness
	CalculateTotals();

	// Create a temporary vector to store new chromosones
	vector <Genome> vecNewPop;

	// Add in some copies of the fittness genomes
	if (!(kNumCopiesElite * kNumElite % 2))
	{
		GrabNBest(kNumElite, kNumCopiesElite, vecNewPop);
	}

	// Repeat until a new population is generated
	while (vecNewPop.size() < mPopulationSize)
	{
		// Grab two chromosones
		Genome mum = GetChromoRoulette();
		Genome dad = GetChromoRoulette();

		// Create some offspring via crossover
		vector<double> baby1, baby2;

		Crossover(mum.Weights, dad.Weights, baby1, baby2);

		// Now we mutate
		Mutate(baby1);
		Mutate(baby2);

		// Now copy into vecNewPop population
		vecNewPop.push_back(Genome(baby1, 0));
		vecNewPop.push_back(Genome(baby2, 0));
	}

	// Finished so assign new pop back into vecNewPop
	mPopulation = vecNewPop;

	return mPopulation;
}

// Returns a number of the fittest genomes
void GeneticAlgorithm::GrabNBest(int NBest,	const int NumCopies, vector<Genome>	&Pop)
{
	// Add the required amount of copies of the n most fittest to the supplied vector
	while (NBest--)
	{
		for (int i = 0; i<NumCopies; ++i)
		{
			Pop.push_back(mPopulation[(mPopulationSize - 1) - NBest]);
		}
	}
}

// Calculates the fittest and weakest genome and the average/total fitness scores
void GeneticAlgorithm::CalculateTotals()
{
	mTotalFitness = 0;

	double HighestSoFar = 0;
	double LowestSoFar = 9999999;

	for (int i = 0; i<mPopulationSize; ++i)
	{
		// Update fittest if necessary
		if (mPopulation[i].Fitness > HighestSoFar)
		{
			HighestSoFar = mPopulation[i].Fitness;

			mFittestGenome = i;

			mBestFitness = HighestSoFar;
		}

		// Update worst if necessary
		if (mPopulation[i].Fitness < LowestSoFar)
		{
			LowestSoFar = mPopulation[i].Fitness;

			mWorstFitness = LowestSoFar;
		}

		mTotalFitness += mPopulation[i].Fitness;


	}

	mAverageFitness = mTotalFitness / mPopulationSize;
}

// Resets all the relevant variables ready for a new generation
void GeneticAlgorithm::Reset()
{
	mTotalFitness = 0;
	mBestFitness = 0;
	mWorstFitness = 9999999;
	mAverageFitness = 0;
}