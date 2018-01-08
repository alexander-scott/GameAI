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
	else // This should never be hit
	{
		// Create output layer
		mLayers.push_back(NeuronLayer(kNumOutputs, kNumInputs));
	}
}

vector<double> NeuralNetwork::GetWeights() const //const makes member level variables uneditable
{
	//this will hold the weights
	vector<double> weights;

	//for each layer
	for (int i = 0; i < kNumHiddenLayers + 1; ++i)
	{
		//for each neuron
		for (int j = 0; j < mLayers[i].NumNeurons; ++j)
		{
			//for each weight
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

	//for each layer
	for (int i = 0; i < kNumHiddenLayers + 1; ++i)
	{
		//for each neuron
		for (int j = 0; j < mLayers[i].NumNeurons; ++j)
		{
			//for each weight
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

	//for each layer
	for (int i = 0; i < kNumHiddenLayers + 1; ++i)
	{
		//for each neuron
		for (int j = 0; j < mLayers[i].NumNeurons; ++j)
		{
			//for each weight
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
	//stores the resultant outputs from each layer
	vector<double> outputs;

	int countWeight = 0;

	//first check that we have the correct amount of inputs
	if (inputs.size() != kNumInputs)
	{
		//just return an empty vector if incorrect.
		return outputs;
	}

	//For each layer....
	for (int i = 0; i < kNumHiddenLayers + 1; i++)
	{
		if (i > 0)
		{
			inputs = outputs;
		}

		outputs.clear();

		countWeight = 0;

		//for each neuron sum the (inputs * corresponding weights).Throw 
		//the total at our sigmoid function to get the output.
		for (int j = 0; j < mLayers[i].NumNeurons; j++)
		{
			double netinput = 0;

			int	NumInputs = mLayers[i].Neurons[j].NumInputs;

			//for each weight
			for (int k = 0; k<NumInputs - 1; ++k)
			{
				//sum the weights x inputs
				netinput += mLayers[i].Neurons[j].Weights[k] *	inputs[countWeight++];
			}

			//add in the bias
			netinput += mLayers[i].Neurons[j].Weights[NumInputs - 1] *	kBias;

			//we can store the outputs from each layer as we generate them. 
			//The combined activation is first filtered through the sigmoid 
			//function
			outputs.push_back(Sigmoid(netinput,	kActivationResponse));

			countWeight = 0;
		}
	}

	return outputs;
}

bool NeuralNetwork::NetworkTrainingEpoch(vector<double> inputs, vector<double> outputs, double score, double maxScore)
{
	//create some iterators  
	vector<double>::iterator currWeight;
	vector<Neuron>::iterator currNeuronsOut, currNeuronsHidden;
	
	//this will hold the cumulative error value for the training set  
	mErrorSum = 0;  
	
	//return if error has occurred    
	if (outputs.size() == 0)
	{
		return false;
	}    

	//for each output neuron calculate the error and adjust weights    
	//accordingly
	for (int iOutput = 0; iOutput < kNumOutputs; ++iOutput)
	{
		double error = maxScore - score;

		//update the error total. (when this value becomes lower than a      
		//preset threshold we know the training is successful)      
		mErrorSum += maxScore - score;

		//keep a record of the error value
		mLayers[1].Neurons[iOutput].Error = error;

		currWeight = mLayers[1].Neurons[iOutput].Weights.begin();
		currNeuronsHidden = mLayers[0].Neurons.begin();

		//for each weight up to but not including the bias
		while (currWeight != mLayers[1].Neurons[iOutput].Weights.end() - 1)
		{        
			//calculate the new weight based on the backprop rules
			*currWeight += error * kLearningRate * currNeuronsHidden->Activation;
			++currWeight;
			++currNeuronsHidden;
		}      
			
		//and the bias for this neuron      
		*currWeight += error * kLearningRate * kBias;
	}

	//**moving backwards to the hidden layer**    
	currNeuronsHidden = mLayers[0].Neurons.begin();
		
	int count = 0;
	//for each neuron in the hidden layer calculate the error signal
	//and then adjust the weights accordingly
	while(currNeuronsHidden != mLayers[0].Neurons.end())
	{
		double error = 0;
		currNeuronsOut = mLayers[1].Neurons.begin();
		//to calculate the error for this neuron we need to iterate through
		//all the neurons in the output layer it is connected to and sum
		//the error * weights
		while(currNeuronsOut != mLayers[1].Neurons.end())
		{
			error += currNeuronsOut->Error * currNeuronsOut->Weights[count];
			++currNeuronsOut;
		}
		//now we can calculate the error
		error *= currNeuronsHidden->Activation * (1 - currNeuronsHidden->Activation);
		//for each weight in this neuron calculate the new weight based      
		//on the error signal and the learning rate      
		for (int w=0; w < inputs.size(); ++w)
		{        
			//calculate the new weight based on the backprop rules        
			currNeuronsHidden->Weights[w] += error * kLearningRate * inputs[w];
		}      
		//and the bias      
		currNeuronsHidden->Weights[kNumInputs] += error * kLearningRate * kBias;
		++currNeuronsHidden;      
		++count;    
	}  
	
	//next input vector  
	return true;
}

double NeuralNetwork::Sigmoid(double netinput, double response)
{
	return (1 / (1 + exp(-netinput / response)));
}
