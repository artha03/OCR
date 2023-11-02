#include <SDL2/SDL.h>
#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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

//////////////////////////////// NEURON////////////////////////////////////////
Neuron NewNeuron(unsigned int nbWeights)
{
    Neuron neuron;
    neuron.bias = 0;
    neuron.delta = 0;
    neuron.nbWeights = nbWeights;
    neuron.value = 0;
    neuron.weights = NULL;
    if (nbWeights != 0)
        neuron.weights = malloc((nbWeights+1) * sizeof (double));
    return neuron;
}

void InitNeuron(Neuron *neuron)
{
    for(unsigned int i = 0; i < neuron->nbWeights; i++)
    {
        neuron->weights[i] = ((double)rand() / RAND_MAX) * 2 - 1;
    }
    neuron->bias = ((double)rand() / RAND_MAX) * 2 - 1;
}

void FreeNeuron(Neuron *neuron)
{
    free(neuron->weights);
}

//////////////////////////////// LAYER////////////////////////////////////////
Layer NewLayer(unsigned int nbNeurons, unsigned int nbWeights)
{
    Layer layer;
    layer.nbNeurons = nbNeurons;
    layer.neurons = malloc((nbNeurons+1) * sizeof (struct Neuron));
    for(unsigned int i = 0; i < nbNeurons; i++)
    {
        layer.neurons[i] = NewNeuron(nbWeights);
    }
    return layer;
}

void InitLayer(Layer *layer)
{
    for(unsigned int i = 0; i < layer->nbNeurons; i++)
    {
        InitNeuron(&layer->neurons[i]);
    }
}

void FreeLayer(Layer *layer)
{
    for(unsigned int i = 0; i < layer->nbNeurons; i++)
    {
        Neuron *neuron = &(layer->neurons[i]);
        FreeNeuron(neuron);
    }
    free(layer->neurons);
}

//////////////////////////////// NETWORK////////////////////////////////////////

Network NewNetwork(unsigned int sizeInput, unsigned int sizeHidden, unsigned int sizeOutput,
                   unsigned int nbHiddenLayers)
{
    Network network;
    network.nbLayers = nbHiddenLayers+2;
    network.sizeInput = sizeInput;
    network.sizeHidden = sizeHidden;
    network.sizeOutput = sizeOutput;
    network.layers = malloc((network.nbLayers+1) * sizeof (struct Layer));
    network.layers[0] = NewLayer(sizeInput, 0);

    for(unsigned int i = 1; i < network.nbLayers-1; i++)
    {
        network.layers[i] = NewLayer(sizeHidden, network.layers[i-1].nbNeurons);
    }
    network.layers[network.nbLayers-1] = NewLayer(sizeOutput, network.layers[network.nbLayers-2].nbNeurons);
    return network;
}

void InitNetwork(Network *network)
{
    unsigned int nbLayers = network->nbLayers;
    unsigned int nbNeurons;

    for (unsigned int i = 0; i < nbLayers; i++)
    {
        Layer *layer = &(network->layers[i]);
        nbNeurons = layer->nbNeurons;
        for (unsigned int j = 0; j < nbNeurons; j++)
        {
            InitNeuron(&(layer->neurons[j]));
        }
    }
}

void FreeNetwork(Network *network)
{
    for(unsigned int i = 0; i < network->nbLayers; i++)
    {
        FreeLayer(&network->layers[i]);
    }
    free(network->layers);
}

void FrontPropagation(Network *network, double *inputs)
{
    for(unsigned int i = 0; i < network->sizeInput; i++)
    {
        network->layers[0].neurons[i].value = inputs[i];
    }

    for(unsigned int i = 1; i < network->nbLayers; i++)
    {
        for(unsigned int j = 0; j < network->layers[i].nbNeurons; j++)
        {
            double sum = 0;
            for(unsigned int k = 0; k < network->layers[i-1].nbNeurons; k++)
            {
                sum += network->layers[i-1].neurons[k].value * network->layers[i].neurons[j].weights[k];
            }
            sum += network->layers[i].neurons[j].bias;
            network->layers[i].neurons[j].value = 1 / (1 + exp(-sum));
        }
    }
}



void BackPropagation(Network *network, double *expected)
{
    for(unsigned int i = 0; i < network->sizeOutput; i++)
    {
        network->layers[2].neurons[i].delta = network->layers[2].neurons[i].value * (1 - network->layers[2].neurons[i].value) * (expected[i] - network->layers[2].neurons[i].value);
    }

    for(unsigned int i = 1; i < network->nbLayers - 1; i++)
    {
        for(unsigned int j = 0; j < network->layers[i].nbNeurons; j++)
        {
            double sum = 0;
            for(unsigned int k = 0; k < network->layers[i+1].nbNeurons; k++)
            {
                sum += network->layers[i+1].neurons[k].delta * network->layers[i+1].neurons[k].weights[j];
            }
            network->layers[i].neurons[j].delta = network->layers[i].neurons[j].value * (1 - network->layers[i].neurons[j].value) * sum;
        }
    }
}

void GradientDescent(Network *network, double learningRate)
{
    for(unsigned int i = 1; i < network->nbLayers; i++)
    {
        for(unsigned int j = 0; j < network->layers[i].nbNeurons; j++)
        {
            for(unsigned int k = 0; k < network->layers[i-1].nbNeurons; k++)
            {
                network->layers[i].neurons[j].weights[k] += learningRate * network->layers[i].neurons[j].delta * network->layers[i-1].neurons[k].value;
            }
            network->layers[i].neurons[j].bias += learningRate * network->layers[i].neurons[j].delta;
        }
    }
}

//affiche le réseau de neurone
void PrintNetwork(Network *network)
{
    printf("Network :\n");
    for(unsigned int i = 0; i < network->nbLayers; i++)
    {
        printf("Layer %d :\n", i);
        for(unsigned int j = 0; j < network->layers[i].nbNeurons; j++)
        {
            printf("Neuron %d :\n", j);
            printf("Value : %f\n", network->layers[i].neurons[j].value);
            //printf("Bias : %f\n", network->layers[i].neurons[j].bias);
            //printf("Delta : %f\n", network->layers[i].neurons[j].delta);
            /*printf("Weights : ");
            for(unsigned int k = 0; k < network->layers[i].neurons[j].nbWeights; k++)
            {
                printf("%f ", network->layers[i].neurons[j].weights[k]);
            }*/
            printf("\n");
        }
    }
}

//////////////////////////////// MAIN////////////////////////////////////////

//main qui test le réseau de neurone pour le XOR
int main() {

    Network network = NewNetwork(2, 10, 1, 1);
    InitNetwork(&network);

    double inputs[4][2] = {{0, 0},
                           {0, 1},
                           {1, 0},
                           {1, 1}};
    double targets[4] = {0, 1, 1, 0};


    printf("Entrainement : \n");
    for (unsigned int i = 0; i < 10000; i++) {
        double total_error = 0;

        for (unsigned int j = 0; j < 4; j++) {
            for (unsigned int k = 0; k < 2; k++) {
                network.layers[0].neurons[k].value = inputs[j][k];
            }
            FrontPropagation(&network, inputs[j]);
            double error = targets[j] - network.layers[network.nbLayers - 1].neurons[0].value;
            total_error += 0.5 * error * error;
            BackPropagation(&network, &targets[j]);
            GradientDescent(&network, 0.1);
        }

        if (i % 1000 == 0) {
            printf("Epoque %d, Error: %lf\n", i, total_error);
        }

    }
        //PrintNetwork(&network);

        //test du réseau de neurone
        printf("Test du réseau de neurone :\n");
        for (unsigned int i = 0; i < 4; i++) {
            FrontPropagation(&network, inputs[i]);
            for(unsigned int j = 0; j < 2; j++)
            {
                network.layers[0].neurons[j].value = inputs[i][j];
            }
            BackPropagation(&network, &targets[i]);
            printf("Input : %f XOR %f =", inputs[i][0], inputs[i][1]);
            printf(" %f\n", network.layers[network.nbLayers - 1].neurons[0].value);
        }

        FreeNetwork(&network);

        return 0;

}