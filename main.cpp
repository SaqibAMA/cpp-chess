#include <iostream>
#include <iomanip>
#include <conio.h>
#include <windows.h>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <ctime>
#include "connector.h"

using namespace std;

#define CELL_WIDTH 4

char printPiece(int pieceCode);
void printBoard(int board[][8]);
bool chessCordToIndex(char cord[], int &x, int &y);
bool isEmpty(int destCell);
bool hasSamePiece(int srcCell, int destCell);
void gotoxy(int x, int y);
bool indExists(int ind);
bool isKingMove(int board[][8], int tempBoard[][8], int srcX, int srcY, int destX, int destY);
void genMoves(int board[][8], int mvTrack[][8]);
void checkPath(int board[][8], int mvTrack[][8], int checkX, int checkY);
bool kingInCheckB(int board[][8], int &checkX, int &checkY);
bool movePiece(int board[][8], int srcX, int srcY, int destX, int destY, int &turn, bool &wCanCastle, bool &bCanCastle, int movedCells[][8]);


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

bool indExists(int ind) {

    // Returns if the index provided is within the range of a chess array

    return (ind >= 0 && ind < 8);

}

bool isKingMove(int board[][8], int tempBoard[][8], int srcX, int srcY, int destX, int destY) {

    if (indExists(destY) && indExists(destX)) {
        if (hasSamePiece(board[srcY][srcX], board[destY][destX]) && !isEmpty(board[destY][destX])) {

            return false;

        } else {

            int checkX = -1, checkY = -1;

            tempBoard[destY][destX] = tempBoard[srcY][srcX];
            tempBoard[srcY][srcX] = 0;

            if (kingInCheckB(tempBoard, checkX, checkY)) {
                tempBoard[srcY][srcY] = tempBoard[destY][destX];
                tempBoard[destY][destX] = 0;
                return false;
            } else {
                tempBoard[srcY][srcY] = tempBoard[destY][destX];
                tempBoard[destY][destX] = 0;
                return true;
            }

        }
    }
    else {
        return false;
    }

}

bool kingHasMoves(int board[][8]) {

    // Copying the board
    int tempBoard[8][8];

    int kingLocX = 4, kingLocY = 0;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {

            tempBoard[i][j] = board[i][j];
            if (board[i][j] == -5) {

                kingLocX = j;
                kingLocY = i;

            }

        }
    }

    if (isKingMove(board, tempBoard, kingLocX, kingLocY, kingLocX - 1, kingLocY) ||
    isKingMove(board, tempBoard, kingLocX, kingLocY, kingLocX + 1, kingLocY) ||
    isKingMove(board, tempBoard, kingLocX, kingLocY, kingLocX, kingLocY + 1) ||
    isKingMove(board, tempBoard, kingLocX, kingLocY, kingLocX, kingLocY - 1) ||
    isKingMove(board, tempBoard, kingLocX, kingLocY, kingLocX + 1, kingLocY + 1) ||
    isKingMove(board, tempBoard, kingLocX, kingLocY, kingLocX + 1, kingLocY - 1) ||
    isKingMove(board, tempBoard, kingLocX, kingLocY, kingLocX - 1, kingLocY + 1) ||
    isKingMove(board, tempBoard, kingLocX, kingLocY, kingLocX - 1, kingLocY - 1)) {
        return true;
    }

    return true;

}

void genMoves(int board[][8], int mvTrack[][8]){

    for (int i = 0; i < 8; i++) {

        for (int j = 0; j < 8; j++) {

            if (board[i][j] == -6) {

                if (isEmpty(board[i + 1][j])) {
                    mvTrack[i + 1][j] += 1;

                    if (isEmpty(board[i + 2][j]) && i == 1) {
                        mvTrack[i + 2][j] += 1;
                    }

                }
                if (!isEmpty(board[i + 1][j + 1]) && !hasSamePiece(board[i][j], board[i + 1][j + 1])) {
                    mvTrack[i + 1][j + 1] += 1;
                }
                if (!isEmpty(board[i + 1][j - 1]) && !hasSamePiece(board[i][j], board[i + 1][j - 1])) {
                    mvTrack[i + 1][j - 1] += 1;
                }

            }
            if (board[i][j] == -1 || board[i][j] == -4) {

                int k = 1;
                bool pathBlocked = false;
                while (i + k < 8 && !pathBlocked) {
                    // bottom
                    if (board[i + k][j] < 0) {
                        pathBlocked = true;
                    }
                    else if (board[i + k][j] > 0) {
                        mvTrack[i + k][j] += 1;
                        pathBlocked = true;
                    }
                    else {
                        mvTrack[i + k][j] += 1;
                    }

                    k++;
                }

                k = 1;
                pathBlocked = false;
                while (j + k < 8 && !pathBlocked) {
                    // right
                    if (board[i][j + k] < 0) {
                        pathBlocked = true;
                    }
                    else if (board[i][j + k] > 0) {
                        mvTrack[i][j + k] += 1;
                        pathBlocked = true;
                    }
                    else {
                        mvTrack[i][j + k] += 1;
                    }

                    k++;
                }

                k = 1;
                pathBlocked = false;
                while (i - k >= 0 && !pathBlocked) {
                    // top
                    if (board[i - k][j] < 0) {
                        pathBlocked = true;
                    }
                    else if (board[i - k][j] > 0) {
                        mvTrack[i - k][j] += 1;
                        pathBlocked = true;
                    }
                    else {
                        mvTrack[i - k][j] += 1;
                    }

                    k++;
                }

                k = 1;
                pathBlocked = false;
                while (j - k >= 0 && !pathBlocked) {
                    // left
                    if (board[i][j - k] < 0) {
                        pathBlocked = true;
                    }
                    else if (board[i][j - k] > 0) {
                        mvTrack[i][j - k] += 1;
                        pathBlocked = true;
                    }
                    else {
                        mvTrack[i][j - k] += 1;
                    }

                    k++;
                }



            }
            if (board[i][j] == -2) {

                if (!hasSamePiece(board[i][j], board[i - 2][j + 1]) || isEmpty(board[i - 2][j + 1])) {
                    mvTrack[i - 2][j + 1] += 1;
                }
                if (!hasSamePiece(board[i][j], board[i - 2][j - 1]) || isEmpty(board[i - 2][j - 1])) {
                    mvTrack[i - 2][j - 1] += 1;
                }
                if (!hasSamePiece(board[i][j], board[i - 1][j + 2]) || isEmpty(board[i - 1][j + 2])) {
                    mvTrack[i - 1][j + 2] += 1;
                }
                if (!hasSamePiece(board[i][j], board[i - 1][j - 2]) || isEmpty(board[i - 1][j - 2])) {
                    mvTrack[i - 1][j - 2] += 1;
                }

                if (!hasSamePiece(board[i][j], board[i + 2][j + 1]) || isEmpty(board[i + 2][j + 1])) {
                    mvTrack[i + 2][j + 1] += 1;
                }
                if (!hasSamePiece(board[i][j], board[i + 2][j - 1]) || isEmpty(board[i + 2][j - 1])) {
                    mvTrack[i + 2][j - 1] += 1;
                }
                if (!hasSamePiece(board[i][j], board[i + 1][j + 2]) || isEmpty(board[i + 1][j + 2])) {
                    mvTrack[i + 1][j + 2] += 1;
                }
                if (!hasSamePiece(board[i][j], board[i + 1][j - 2]) || isEmpty(board[i + 1][j - 2])) {
                    mvTrack[i + 1][j - 2] += 1;
                }

            }
            if (board[i][j] == -3 || board[i][j] == -4) {

                int k = 1;
                bool pathBlocked = false;
                while (i + k < 8 && !pathBlocked) {
                    // bottom right
                    if (board[i + k][j + k] < 0) {
                        pathBlocked = true;
                    }
                    else if (board[i + k][j + k] > 0) {
                        mvTrack[i + k][j + k] += 1;
                        pathBlocked = true;
                    }
                    else {
                        mvTrack[i + k][j + k] += 1;
                    }

                    k++;
                }

                k = 1;
                pathBlocked = false;
                while (j + k < 8 && !pathBlocked) {
                    // top right
                    if (board[i - k][j + k] < 0) {
                        pathBlocked = true;
                    }
                    else if (board[i - k][j + k] > 0) {
                        mvTrack[i - k][j + k] += 1;
                        pathBlocked = true;
                    }
                    else {
                        mvTrack[i - k][j + k] += 1;
                    }

                    k++;
                }

                k = 1;
                pathBlocked = false;
                while (i - k >= 0 && !pathBlocked) {
                    // top left
                    if (board[i - k][j - k] < 0) {
                        pathBlocked = true;
                    }
                    else if (board[i - k][j - k] > 0) {
                        mvTrack[i - k][j - k] += 1;
                        pathBlocked = true;
                    }
                    else {
                        mvTrack[i - k][j - k] += 1;
                    }

                    k++;
                }

                k = 1;
                pathBlocked = false;
                while (j - k >= 0 && !pathBlocked) {
                    // bottom left
                    if (board[i + k][j - k] < 0) {
                        pathBlocked = true;
                    }
                    else if (board[i + k][j - k] > 0) {
                        mvTrack[i + k][j - k] += 1;
                        pathBlocked = true;
                    }
                    else {
                        mvTrack[i + k][j - k] += 1;
                    }

                    k++;
                }

            }

        }

    }

}

void checkPath(int board[][8], int mvTrack[][8], int checkX, int checkY) {


    // Finding the king!
    bool kingFound = false;
    int kingLocX = 4, kingLocY = 0;

    for (int i = 0; i < 8 && !kingFound; i++) {
        for (int j = 0; j < 8 && !kingFound; j++) {

            if (board[i][j] == -5) {
                kingLocX = j;
                kingLocY = i;
                kingFound = true;
            }

        }
    }

    if (board[checkY][checkX] == 6) {
        mvTrack[checkY][checkX] = 1000;
    }
    if (board[checkY][checkX] == 1 || board[checkY][checkX] == 4) {

        if (checkX == kingLocX) {

            // The check is from top or bottom

            if (kingLocY < checkY) {
                // The check is downwards

                int i = 1;
                while (kingLocY + i <= checkY) {
                    mvTrack[kingLocY + i][kingLocX] = 1000;
                    i++;
                }

            }
            if (kingLocY > checkY) {
                // The check is upwards
                int i = 1;
                while (kingLocY - i >= checkY) {
                    mvTrack[kingLocY - i][kingLocX] = 1000;
                    i++;
                }

            }

        }
        else if (checkY == kingLocY) {

            // The check is from right or left

            if (kingLocX < checkX) {
                // The check is right

                int i = 1;
                while (kingLocX + i <= checkX) {
                    mvTrack[kingLocY][kingLocX + i] = 1000;
                    i++;
                }

            }
            if (kingLocX > checkX) {
                // The check is left
                int i = 1;
                while (kingLocX - i >= checkX) {
                    mvTrack[kingLocY][kingLocX - i] = 1000;
                    i++;
                }

            }

        }

    }
    if (board[checkY][checkX] == 3 || board[checkY][checkX] == 4) {

        if (kingLocX < checkX && kingLocY < checkY) {
            // bottom right
            int i = 1;
            while (kingLocX + i <= checkX) {
                mvTrack[kingLocY + i][kingLocX + i] = 1000;
                i++;
            }

        }
        else if (kingLocX > checkX && kingLocY < checkY) {
            // bottom left
            int i = 1;
            while (kingLocX - i >= checkX) {
                mvTrack[kingLocY + i][kingLocX - i] = 1000;
                i++;
            }

        }
        else if (kingLocX > checkX && kingLocY > checkY) {
            // top left
            int i = 1;
            while (kingLocX - i >= checkX) {
                mvTrack[kingLocY - i][kingLocX - i] = 1000;
                i++;
            }

        }
        else if (kingLocX < checkX && kingLocY > checkY) {
            // top right
            int i = 1;
            while (kingLocX + i <= checkX) {
                mvTrack[kingLocY - i][kingLocX + i] = 1000;
                i++;
            }

        }

    }
    if (board[checkY][checkX] == 2) {

        mvTrack[checkY][checkX] = 1000;

    }

}

bool kingInCheckMateB(int board[][8], int checkX, int checkY) {

    int mvTrack[8][8] = {0};

    ofstream errorLog;
    errorLog.open("dataLog.txt");

    checkPath(board, mvTrack, checkX, checkY);

    genMoves(board, mvTrack);

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {

            errorLog << mvTrack[i][j] << "...";

        }
        errorLog << endl;
    }

    bool notCheckMate = false;

    if(kingHasMoves(board)){
        notCheckMate = true;
    }
    else {

        for (int i = 0; i < 8 && !notCheckMate; i++) {
            for (int j = 0; j < 8 && !notCheckMate; j++) {
                if (mvTrack[i][j] > 1000) {
                    notCheckMate = true;
                }
            }
        }
    }

    return !notCheckMate;

}

bool kingInCheckB(int board[][8], int &checkX, int &checkY) {

    // Error tracking file

    ofstream errorLog;
    errorLog.open("errorLog.txt");

    errorLog << "Program Log\n\n";

    // finding king

    bool kingFound = false;
    int kingLocY = 0, kingLocX = 4;

    for (int i = 0; i < 8 && !kingFound; i++) {
        for (int j = 0; j < 8 && !kingFound; j++) {

            kingFound = (board[i][j] == -5);
            kingLocY = i;
            kingLocX = j;

        }
    }

    gotoxy(80, 30);

    if (indExists(kingLocY + 1) && indExists(kingLocX + 1) && board[kingLocY + 1][kingLocX + 1] == 6) {
        errorLog << "In Check from Pawn." << endl;
        checkX = kingLocX + 1;
        checkY = kingLocY + 1;
        return true;
    }
    if (indExists(kingLocY + 1) && indExists(kingLocX - 1) && board[kingLocY + 1][kingLocX - 1] == 6) {
        checkX = kingLocX - 1;
        checkY = kingLocY + 1;
        errorLog << "In Check from Pawn." << endl;
        return true;
    }
    if (indExists(kingLocY + 1) && indExists(kingLocX + 2) && board[kingLocY + 1][kingLocX + 2] == 2) {
        checkY = kingLocY + 1;
        checkX = kingLocX + 2;
        errorLog << "In Check from Knight." << endl;
        return true;
    }
    if (indExists(kingLocY + 1) && indExists(kingLocX - 2) && board[kingLocY + 1][kingLocX - 2] == 2) {
        checkY = kingLocY + 1;
        checkX = kingLocX - 2;
        errorLog << "In Check from Knight." << endl;
        return true;
    }
    if (indExists(kingLocY + 2) && indExists(kingLocX + 1) && board[kingLocY + 2][kingLocX + 1] == 2) {
        checkY = kingLocY + 2;
        checkX = kingLocX + 1;
        errorLog << "In Check from Knight." << endl;
        return true;
    }
    if (indExists(kingLocY + 2) && indExists(kingLocX - 1) && board[kingLocY + 2][kingLocX - 1] == 2) {
        checkY = kingLocY + 2;
        checkX = kingLocX - 1;
        errorLog << "In Check from Knight." << endl;
        return true;
    }
    if (indExists(kingLocY - 1) && indExists(kingLocX + 2) && board[kingLocY - 1][kingLocX + 2] == 2){
        checkY = kingLocY - 1;
        checkX = kingLocX + 2;
        errorLog << "In Check from Knight." << endl;
        return true;
    }
    if (indExists(kingLocY - 1) && indExists(kingLocX - 2) && board[kingLocY - 1][kingLocX - 2] == 2) {
        checkY = kingLocY - 1;
        checkX = kingLocX - 2;
        errorLog << "In Check from Knight." << endl;
        return true;
    }
    if (indExists(kingLocY - 2) && indExists(kingLocX + 1) && board[kingLocY - 2][kingLocX + 1] == 2){
        checkY = kingLocY - 2;
        checkX = kingLocX + 1;
        errorLog << "In Check from Knight." << endl;
        return true;
    }
    if (indExists(kingLocY - 2) && indExists(kingLocX - 1) && board[kingLocY - 2][kingLocX - 1] == 2){
        checkY = kingLocY - 2;
        checkX = kingLocX - 1;
        errorLog << "In Check from Knight." << endl;
        return true;
    }

    if (indExists(kingLocY - 1) && board[kingLocY - 1][kingLocX] == 5) {
        checkY = kingLocY - 1;
        checkX = kingLocX;
        errorLog << "In check from King." << endl;
        return true;
    }
    if (indExists(kingLocY + 1) && board[kingLocY + 1][kingLocX] == 5) {
        checkY = kingLocY + 1;
        checkX = kingLocX;
        errorLog << "In check from King." << endl;
        return true;
    }
    if (indExists(kingLocX - 1) && board[kingLocY][kingLocX - 1] == 5) {
        checkY = kingLocY;
        checkX = kingLocX - 1;
        errorLog << "In check from King." << endl;
        return true;
    }
    if (indExists(kingLocX + 1) && board[kingLocY][kingLocX + 1] == 5) {
        checkY = kingLocY;
        checkX = kingLocX + 1;
        errorLog << "In check from King." << endl;
        return true;
    }

    if (indExists(kingLocY - 1) && indExists(kingLocX - 1) && board[kingLocY - 1][kingLocX - 1] == 5) {
        checkY = kingLocY - 1;
        checkX = kingLocX - 1;
        errorLog << "In check from King." << endl;
        return true;
    }
    if (indExists(kingLocY + 1) && indExists(kingLocX - 1) && board[kingLocY + 1][kingLocX - 1] == 5) {
        checkY = kingLocY + 1;
        checkX = kingLocX - 1;
        errorLog << "In check from King." << endl;
        return true;
    }
    if (indExists(kingLocY + 1) && indExists(kingLocX + 1) && board[kingLocY + 1][kingLocX + 1] == 5) {
        checkY = kingLocY + 1;
        checkX = kingLocX + 1;
        errorLog << "In check from King." << endl;
        return true;
    }
    if (indExists(kingLocY - 1) && indExists(kingLocX + 1) && board[kingLocY - 1][kingLocX + 1] == 5) {
        checkY = kingLocY - 1;
        checkX = kingLocX + 1;
        errorLog << "In check from King." << endl;
        return true;
    }

    // Check by Rook
    int i = 1;
    bool pathBlocked = false;
    while (kingLocX + i < 8 && !pathBlocked) {

        if (board[kingLocY][kingLocX + i] == 1 || board[kingLocY][kingLocX + i] == 4) {
            errorLog << "In Check from Rook Right." << endl;
            checkY = kingLocY;
            checkX = kingLocX + i;
            return true;
        }
        else if (board[kingLocY][kingLocX + i] != 0) {
            pathBlocked = true;
        }

        i++;
    }

    i = 1;
    pathBlocked = false;
    while (kingLocX - i >= 0 && !pathBlocked) {

        if (board[kingLocY][kingLocX - i] == 1 || board[kingLocY][kingLocX - i] == 4) {
            checkY = kingLocY;
            checkX = kingLocX - i;
            errorLog << "In Check from Rook Left." << endl;
            return true;
        }
        else if (board[kingLocY][kingLocX - i] != 0) {
            pathBlocked = true;
        }

        i++;
    }

    i = 1;
    pathBlocked = false;
    while (kingLocY + i < 8 && !pathBlocked) {

        if (board[kingLocY + i][kingLocX] == 1 || board[kingLocY + i][kingLocX] == 4) {
            checkY = kingLocY + i;
            checkX = kingLocX;
            errorLog << "In Check from Rook Bottom." << endl;
            return true;
        }
        else if (board[kingLocY + i][kingLocX] != 0) {
            pathBlocked = true;
        }

        i++;
    }

    i = 1;
    pathBlocked = false;
    while (kingLocY - i >= 0 && !pathBlocked) {

        if (board[kingLocY - i][kingLocX] == 1 || board[kingLocY - i][kingLocX] == 4) {
            checkY = kingLocY - i;
            checkX = kingLocX;
            errorLog << "In Check from Rook Top." << endl;
            return true;
        }
        else if (board[kingLocY - i][kingLocX] != 0) {
            pathBlocked = true;
        }

        i++;
    }


    // Check by Bishop
    i = 1;
    pathBlocked = false;
    while (kingLocY + i < 8 && kingLocX + i < 8 && !pathBlocked) {

        if (board[kingLocY + i][kingLocX + i] == 3 || board[kingLocY + i][kingLocX + i] == 4) {
            checkY = kingLocY + i;
            checkX = kingLocX + i;
            errorLog << "In Check from Bishop Bottom Right." << endl;
            return true;
        }
        else if (board[kingLocY + i][kingLocX + i] != 0) {
            pathBlocked = true;
        }

        i++;
    }

    i = 1;
    pathBlocked = false;
    while (kingLocY + i < 8 && kingLocX - i >= 0 && !pathBlocked) {

        if (board[kingLocY + i][kingLocX - i] == 3 || board[kingLocY + i][kingLocX - i] == 4) {
            checkY = kingLocY + i;
            checkX = kingLocX - i;
            errorLog << "In Check from Bishop Bottom Left." << endl;
            return true;
        }
        else if (board[kingLocY + i][kingLocX - i] != 0) {
            pathBlocked = true;
        }

        i++;
    }

    i = 1;
    pathBlocked = false;
    while (kingLocY - i >= 0 && kingLocX + i < 8 && !pathBlocked) {

        if (board[kingLocY - i][kingLocX + i] == 3 || board[kingLocY - i][kingLocX + i] == 4) {
            checkY = kingLocY - i;
            checkX = kingLocX + i;
            errorLog << "In Check from Bishop Top Right." << endl;
            return true;
        }
        else if (board[kingLocY - i][kingLocX + i] != 0) {
            pathBlocked = true;
        }

        i++;
    }

    i = 1;
    pathBlocked = false;
    while (kingLocY - i >= 0 && kingLocX - i >= 0 && !pathBlocked) {

        if (board[kingLocY - i][kingLocX - i] == 3 || board[kingLocY - i][kingLocX - i] == 4) {
            checkY = kingLocY - i;
            checkX = kingLocX - i;
            errorLog << "In Check from Bishop Top Left." << endl;
            return true;
        }
        else if (board[kingLocY - i][kingLocX - i] != 0) {
            pathBlocked = true;
        }

        i++;
    }

    return false;
}

void pawnPromotion(int board[][8], int srcX, int srcY) {

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    gotoxy(0, 26);

    char choice;
    cout << "Would you like to promote the pawn? (y or n) ";
    cin >> choice;

    gotoxy(0, 26);
    SetConsoleTextAttribute(h, 0);
    cout << "Would you like to promote the pawn? (y or n) " << choice;
    SetConsoleTextAttribute(h, 15);

    if (choice == 'y' || choice == 'Y') {

        gotoxy(0, 27);
        cout << "Which piece would you like to promote your pawn to? (r, n, b, q) ";
        cin >> choice;

        if ((board[srcY][srcX] > 0 && choice >= 'a' && choice <= 'z') || (board[srcY][srcX] < 0 && choice >= 'A' && choice <= 'Z')) {

            if (choice == 'r') {
                board[srcY][srcX] = 1;
            }
            else if (choice == 'n') {
                board[srcY][srcX] = 2;
            }
            else if (choice == 'b') {
                board[srcY][srcX] = 3;
            }
            else if (choice == 'q') {
                board[srcY][srcX] = 4;
            }
            else if (choice == 'R') {
                board[srcY][srcX] = -1;
            }
            else if (choice == 'N') {
                board[srcY][srcX] = -2;
            }
            else if (choice == 'B') {
                board[srcY][srcX] = -3;
            }
            else if (choice == 'Q') {
                board[srcY][srcX] = -4;
            }
        }

        gotoxy(0, 27);
        SetConsoleTextAttribute(h, 0);
        cout << "Which piece would you like to promote your pawn to? (r, n, b, q) " << choice;
    }

    SetConsoleTextAttribute(h, 15);

}

bool movePiece(int board[][8], int srcX, int srcY, int destX, int destY, int &turn, bool &wCanCastle, bool &bCanCastle, int movedCells[][8]) {

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    if (turn == 0) {

        // Pawn Logic

        if (board[srcY][srcX] == 6) {

            int maxStep = (srcY == 6) + 1;

            if (isEmpty(board[srcY - abs(destY - srcY)][srcX]) &&
            srcX == destX &&
            (srcY - destY) <= maxStep &&
            (srcY - destY) > 0) {

                board[destY][destX] = board[srcY][srcX];
                board[srcY][srcX] = 0;

                if (srcY == 1 && destY == 0) {
                    pawnPromotion(board, destX, destY);
                }

                return true;
            }
            else if (((srcX == destX - 1) || (srcX == destX + 1)) &&
                    (srcY - destY) == 1 &&
                    !isEmpty(board[destY][destX])) {

                board[destY][destX] = board[srcY][srcX];
                board[srcY][srcX] = 0;

                if (srcY == 1 && destY == 0) {
                    pawnPromotion(board, destX, destY);
                }

                return true;
            }
            else {
                return false;
            }

        }
        // End of pawn logic.
        // Start of knight logic
        else if (board[srcY][srcX] == 2) {

            if (board[destY][destX] <= 0) {

                if (
                        (destY == srcY + 1 && destX == srcX + 2) ||
                        (destY == srcY - 1 && destX == srcX + 2) ||

                        (destY == srcY + 1 && destX == srcX - 2) ||
                        (destY == srcY - 1 && destX == srcX - 2) ||

                        (destY == srcY - 2 && destX == srcX + 1) ||
                        (destY == srcY - 2 && destX == srcX - 1) ||

                        (destY == srcY + 2 && destX == srcX + 1) ||
                        (destY == srcY + 2 && destX == srcX - 1)) {

                    board[destY][destX] = board[srcY][srcX];
                    board[srcY][srcX] = 0;
                    return true;

                }
                else {
                    return false;
                }

            }

        }
        // end of knight logic
        // start of rook logic
        else if (board[srcY][srcX] == 1) {

            /*bool isValidMove;

            isValidMove =  (srcX == destX && abs(srcY-destY) > 0) || (srcY == destY && abs(srcX - destY) > 0);*/

            bool isValidMove = false;

            for (int i = 1; i < 8 && !isValidMove; i++) {

                if (
                        ((srcX == destX) && (srcY + i == destY)) ||
                        ((srcX == destX) && (srcY - i == destY)) ||
                        ((srcY == destY) && (srcX + i == destX)) ||
                        ((srcY == destY) && (srcX - i == destX))
                        ) {

                    isValidMove = true;

                }

            }

            if(isValidMove) {

                bool canMove = true;

                if (srcX == destX) {

                    // The movement is vertical ^ or v

                    if (destY < srcY) {

                        for (int i = 1; i < abs(srcY - destY) && canMove; i++) {
                            if (!isEmpty(board[srcY - i][srcX])) {
                                canMove = false;
                            }
                        }

                    }
                    else if (destY > srcY) {

                        for (int i = 1; i < abs(srcY - destY) && canMove; i++) {
                            if (!isEmpty(board[srcY + i][srcX])) {
                                canMove = false;
                            }
                        }

                    }
                }
                else if (srcY == destY) {

                    // The movement is horizontal <-->

                    if (destX < srcX) {

                        for (int i = 1; i < abs(srcX - destX) && canMove; i++) {
                            if (!isEmpty(board[srcY][srcX - i])) {
                                canMove = false;
                            }
                        }

                    }
                    else if (destX > srcX) {

                        for (int i = 1; i < abs(srcX - destX) && canMove; i++) {
                            if (!isEmpty(board[srcY][srcX + i])) {
                                canMove = false;
                            }
                        }

                    }

                }

                if (canMove) {
                    board[destY][destX] = board[srcY][srcX];
                    board[srcY][srcX] = 0;
                    return true;
                }
                else {
                    return false;
                }
            }
            else {
                return false;
            }


        }
        // end of rook logic
        // start of bishop logic
        else if (board[srcY][srcX] == 3) {


            bool isValidMove;

            isValidMove = (abs(srcX - destX) == abs(srcY - destY));

            if(isValidMove) {

                bool canMove = true;

                if (destY < srcY) {

                    if (destX < srcX) {
                        for (int i = 1; i < abs(destY - srcY); i++){
                            if (!isEmpty(board[srcY - i][srcX - i])) {
                                canMove = false;
                            }
                        }
                    }
                    else {
                        for (int i = 1; i < abs(destY - srcY); i++){
                            if (!isEmpty(board[srcY - i][srcX + i])) {
                                canMove = false;
                            }
                        }
                    }

                }
                else if (destY > srcY) {

                    if (destX < srcX) {
                        for (int i = 1; i < abs(destY - srcY); i++){
                            if (!isEmpty(board[srcY + i][srcX - i])) {
                                canMove = false;
                            }
                        }
                    }
                    else {
                        for (int i = 1; i < abs(destY - srcY); i++){
                            if (!isEmpty(board[srcY + i][srcX + i])) {
                                canMove = false;
                            }
                        }
                    }

                }

                if (canMove) {
                    board[destY][destX] = board[srcY][srcX];
                    board[srcY][srcX] = 0;
                    return true;
                }
                else {
                    return false;
                }
            }
            else {
                return false;
            }


        }
        else if (board[srcY][srcX] == 4) {

            bool isValidMove = false;

            for (int i = 1; i < 8 && !isValidMove; i++) {

                if (
                        ((srcX == destX) && (srcY + i == destY)) ||
                        ((srcX == destX) && (srcY - i == destY)) ||
                        ((srcY == destY) && (srcX + i == destX)) ||
                        ((srcY == destY) && (srcX - i == destX))
                        ) {

                    isValidMove = true;

                }

            }

            if(isValidMove) {

                bool canMove = true;

                if (srcX == destX) {

                    // The movement is vertical ^ or v

                    if (destY < srcY) {

                        for (int i = 1; i < abs(srcY - destY) && canMove; i++) {
                            if (!isEmpty(board[srcY - i][srcX])) {
                                canMove = false;
                            }
                        }

                    }
                    else if (destY > srcY) {

                        for (int i = 1; i < abs(srcY - destY) && canMove; i++) {
                            if (!isEmpty(board[srcY + i][srcX])) {
                                canMove = false;
                            }
                        }

                    }
                }
                else if (srcY == destY) {

                    // The movement is horizontal <-->

                    if (destX < srcX) {

                        for (int i = 1; i < abs(srcX - destX) && canMove; i++) {
                            if (!isEmpty(board[srcY][srcX - i])) {
                                canMove = false;
                            }
                        }

                    }
                    else if (destX > srcX) {

                        for (int i = 1; i < abs(srcX - destX) && canMove; i++) {
                            if (!isEmpty(board[srcY][srcX + i])) {
                                canMove = false;
                            }
                        }

                    }

                }

                if (canMove) {
                    board[destY][destX] = board[srcY][srcX];
                    board[srcY][srcX] = 0;
                    return true;
                }
            }

            isValidMove = (abs(srcX - destX) == abs(srcY - destY));

            if(isValidMove) {

                bool canMove = true;

                if (destY < srcY) {

                    if (destX < srcX) {
                        for (int i = 1; i < abs(destY - srcY); i++){
                            if (!isEmpty(board[srcY - i][srcX - i])) {
                                canMove = false;
                            }
                        }
                    }
                    else {
                        for (int i = 1; i < abs(destY - srcY); i++){
                            if (!isEmpty(board[srcY - i][srcX + i])) {
                                canMove = false;
                            }
                        }
                    }

                }
                else if (destY > srcY) {

                    if (destX < srcX) {
                        for (int i = 1; i < abs(destY - srcY); i++){
                            if (!isEmpty(board[srcY + i][srcX - i])) {
                                canMove = false;
                            }
                        }
                    }
                    else {
                        for (int i = 1; i < abs(destY - srcY); i++){
                            if (!isEmpty(board[srcY + i][srcX + i])) {
                                canMove = false;
                            }
                        }
                    }

                }

                if (canMove) {
                    board[destY][destX] = board[srcY][srcX];
                    board[srcY][srcX] = 0;
                    return true;
                }
                else {
                    return false;
                }
            }
            else {
                return false;
            }

        }
        else if (board[srcY][srcX] == 5) {

            bool isValidMove;

            isValidMove = ((srcX == 4 && srcY == 7 && destX == 6 && destY == 7) && movedCells[7][7] == 0 && movedCells[7][4] == 0)
                    || ((srcX == 4 && srcY == 7 && destX == 2 && destY == 7) && movedCells[7][0] == 0 && movedCells[7][4] == 0);

            if (isValidMove) {

                board[destY][destX] = board[srcY][srcX];
                board[srcY][srcX] = 0;
                if (((srcX == 4 && srcY == 7 && destX == 6 && destY == 7) && movedCells[7][7] == 0 && movedCells[7][4] == 0)) {
                    board[destY][destX - 1] = 1;
                    board[7][7] = 0;
                }
                else {
                    board[destY][destX + 1] = 1;
                    board[7][0] = 0;
                }
                return true;

            }
            else {
                isValidMove = (abs(srcX - destX) == 1 && abs(srcY - destY) == 1)
                              || (srcX == destX && abs(srcY - destY) == 1)
                              || (srcY == destY && abs(srcX - destX) == 1);

                if (isValidMove) {

                    board[destY][destX] = board[srcY][srcX];
                    board[srcY][srcX] = 0;
                    return true;
                } else {
                    return false;
                }
            }


        }
    }
    else if (turn == 1) {
        // Pawn Logic

        if (board[srcY][srcX] == -6) {

            int maxStep = (srcY == 1) + 1;

            if (isEmpty(board[srcY + abs(destY - srcY)][srcX]) &&
                srcX == destX &&
                (destY - srcY) <= maxStep &&
                (srcY - destY) < 0) {

                board[destY][destX] = board[srcY][srcX];
                board[srcY][srcX] = 0;

                if (srcY == 6 && destY == 7) {
                    pawnPromotion(board, destX, destY);
                }

                return true;
            }
            else if (((srcX == destX - 1) || (srcX == destX + 1)) &&
                     (destY - srcY) == 1 &&
                     !isEmpty(board[destY][destX])) {

                board[destY][destX] = board[srcY][srcX];
                board[srcY][srcX] = 0;

                if (srcY == 6 && destY == 7) {
                    pawnPromotion(board, destX, destY);
                }

                return true;
            }
            else {
                return false;
            }

        }
            // End of pawn logic.
            // Start of knight logic
        else if (board[srcY][srcX] == -2) {

            if (board[destY][destX] >= 0) {

                if (
                        (destY == srcY + 1 && destX == srcX + 2) ||
                        (destY == srcY - 1 && destX == srcX + 2) ||

                        (destY == srcY + 1 && destX == srcX - 2) ||
                        (destY == srcY - 1 && destX == srcX - 2) ||

                        (destY == srcY - 2 && destX == srcX + 1) ||
                        (destY == srcY - 2 && destX == srcX - 1) ||

                        (destY == srcY + 2 && destX == srcX + 1) ||
                        (destY == srcY + 2 && destX == srcX - 1)) {

                    board[destY][destX] = board[srcY][srcX];
                    board[srcY][srcX] = 0;
                    return true;

                }
                else {
                    return false;
                }

            }

        }
            // end of knight logic
            // start of rook logic
        else if (board[srcY][srcX] == -1) {

            /*bool isValidMove;

            isValidMove =  ((srcX == destX && abs(srcY-destY) > 0) || (srcY == destY && abs(srcX - destY) > 0));*/

            bool isValidMove = false;

            for (int i = 1; i < 8 && !isValidMove; i++) {

                if (
                        ((srcX == destX) && (srcY + i == destY)) ||
                        ((srcX == destX) && (srcY - i == destY)) ||
                        ((srcY == destY) && (srcX + i == destX)) ||
                        ((srcY == destY) && (srcX - i == destX))
                        ) {

                    isValidMove = true;

                }

            }

            if(isValidMove) {

                bool canMove = true;

                if (srcX == destX) {

                    // The movement is vertical ^ or v

                    if (destY < srcY) {

                        for (int i = 1; i < abs(srcY - destY) && canMove; i++) {
                            if (!isEmpty(board[srcY - i][srcX])) {
                                canMove = false;
                            }
                        }

                    }
                    else if (destY > srcY) {

                        for (int i = 1; i < abs(srcY - destY) && canMove; i++) {
                            if (!isEmpty(board[srcY + i][srcX])) {
                                canMove = false;
                            }
                        }

                    }
                }
                else if (srcY == destY) {

                    // The movement is horizontal <-->

                    if (destX < srcX) {

                        for (int i = 1; i < abs(srcX - destX) && canMove; i++) {
                            if (!isEmpty(board[srcY][srcX - i])) {
                                canMove = false;
                            }
                        }

                    }
                    else if (destX > srcX) {

                        for (int i = 1; i < abs(srcX - destX) && canMove; i++) {
                            if (!isEmpty(board[srcY][srcX + i])) {
                                canMove = false;
                            }
                        }

                    }

                }

                if (canMove) {
                    board[destY][destX] = board[srcY][srcX];
                    board[srcY][srcX] = 0;
                    return true;
                }
                else {
                    return false;
                }
            }
            else {
                return false;
            }


        }
            // end of rook logic
            // start of bishop logic
        else if (board[srcY][srcX] == -3) {


            bool isValidMove;

            isValidMove = (abs(srcX - destX) == abs(srcY - destY));

            if(isValidMove) {

                bool canMove = true;

                if (destY < srcY) {

                    if (destX < srcX) {
                        for (int i = 1; i < abs(destY - srcY); i++){
                            if (!isEmpty(board[srcY - i][srcX - i])) {
                                canMove = false;
                            }
                        }
                    }
                    else {
                        for (int i = 1; i < abs(destY - srcY); i++){
                            if (!isEmpty(board[srcY - i][srcX + i])) {
                                canMove = false;
                            }
                        }
                    }

                }
                else if (destY > srcY) {

                    if (destX < srcX) {
                        for (int i = 1; i < abs(destY - srcY); i++){
                            if (!isEmpty(board[srcY + i][srcX - i])) {
                                canMove = false;
                            }
                        }
                    }
                    else {
                        for (int i = 1; i < abs(destY - srcY); i++){
                            if (!isEmpty(board[srcY + i][srcX + i])) {
                                canMove = false;
                            }
                        }
                    }

                }

                if (canMove) {
                    board[destY][destX] = board[srcY][srcX];
                    board[srcY][srcX] = 0;
                    return true;
                }
                else {
                    return false;
                }
            }
            else {
                return false;
            }


        }
        else if (board[srcY][srcX] == -4) {

            bool isValidMove = false;

            for (int i = 1; i < 8 && !isValidMove; i++) {

                if (
                        ((srcX == destX) && (srcY + i == destY)) ||
                        ((srcX == destX) && (srcY - i == destY)) ||
                        ((srcY == destY) && (srcX + i == destX)) ||
                        ((srcY == destY) && (srcX - i == destX))
                        ) {

                    isValidMove = true;

                }

            }

            if(isValidMove) {

                bool canMove = true;

                if (srcX == destX) {

                    // The movement is vertical ^ or v

                    if (destY < srcY) {

                        for (int i = 1; i < abs(srcY - destY) && canMove; i++) {
                            if (!isEmpty(board[srcY - i][srcX])) {
                                canMove = false;
                            }
                        }

                    }
                    else if (destY > srcY) {

                        for (int i = 1; i < abs(srcY - destY) && canMove; i++) {
                            if (!isEmpty(board[srcY + i][srcX])) {
                                canMove = false;
                            }
                        }

                    }
                }
                else if (srcY == destY) {

                    // The movement is horizontal <-->

                    if (destX < srcX) {

                        for (int i = 1; i < abs(srcX - destX) && canMove; i++) {
                            if (!isEmpty(board[srcY][srcX - i])) {
                                canMove = false;
                            }
                        }

                    }
                    else if (destX > srcX) {

                        for (int i = 1; i < abs(srcX - destX) && canMove; i++) {
                            if (!isEmpty(board[srcY][srcX + i])) {
                                canMove = false;
                            }
                        }

                    }

                }

                if (canMove) {
                    board[destY][destX] = board[srcY][srcX];
                    board[srcY][srcX] = 0;
                    return true;
                }
            }

            isValidMove = (abs(srcX - destX) == abs(srcY - destY));

            if(isValidMove) {

                bool canMove = true;

                if (destY < srcY) {

                    if (destX < srcX) {
                        for (int i = 1; i < abs(destY - srcY); i++){
                            if (!isEmpty(board[srcY - i][srcX - i])) {
                                canMove = false;
                            }
                        }
                    }
                    else {
                        for (int i = 1; i < abs(destY - srcY); i++){
                            if (!isEmpty(board[srcY - i][srcX + i])) {
                                canMove = false;
                            }
                        }
                    }

                }
                else if (destY > srcY) {

                    if (destX < srcX) {
                        for (int i = 1; i < abs(destY - srcY); i++){
                            if (!isEmpty(board[srcY + i][srcX - i])) {
                                canMove = false;
                            }
                        }
                    }
                    else {
                        for (int i = 1; i < abs(destY - srcY); i++){
                            if (!isEmpty(board[srcY + i][srcX + i])) {
                                canMove = false;
                            }
                        }
                    }

                }

                if (canMove) {
                    board[destY][destX] = board[srcY][srcX];
                    board[srcY][srcX] = 0;
                    return true;
                }
                else {
                    return false;
                }
            }
            else {
                return false;
            }

        }
        else if (board[srcY][srcX] == -5) {

            bool isValidMove;

            isValidMove = ((srcX == 4 && srcY == 0 && destX == 6 && destY == 0) && movedCells[0][7] == 0 && movedCells[0][4] == 0)
                          || ((srcX == 4 && srcY == 0 && destX == 2 && destY == 0) && movedCells[0][0] == 0 && movedCells[0][4] == 0);

            if (isValidMove) {

                board[destY][destX] = board[srcY][srcX];
                board[srcY][srcX] = 0;
                if (((srcX == 4 && srcY == 0 && destX == 6 && destY == 0) && movedCells[0][7] == 0 && movedCells[0][4] == 0)) {
                    board[destY][destX - 1] = -1;
                    board[0][7] = 0;
                }
                else {
                    board[destY][destX + 1] = -1;
                    board[0][0] = 0;
                }
                return true;

            }
            else {
                isValidMove = (abs(srcX - destX) == 1 && abs(srcY - destY) == 1)
                              || (srcX == destX && abs(srcY - destY) == 1)
                              || (srcY == destY && abs(srcX - destX) == 1);

                if (isValidMove) {

                    board[destY][destX] = board[srcY][srcX];
                    board[srcY][srcX] = 0;
                    return true;
                } else {
                    return false;
                }
            }

        }
    }

    return false;
}

void timer() {

    clock_t t = clock();
    clock_t t2 = clock();

    while (abs(t2 - t) < 50000) {

        gotoxy(80, 20);
        cout << t2 / 1000 << endl;
        gotoxy(0, 0);
        this_thread::sleep_for(chrono::milliseconds(1000));

        t2 = clock();
    }

}

int main() {

    /*int board[8][8] = {
            {-1,-2,-3,-4,-5,-3,-2,-1},
            {-6,-6,-6,-6,-6,-6,-6,-6},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {6, 6, 6, 6, 6, 6, 6, 6},
            {1, 2, 3, 4, 5, 3, 2, 1}
    };*/
    int board[8][8] = {
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 5, 0, -5},
            {0, 6, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {1, 0, 0, 0, 0, 0, 0, 0}
    };

    int movedCells[8][8] = {0};

    int moveCounter = 0;
    int turn = 0;   // Keeps a track of the current player turn

    bool bCanCastle = true;
    bool wCanCastle = true;

    int checkY = -1, checkX = -1;

    string currentMove;
    string previousMove;

    ofstream currentGameFile;
    currentGameFile.open("currentGameMoves.txt");

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    gotoxy(80, 0);
    cout << ":: LAST MOVE ::" << endl;

    char enginePath[] = "stockfish.exe";
    connectToEngine(enginePath);

    gotoxy(100, 0);
    cout << ":: GAME STATUS ::" << endl;

    gotoxy(80, 5);
    cout << ":: BEST MOVE ::" << endl;
    gotoxy(0, 0);

    while (moveCounter < INT_MAX) {

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
            cout << "\n:: >> " << srcCell << " " << destCell << endl;
            cout << destCell;


            // Printing these in white
            gotoxy(19, 19);
            SetConsoleTextAttribute(h, 15);
            cout << "\n:: >> ";
            cin >> srcCell >> destCell;
            cout << endl;

        }
        else {

            /*Printing these in black to cover up the previous
            input due to cursor re-positioning.*/
            SetConsoleTextAttribute(h, 0);
            gotoxy(19, 19);
            cout << "\n:: >>      " << endl;
            cout << destCell;

            // Taking input for the first time
            SetConsoleTextAttribute(h, 15);
            gotoxy(19, 19);
            cout << "\n:: >> ";
            cin >> srcCell >> destCell;

        }

        int srcX, srcY, destX, destY;


        if ((chessCordToIndex(srcCell, srcX, srcY) &&
        chessCordToIndex(destCell, destX, destY)) &&
        !isEmpty(board[srcY][srcX]) &&
        !hasSamePiece(board[srcY][srcX], board[destY][destX]) &&
        movePiece(board, srcX, srcY, destX, destY, turn, wCanCastle, bCanCastle, movedCells)) {

            moveCounter++;
            turn = (moveCounter % 2);

            movedCells[srcY][srcX] += 1;

            // Prints the previous move in black to erase it
            gotoxy(85, 2);
            SetConsoleTextAttribute(h, 0);
            cout << srcCell << destCell << endl;
            SetConsoleTextAttribute(h, 15);
            gotoxy(0, 0);

            (moveCounter > 0)? previousMove += " " + currentMove : previousMove = currentMove;

            string srcAd = srcCell;
            string destAd = destCell;

            currentMove = srcAd + destAd;

            gotoxy(85, 7);
            SetConsoleTextAttribute(h, 11);

            // Prints best move on the screen
            if (moveCounter < 1) {
                cout << getNextMove(currentMove) << endl;
            }
            else {
                cout << getNextMove(previousMove + " " + currentMove) << endl;
            }
            SetConsoleTextAttribute(h, 15);
            gotoxy(0, 0);

            if(kingInCheckB(board, checkX, checkY)) {

                gotoxy(102, 2);
                SetConsoleTextAttribute(h, FOREGROUND_RED);

                if (kingInCheckMateB(board, checkX, checkY)){
                    cout << "CHECKMATE!" << endl;
                }
                else {
                    gotoxy(102, 3);
                    SetConsoleTextAttribute(h, FOREGROUND_RED);
                    cout << "CHECK" << endl;
                }
            }
            else {
                gotoxy(102, 3);

                SetConsoleTextAttribute(h, 0);
                cout << "CHECK" << endl;
            }

            gotoxy(85, 2);
            SetConsoleTextAttribute(h, 11);
            cout << srcCell << destCell;

            currentGameFile << srcCell << destCell << endl;

            gotoxy(4, 25);

            SetConsoleTextAttribute(h, 0);
            cout << "INVALID MOVE" << endl;
            SetConsoleTextAttribute(h, 15);
            gotoxy(0, 0);

        }
        else {
            gotoxy(4, 25);

            SetConsoleTextAttribute(h, FOREGROUND_RED);
            cout << "INVALID MOVE" << endl;
            SetConsoleTextAttribute(h, 15);
            gotoxy(0, 0);
        }

        gotoxy(0, 0);
    }

    getch();
    return 0;
}