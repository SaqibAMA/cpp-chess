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
bool kingInCheckW(int board[][8], int &checkX, int &checkY);
bool kingInCheckMateB(int board[][8], int checkX, int checkY);
bool kingInCheckMateW(int board[][8], int checkX, int checkY);
bool isKingMoveW(int board[][8], int tempBoard[][8], int srcX, int srcY, int destX, int destY);
bool kingHasMoves(int board[][8]);
bool kingHasMovesW(int board[][8]);
void genMovesW(int board[][8], int mvTrack[][8]);
void checkPathW(int board[][8], int mvTrack[][8], int checkX, int checkY);
bool movePiece(int board[][8], int srcX, int srcY, int destX, int destY, int &turn, int movedCells[][8], int canBeEP[]);
void pawnPromotion(int board[][8], int srcX, int srcY);
bool isLegal(int board[][8], int srcX, int srcY, int destX, int destY, int turn);


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

            bool kInCheck = kingInCheckB(tempBoard, checkX, checkY);

            tempBoard[srcY][srcY] = tempBoard[destY][destX];
            tempBoard[destY][destX] = 0;

            return !kInCheck && checkX != -1 && checkY != -1;

        }
    }
    else {
        return false;
    }

}

bool isKingMoveW(int board[][8], int tempBoard[][8], int srcX, int srcY, int destX, int destY) {

    if (indExists(destY) && indExists(destX)) {
        if (hasSamePiece(board[srcY][srcX], board[destY][destX]) && !isEmpty(board[destY][destX])) {

            return false;

        } else {

            int checkX = -1, checkY = -1;

            tempBoard[destY][destX] = tempBoard[srcY][srcX];
            tempBoard[srcY][srcX] = 0;

            bool kInCheck = kingInCheckW(tempBoard, checkX, checkY);

            tempBoard[srcY][srcY] = tempBoard[destY][destX];
            tempBoard[destY][destX] = 0;

            return !kInCheck && checkX != -1 && checkY != -1;

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

    return isKingMove(board, tempBoard, kingLocX, kingLocY, kingLocX - 1, kingLocY) ||
           isKingMove(board, tempBoard, kingLocX, kingLocY, kingLocX + 1, kingLocY) ||
           isKingMove(board, tempBoard, kingLocX, kingLocY, kingLocX, kingLocY + 1) ||
           isKingMove(board, tempBoard, kingLocX, kingLocY, kingLocX, kingLocY - 1) ||
           isKingMove(board, tempBoard, kingLocX, kingLocY, kingLocX + 1, kingLocY + 1) ||
           isKingMove(board, tempBoard, kingLocX, kingLocY, kingLocX + 1, kingLocY - 1) ||
           isKingMove(board, tempBoard, kingLocX, kingLocY, kingLocX - 1, kingLocY + 1) ||
           isKingMove(board, tempBoard, kingLocX, kingLocY, kingLocX - 1, kingLocY - 1);

}

bool kingHasMovesW(int board[][8]) {

    // Copying the board
    int tempBoard[8][8];

    int kingLocX = 4, kingLocY = 7;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {

            tempBoard[i][j] = board[i][j];
            if (board[i][j] == 5) {

                kingLocX = j;
                kingLocY = i;

            }

        }
    }

    return isKingMoveW(board, tempBoard, kingLocX, kingLocY, kingLocX - 1, kingLocY) ||
           isKingMoveW(board, tempBoard, kingLocX, kingLocY, kingLocX + 1, kingLocY) ||
           isKingMoveW(board, tempBoard, kingLocX, kingLocY, kingLocX, kingLocY + 1) ||
           isKingMoveW(board, tempBoard, kingLocX, kingLocY, kingLocX, kingLocY - 1) ||
           isKingMoveW(board, tempBoard, kingLocX, kingLocY, kingLocX + 1, kingLocY + 1) ||
           isKingMoveW(board, tempBoard, kingLocX, kingLocY, kingLocX + 1, kingLocY - 1) ||
           isKingMoveW(board, tempBoard, kingLocX, kingLocY, kingLocX - 1, kingLocY + 1) ||
           isKingMoveW(board, tempBoard, kingLocX, kingLocY, kingLocX - 1, kingLocY - 1);

}

void genMoves(int board[][8], int mvTrack[][8]){

    for (int i = 0; i < 8; i++) {

        for (int j = 0; j < 8; j++) {

            if (board[i][j] == -6) {

                if (indExists(i + 1) && isEmpty(board[i + 1][j])) {
                    mvTrack[i + 1][j] += 1;

                    if (indExists(i + 2) && isEmpty(board[i + 2][j]) && i == 1) {
                        mvTrack[i + 2][j] += 1;
                    }

                }
                if (indExists(i + 1) && indExists(j + 1) && (!isEmpty(board[i + 1][j + 1]) && !hasSamePiece(board[i][j], board[i + 1][j + 1]))) {
                    mvTrack[i + 1][j + 1] += 1;
                }
                if (indExists(i + 1) && indExists(j + 1) && !isEmpty(board[i + 1][j - 1]) && !hasSamePiece(board[i][j], board[i + 1][j - 1])) {
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

                if (indExists(i - 2) && indExists(j + 1) && (!hasSamePiece(board[i][j], board[i - 2][j + 1]) || isEmpty(board[i - 2][j + 1]))) {
                    mvTrack[i - 2][j + 1] += 1;
                }
                if (indExists(i - 2) && indExists(j - 1) && (!hasSamePiece(board[i][j], board[i - 2][j - 1]) || isEmpty(board[i - 2][j - 1]))) {
                    mvTrack[i - 2][j - 1] += 1;
                }
                if (indExists(i - 1) && indExists(j + 2) && (!hasSamePiece(board[i][j], board[i - 1][j + 2]) || isEmpty(board[i - 1][j + 2]))) {
                    mvTrack[i - 1][j + 2] += 1;
                }
                if (indExists(i - 1) && indExists(j - 2) && (!hasSamePiece(board[i][j], board[i - 1][j - 2]) || isEmpty(board[i - 1][j - 2]))) {
                    mvTrack[i - 1][j - 2] += 1;
                }

                if (indExists(i + 2) && indExists(j + 1) && (!hasSamePiece(board[i][j], board[i + 2][j + 1]) || isEmpty(board[i + 2][j + 1]))) {
                    mvTrack[i + 2][j + 1] += 1;
                }
                if (indExists(i + 2) && indExists(j - 1) && (!hasSamePiece(board[i][j], board[i + 2][j - 1]) || isEmpty(board[i + 2][j - 1]))) {
                    mvTrack[i + 2][j - 1] += 1;
                }
                if (indExists(i + 1) && indExists(j + 2) && (!hasSamePiece(board[i][j], board[i + 1][j + 2]) || isEmpty(board[i + 1][j + 2]))) {
                    mvTrack[i + 1][j + 2] += 1;
                }
                if (indExists(i + 1) && indExists(j - 2) && (!hasSamePiece(board[i][j], board[i + 1][j - 2]) || isEmpty(board[i + 1][j - 2]))) {
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

void genMovesW(int board[][8], int mvTrack[][8]){

    for (int i = 0; i < 8; i++) {

        for (int j = 0; j < 8; j++) {

            if (board[i][j] == 6) {

                if (indExists(i - 1) && isEmpty(board[i - 1][j]) && isLegal(board, i, j, i - 1, j, 0)) {
                    mvTrack[i - 1][j] += 1;

                    if (indExists(i - 2) && isEmpty(board[i - 2][j]) && i == 6 && isLegal(board, i, j, i - 2, j, 0)) {
                        mvTrack[i - 2][j] += 1;
                    }

                }
                if (indExists(i - 1) && indExists(j + 1) && !isEmpty(board[i + 1][j + 1]) && !hasSamePiece(board[i][j], board[i + 1][j + 1]) && isLegal(board, i, j, i + 1, j + 1, 0)) {
                    mvTrack[i + 1][j + 1] += 1;
                }
                if (indExists(i - 1) && indExists(j + 1) && !isEmpty(board[i + 1][j - 1]) && !hasSamePiece(board[i][j], board[i + 1][j - 1]) && isLegal(board, i, j, i + 1, j - 1, 0)) {
                    mvTrack[i - 1][j - 1] += 1;
                }

            }

            if (board[i][j] == 1 || board[i][j] == 4) {

                int k = 1;
                bool pathBlocked = false;
                while (i + k < 8 && !pathBlocked) {
                    // bottom
                    if (board[i + k][j] > 0) {
                        pathBlocked = true;
                    }
                    else if (board[i + k][j] < 0) {
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
                    if (board[i][j + k] > 0) {
                        pathBlocked = true;
                    }
                    else if (board[i][j + k] < 0) {
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
                    if (board[i - k][j] > 0) {
                        pathBlocked = true;
                    }
                    else if (board[i - k][j] < 0) {
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
                    if (board[i][j - k] > 0) {
                        pathBlocked = true;
                    }
                    else if (board[i][j - k] < 0) {
                        mvTrack[i][j - k] += 1;
                        pathBlocked = true;
                    }
                    else {
                        mvTrack[i][j - k] += 1;
                    }

                    k++;
                }



            }

            if (board[i][j] == 2) {

                if (indExists(i - 2) && indExists(j + 1) && (!hasSamePiece(board[i][j], board[i - 2][j + 1]) || isEmpty(board[i - 2][j + 1]))) {
                    mvTrack[i - 2][j + 1] += 1;
                }
                if (indExists(i - 2) && indExists(j - 1) && (!hasSamePiece(board[i][j], board[i - 2][j - 1]) || isEmpty(board[i - 2][j - 1]))) {
                    mvTrack[i - 2][j - 1] += 1;
                }
                if (indExists(i - 1) && indExists(j + 2) && (!hasSamePiece(board[i][j], board[i - 1][j + 2]) || isEmpty(board[i - 1][j + 2]))) {
                    mvTrack[i - 1][j + 2] += 1;
                }
                if (indExists(i - 1) && indExists(j - 2) && (!hasSamePiece(board[i][j], board[i - 1][j - 2]) || isEmpty(board[i - 1][j - 2]))) {
                    mvTrack[i - 1][j - 2] += 1;
                }

                if (indExists(i + 2) && indExists(j + 1) && (!hasSamePiece(board[i][j], board[i + 2][j + 1]) || isEmpty(board[i + 2][j + 1]))) {
                    mvTrack[i + 2][j + 1] += 1;
                }
                if (indExists(i + 2) && indExists(j - 1) && (!hasSamePiece(board[i][j], board[i + 2][j - 1]) || isEmpty(board[i + 2][j - 1]))) {
                    mvTrack[i + 2][j - 1] += 1;
                }
                if (indExists(i + 1) && indExists(j + 2) && (!hasSamePiece(board[i][j], board[i + 1][j + 2]) || isEmpty(board[i + 1][j + 2]))) {
                    mvTrack[i + 1][j + 2] += 1;
                }
                if (indExists(i + 1) && indExists(j - 2) && (!hasSamePiece(board[i][j], board[i + 1][j - 2]) || isEmpty(board[i + 1][j - 2]))) {
                    mvTrack[i + 1][j - 2] += 1;
                }

            }

            if (board[i][j] == 3 || board[i][j] == 4) {

                int k = 1;
                bool pathBlocked = false;
                while (i + k < 8 && !pathBlocked) {
                    // bottom right
                    if (board[i + k][j + k] > 0) {
                        pathBlocked = true;
                    }
                    else if (board[i + k][j + k] < 0) {
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
                    if (board[i - k][j + k] > 0) {
                        pathBlocked = true;
                    }
                    else if (board[i - k][j + k] < 0) {
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
                    if (board[i - k][j - k] > 0) {
                        pathBlocked = true;
                    }
                    else if (board[i - k][j - k] < 0) {
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
                    if (board[i + k][j - k] > 0) {
                        pathBlocked = true;
                    }
                    else if (board[i + k][j - k] < 0) {
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

void checkPathW(int board[][8], int mvTrack[][8], int checkX, int checkY) {


    // Finding the king!
    bool kingFound = false;
    int kingLocX = 4, kingLocY = 7;

    for (int i = 0; i < 8 && !kingFound; i++) {
        for (int j = 0; j < 8 && !kingFound; j++) {

            if (board[i][j] == 5) {
                kingLocX = j;
                kingLocY = i;
                kingFound = true;
            }

        }
    }

    if (board[checkY][checkX] == -6) {
        mvTrack[checkY][checkX] = 1000;
    }
    if (board[checkY][checkX] == -1 || board[checkY][checkX] == -4) {

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
    if (board[checkY][checkX] == -3 || board[checkY][checkX] == -4) {

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
    if (board[checkY][checkX] == -2) {

        mvTrack[checkY][checkX] = 1000;

    }

}

bool kingInCheckMateB(int board[][8], int checkX, int checkY) {

    int mvTrack[8][8] = {0};

    checkPath(board, mvTrack, checkX, checkY);

    genMoves(board, mvTrack);

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

bool kingInCheckMateW(int board[][8], int checkX, int checkY) {

    int mvTrack[8][8] = {0};

    checkPathW(board, mvTrack, checkX, checkY);

    genMovesW(board, mvTrack);

    bool notCheckMate = false;

    if(kingHasMovesW(board)){
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


    checkX = -1, checkY = -1;

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

    if (indExists(kingLocY + 1) && indExists(kingLocX + 1) && board[kingLocY + 1][kingLocX + 1] == 6) {
        checkX = kingLocX + 1;
        checkY = kingLocY + 1;
        return true;
    }
    if (indExists(kingLocY + 1) && indExists(kingLocX - 1) && board[kingLocY + 1][kingLocX - 1] == 6) {
        checkX = kingLocX - 1;
        checkY = kingLocY + 1;
        return true;
    }
    if (indExists(kingLocY + 1) && indExists(kingLocX + 2) && board[kingLocY + 1][kingLocX + 2] == 2) {
        checkY = kingLocY + 1;
        checkX = kingLocX + 2;
        return true;
    }
    if (indExists(kingLocY + 1) && indExists(kingLocX - 2) && board[kingLocY + 1][kingLocX - 2] == 2) {
        checkY = kingLocY + 1;
        checkX = kingLocX - 2;
        return true;
    }
    if (indExists(kingLocY + 2) && indExists(kingLocX + 1) && board[kingLocY + 2][kingLocX + 1] == 2) {
        checkY = kingLocY + 2;
        checkX = kingLocX + 1;
        return true;
    }
    if (indExists(kingLocY + 2) && indExists(kingLocX - 1) && board[kingLocY + 2][kingLocX - 1] == 2) {
        checkY = kingLocY + 2;
        checkX = kingLocX - 1;
        return true;
    }
    if (indExists(kingLocY - 1) && indExists(kingLocX + 2) && board[kingLocY - 1][kingLocX + 2] == 2){
        checkY = kingLocY - 1;
        checkX = kingLocX + 2;
        return true;
    }
    if (indExists(kingLocY - 1) && indExists(kingLocX - 2) && board[kingLocY - 1][kingLocX - 2] == 2) {
        checkY = kingLocY - 1;
        checkX = kingLocX - 2;

        return true;
    }
    if (indExists(kingLocY - 2) && indExists(kingLocX + 1) && board[kingLocY - 2][kingLocX + 1] == 2){
        checkY = kingLocY - 2;
        checkX = kingLocX + 1;

        return true;
    }
    if (indExists(kingLocY - 2) && indExists(kingLocX - 1) && board[kingLocY - 2][kingLocX - 1] == 2){
        checkY = kingLocY - 2;
        checkX = kingLocX - 1;

        return true;
    }

    if (indExists(kingLocY - 1) && board[kingLocY - 1][kingLocX] == 5) {
        checkY = kingLocY - 1;
        checkX = kingLocX;

        return true;
    }
    if (indExists(kingLocY + 1) && board[kingLocY + 1][kingLocX] == 5) {
        checkY = kingLocY + 1;
        checkX = kingLocX;

        return true;
    }
    if (indExists(kingLocX - 1) && board[kingLocY][kingLocX - 1] == 5) {
        checkY = kingLocY;
        checkX = kingLocX - 1;

        return true;
    }
    if (indExists(kingLocX + 1) && board[kingLocY][kingLocX + 1] == 5) {
        checkY = kingLocY;
        checkX = kingLocX + 1;

        return true;
    }
    if (indExists(kingLocY - 1) && indExists(kingLocX - 1) && board[kingLocY - 1][kingLocX - 1] == 5) {
        checkY = kingLocY - 1;
        checkX = kingLocX - 1;

        return true;
    }
    if (indExists(kingLocY + 1) && indExists(kingLocX - 1) && board[kingLocY + 1][kingLocX - 1] == 5) {
        checkY = kingLocY + 1;
        checkX = kingLocX - 1;

        return true;
    }
    if (indExists(kingLocY + 1) && indExists(kingLocX + 1) && board[kingLocY + 1][kingLocX + 1] == 5) {
        checkY = kingLocY + 1;
        checkX = kingLocX + 1;

        return true;
    }
    if (indExists(kingLocY - 1) && indExists(kingLocX + 1) && board[kingLocY - 1][kingLocX + 1] == 5) {
        checkY = kingLocY - 1;
        checkX = kingLocX + 1;

        return true;
    }

    // Check by Rook
    int i = 1;
    bool pathBlocked = false;
    while (kingLocX + i < 8 && !pathBlocked) {

        if (board[kingLocY][kingLocX + i] == 1 || board[kingLocY][kingLocX + i] == 4) {

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

            return true;
        }
        else if (board[kingLocY - i][kingLocX - i] != 0) {
            pathBlocked = true;
        }

        i++;
    }

    return false;
}

bool kingInCheckW(int board[][8], int &checkX, int &checkY) {

    // Error tracking file

    // finding king

    bool kingFound = false;
    int kingLocY = 7, kingLocX = 4;

    for (int i = 0; i < 8 && !kingFound; i++) {
        for (int j = 0; j < 8 && !kingFound; j++) {

            kingFound = (board[i][j] == 5);
            kingLocY = i;
            kingLocX = j;

        }
    }

    gotoxy(80, 30);

    if (indExists(kingLocY - 1) && indExists(kingLocX + 1) && board[kingLocY - 1][kingLocX + 1] == -6) {

        checkX = kingLocX + 1;
        checkY = kingLocY - 1;
        return true;
    }
    if (indExists(kingLocY - 1) && indExists(kingLocX - 1) && board[kingLocY + 1][kingLocX - 1] == -6) {
        checkX = kingLocX - 1;
        checkY = kingLocY - 1;

        return true;
    }
    if (indExists(kingLocY + 1) && indExists(kingLocX + 2) && board[kingLocY + 1][kingLocX + 2] == -2) {
        checkY = kingLocY + 1;
        checkX = kingLocX + 2;

        return true;
    }
    if (indExists(kingLocY + 1) && indExists(kingLocX - 2) && board[kingLocY + 1][kingLocX - 2] == -2) {
        checkY = kingLocY + 1;
        checkX = kingLocX - 2;

        return true;
    }
    if (indExists(kingLocY + 2) && indExists(kingLocX + 1) && board[kingLocY + 2][kingLocX + 1] == -2) {
        checkY = kingLocY + 2;
        checkX = kingLocX + 1;

        return true;
    }
    if (indExists(kingLocY + 2) && indExists(kingLocX - 1) && board[kingLocY + 2][kingLocX - 1] == -2) {
        checkY = kingLocY + 2;
        checkX = kingLocX - 1;

        return true;
    }
    if (indExists(kingLocY - 1) && indExists(kingLocX + 2) && board[kingLocY - 1][kingLocX + 2] == -2){
        checkY = kingLocY - 1;
        checkX = kingLocX + 2;

        return true;
    }
    if (indExists(kingLocY - 1) && indExists(kingLocX - 2) && board[kingLocY - 1][kingLocX - 2] == -2) {
        checkY = kingLocY - 1;
        checkX = kingLocX - 2;

        return true;
    }
    if (indExists(kingLocY - 2) && indExists(kingLocX + 1) && board[kingLocY - 2][kingLocX + 1] == -2){
        checkY = kingLocY - 2;
        checkX = kingLocX + 1;

        return true;
    }
    if (indExists(kingLocY - 2) && indExists(kingLocX - 1) && board[kingLocY - 2][kingLocX - 1] == -2){
        checkY = kingLocY - 2;
        checkX = kingLocX - 1;

        return true;
    }

    if (indExists(kingLocY - 1) && board[kingLocY - 1][kingLocX] == -5) {
        checkY = kingLocY - 1;
        checkX = kingLocX;

        return true;
    }
    if (indExists(kingLocY + 1) && board[kingLocY + 1][kingLocX] == -5) {
        checkY = kingLocY + 1;
        checkX = kingLocX;

        return true;
    }
    if (indExists(kingLocX - 1) && board[kingLocY][kingLocX - 1] == -5) {
        checkY = kingLocY;
        checkX = kingLocX - 1;

        return true;
    }
    if (indExists(kingLocX + 1) && board[kingLocY][kingLocX + 1] == -5) {
        checkY = kingLocY;
        checkX = kingLocX + 1;

        return true;
    }
    if (indExists(kingLocY - 1) && indExists(kingLocX - 1) && board[kingLocY - 1][kingLocX - 1] == -5) {
        checkY = kingLocY - 1;
        checkX = kingLocX - 1;

        return true;
    }
    if (indExists(kingLocY + 1) && indExists(kingLocX - 1) && board[kingLocY + 1][kingLocX - 1] == -5) {
        checkY = kingLocY + 1;
        checkX = kingLocX - 1;

        return true;
    }
    if (indExists(kingLocY + 1) && indExists(kingLocX + 1) && board[kingLocY + 1][kingLocX + 1] == -5) {
        checkY = kingLocY + 1;
        checkX = kingLocX + 1;

        return true;
    }
    if (indExists(kingLocY - 1) && indExists(kingLocX + 1) && board[kingLocY - 1][kingLocX + 1] == -5) {
        checkY = kingLocY - 1;
        checkX = kingLocX + 1;

        return true;
    }

    // Check by Rook
    int i = 1;
    bool pathBlocked = false;
    while (kingLocX + i < 8 && !pathBlocked) {

        if (board[kingLocY][kingLocX + i] == -1 || board[kingLocY][kingLocX + i] == -4) {

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

        if (board[kingLocY][kingLocX - i] == -1 || board[kingLocY][kingLocX - i] == -4) {
            checkY = kingLocY;
            checkX = kingLocX - i;

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

        if (board[kingLocY + i][kingLocX] == -1 || board[kingLocY + i][kingLocX] == -4) {
            checkY = kingLocY + i;
            checkX = kingLocX;

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

        if (board[kingLocY - i][kingLocX] == -1 || board[kingLocY - i][kingLocX] == -4) {
            checkY = kingLocY - i;
            checkX = kingLocX;

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

        if (board[kingLocY + i][kingLocX + i] == -3 || board[kingLocY + i][kingLocX + i] == -4) {
            checkY = kingLocY + i;
            checkX = kingLocX + i;

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

        if (board[kingLocY + i][kingLocX - i] == -3 || board[kingLocY + i][kingLocX - i] == -4) {
            checkY = kingLocY + i;
            checkX = kingLocX - i;

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

        if (board[kingLocY - i][kingLocX + i] == -3 || board[kingLocY - i][kingLocX + i] == -4) {
            checkY = kingLocY - i;
            checkX = kingLocX + i;

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

        if (board[kingLocY - i][kingLocX - i] == -3 || board[kingLocY - i][kingLocX - i] == -4) {
            checkY = kingLocY - i;
            checkX = kingLocX - i;

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

bool movePiece(int board[][8], int srcX, int srcY, int destX, int destY, int &turn, int movedCells[][8]) {

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

bool isLegal(int board[][8], int srcX, int srcY, int destX, int destY, int turn) {

    int tempBoard[8][8] = {0};

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {

            tempBoard[i][j] = board[i][j];

        }
    }

    if(turn == 1) {

        int checkX = -1, checkY = -1;

        tempBoard[destY][destX] = tempBoard[srcY][srcX];
        tempBoard[srcY][srcX] = 0;

        bool kInCheck = kingInCheckB(tempBoard, checkX, checkY);

        return !kInCheck;

    }
    if (turn == 0) {

        int checkX = -1, checkY = -1;

        tempBoard[destY][destX] = tempBoard[srcY][srcX];
        tempBoard[srcY][srcX] = 0;

        bool kInCheck = kingInCheckW(tempBoard, checkX, checkY);

        return !kInCheck;
        return true;

    }

    return true;

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

bool handshake(){

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    gotoxy(0, 26);

    char choice;
    cout << "Would you like to handshake and draw? (y or n) ";
    cin >> choice;

    gotoxy(0, 26);
    SetConsoleTextAttribute(h, 0);
    cout << "Would you like to handshake and draw? (y or n) " << choice;
    SetConsoleTextAttribute(h, 15);

    return choice == 'y' || choice == 'Y';

}
/*

bool saveGame(){

    gotoxy(0, 26);

    int choice;
    cout << "ENTER SLOT NO (1 - 3): ";
    cin >> choice;

    gotoxy(0, 26);
    SetConsoleTextAttribute(h, 0);
    cout << "ENTER SLOT NO (1 - 3): " << choice;
    SetConsoleTextAttribute(h, 15);

    ofstream saveFile;
    ifstream currentGame;

    currentGame.open("currentGameMoves.txt");

    while(!currentGame.eof()) {
        char ch = currentGame.get();
        cout << ch;
    }

    if (choice >= 1 && choice <= 3) {

        if (choice == 1) {
            saveFile.close();
            saveFile.open("savedGame1.txt");
        }
        else if (choice == 2) {
            saveFile.close();
            saveFile.open("savedGame2.txt");
        }
        else {
            saveFile.close();
            saveFile.open("savedGame3.txt");
        }

        currentGame.open("currentGameMoves.txt");

        while (!currentGame.eof()) {
            saveFile << currentGame.get();
        }


        return true;
    }
    else {
        return false;
    }

}
*/

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
    /*int board[8][8] = {
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, -5, 0, 0},
            {0, -6, 0, 0, 0, 0, 0, 0},
            {0, 0, 4, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 5, 0, 0},
            {-1, 0, 0, 0, 0, 0, 0, 0}
    };*/

    int mvTrack[8][8] = {0};

    int movedCells[8][8] = {0};

    int moveCounter = 0;
    int turn = 0;   // Keeps a track of the current player turn

    int canBeEP[2] = {0, 0};

    bool gameEnds = false;

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

    gotoxy(80, 10);
    cout << ":: CURRENT TURN ::";

    gotoxy(80, 15);
    cout << " - hh hh to handshake";
    gotoxy(80, 16);
    cout << " - rr rr to resign";
    gotoxy(80, 17);
    cout << " - ss ss to save";


    gotoxy(0, 0);
/*

    while (!savedGame.eof()) {

        printBoard(board);

        char srcCell[3];
        char destCell[3];

        srcCell[0] = savedGame.get();
        srcCell[1] = savedGame.get();
        srcCell[2] = '\0';

        gotoxy(80, 30);
        cout << srcCell << endl;

        destCell[0] = savedGame.get();
        destCell[1] = savedGame.get();
        destCell[2] = '\0';

        savedGame.get();

        gotoxy(0, 0);

        int srcX, srcY, destX, destY;


        if ((chessCordToIndex(srcCell, srcX, srcY) &&
             chessCordToIndex(destCell, destX, destY)) &&
            !isEmpty(board[srcY][srcX]) &&
            !hasSamePiece(board[srcY][srcX], board[destY][destX]) &&
            isLegal(board, srcX, srcY, destX, destY, turn) &&
            movePiece(board, srcX, srcY, destX, destY, turn, wCanCastle, bCanCastle, movedCells)) {

            moveCounter++;
            turn = (moveCounter % 2);


            gotoxy(85, 12);
            if (turn == 0) {
                SetConsoleTextAttribute(h, 0);
                cout << "BLACK" << endl;
                gotoxy(85, 12);
                SetConsoleTextAttribute(h, 11);
                cout << "WHITE" << endl;
            }
            else {
                SetConsoleTextAttribute(h, 0);
                cout << "WHITE" << endl;
                gotoxy(85, 12);
                SetConsoleTextAttribute(h, 11);
                cout << "BLACK" << endl;
            }

            SetConsoleTextAttribute(h, 15);
            gotoxy(0, 0);

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

            if (kingInCheckB(board, checkX, checkY)) {
                bool isCheck = true, isCheckMate = true, isStaleMate = false;

                checkPath(board, mvTrack, checkX, checkY);
                genMoves(board, mvTrack);

                isCheckMate = !kingHasMoves(board);

                for (int i = 0; i < 8 && isCheckMate; i++) {
                    for (int j = 0; j < 8 && isCheckMate; j++) {

                        if (mvTrack[i][j] > 1000) {
                            isCheckMate = false;
                        }

                    }
                }

                if (isCheckMate) {
                    gotoxy(104, 4);
                    SetConsoleTextAttribute(h, FOREGROUND_RED);
                    cout << "CHECKMATE!" << endl;
                    gotoxy(104, 5);
                    cout << "WHITE WINS!" << endl;
                }
                else {
                    gotoxy(104, 3);
                    SetConsoleTextAttribute(h, FOREGROUND_RED);
                    cout << "CHECK!" << endl;
                }
                SetConsoleTextAttribute(h, 15);

            }
            else {

                int mvTrack[8][8] = {0};
                bool staleMate = true;

                genMovesW(board, mvTrack);

                for (int i = 0; i < 8 && staleMate; i++) {
                    for (int j = 0; j < 8 && staleMate; j ++) {
                        if (mvTrack[i][j] > 0) {
                            staleMate = false;
                        }
                    }
                }

                if (staleMate) {
                    gotoxy(104, 4);
                    SetConsoleTextAttribute(h, FOREGROUND_RED);
                    cout << "STALEMATE!" << endl;
                }
                SetConsoleTextAttribute(h, 15);

            }

            if (kingInCheckW(board, checkX, checkY)) {
                bool isCheck = true, isCheckMate = true, isStaleMate = false;

                checkPathW(board, mvTrack, checkX, checkY);
                genMovesW(board, mvTrack);

                isCheckMate = !kingHasMovesW(board);

                for (int i = 0; i < 8 && isCheckMate; i++) {
                    for (int j = 0; j < 8 && isCheckMate; j++) {

                        if (mvTrack[i][j] > 1000) {
                            isCheckMate = false;
                        }

                    }
                }

                if (isCheckMate) {
                    gotoxy(104, 4);
                    SetConsoleTextAttribute(h, FOREGROUND_RED);
                    cout << "CHECKMATE!" << endl;
                    gotoxy(104, 5);
                    cout << "BLACK WINS!" << endl;
                }
                else {
                    gotoxy(104, 3);
                    SetConsoleTextAttribute(h, FOREGROUND_RED);
                    cout << "CHECK!" << endl;
                }
                SetConsoleTextAttribute(h, 15);

            }
            else {

                int mvTrack[8][8] = {0};
                bool staleMate = true;

                genMoves(board, mvTrack);

                for (int i = 0; i < 8 && staleMate; i++) {
                    for (int j = 0; j < 8 && staleMate; j ++) {
                        if (mvTrack[i][j] > 0) {
                            staleMate = false;
                        }
                    }
                }

                if (staleMate) {
                    gotoxy(104, 4);
                    SetConsoleTextAttribute(h, FOREGROUND_RED);
                    cout << "STALEMATE!" << endl;
                    SetConsoleTextAttribute(h, 15);
                }

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

        this_thread::sleep_for(chrono::milliseconds(1000));
    }
*/

    while (!gameEnds) {

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

        if (strcmp(srcCell, "hh") == 0 && strcmp(destCell, "hh") == 0) {

            if(handshake()) {
                gameEnds = true;
                gotoxy(104, 6);
                SetConsoleTextAttribute(h, FOREGROUND_RED);
                cout << "DRAW!" << endl;
            }

        }

        if (strcmp(srcCell, "rr") == 0 && strcmp(destCell, "rr") == 0) {

            gameEnds = true;
            gotoxy(104, 6);
            SetConsoleTextAttribute(h, FOREGROUND_RED);
            cout << "Player " << endl;
            gotoxy(104, 7);
            cout << turn + 1 << " resigned." << endl;

        }

        if (strcmp(srcCell, "ss") == 0 && strcmp(destCell, "ss") == 0) {
            continue;
        }

        int srcX, srcY, destX, destY;

        if ((chessCordToIndex(srcCell, srcX, srcY) &&
        chessCordToIndex(destCell, destX, destY)) &&
        !isEmpty(board[srcY][srcX]) &&
        !hasSamePiece(board[srcY][srcX], board[destY][destX]) &&
        isLegal(board, srcX, srcY, destX, destY, turn) &&
        movePiece(board, srcX, srcY, destX, destY, turn, movedCells)) {

            moveCounter++;
            turn = (moveCounter % 2);


            gotoxy(85, 12);
            if (turn == 0) {
                SetConsoleTextAttribute(h, 0);
                cout << "BLACK" << endl;
                gotoxy(85, 12);
                SetConsoleTextAttribute(h, 11);
                cout << "WHITE" << endl;
            }
            else {
                SetConsoleTextAttribute(h, 0);
                cout << "WHITE" << endl;
                gotoxy(85, 12);
                SetConsoleTextAttribute(h, 11);
                cout << "BLACK" << endl;
            }

            SetConsoleTextAttribute(h, 15);
            gotoxy(0, 0);

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
                string out = getNextMove(previousMove + " " + currentMove);
                if (out == "(non") {
                    gotoxy(104, 5);
                    SetConsoleTextAttribute(h, FOREGROUND_RED);
                    cout << "NO MOVES!" << endl;
                    gameEnds = true;
                }
                else if (out != "error"){

                    gotoxy(85, 7);
                    SetConsoleTextAttribute(h, 11);
                    cout << getNextMove(currentMove) << endl;

                }
                SetConsoleTextAttribute(h, 15);
            }

            gotoxy(104, 3);
            SetConsoleTextAttribute(h, 0);
            cout << "CHECK!" << endl;

            SetConsoleTextAttribute(h, 15);

            gotoxy(0, 0);

            if (kingInCheckB(board, checkX, checkY)) {
                bool isCheck = true, isCheckMate = true, isStaleMate = false;

                checkPath(board, mvTrack, checkX, checkY);
                genMoves(board, mvTrack);

                isCheckMate = !kingHasMoves(board);

                for (int i = 0; i < 8 && isCheckMate; i++) {
                    for (int j = 0; j < 8 && isCheckMate; j++) {

                        if (mvTrack[i][j] > 1000) {
                            isCheckMate = false;
                        }

                    }
                }

                if (isCheckMate) {
                    gotoxy(104, 4);
                    SetConsoleTextAttribute(h, FOREGROUND_RED);
                    cout << "CHECKMATE!" << endl;
                    gotoxy(104, 5);
                    cout << "WHITE WINS!" << endl;
                    gameEnds = true;
                }
                else {
                    gotoxy(104, 3);
                    SetConsoleTextAttribute(h, FOREGROUND_RED);
                    cout << "CHECK!" << endl;
                }
                SetConsoleTextAttribute(h, 15);

            }
            else {

                int mvTrack[8][8] = {0};
                bool staleMate = true;

                genMovesW(board, mvTrack);

                for (int i = 0; i < 8 && staleMate; i++) {
                    for (int j = 0; j < 8 && staleMate; j ++) {
                        if (mvTrack[i][j] > 0) {
                            staleMate = false;
                        }
                    }
                }

                if (staleMate) {
                    gotoxy(104, 4);
                    SetConsoleTextAttribute(h, FOREGROUND_RED);
                    cout << "NO MOVES!" << endl;
                }
                SetConsoleTextAttribute(h, 15);

            }

            if (kingInCheckW(board, checkX, checkY)) {
                bool isCheck = true, isCheckMate = true, isStaleMate = false;

                checkPathW(board, mvTrack, checkX, checkY);
                genMovesW(board, mvTrack);

                isCheckMate = !kingHasMovesW(board);

                for (int i = 0; i < 8 && isCheckMate; i++) {
                    for (int j = 0; j < 8 && isCheckMate; j++) {

                        if (mvTrack[i][j] > 1000) {
                            isCheckMate = false;
                        }

                    }
                }

                if (isCheckMate) {
                    gotoxy(104, 4);
                    SetConsoleTextAttribute(h, FOREGROUND_RED);
                    cout << "CHECKMATE!" << endl;
                    gotoxy(104, 5);
                    cout << "BLACK WINS!" << endl;
                    gameEnds = true;
                }
                else {
                    gotoxy(104, 3);
                    SetConsoleTextAttribute(h, FOREGROUND_RED);
                    cout << "CHECK!" << endl;
                }
                SetConsoleTextAttribute(h, 15);

            }
            else {

                int mvTrack[8][8] = {0};
                bool staleMate = true;

                genMoves(board, mvTrack);

                for (int i = 0; i < 8 && staleMate; i++) {
                    for (int j = 0; j < 8 && staleMate; j ++) {
                        if (mvTrack[i][j] > 0) {
                            staleMate = false;
                        }
                    }
                }

                if (staleMate) {
                    gotoxy(104, 4);
                    SetConsoleTextAttribute(h, FOREGROUND_RED);
                    cout << "NO MOVES!" << endl;
                    SetConsoleTextAttribute(h, 15);
                }

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
