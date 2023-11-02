#include <stdio.h>
#include <stdbool.h>
#include <string.h>
// Taille de la grille de sudoku (9x9)
#define TAILLE_GRILLE 9

void afficherListeSudoku(int liste[]) {
    for (int i = 0; i < TAILLE_GRILLE * TAILLE_GRILLE; i++) {
        if (liste[i] == 0)
            printf(".");
        else
            printf("%d", liste[i]);
        if((i + 1) % 3 == 0)
            printf("   ");
        if ((i + 1) % TAILLE_GRILLE == 0) {
            printf("\n");
        }
        if ((i + 1) % (TAILLE_GRILLE * 3) == 0 && i != TAILLE_GRILLE * TAILLE_GRILLE - 1) {
            printf("\n");
        }
    }
}



// Fonction pour vérifier si la liste a la bonne dimension pour une grille de sudoku (9x9)
bool estGrilleSudokuValide(int liste[], int taille) {
    // La taille correcte pour une grille de sudoku est de 9x9 = 81 éléments
    if (taille != 81)
        return false;

    // Vérifier que chaque élément de la liste est dans la plage 1-9
    for (int i = 0; i < 81; i++) {
        if (liste[i] < 0 || liste[i] > 9)
            return false;
    }

    return true;
}


// Fonction pour vérifier si une ligne, colonne ou région contient des doublons
bool contientDoublons(int tableau[]) {
    bool chiffres[10] = {false}; // Tableau pour marquer les chiffres déjà vus

    for (int i = 0; i < TAILLE_GRILLE; i++) {
        int chiffre = tableau[i];

        if (chiffre != 0) {
            if (chiffres[chiffre]) {
                return true; // Doublon trouvé
            } else {
                chiffres[chiffre] = true; // Marquer le chiffre comme vu
            }
        }
    }

    return false; // Pas de doublons
}

// Fonction pour vérifier si une liste représentant une grille de sudoku est correctement remplie
bool estListeSudokuCorrecte(int liste[], int taille) {
    // Vérifier que la taille de la liste est correcte
    if (taille != TAILLE_GRILLE * TAILLE_GRILLE) {
        return false;
    }

    // Créer une grille à partir de la liste
    int grille[TAILLE_GRILLE][TAILLE_GRILLE];
    int index = 0;
    for (int i = 0; i < TAILLE_GRILLE; i++) {
        for (int j = 0; j < TAILLE_GRILLE; j++) {
            grille[i][j] = liste[index++];
        }
    }

    // Vérifier les lignes
    for (int i = 0; i < TAILLE_GRILLE; i++) {
        if (contientDoublons(grille[i])) {
            return false;
        }
    }

    // Vérifier les colonnes
    for (int j = 0; j < TAILLE_GRILLE; j++) {
        int colonne[TAILLE_GRILLE];
        for (int i = 0; i < TAILLE_GRILLE; i++) {
            colonne[i] = grille[i][j];
        }
        if (contientDoublons(colonne)) {
            return false;
        }
    }

    // Vérifier les régions (carrés 3x3)
    for (int i = 0; i < TAILLE_GRILLE; i += 3) {
        for (int j = 0; j < TAILLE_GRILLE; j += 3) {
            int region[TAILLE_GRILLE];
            int index = 0;
            for (int x = 0; x < 3; x++) {
                for (int y = 0; y < 3; y++) {
                    region[index++] = grille[i + x][j + y];
                }
            }
            if (contientDoublons(region)) {
                return false;
            }
        }
    }

    return true; // La grille est correcte
}

bool remplirListeSudokuRecursivement(int liste[], int index) {
    // Cas de base : Si on a rempli toute la liste, c'est terminé
    if (index == TAILLE_GRILLE * TAILLE_GRILLE) {
        return true;
    }

    // Si la case actuelle n'est pas vide, passer à la suivante
    if (liste[index] != 0) {
        return remplirListeSudokuRecursivement(liste, index + 1);
    }

    // Essayer de placer les chiffres de 1 à 9 dans la case
    for (int num = 1; num <= TAILLE_GRILLE; num++) {
        liste[index] = num;

        // Vérifier si le placement est valide
        if (estListeSudokuCorrecte(liste,TAILLE_GRILLE*TAILLE_GRILLE) && remplirListeSudokuRecursivement(liste, index + 1)) {
            return true;
        }

        // Si le placement n'est pas valide, réinitialiser la case
        liste[index] = 0;
    }

    // Aucun chiffre valide n'a été trouvé, retourner false pour indiquer une impasse
    return false;
}

// Fonction pour résoudre une liste représentant une grille de sudoku
bool resoudreListeSudoku(int liste[]) {
    return remplirListeSudokuRecursivement(liste, 0);
}

void exportListeSudoku(int liste[], char *path) {
    //creer un fichier
    FILE *fichier = fopen(path, "w");
    if (fichier == NULL) {
        printf("Impossible d'ouvrir le fichier %s\n", path);
        return;
    }

    //ecrire dans le fichier
    for (int i = 0; i < TAILLE_GRILLE * TAILLE_GRILLE; i++) {
        if (liste[i] == 0)
            fprintf(fichier, ".");
        else
            fprintf(fichier, "%d", liste[i]);
        if((i + 1) % 3 == 0)
            fprintf(fichier, "   ");
        if ((i + 1) % TAILLE_GRILLE == 0) {
            fprintf(fichier, "\n");
        }
        if ((i + 1) % (TAILLE_GRILLE * 3) == 0 && i != TAILLE_GRILLE * TAILLE_GRILLE - 1) {
            fprintf(fichier, "\n");
        }
    }

    fclose(fichier);
}

/*int main() {
    int listeSudoku[TAILLE_GRILLE * TAILLE_GRILLE] = {
            5, 3, 0, 0, 7, 0, 0, 0, 0,
            6, 0, 0, 1, 9, 5, 0, 0, 0,
            0, 9, 8, 0, 0, 0, 0, 6, 0,
            8, 0, 0, 0, 6, 0, 0, 0, 3,
            4, 0, 0, 8, 0, 3, 0, 0, 1,
            7, 0, 0, 0, 2, 0, 0, 0, 6,
            0, 6, 0, 0, 0, 0, 2, 8, 0,
            0, 0, 0, 4, 1, 9, 0, 0, 5,
            0, 0, 0, 0, 8, 0, 0, 7, 9
    };

    exportListeSudoku(listeSudoku, "/home/xaviax/spe/OCR/Solver/grille_sudoku.txt");

    printf("Liste représentant la grille de sudoku à résoudre :\n");
    afficherListeSudoku(listeSudoku);

    if (resoudreListeSudoku(listeSudoku)) {
        printf("\nListe représentant la grille de sudoku résolue :\n");
        afficherListeSudoku(listeSudoku);
    } else {
        printf("\nAucune solution trouvée pour la liste représentant la grille de sudoku.\n");
    }

    printf("\n");

    exportListeSudoku(listeSudoku, "/home/xaviax/spe/OCR/Solver/grille_sudoku_resolue.txt");

    return 0;
}*/

//Main qui en entrer prend une liste d'argument, le premier argument est le nom du fichier d'entrer contenant la grille de sudoku a resoudre,
//en sortie le programme affiche la grille de sudoku resolue dans le fichier de sortie dont le nom est le meme que le fichier d'entrer avec l'extension".result"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <fichier>\n", argv[0]);
        return 1;
    }

    // Lire la liste représentant la grille de sudoku à résoudre
    FILE *fichier = fopen(argv[1], "r");
    if (fichier == NULL) {
        printf("Impossible d'ouvrir le fichier %s\n", argv[1]);
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
        exportListeSudoku(listeSudoku, strcat(argv[1], ".result"));
    }
}