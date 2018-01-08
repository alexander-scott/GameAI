#include "NeuralNetwork.h"

std::random_device seeder;
std::mt19937 NNrng(seeder());
std::uniform_int_distribution<int> gen(-1, 1); //(min, max)

Neuron::Neuron(int _numOfInputs) : NumInputs(_numOfInputs + 1) //+1 for the bias.
{
	for (int i = 0; i < NumInputs; i++)
		Weights.push_back(gen(NNrng));
}

NeuronLayer::NeuronLayer(int _numNeurons, int _numInputsPerNeuron) : NumNeurons(_numNeurons)
{
	for (int i = 0; i < NumNeurons; i++)
		Neurons.push_back(Neuron(_numInputsPerNeuron));
}

NeuralNetwork::NeuralNetwork()
{
	CreateNet();
}

NeuralNetwork::~NeuralNetwork()
{
}

void NeuralNetwork::CreateNet()
{
	// Create the layers of the network
	if (kNumHiddenLayers > 0)
	{
		// Create first hidden layer
		mLayers.push_back(NeuronLayer(kNeuronsPerHiddenLyr, kNumInputs)); // Because the number of inputs can be different from the number of neurons in a hidden layer, so it can get wider

		for (int i = 0; i < kNumHiddenLayers - 1; i++)
		{
			mLayers.push_back(NeuronLayer(kNeuronsPerHiddenLyr, kNeuronsPerHiddenLyr));
		}

		// Create output layer
		mLayers.push_back(NeuronLayer(kNumOutputs, kNeuronsPerHiddenLyr));
	}
	else 
	{
		// Create output layer
		mLayers.push_back(NeuronLayer(kNumOutputs, kNumInputs));
	}
}

vector<double> NeuralNetwork::GetWeights() const
{
	vector<double> weights;

	// For each layer
	for (int i = 0; i < kNumHiddenLayers + 1; ++i)
	{
		// For each neuron
		for (int j = 0; j < mLayers[i].NumNeurons; ++j)
		{
			// For each weight
			for (int k = 0; k < mLayers[i].Neurons[j].NumInputs; k++)
			{
				weights.push_back(mLayers[i].Neurons[j].Weights[k]);
			}
		}
	}

	return weights;
}

void NeuralNetwork::PutWeights(vector<double> &weights)
{
	int w = 0;

	// For each layer
	for (int i = 0; i < kNumHiddenLayers + 1; ++i)
	{
		// For each neuron
		for (int j = 0; j < mLayers[i].NumNeurons; ++j)
		{
			// For each weight
			for (int k = 0; k < mLayers[i].Neurons[j].NumInputs; k++)
			{
				mLayers[i].Neurons[j].Weights[k] = weights[w];
				w++;
			}
		}
	}
}

int NeuralNetwork::GetNumberOfWeights() const
{
	int w = 0;

	// For each layer
	for (int i = 0; i < kNumHiddenLayers + 1; ++i)
	{
		// For each neuron
		for (int j = 0; j < mLayers[i].NumNeurons; ++j)
		{
			// For each weight
			for (int k = 0; k < mLayers[i].Neurons[j].NumInputs; k++)
			{
				w++;
			}
		}
	}

	return w;
}

vector<double> NeuralNetwork::Update(vector<double> &inputs)
{
	// Stores the resultant outputs from each layer
	vector<double> outputs;

	int countWeight = 0;

	// First check that we have the correct amount of inputs
	if (inputs.size() != kNumInputs)
	{
		return outputs;
	}

	// For each layer....
	for (int i = 0; i < kNumHiddenLayers + 1; i++)
	{
		if (i > 0)
		{
			inputs = outputs;
		}

		outputs.clear();

		countWeight = 0;

		// For each neuron sum the (inputs * corresponding weights).T hrow the total at our sigmoid function to get the output.
		for (int j = 0; j < mLayers[i].NumNeurons; j++)
		{
			double netinput = 0;

			int	NumInputs = mLayers[i].Neurons[j].NumInputs;

			// For each weight
			for (int k = 0; k<NumInputs - 1; ++k)
			{
				// Sum the weights x inputs
				netinput += mLayers[i].Neurons[j].Weights[k] *	inputs[countWeight++];
			}

			// Add in the bias
			netinput += mLayers[i].Neurons[j].Weights[NumInputs - 1] *	kBias;

			// We can store the outputs from each layer as we generate them. 
			// The combined activation is first filtered through the sigmoid function
			outputs.push_back(Sigmoid(netinput,	kActivationResponse));

			countWeight = 0;
		}
	}

	return outputs;
}

double NeuralNetwork::Sigmoid(double netinput, double response)
{
	return (1 / (1 + exp(-netinput / response)));
}
