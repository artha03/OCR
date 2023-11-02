#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>

#define INPUT_SIZE 2
#define HIDDEN_SIZE 10
#define OUTPUT_SIZE 1

// Structure du réseau neuronal
typedef struct {
    double input[INPUT_SIZE];
    double hidden[HIDDEN_SIZE];
    double output[OUTPUT_SIZE];
    double weights_ih[INPUT_SIZE][HIDDEN_SIZE];
    double weights_ho[HIDDEN_SIZE][OUTPUT_SIZE];
} NeuralNetwork;

// Initialisation du réseau neuronal
void initialize(NeuralNetwork *nn) {
    // Initialisation des poids avec des valeurs aléatoires
    for (int i = 0; i < INPUT_SIZE; i++) {
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            nn->weights_ih[i][j] = ((double)rand() / RAND_MAX) * 2 - 1;
        }
    }

    for (int i = 0; i < HIDDEN_SIZE; i++) {
        for (int j = 0; j < OUTPUT_SIZE; j++) {
            nn->weights_ho[i][j] = ((double)rand() / RAND_MAX) * 2 - 1;
        }
    }
}

// Fonction d'activation (sigmoid)
double sigmoid(double x) {
    return (1.0/(1.0 + exp(-x)));
}


// Propagation avant
void forward(NeuralNetwork *nn) {
    for (int i = 0; i < HIDDEN_SIZE; i++) {
        nn->hidden[i] = 0;
        for (int j = 0; j < INPUT_SIZE; j++) {
            nn->hidden[i] += nn->input[j] * nn->weights_ih[j][i];
        }
        nn->hidden[i] = sigmoid(nn->hidden[i]);
    }

    for (int i = 0; i < OUTPUT_SIZE; i++) {
        nn->output[i] = 0;
        for (int j = 0; j < HIDDEN_SIZE; j++) {
            nn->output[i] += nn->hidden[j] * nn->weights_ho[j][i];
        }
        nn->output[i] = sigmoid(nn->output[i]);
    }
}

// Rétropropagation de l'erreur
void backward(NeuralNetwork *nn, double target) {
    double output_error = target - nn->output[0];
    double output_delta = output_error * nn->output[0] * (1 - nn->output[0]);

    for (int i = 0; i < HIDDEN_SIZE; i++) {
        for (int j = 0; j < OUTPUT_SIZE; j++) {
            nn->weights_ho[i][j] += 0.1 * output_delta * nn->hidden[i];
        }
    }

    double hidden_errors[HIDDEN_SIZE];
    for (int i = 0; i < HIDDEN_SIZE; i++) {
        hidden_errors[i] = output_delta * nn->weights_ho[i][0];
    }

    for (int i = 0; i < HIDDEN_SIZE; i++) {
        double hidden_delta = hidden_errors[i] * nn->hidden[i] * (1 - nn->hidden[i]);
        for (int j = 0; j < INPUT_SIZE; j++) {
            nn->weights_ih[j][i] += 0.1 * hidden_delta * nn->input[j];
        }
    }
}

int main() {
    //srand(time(NULL));

    NeuralNetwork nn;
    initialize(&nn);

    // Données XOR
    double inputs[4][2] = { {0, 0}, {0, 1}, {1, 0}, {1, 1} };
    double targets[4] = {0, 1, 1, 0};

    printf("Entrainement : \n");
    for (int epoque = 0; epoque < 10000; epoque++) {
        double total_error = 0;

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < INPUT_SIZE; j++) {
                nn.input[j] = inputs[i][j];
            }

            forward(&nn);
            double error = targets[i] - nn.output[0];
            total_error += 0.5 * error * error;
            backward(&nn, targets[i]);
        }

        if (epoque % 1000 == 0) {
            printf("Epoque %d, Error: %lf\n", epoque, total_error);
        }
    }

    // Test du réseau neuronal
    printf("Test du réseau neuronal :\n");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < INPUT_SIZE; j++) {
            nn.input[j] = inputs[i][j];
        }
        forward(&nn);
        printf("%lf XOR %lf = %lf\n", inputs[i][0], inputs[i][1], nn.output[0]);
    }

    return 0;
}