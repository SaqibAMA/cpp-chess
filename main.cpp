#include <iostream>
#include <fstream>
#include <iomanip>
#include <conio.h>
#include <windows.h>
#include <ctime>

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
    for (int i = 0, row = 8; i < 8; i++, row--) {
        SetConsoleTextAttribute(h, 10);
        cout << setw(CELL_WIDTH) << row << setw(CELL_WIDTH) << " ";
        SetConsoleTextAttribute(h, 15);
        cout << "|";

        for (int j = 0; j < 8; j++) {
            if (board[i][j] < 0) {
                SetConsoleTextAttribute(h, 15 | 7);
                cout << setw(CELL_WIDTH) << printPiece(board[i][j]);
                SetConsoleTextAttribute(h, 15);
            }
            else if (board[i][j] > 0) {
                SetConsoleTextAttribute(h, 0 | 15);
                cout << setw(CELL_WIDTH) << printPiece(board[i][j]);
                SetConsoleTextAttribute(h, 15);
            }
            else
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

bool chessCordToIndex(char cord[], int &x, int &y) {

    // Function to convert chess coordinates to array coordinates
    // Returns false if the cell location is invalid

    if ((cord[0] >= 'a' && cord[0] <= 'h') && (cord[1] >= '1' && cord[1] <= '8')) {

        x = cord[0] - 97;
        y = 8 - (cord[1] - 48);
        return true;
    }

    return false;
}

bool isEmpty(int destCell) {

    // Checks if the cell is empty

    return (destCell == 0);

}

bool hasSamePiece(int srcCell, int destCell) {

    // Checks if the destination cell has the same piece

    return (srcCell < 0 && destCell < 0) || (srcCell > 0 && destCell > 0);

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

    while (true) {
        char srcCell[3];
        char destCell[3];

        cout << endl << endl;
        cout << "Enter cell to select: ";
        cin >> srcCell;
        cout << "Enter destination cell: ";
        cin >> destCell;
        cout << endl;

        int srcX, srcY, destX, destY;


        if ((chessCordToIndex(srcCell, srcX, srcY) &&  chessCordToIndex(destCell, destX, destY)) &&
        !isEmpty(board[srcY][srcX]) &&
        !hasSamePiece(board[srcY][srcX], board[destY][destX])) {
                board[destY][destX] = board[srcY][srcX];
                board[srcY][srcX] = 0;
                printBoard(board);
        }
        else {
            cout << "Invalid move.";
        }

        cout << endl;
    }

    getch();
    return 0;
}