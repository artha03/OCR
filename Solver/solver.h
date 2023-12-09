#ifndef SOLVER_H
#define SOLVER_H

void afficherListeSudoku(int liste[]);
bool estGrilleSudokuValide(int liste[], int taille) ;
bool contientDoublons(int tableau[]) ;
bool estListeSudokuCorrecte(int liste[], int taille) ;
bool remplirListeSudokuRecursivement(int liste[], int index) ;
bool resoudreListeSudoku(int liste[]) ;
void exportListeSudoku(int liste[], char *path) ;
void exportListeSudokuFalse(int listeSudoku[], char *filename)  ;

#endif