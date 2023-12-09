#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include<dirent.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include "neural_network.h"
#include "apprentissage.h"

#define NUM_CLASSES 10 // Nombre de classes (chiffres de 0 à 9)
#define IMG_WIDTH 28   // Largeur des images (à adapter selon vos images)
#define IMG_HEIGHT 28  // Hauteur des images (à adapter selon vos images)

void saveSudokuGrid(const char *filePath, int *predictions) {
    FILE *file = fopen(filePath, "w");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier pour l'écriture.\n");
        return;
    }

    for (int i = 0; i < 81; ++i) {
        if (i % 9 == 0 && i != 0) {
            fprintf(file, "\n");
        }
        if (i%27 == 0 && i != 0) {
            fprintf(file, "\n");
        }

        if (predictions[i] == 0) {
            fprintf(file, ".");
        } else {
            fprintf(file, "%d", predictions[i]);
        }

        if (i % 3 == 2 && i % 9 != 8) {
            fprintf(file, "   ");
        } else {
            fprintf(file, "");
        }
    }

    fclose(file);
}

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
   /* for (int i = 0; i < 81; ++i) {
        char imagePath[PATH_MAX];
        sprintf(imagePath, "%s/Image_%d.png", testPath2, i);

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
       // printf("Image de test : %s, Chiffre prédit : %d\n", imagePath, predictedLabel);

        free(testData);
        SDL_FreeSurface(testImage);
    }

    // Libération de la mémoire allouée
    //FreeNetwork(&network);
    FreeNetwork(&loadedNetwork);
    // free(trainingData);
    //free(trainingLabels);

    return 0;*/
    const char *testPath = "./../DetectionLignes/Decoupage/resize/"; // Changer le chemin approprié
    int predictions[81] = {0}; // Tableau pour stocker les prédictions

    for (int i = 0; i < 81; ++i) {
        char imagePath[PATH_MAX];
        sprintf(imagePath, "%s/Image_invert_%d.png", testPath, i);

        // Charger l'image de test
        SDL_Surface *testImage = IMG_Load(imagePath);
        if (testImage == NULL) {
            printf("Erreur lors du chargement de l'image de test : %s\n", imagePath);
            continue;
        }

        // Convertir l'image en données pour le réseau neuronal
        double *testData = ImageToData(testImage);
        if (testData == NULL) {
            printf("Erreur lors de la conversion de l'image de test en données : %s\n", imagePath);
            continue;
        }

        // Effectuer la propagation avant avec le réseau de neurones chargé
        FrontPropagation(&loadedNetwork, testData);

        // Récupérer l'étiquette prédite pour l'image de test
        int predictedLabel = getPredictedLabel(&loadedNetwork);
        predictions[i] = predictedLabel;

        free(testData);
        SDL_FreeSurface(testImage);
    }

    // Enregistrer les prédictions dans un fichier texte
    saveSudokuGrid("./../Solver/grille_sudoku.txt", predictions);

    // Libération de la mémoire allouée
    FreeNetwork(&network);
    FreeNetwork(&loadedNetwork);

    return 0;
}