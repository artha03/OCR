#ifndef NEURAL_H
#define NEURAL_H

typedef struct Neuron
{
    unsigned int nbWeights;
    double *weights;

    double value;
    double bias;
    double delta;
} Neuron;

typedef struct Layer
{
    unsigned int nbNeurons;
    Neuron *neurons;
} Layer;

typedef struct Network
{
    unsigned int nbLayers;
    unsigned int sizeInput;
    unsigned int sizeHidden;
    unsigned int sizeOutput;
    Layer *layers;
} Network;

/////////////////////////////// NEURON ////////////////////////////////////////
Neuron NewNeuron(unsigned int nbWeights);
void InitNeuron(Neuron *neuron);
void FreeNeuron(Neuron *neuron);

/////////////////////////////// LAYER ////////////////////////////////////////
Layer NewLayer(unsigned int nbNeurons, unsigned int nbWeights);
void InitLayer(Layer *layer);
void FreeLayer(Layer *layer);

/////////////////////////////// NETWORK ////////////////////////////////////////

Network NewNetwork(unsigned int sizeInput, unsigned int sizeHidden, unsigned int sizeOutput, unsigned int nbHiddenLayers);
void InitNetwork(Network *network);
void FreeNetwork(Network *network);

void FrontPropagation(Network *network, double *inputs);
void BackPropagation(Network *network, double *expected);
void GradientDescent(Network *network, double learningRate);

#endif