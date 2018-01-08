#pragma once
#include <vector>
#include <random> // rand is biased to lower numbers

#include "Constants_RainbowIslands.h"

using namespace std;

struct Neuron
{
	int				NumInputs;
	vector<double>	Weights;
	int				Error;
	double			Activation = 0.5;

	Neuron(int);
};

struct NeuronLayer
{
	int				NumNeurons;
	vector<Neuron>	Neurons;

	NeuronLayer(int _numNeurons, int _numInputsPerNeuron);
};

class NeuralNetwork
{
public:
	NeuralNetwork();
	~NeuralNetwork();

	void CreateNet();
	bool NetworkTrainingEpoch(vector<double> inputs, vector<double> outputs, double score, double maxScore);

	vector<double> GetWeights()const;
	int GetNumberOfWeights()const;
	void PutWeights(vector<double> &weights);

	vector<double> Update(vector<double> &inputs);

	inline double Sigmoid(double activation, double response);

private:
	vector<NeuronLayer>			mLayers;
	int							mNumEpochs;
	double						mErrorSum;

};

