#include <iostream>
#include <iomanip>
#include <conio.h>
#include <windows.h>
#include <cwchar>

using namespace std;

#define CELL_WIDTH 4

char printPiece(int pieceCode) {

    /*
     * This function takes an integer from the board array
     * and returns the character associated with it.
     */

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

    /*
     * This function prints the current state of board
     * whenever it is called.
     */

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
                SetConsoleTextAttribute(h, 0 | BACKGROUND_GREEN | BACKGROUND_RED);
                cout << setw(CELL_WIDTH) << printPiece(board[i][j]);
                cout << setw(CELL_WIDTH / 2) << "   ";
                SetConsoleTextAttribute(h, 15);
            }
            else if (board[i][j] > 0) {
                SetConsoleTextAttribute(h, 0 | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY);
                cout << setw(CELL_WIDTH) << printPiece(board[i][j]);
                cout << setw(CELL_WIDTH / 2) << "   ";
                SetConsoleTextAttribute(h, 15);
            }
            else {
                if(((j+i)%2) - 1 == 0)
                    SetConsoleTextAttribute(h, 15 | BACKGROUND_GREEN);
                cout << setw(CELL_WIDTH) << printPiece(board[i][j]);
                cout << setw(CELL_WIDTH / 2) << "   ";
                SetConsoleTextAttribute(h, 15);
            }

            cout /*<< setw(CELL_WIDTH / 2)*/ << "|";
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

    if ((cord[0] >= 'a' && cord[0] <= 'h') && (cord[1] >= '1' && cord[1] <= '8') && (cord[2] == '\0')) {

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

void gotoxy(int x, int y) {

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD scrn;

    scrn.X = x, scrn.Y = y;

    SetConsoleCursorPosition(h,scrn);

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

    int moveCounter = 0;

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    gotoxy(80, 0 + moveCounter);
    cout << ":: RECENT MOVES ::" << endl;
    gotoxy(0, 0);

    while (true && moveCounter < INT_MAX) {

        printBoard(board);

        char srcCell[3];
        char destCell[3];

        cout << endl << endl;

        /*This segment handles the input for moves.*/
        if(moveCounter > 0) {

            /*Printing these in black to cover up the previous
            input due to cursor re-positioning.*/
            SetConsoleTextAttribute(h, 0);
            gotoxy(19, 19);
            cout << "\n:: Enter cell to select: " << srcCell << endl;
            cout << ":: Enter destination cell: " << destCell << endl;


            // Printing these in white
            gotoxy(19, 19);
            SetConsoleTextAttribute(h, 15);
            cout << "\n:: Enter cell to select: ";
            cin >> srcCell;
            cout << ":: Enter destination cell: ";
            cin >> destCell;
            cout << endl;

        }
        else {
            // Taking input for the first time
            cout << "\n:: Enter cell to select: ";
            cin >> srcCell;
            cout << ":: Enter destination cell: ";
            cin >> destCell;
        }

        int srcX, srcY, destX, destY;


        if ((chessCordToIndex(srcCell, srcX, srcY) &&  chessCordToIndex(destCell, destX, destY)) &&
        !isEmpty(board[srcY][srcX]) &&
        !hasSamePiece(board[srcY][srcX], board[destY][destX])) {
                board[destY][destX] = board[srcY][srcX];
                board[srcY][srcX] = 0;

                moveCounter++;

                gotoxy(80, 1 + moveCounter);
                cout << srcCell << " to "<< destCell;

                //printBoard(board);
        }
        else {
            cout << "Invalid move.";
        }

        gotoxy(0, 0);
    }


    getch();
    return 0;
}