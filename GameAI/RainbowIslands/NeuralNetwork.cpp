#include "NeuralNetwork.h"

std::random_device seeder;
std::mt19937 NNrng(seeder());
std::uniform_int_distribution<int> gen(-1, 1); //(min, max)

Neuron::Neuron(int _numOfInputs) : numInputs(_numOfInputs + 1) //+1 for the bias.
{
	for (int i = 0; i < numInputs; i++)
		v_Weights.push_back(gen(NNrng));
}

NeuronLayer::NeuronLayer(int _numNeurons, int _numInputsPerNeuron) : numNeurons(_numNeurons)
{
	for (int i = 0; i < numNeurons; i++)
		v_Neurons.push_back(Neuron(_numInputsPerNeuron));
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
	//create the layers of the network
	if (kNumHiddenLayers > 0)
	{
		//create first hidden layer
		v_Layers.push_back(NeuronLayer(kNeuronsPerHiddenLyr, kNumInputs)); //Because the number of inputs can be different from the number of neurons in a hidden layer, so it can get wider

		for (int i = 0; i < kNumHiddenLayers - 1; i++)
		{
			v_Layers.push_back(NeuronLayer(kNeuronsPerHiddenLyr, kNeuronsPerHiddenLyr));
		}

		//create output layer
		v_Layers.push_back(NeuronLayer(kNumOutputs, kNeuronsPerHiddenLyr));
	}
	else //this should never be hit
	{
		//create output layer
		v_Layers.push_back(NeuronLayer(kNumOutputs, kNumInputs));
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
		for (int j = 0; j < v_Layers[i].numNeurons; ++j)
		{
			//for each weight
			for (int k = 0; k < v_Layers[i].v_Neurons[j].numInputs; k++)
			{
				weights.push_back(v_Layers[i].v_Neurons[j].v_Weights[k]);
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
		for (int j = 0; j < v_Layers[i].numNeurons; ++j)
		{
			//for each weight
			for (int k = 0; k < v_Layers[i].v_Neurons[j].numInputs; k++)
			{
				v_Layers[i].v_Neurons[j].v_Weights[k] = weights[w];
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
		for (int j = 0; j < v_Layers[i].numNeurons; ++j)
		{
			//for each weight
			for (int k = 0; k < v_Layers[i].v_Neurons[j].numInputs; k++)
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

	int cWeight = 0;

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

		cWeight = 0;

		//for each neuron sum the (inputs * corresponding weights).Throw 
		//the total at our sigmoid function to get the output.
		for (int j = 0; j < v_Layers[i].numNeurons; j++)
		{
			double netinput = 0;

			int	NumInputs = v_Layers[i].v_Neurons[j].numInputs;

			//for each weight
			for (int k = 0; k<NumInputs - 1; ++k)
			{
				//sum the weights x inputs
				netinput += v_Layers[i].v_Neurons[j].v_Weights[k] *	inputs[cWeight++];
			}

			//add in the bias
			netinput += v_Layers[i].v_Neurons[j].v_Weights[NumInputs - 1] *	kBias;

			//we can store the outputs from each layer as we generate them. 
			//The combined activation is first filtered through the sigmoid 
			//function
			outputs.push_back(Sigmoid(netinput,	kActivationResponse));

			cWeight = 0;
		}
	}

	return outputs;
}

bool NeuralNetwork::NetworkTrainingEpoch(vector<double> inputs, vector<double> outputs, double score, double maxScore)
{
	//create some iterators  
	vector<double>::iterator curWeight;
	vector<Neuron>::iterator curNrnOut, curNrnHid;
	
	//this will hold the cumulative error value for the training set  
	m_dErrorSum = 0;  
	
	//run each input pattern through the network, calculate the errors and update  //RUNNING IT REALTIME INSTEAD OF TRAINING
	//the weights accordingly  
	//for (int vec = 0; vec < kNumInputs; ++vec)
	//{


	//first run this input vector through the network and retrieve the outputs   
	//vector<double> outputs = Update(SetIn[vec]);

	//return if error has occurred    
	if (outputs.size() == 0)
	{
		return false;
	}    

	//for each output neuron calculate the error and adjust weights    
	//accordingly
	for (int op = 0; op < kNumOutputs; ++op)
	{
		double err = maxScore - score;

		//update the error total. (when this value becomes lower than a      
		//preset threshold we know the training is successful)      
		m_dErrorSum += maxScore - score;

		//keep a record of the error value

		v_Layers[1].v_Neurons[op].error = err;
		curWeight = v_Layers[1].v_Neurons[op].v_Weights.begin();
		curNrnHid = v_Layers[0].v_Neurons.begin();

		//for each weight up to but not including the bias
		while (curWeight != v_Layers[1].v_Neurons[op].v_Weights.end() - 1)
		{        
			//calculate the new weight based on the backprop rules
			*curWeight += err * learningRate * curNrnHid->m_dActivation;
			++curWeight;
			++curNrnHid;
		}      
			
		//and the bias for this neuron      
		*curWeight += err * learningRate * kBias;
	}

	//**moving backwards to the hidden layer**    
	curNrnHid = v_Layers[0].v_Neurons.begin();
		
	int n = 0;
	//for each neuron in the hidden layer calculate the error signal
	//and then adjust the weights accordingly
	while(curNrnHid != v_Layers[0].v_Neurons.end())
	{
		double err = 0;
		curNrnOut = v_Layers[1].v_Neurons.begin();
		//to calculate the error for this neuron we need to iterate through
		//all the neurons in the output layer it is connected to and sum
		//the error * weights
		while(curNrnOut != v_Layers[1].v_Neurons.end())
		{
			err += curNrnOut->error * curNrnOut->v_Weights[n];
			++curNrnOut;
		}
		//now we can calculate the error
		err *= curNrnHid->m_dActivation * (1 - curNrnHid->m_dActivation);
		//for each weight in this neuron calculate the new weight based      
		//on the error signal and the learning rate      
		for (int w=0; w < inputs.size(); ++w)
		{        
			//calculate the new weight based on the backprop rules        
			curNrnHid->v_Weights[w] += err * learningRate * inputs[w];
		}      
		//and the bias      
		curNrnHid->v_Weights[kNumInputs] += err * learningRate * kBias;
		++curNrnHid;      
		++n;    
	}  
	
	//next input vector  
	return true;
}


double NeuralNetwork::Sigmoid(double netinput, double response)
{
	return (1 / (1 + exp(-netinput / response)));
}
