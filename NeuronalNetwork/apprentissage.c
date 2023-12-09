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

//////////////////////////////////////////GetImages//////////////////////////////////////////
SDL_Surface* GetImages(char *path)
{
    SDL_Surface *source = IMG_Load(path);

    if (source == NULL) {
        printf("Impossible de charger l'image depuis le chemin : %s\n", path);
        return NULL;
    }

    SDL_Surface *surface = SDL_ConvertSurfaceFormat(source, SDL_PIXELFORMAT_RGB888, 0);

    SDL_FreeSurface(source); // Libération de la mémoire de la surface initiale

    return surface;
}

/////////////////////////////////////GetPixel/////////////////////////////////////////
Uint32 get_pixel(SDL_Surface *surface, int x, int y) {
    // Déplacement jusqu'à l'adresse du pixel
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    // En fonction du format des pixels, récupérer la valeur du pixel
    switch (bpp) {
        case 1:
            return *p;
        case 2:
            return *(Uint16 *)p;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
        case 4:
            return *(Uint32 *)p;
        default:
            return 0; // Valeur par défaut si le format de pixel n'est pas géré
    }
}

/////////////////////////////////////ImageToData/////////////////////////////////////////
double *ImageToData(SDL_Surface *image) {

    if (image == NULL) {
        printf("L'image est nulle.\n");
        return NULL;
    }
    double *data = (double *)malloc(sizeof(double) * image->w * image->h);

    if (data == NULL) {
        printf("Allocation de mémoire échouée.\n");
        return NULL;
    }

    // Convertir l'image en données numériques
    for (int i = 0; i < image->h; ++i) {
        for (int j = 0; j < image->w; ++j) {
            Uint32 pixel = get_pixel(image, j, i); // Obtenez le pixel à la position (j, i)
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image->format, &r, &g, &b);

            // Convertir les valeurs de couleur en niveaux de gris (ou d'autres transformations selon les besoins)
            double grayscale = (double)(r + g + b) / 3.0;
            data[i * image->w + j] = grayscale / 255.0; // Normaliser les valeurs de 0 à 1
        }
    }
    //  SDL_FreeSurface(image); // Libération de la mémoire de l'image
    return data;
}

/////////////////////////////////////CountFiles/////////////////////////////////////////
int CountFiles(char* path)
{
    int count = 0;
    DIR *d;
    struct dirent *dir;
    d = opendir(path);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if(strcmp(dir->d_name,".") != 0 && strcmp(dir->d_name,"..") != 0)
                count++;
        }
        closedir(d);
    }
    return count;
}

/////////////////////////////////////GetLabel/////////////////////////////////////////

int getPredictedLabel(Network *network) {
    int predictedLabel = 0;
    double maxOutput = network->layers[network->nbLayers - 1].neurons[0].value;

    for (int i = 1; i < network->sizeOutput; ++i) {
        if (network->layers[network->nbLayers - 1 ].neurons[i].value > maxOutput) {
            maxOutput = network->layers[network->nbLayers - 1 ].neurons[i].value;
            predictedLabel = i ;
        }
    }

    return predictedLabel;
}

//////////////////////////////////////////loadTrainingData//////////////////////////////////////////
#define NUM_CLASSES 10 // Nombre de classes (chiffres de 0 à 9)
#define IMG_WIDTH 28   // Largeur des images (à adapter selon vos images)
#define IMG_HEIGHT 28  // Hauteur des images (à adapter selon vos images)


typedef struct {
    double *data;
    int label;
} TrainingSample;

TrainingSample* loadTrainingData(const char *basePath, int numSamplesPerClass) {
    TrainingSample *trainingData = malloc(NUM_CLASSES * numSamplesPerClass * sizeof(TrainingSample));

    if (trainingData == NULL) {
        printf("Erreur d'allocation mémoire pour les données d'entraînement.\n");
        return NULL;
    }

    for (int class = 0; class < NUM_CLASSES; ++class) {
        for (int sample = 1; sample < numSamplesPerClass; ++sample) {
            char imagePath[PATH_MAX];
            sprintf(imagePath, "%s/%d/%d.png", basePath, class, sample); // Chemin de l'image spécifique

            SDL_Surface *image = GetImages(imagePath);
            if (image == NULL) {
                printf("Erreur lors du chargement de l'image : %s\n", imagePath);
                return NULL;
            }

            double *imageData = ImageToData(image);
            if (imageData == NULL) {
                printf("Erreur lors de la conversion de l'image en données : %s\n", imagePath);
                return NULL;
            }

            trainingData[class * numSamplesPerClass + sample].data = imageData;
            trainingData[class * numSamplesPerClass + sample].label = class;

            SDL_FreeSurface(image); // Libération de la mémoire de l'image
        }
    }

    return trainingData;
}


//////////////////////////////////////////loadTestData//////////////////////////////////////////
int* prepareTrainingLabels(int numClasses, int numSamplesPerClass) {
    int* trainingLabels = malloc(numClasses * numSamplesPerClass * sizeof(int));

    if (trainingLabels == NULL) {
        printf("Erreur d'allocation mémoire pour les étiquettes d'entraînement.\n");
        return NULL;
    }

    for (int class = 0; class < numClasses; ++class) {
        for (int sample = 0; sample < numSamplesPerClass; ++sample) {
            trainingLabels[class * numSamplesPerClass + sample] = class; // Assigner l'étiquette correspondante
        }
    }

    return trainingLabels;
}

//////////////////////////////////////////TrainNetwork//////////////////////////////////////////
void TrainNetwork(Network *network, TrainingSample *trainingData, int *trainingLabels, int numClasses, int numSamples, double learningRate, int epochs) {
    for (int epoch = 0; epoch < epochs; ++epoch) {
        double totalLoss = 0.0;


        for (int sample = 0; sample < numSamples; ++sample) {
            double *inputs = trainingData[sample].data;
            int label = trainingLabels[sample];

            // Effectuer la propagation avant pour obtenir la sortie prédite
            FrontPropagation(network, inputs);

            // Préparer les étiquettes attendues (one-hot encoding)
            double expectedOutput[numClasses];
            for (int i = 0; i < numClasses; ++i) {
                expectedOutput[i] = (i == label ) ? 1.0 : 0.0;
            }

            // Calculer la perte (loss) avec la sortie attendue
            double loss = 0.0;
            for (int i = 0; i < numClasses; ++i) {
                loss += 0.5* pow(expectedOutput[i] - network->layers[network->nbLayers - 1].neurons[i].value, 2);
               // printf("expectedOutput: %f\n", expectedOutput[i]);
               // printf("network->layers[network->nbLayers - 1].neurons[i].value: %f\n", network->layers[network->nbLayers - 1].neurons[i].value);
            }
            //printf("loss: %f\n", loss);
            totalLoss += loss;

            // Effectuer la rétropropagation pour calculer les deltas
            BackPropagation(network, expectedOutput);

            // Effectuer la descente de gradient pour ajuster les poids et les biais
            GradientDescent(network, learningRate);
        }
        //printf("totalLoss: %f\n", totalLoss);
        // Calculer la perte moyenne pour chaque epoch
        double averageLoss = totalLoss / numSamples;
        printf("Epoch %d - AverageLoss: %f\n", epoch + 1, averageLoss);
    }
}
//////////////////////////////////////////SaveNetwork//////////////////////////////////////////
void SaveNetwork(Network *network, const char *filename) {
    FILE *file = fopen(filename, "wb"); // Ouvrir le fichier en mode binaire

    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier pour sauvegarde.\n");
        return;
    }

    // Enregistrer les informations du réseau dans le fichier
    fwrite(&network->nbLayers, sizeof(unsigned int), 1, file);
    fwrite(&network->sizeInput, sizeof(unsigned int), 1, file);
    fwrite(&network->sizeHidden, sizeof(unsigned int), 1, file);
    fwrite(&network->sizeOutput, sizeof(unsigned int), 1, file);

    for (unsigned int i = 0; i < network->nbLayers; i++) {
        fwrite(&network->layers[i].nbNeurons, sizeof(unsigned int), 1, file);

        for (unsigned int j = 0; j < network->layers[i].nbNeurons; j++) {
            Neuron *neuron = &(network->layers[i].neurons[j]);

            fwrite(&neuron->nbWeights, sizeof(unsigned int), 1, file);
            fwrite(neuron->weights, sizeof(double), neuron->nbWeights, file);
            fwrite(&neuron->bias, sizeof(double), 1, file);
        }
    }

    fclose(file);
}
///////////////////////////////////////////LoadNetwork/////////////////////////////////////////

Network LoadNetwork(const char *filename) {
    FILE *file = fopen(filename, "rb"); // Ouvrir le fichier en mode binaire

    Network network;

    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier pour chargement.\n");
        // Retourner un réseau vide ou initialisé d'une manière spécifique en cas d'échec de chargement
        // Par exemple, initialiser un réseau par défaut ou retourner un réseau avec des paramètres par défaut
        return network;
    }

    // Lecture des informations du réseau à partir du fichier
    fread(&network.nbLayers, sizeof(unsigned int), 1, file);
    fread(&network.sizeInput, sizeof(unsigned int), 1, file);
    fread(&network.sizeHidden, sizeof(unsigned int), 1, file);
    fread(&network.sizeOutput, sizeof(unsigned int), 1, file);

    network.layers = malloc((network.nbLayers+1) * sizeof(Layer));

    for (unsigned int i = 0; i < network.nbLayers; i++) {
        fread(&network.layers[i].nbNeurons, sizeof(unsigned int), 1, file);
        network.layers[i].neurons = malloc(network.layers[i].nbNeurons * sizeof(Neuron));

        for (unsigned int j = 0; j < network.layers[i].nbNeurons; j++) {
            Neuron neuron;

            fread(&neuron.nbWeights, sizeof(unsigned int), 1, file);
            neuron.weights = malloc(neuron.nbWeights * sizeof(double));
            fread(neuron.weights, sizeof(double), neuron.nbWeights, file);
            fread(&neuron.bias, sizeof(double), 1, file);

            neuron.value = 0; // Initialisation des valeurs à 0 pour une utilisation ultérieure

            network.layers[i].neurons[j] = neuron;
        }
    }

    fclose(file);

    return network;
}
//////////////////////////////////////////main//////////////////////////////////////////
/*
int main() {
    srand(time(NULL)); // Initialisation du générateur de nombres aléatoires

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

    // Création du réseau de neurones avec la structure souhaitée (à adapter selon votre structure)
    Network network = NewNetwork(IMG_WIDTH * IMG_HEIGHT, 32, NUM_CLASSES, 1);
    InitNetwork(&network);

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
*/