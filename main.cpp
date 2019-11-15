#include <iostream>
#include <fstream>
#include <iomanip>
#include <conio.h>
#include <windows.h>

using namespace std;

#define CELL_WIDTH 4

char printPiece(int pieceCode) {

    char wPieces[7] = {'r', 'n', 'b', 'q', 'k', 'p'};
    char bPieces[7] = {'R', 'N', 'B', 'Q', 'K', 'P'};

    if(pieceCode < 0)
        return bPieces[abs(pieceCode) - 1];
    else if (pieceCode > 0)
        return wPieces[pieceCode - 1];
    else
        return ' ';


}

void printBoard(int board[][8]) {

    // Handler for printing in color
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, 15);

    // Printing the top line
    cout << setw(CELL_WIDTH) << "\t";
    for (int k = 0; k < 66; k++)
        (k == 65)? cout << endl : cout << "-";

    // Printing the board
    for (int i = 0; i < 8; i++) {
        SetConsoleTextAttribute(h, 10);
        cout << setw(CELL_WIDTH) << i + 1 << setw(CELL_WIDTH) << " ";
        SetConsoleTextAttribute(h, 15);
        cout << "|";

        for (int j = 0; j < 8; j++) {
            cout << setw(CELL_WIDTH) << printPiece(board[i][j]);
            cout << setw(CELL_WIDTH) << "|";
        }
        cout << endl;
        // Printing the bottom line
        cout << setw(CELL_WIDTH) << "\t";
        for (int k = 0; k < 66; k++)
            (k == 65)? cout << endl : cout << "-";
    }

    SetConsoleTextAttribute(h, 10);
    // Printing the a, b, c, ...
    cout << setw(CELL_WIDTH) << "\t";
    cout << setw(CELL_WIDTH + 1) << 'a';
    for (char c = 'b'; c <= 'h'; c++) {
        cout << setw((2 * CELL_WIDTH) + 0.8) << c;
    }
    SetConsoleTextAttribute(h, 15);

}

void movePiece(){



}



int main() {

    int board[8][8] = {
            {-1,-2,-3,-4,-5,-3,-2,-1},
            {-6,-6,-6,-6,-6,-6,-6,-6},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {6, 6, 6, 6, 6, 6, 6, 6},
            {1, 2, 3, 4, 5, 3, 2, 1}
    };

    printBoard(board);

    getch();
    return 0;
}