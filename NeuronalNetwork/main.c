#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include<dirent.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include "network.h"
#include "apprentissage.h"

int main() {
    srand(time(NULL)); // Initialisation du générateur de nombres aléatoires
/*
    // Chemin vers le dossier contenant les données d'entraînement
    char *trainPath = "Train";

    // Chargement des données d'entraînement (10 échantillons par classe)
    int numSamplesPerClass = 26;
    TrainingSample *trainingData = loadTrainingData(trainPath, numSamplesPerClass);
    printf("trainingData: %p\n", trainingData);

    if (trainingData == NULL) {
        printf("Erreur lors du chargement des données d'entraînement.\n");
        return 1;
    }

    // Préparation des étiquettes d'entraînement
    int *trainingLabels = prepareTrainingLabels(NUM_CLASSES, numSamplesPerClass);

    if (trainingLabels == NULL) {
        printf("Erreur lors de la préparation des étiquettes d'entraînement.\n");
        return 1;
    }
*/

    // Création du réseau de neurones avec la structure souhaitée (à adapter selon votre structure)
    Network network = NewNetwork(IMG_WIDTH * IMG_HEIGHT, 32, NUM_CLASSES, 1);
    InitNetwork(&network);
/*
    // Entraînement du réseau de neurones avec les données d'entraînement
    double learningRate = 0.01;
    int epochs = 5000;
    int numSamples = NUM_CLASSES * numSamplesPerClass;
    TrainNetwork(&network, trainingData, trainingLabels, NUM_CLASSES, numSamples, learningRate, epochs);

    // Sauvegarde du réseau de neurones entraîné
    SaveNetwork(&network, "network.txt");



    // Test du réseau de neurones sur des images de testchiffre
    char *testPath = "testchiffre";
    for (int i = 0; i < NUM_CLASSES; ++i) {
        char imagePath[PATH_MAX];
        sprintf(imagePath, "%s/test%d.png", testPath, i);

        SDL_Surface *testImage = GetImages(imagePath);
        if (testImage == NULL) {
            printf("Erreur lors du chargement de l'image de test : %s\n", imagePath);
            continue;
        }

        double *testData = ImageToData(testImage);
        if (testData == NULL) {
            printf("Erreur lors de la conversion de l'image de test en données : %s\n", imagePath);
            continue;
        }

        // Effectuer la propagation avant avec le réseau de neurones chargé
        FrontPropagation(&network, testData);

        // Récupérer l'étiquette prédite pour l'image de test
        int predictedLabel = getPredictedLabel(&network);
        printf("Image de test : %s, Chiffre prédit : %d\n", imagePath, predictedLabel);

        free(testData);
        SDL_FreeSurface(testImage);
    }
*/
    // Chargement du réseau de neurones entraîné
    Network loadedNetwork = LoadNetwork("network.txt");
    char *testPath2 = "testchiffre";
    for (int i = 0; i < NUM_CLASSES; ++i) {
        char imagePath[PATH_MAX];
        sprintf(imagePath, "%s/test%d.png", testPath2, i);

        SDL_Surface *testImage = GetImages(imagePath);
        if (testImage == NULL) {
            printf("Erreur lors du chargement de l'image de test : %s\n", imagePath);
            continue;
        }

        double *testData = ImageToData(testImage);
        if (testData == NULL) {
            printf("Erreur lors de la conversion de l'image de test en données : %s\n", imagePath);
            continue;
        }

        // Effectuer la propagation avant avec le réseau de neurones chargé
        FrontPropagation(&loadedNetwork, testData);

        // Récupérer l'étiquette prédite pour l'image de test
        int predictedLabel = getPredictedLabel(&loadedNetwork);
        printf("Image de test : %s, Chiffre prédit : %d\n", imagePath, predictedLabel);

        free(testData);
        SDL_FreeSurface(testImage);
    }

    // Libération de la mémoire allouée
    //FreeNetwork(&network);
    FreeNetwork(&loadedNetwork);
    // free(trainingData);
    //free(trainingLabels);

    return 0;
}