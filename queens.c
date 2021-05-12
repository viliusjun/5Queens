/*
Vilius Junevičius, programų sistemos VU 1 kursas, 4 grupė, 1 pogrupis
vilius.junevcius@mif.stud.vu.lt

Antroji ADS užduotis, 5 variantas:
Sustatyti šachmatų lentoje 5 valdoves, kad kiekvieną langelį kirstų bent viena valdovė. Rasti VISUS VARIANTUS.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_BOARD_SIZE 8 // Maksimalus šachmatų lentos kraštinės langelių skaičisu
#define MAX_QUEENS 5 // Maksimalus valdovių skaičius
#define FILE_NAME "Protokolas.txt"

int test = 0; // Atliktų bandymų kiekis
int solutions = 0; // Rastų sprendimų kiekis

struct coordinates{
    int row[MAX_QUEENS + 1];
    int col[MAX_QUEENS + 1];
};

// Rodyklė į struktūrą, kurioje saugomi masyvai su coordinatėmis, kuriuose yra pateikti teisingi sprendimo būdo variantai
struct coordinates *solutionCoordinates;

int placeQueen(FILE *protocol, int board[], int queens, int sizeOfBoard, int placedQueens, int position);
void attack(int board[], int sizeOfBoard, int row, int col, int placedQueens);
void remove(int board[], int sizeOfBoard, int placedQueens);
bool checkIfAllAttacked(int board[], int sizeOfBoard);
void inputWithValplacedQueensation(int *n, int(*isValplacedQueensFunc)(int), int errorNum);
int isBetween1AndMAX_BOARD_SIZE(int n);
int isBetween1AndMAX_QUEENS(int n);
int isPositive(int n);
void printToFile1(FILE *protocol, int sizeOfBoard, int queens);
void printToFile2(FILE *protocol, int queens);

int main(){
    printf("\nSustatyti šachmatų lentoje 5 valdoves, kad kiekvieną langelį kirstų bent viena valdovė. Rasti VISUS VARIANTUS.\n\n");

    // Vartotojas įveda šachmatų lentos kraštinės langelių skaičių ir valdovių skaičių
    int sizeOfBoard = 0, queens = 0;
    printf("Įveskite šachmatų lentos kraštinės langelių skaičių:\n");
    inputWithValplacedQueensation(&sizeOfBoard, isPositive, 0);

    printf("\nĮveskite valdovių skačių:\n");
    inputWithValplacedQueensation(&queens, isPositive, 1);

    solutionCoordinates = (struct coordinates *) malloc(sizeof(struct coordinates));

    // Suvedami duomenys į protokolą
    FILE *protocol;
    protocol = fopen(FILE_NAME, "w");
    printToFile1(protocol, sizeOfBoard, queens);

    if (queens > MAX_QUEENS && sizeOfBoard > MAX_BOARD_SIZE){
        fprintf(protocol, "Tolimesni duomenys neanalizuojami.\n\n\nIII DALIS - REZULTATAI\n\nSprendimo būdai neanalizuojami, nes įvestas valdovių skaičius ir šachmatų lentos kraštinės langelių skaičius viršija nurodytą ribą. Valdovių riba: %d. Šachmatų lentos riba: %d\n", MAX_QUEENS, MAX_BOARD_SIZE);
        printf("\nĮvestas valdovių skaičius ir šachmatų lentos kraštinės langelių skaičius viršija nurodytą ribą.\n\n");
        printf("Sukurtas protokolas: %s\n", FILE_NAME);
        return 0;
    }

    // Valdovių skaičius viršija riba
    if (queens > MAX_QUEENS){
        fprintf(protocol, "Tolimesni duomenys neanalizuojami.\n\n\nIII DALIS - REZULTATAI\n\nSprendimo būdai neanalizuojami, nes įvestas valdovių skaičius viršija nurodytą ribą. Valdovių riba: %d.\n", MAX_QUEENS);
        printf("\nĮvestas valdovių skaičius viršija nurodytą ribą.\n\n");
        printf("Sukurtas protokolas: %s\n", FILE_NAME);
        return 0;
    }

    // Šachmatų lenta viršija riba
    if (sizeOfBoard > MAX_BOARD_SIZE){
        fprintf(protocol, "Tolimesni duomenys neanalizuojami.\n\n\nIII DALIS - REZULTATAI\n\nSprendimo būdai neanalizuojami, nes įvestas šachmatų lentos kraštinės langelių skaičius viršija nurodytą ribą. Šachmatų lentos kraštinės langelių riba: %d.\n", MAX_BOARD_SIZE);
        printf("\nĮvestas šachmatų lentos kraštinės langelių skaičius viršija nurodytą ribą.\n\n");
        printf("Sukurtas protokolas: %s\n", FILE_NAME); 
        return 0;
    }

    // Jeigu valdovių skaičius yra didesnis, negu šachmatų lentos langelių skaičius, tai baigiam programos darbą
    if (queens > sizeOfBoard * sizeOfBoard){
        fprintf(protocol, "Nevykdoma.\n\n\nIII DALIS - REZULTATAI\n\nSprendimo būdų nėra, nes visų valdovių sustatyti neįmanoma. Valdovių skaičius didesnis nei šachmatų lentos langelių skaičius.\n");
        printf("\nVisų valdovių sustatyti neįmanoma. Valdovių skaičius didesnis nei šachmatų lentos langelių skaičius.\n\n");
        printf("Sukurtas protokolas: %s\n", FILE_NAME);
        return 0;
    }

    // Šachmatų lenta inicializuojama nuliais
    int board[sizeOfBoard*sizeOfBoard];
    for (int i = 0; i < sizeOfBoard * sizeOfBoard; ++i){
        board[i] = 0;
    }

    // Tikrinam, sprendimo būdus iš eilės
    for (int i = 0; i < sizeOfBoard * sizeOfBoard; ++i){
        if (placeQueen(protocol, board, queens, sizeOfBoard, 0, i) == -1){
            break;
        }
    }

    printToFile2(protocol, queens);
    
    printf("Teisingų sprendinių kiekis: %d\n", solutions);
    printf("Atlikta bandymų: %d\n", test);
    printf("Sukurtas protokolas: %s\n", FILE_NAME);

    fclose(protocol);
    free(solutionCoordinates);
    return 0;
}

// Funckija, kuri padeda valdovę į norimą poziciją
// Funkcija grąžina:
// 1, jeigu rastas sprendinys
// 0, jeigu nerastas sprendinys
// -1, jeigu iš anksto matome, jog padėjus tam tikrą valdovę, statydami kitas valdoves, sprendinio nebegausime
int placeQueen(FILE *protocol, int board[], int queens, int sizeOfBoard, int placedQueens, int position){
    test++;

    int row = position / sizeOfBoard;
    int col = position % sizeOfBoard;
    placedQueens++;

    solutionCoordinates[solutions].row[placedQueens] = row;
    solutionCoordinates[solutions].col[placedQueens] = col;

    attack(board, sizeOfBoard, row, col, placedQueens);

    fprintf(protocol, "\n%8d) ", test);
    for (int i = 0; i < placedQueens; ++i){
        fprintf(protocol, "-");
    }
    fprintf(protocol, " Pozicija: (%d, %d). ", row, col);

    // Jeigu sudėtos visos karalienės, tikrinam, ar visi langeliai kertami bent viena karaliene
    if (placedQueens == queens){
        bool allAttacked = checkIfAllAttacked(board, sizeOfBoard);

        if (allAttacked == false){
            fprintf(protocol, "NE visus langelius kerta.");
            remove(board, sizeOfBoard, placedQueens);
            return 0;
        }
        else {
            fprintf(protocol, "Visus langelius kerta.");
            solutions++;
            solutionCoordinates = (struct coordinates*) realloc(solutionCoordinates, (solutions + 1)*sizeof(struct coordinates));
            solutionCoordinates[solutions] = solutionCoordinates[solutions - 1];
            remove(board, sizeOfBoard, placedQueens);
            return 1;
        }

    }
    else {
        // Jeigu, toliau nebegalėsim sustatyti karalienių, tai nebe tikrinis tolimesnių sprendimo būdų
        if (position + 1 >= sizeOfBoard * sizeOfBoard || sizeOfBoard * sizeOfBoard - (position + 1) < queens - placedQueens){
            fprintf(protocol, "Nebe galime sutalpinti karalienių lentoje.");
            return -1;
        }

        // Vykdome rekursiją, statydami sekančią karalienę vienu langeliu toliau
        for (int i = position + 1; i < sizeOfBoard * sizeOfBoard; i++) {
            int rez = placeQueen(protocol, board, queens, sizeOfBoard, placedQueens, i);
            if (rez == -1){
                break;
            }
        }

        fprintf(protocol, " Backtrack.");
        remove(board, sizeOfBoard, placedQueens);
        return 1;
    } 
}

// Funkcija, kuri užpildo tuščius langelius, kuriuos nurodyta karalienė kerta
void attack(int board[], int sizeOfBoard, int row, int col, int placedQueens){

    //kerta save
    if (board[row * sizeOfBoard + col] == 0){
        board[row * sizeOfBoard + col] = placedQueens;
    }

    // kerta vertikaliai
    for (int i = 0; i < sizeOfBoard; ++i){     
        if (board[i * sizeOfBoard + col] == 0){
            board[i * sizeOfBoard + col] = placedQueens;
        }
    }

    // kerta horizontaliai
    for (int i = 0; i < sizeOfBoard; ++i){
        if (board[row*sizeOfBoard+i] == 0){
            board[row*sizeOfBoard+i] = placedQueens;
        }
    }

    // kerta pagrindine įstrižaine
    int temp = 0;
    for (int i = row; i < sizeOfBoard; ++i){
        if (col - temp >= 0){
            if (board[i * sizeOfBoard + (col - temp)] == 0){
                board[i*sizeOfBoard + (col - temp)] = placedQueens;
            }
        }

        if (col + temp < sizeOfBoard){
            if (board[i * sizeOfBoard + (col + temp)] == 0){
                board[i*sizeOfBoard+(col+temp)] = placedQueens;
            }
        }
        temp++;
    }

    // kerta šalutine įstrižaine
    temp = 0;
    for (int i = row; i >= 0; --i){
        if (col - temp >= 0){
            if (board[i * sizeOfBoard + (col - temp)] == 0){
                board[i * sizeOfBoard + (col - temp)] = placedQueens;
            }
        }

        if (col + temp < sizeOfBoard){
            if (board[i * sizeOfBoard + (col + temp)] == 0){
                board[i * sizeOfBoard + (col + temp)] = placedQueens;
            }
        }
        temp++;
    }

    return;
}

// Funkcija, kuri išvalo visus langelius, kuriuos kerta nurodyta karalenė
void remove(int board[], int sizeOfBoard, int placedQueens){
    for (int i = 0; i < sizeOfBoard * sizeOfBoard; ++i){
        if (board[i] == placedQueens){
            board[i] = 0;
        }
    }

    return;
}

// Funkcija, kuri patikrina, ar visi langeliai kertami bent viena karaliene
bool checkIfAllAttacked(int board[], int sizeOfBoard){
    bool allAttacked = true;

    for (int i = 0; i < sizeOfBoard * sizeOfBoard; i++){
        if (board[i] == 0){
            allAttacked = false;
        }
    }

    return allAttacked;
}

// Funkcija, kuri užtikrina validaciją
void inputWithValplacedQueensation(int *n, int(*isValplacedQueensFunc)(int), int errorNum){

    while (scanf("%d", n) != 1 || !isValplacedQueensFunc(*n)){

        while (getchar() != '\n');
        // if (errorNum == 0){
        //     printf("Įvestis bloga. Šachmatų kraštinės langelių skaičiu turi būti tarp 1 ir %d.\n", MAX_BOARD_SIZE);
        // }
        // else if (errorNum == 1){
        //     printf("Įvestis bloga. Valdovių skaičiu turi būti tarp 1 ir %d.\n", MAX_QUEENS);
        // }
        printf("Įvestis bloga. Turi būti įvestas teigiamas skaičius.\n");
    }

    return;
}

// Funkcija, kuri grąžina tiesą, jei įvestas skaičius yra tarp 1 ir maksimalaus šachmatų lentos kraštinės langelių skaičiaus
int isBetween1AndMAX_BOARD_SIZE(int n){
    return n >= 1 && n <= MAX_BOARD_SIZE;
}

// Funkcija, kuri grąžina tiesą, jei įvestas skaičius yra tarp 1 ir maksimalaus valdovių skaičaus
int isBetween1AndMAX_QUEENS(int n){
    return n >= 1 && n <= MAX_QUEENS;
}

// Funkcija, kuri grąžina tiesą, jei įvestas teigiamas skaičius
int isPositive(int n){
    return n >= 1;
}

// Funkcija, kuri išspausdina reikiamus rezultatus į failą
void printToFile1(FILE *protocol, int sizeOfBoard, int queens){
    fprintf(protocol, "Vilius Junevičius, programų sistemos 1 kursas, 1 grupė, 1 pogrupis\nAntroji užduotis, 5 variantas\n\nSąlyga:\nSustatyti šachmatų lentoje 5 valdoves, kad kiekvieną langelį kirstų bent viena valdovė. Rasti VISUS VARIANTUS.\nRibojimai:\nMaksimalus šachmatų lentos dydis %dx%d.\nMaksimalus valdovių skaičius %d.\n\n\n\n", MAX_BOARD_SIZE, MAX_BOARD_SIZE, MAX_QUEENS);
    fprintf(protocol, "I DALIS - ĮVESTI DUOMENYS\n\nValdovių skaičius: %d\nŠachmatų lentos dydis: %dx%d\n", queens, sizeOfBoard, sizeOfBoard);

    if (sizeOfBoard <= MAX_BOARD_SIZE){
        fprintf(protocol, "Šachmatų lenta:\n\n");

        for (int i = 0; i < sizeOfBoard + 2; ++i){
            for (int j = 0; j < sizeOfBoard + 2; ++j){
                if (i == 0){
                    if (j == 0){
                        fprintf(protocol, "  ");
                    }
                    else if (j == sizeOfBoard + 1){
                        fprintf(protocol, "y ");
                    }
                    else {
                        fprintf(protocol, "%d ", j - 1);
                    }
                }
                else if (j == 0){
                    if (i == sizeOfBoard + 1){
                        fprintf(protocol, "x ");
                    }
                    else {
                        fprintf(protocol, "%d ", i - 1);
                    }
                }
                else if (i == sizeOfBoard + 1){
                    fprintf(protocol, "  ");
                }
                else if (j == sizeOfBoard + 1){
                    fprintf(protocol, "  ");
                }
                else {
                    fprintf(protocol, "X ");
                }
            }
            fprintf(protocol, "\n");
        }
    }
    fprintf(protocol, "\n\n");

    fprintf(protocol, "II DALIS - VYKDYMAS\n", sizeOfBoard, sizeOfBoard, queens);

    return;
}

// Funkcija, kuri išspausdina reikiamus rezultatus į failą
void printToFile2(FILE *protocol, int queens){
    fprintf(protocol, "\n\n\nIII DALIS - SPRENDIMAI\n\n");
    fprintf(protocol, "Rasta teisingų sprendinių: %d\nĮvykdyta bandymų: %d\n\n", solutions, test);

    for (int i = 0; i < solutions; ++i){
        fprintf(protocol, "%4d) ", i + 1);
        for (int j = 1; j <= queens; ++j){
            fprintf(protocol, " (%d, %d) ", solutionCoordinates[i].row[j], solutionCoordinates[i].col[j]);
        }
        fprintf(protocol, "\n");
    }
}