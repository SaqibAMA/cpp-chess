#include <iostream>
#include <fstream>
#include <iomanip>
#include <conio.h>

using namespace std;

#define CELL_WIDTH 4

void printBoard(int board[][8]) {

    for (int k = 0; k < 66; k++)
        (k == 65)? cout << endl : cout << "-";

    for (int i = 0; i < 8; i++) {
        cout << "|";
        for (int j = 0; j < 8; j++) {
            cout << setw(CELL_WIDTH) << board[i][j];
            cout << setw(CELL_WIDTH) << "|";
        }
        cout << endl;
        for (int k = 0; k < 67; k++)
            (k == 66)? cout << endl : cout << "-";
    }

}

int main() {

    int board[8][8] = {
            {-1,-2,-3,-4,-4,-3,-2,-1},
            {-6,-6,-6,-6,-6,-6,-6,-6},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {6, 6, 6, 6, 6, 6, 6, 6},
            {1, 2, 3, 4, 4, 3, 2, 1}
    };

    printBoard(board);

    getch();
    return 0;
}