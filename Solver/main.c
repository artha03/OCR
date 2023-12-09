#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "solver.h"
#include "res_sudoku.h"

#define TAILLE_GRILLE 9

int main() {

    // Lire la liste représentant la grille de sudoku à résoudre
    FILE *fichier = fopen("grille_sudoku.txt", "r");
    if (fichier == NULL) {
        printf("Impossible d'ouvrir le fichier %s\n", "grille_sudoku.txt");
        return 1;
    }

    int listeSudoku[TAILLE_GRILLE * TAILLE_GRILLE];
    int index = 0;
    char c;
    while ((c = fgetc(fichier)) != EOF) {
        if (c >= '1' && c <= '9') {
            listeSudoku[index++] = c - '0';
        } else if (c == '.') {
            listeSudoku[index++] = 0;
        }
    }
    fclose(fichier);


    // Vérifier que la liste est valide
    if (!estGrilleSudokuValide(listeSudoku, index)) {
        printf("La liste n'est pas valide.\n");
        return 1;
    }

    // Résoudre la liste représentant la grille de sudoku
    if (resoudreListeSudoku(listeSudoku)) {
        // Exporter la liste représentant la grille de sudoku résolue
        exportListeSudoku(listeSudoku, "grille_sudoku_resolue.txt");
    }

    const char *sudokuFile = "grille_sudoku_resolue.txt";
    const char *imagePath = "grille_vide.png"; // Chemin de votre image de fond
    const char *numberFile = sudokuFile; // Fichier contenant les chiffres de la grille

    int result = displaySudoku(imagePath, numberFile);

    if (result == 0) {
        printf("Grille de sudoku affichée et enregistrée avec succès !\n");
    } else {
        printf("Une erreur s'est produite lors de l'affichage et de l'enregistrement de la grille de sudoku.\n");
    }

    return result;
}