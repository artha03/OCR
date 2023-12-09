#ifndef APPRENTISSAGE_H
#define APPRENTISSAGE_H

typedef struct {
    double *data;
    int label;
} TrainingSample;

SDL_Surface* GetImages(char *path);
Uint32 get_pixel(SDL_Surface *surface, int x, int y) ;
double *ImageToData(SDL_Surface *image);
int CountFiles(char* path);
int getPredictedLabel(Network *network) ;
TrainingSample* loadTrainingData(const char *basePath, int numSamplesPerClass) ;
int* prepareTrainingLabels(int numClasses, int numSamplesPerClass) ;
void TrainNetwork(Network *network, TrainingSample *trainingData, int *trainingLabels, int numClasses, int numSamples, double learningRate, int epochs);
void SaveNetwork(Network *network, const char *filename);
Network LoadNetwork(const char *filename);

#endif