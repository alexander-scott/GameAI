#pragma once
#include <vector>
#include <random> // rand is biased to lower numbers

#include "Constants_RainbowIslands.h"

using namespace std;

struct Neuron
{
	int numInputs;

	vector<double> v_Weights;

	int error;
	double m_dActivation = 0.5;

	Neuron(int);
};

struct NeuronLayer
{
	int numNeurons;

	vector<Neuron> v_Neurons;

	NeuronLayer(int _numNeurons, int _numInputsPerNeuron);
};

class NeuralNetwork
{
public:
	NeuralNetwork();
	~NeuralNetwork();
	void CreateNet();
	vector<double> GetWeights()const;
	int GetNumberOfWeights()const;
	void PutWeights(vector<double> &weights);
	vector<double> Update(vector<double> &inputs);
	inline double Sigmoid(double activation, double response);
	bool NetworkTrainingEpoch(vector<double> inputs, vector<double> outputs, double score, double maxScore);


private:
	vector<NeuronLayer> v_Layers;
	int m_iNumEpochs;
	double m_dErrorSum;

};

