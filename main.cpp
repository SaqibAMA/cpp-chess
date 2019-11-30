#include <iostream>
#include <iomanip>
#include <conio.h>
#include <windows.h>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include "connector.h"

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

bool indExists(int ind) {

    // Returns if the index provided is within the range of a chess array

    return (ind >= 0 && ind < 8);

}

bool kingInCheck(int board[][8]) {

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

    // Checking for check by pawn
    if ((board[kingLocY + 1][kingLocX + 1] == 6) || (board[kingLocY + 1][kingLocX - 1] == 6)) {
        return true;
    }
    // Checking for check by knight
    else if (indExists(kingLocY - 1)) {

        if (indExists(kingLocX - 2) && board[kingLocY - 1][kingLocX - 2] == 2) {
            return true;
        }
        else if (indExists((kingLocX + 2) && board[kingLocY - 1][kingLocX + 2] == 2)) {
            return true;
        }

        if (indExists(kingLocY - 2)) {

            if (indExists(kingLocX - 1) && board[kingLocY - 2][kingLocX - 1] == 2) {
                return true;
            }
            else if (indExists(kingLocX + 1) &&board[kingLocY - 2][kingLocX + 1] == 2) {
                return true;
            }

        }

    }
    else if (indExists(kingLocY + 1)) {

        if (indExists(kingLocX - 2) && board[kingLocY + 1][kingLocX - 2] == 2) {
            return true;
        }
        else if (indExists(kingLocX + 2) && board[kingLocY + 1][kingLocX + 2] == 2) {
            return true;
        }

        if (indExists(kingLocY + 2)) {

            if (indExists(kingLocX - 1) && board[kingLocY + 2][kingLocX - 1] == 2) {
                return true;
            }
            else if (indExists(kingLocX + 1) &&board[kingLocY + 2][kingLocX + 1] == 2) {
                return true;
            }

        }

    }
    // Checking for check by rook & queen (front)
    int i = 1;
    bool isInCheck = false;

    /*while (kingLocY + i < 8 && !isInCheck && board[kingLocY + i][kingLocX] <= 0) {

        if (board[kingLocY + i][kingLocX] == 1 || board[kingLocY + i][kingLocX] == 4) {
            isInCheck = true;
        }

        i++;

    }
    // Checking for check by rook & queen(back)
    i = 1;
    while (kingLocY - i >= 0 && !isInCheck) {

        if (board[kingLocY - i][kingLocX] == 1 || board[kingLocY - i][kingLocX] == 4) {
            isInCheck = true;
        }

        i++;

    }

    // Checking for check by rook & queen (right)
    i = 1;
    while (kingLocX + i < 8 && !isInCheck) {

        if (board[kingLocY][kingLocX + i] == 1 || board[kingLocY][kingLocX + i] == 4) {
            isInCheck = true;
        }

        i++;

    }

    // Checking for check by rook & queen (left)
    i = 1;
    while (kingLocX - i >= 0 && !isInCheck) {

        if (board[kingLocY][kingLocX - i] == 1 || board[kingLocY][kingLocX - i] == 4) {
            isInCheck = true;
        }

        i++;

    }

    // Checking for check by bishop and queen (bottom right)
    i = 1;
    while ((kingLocY + i < 8 || kingLocX + i < 8) && !isInCheck) {

        if (board[kingLocY + i][kingLocX + i] == 3 || board[kingLocY + i][kingLocX + i] == 4) {
            isInCheck = true;
        }

        i++;

    }

    // Checking for check by bishop & queen (bottom left)
    i = 1;
    while ((kingLocY + i < 8 || kingLocX - i >= 0) && !isInCheck) {

        if (board[kingLocY + i][kingLocX - i] == 3 || board[kingLocY + i][kingLocX - i] == 4) {
            isInCheck = true;
        }

        i++;
    }

    // Checking for check by bishop & queen (top right)
    i = 1;
    while ((kingLocY - i >= 0 || kingLocX + i < 8) && !isInCheck) {

        if (board[kingLocY - i][kingLocX + i] == 3 || board[kingLocY - i][kingLocX + i] == 4) {
            isInCheck = true;
        }

        i++;

    }

    // Checking for check by bishop & queen (top left)
    i = 1;
    while ((kingLocY - i >= 0 || kingLocX - i >= 0) && !isInCheck) {

        if (board[kingLocY - i][kingLocX - i] == 3 || board[kingLocY - i][kingLocX - i] == 4) {
            isInCheck = true;
        }

        i++;

    }*/

    return isInCheck;
}

bool movePiece(int board[][8], int srcX, int srcY, int destX, int destY, int &turn, bool &wCanCastle, bool &bCanCastle, int movedCells[][8]) {

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    if (turn == 0) {

        // Pawn Logic

        if (board[srcY][srcX] == 6) {

            int maxStep = (srcY == 6) + 1;

            if (isEmpty(board[srcY - 1][srcX]) &&
            srcX == destX &&
            (srcY - destY) <= maxStep &&
            (srcY - destY) > 0) {

                board[destY][destX] = board[srcY][srcX];
                board[srcY][srcX] = 0;

                return true;
            }
            else if (((srcX == destX - 1) || (srcX == destX + 1)) &&
                    (srcY - destY) == 1 &&
                    !isEmpty(board[destY][destX])) {

                board[destY][destX] = board[srcY][srcX];
                board[srcY][srcX] = 0;

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

            bool isValidMove;

            isValidMove =  ((srcX == destX && abs(srcY-destY) > 0) || (srcY == destY && abs(srcX - destY) > 0));

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

            bool isValidMove;

            isValidMove =  ((srcX == destX && abs(srcY-destY) > 0) || (srcY == destY && abs(srcX - destY) > 0));

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
                    board[7][7] = 0;
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

            if (isEmpty(board[srcY + 1][srcX]) &&
                srcX == destX &&
                (destY - srcY) <= maxStep &&
                (srcY - destY) < 0) {

                board[destY][destX] = board[srcY][srcX];
                board[srcY][srcX] = 0;

                return true;
            }
            else if (((srcX == destX - 1) || (srcX == destX + 1)) &&
                     (destY - srcY) == 1 &&
                     !isEmpty(board[destY][destX])) {

                board[destY][destX] = board[srcY][srcX];
                board[srcY][srcX] = 0;

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

            bool isValidMove;

            isValidMove =  ((srcX == destX && abs(srcY-destY) > 0) || (srcY == destY && abs(srcX - destY) > 0));

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

            bool isValidMove;

            isValidMove =  ((srcX == destX && abs(srcY-destY) > 0) || (srcY == destY && abs(srcX - destY) > 0));

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

void replayGame(int board[][8], int movedCells[][8]) {

    ifstream moves;
    moves.open("sampleReplay.txt");

    char srcCell[3];
    char destCell[3];

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    int srcX, srcY, destX, destY;
    int turn = 0;
    int moveCounter = 0;
    bool endOfMoves = false;
    bool wCanCastle = true, bCanCastle = true;

    while(!moves.eof()) {

        printBoard(board);

        srcCell[0] = moves.get();
        srcCell[1] = moves.get();
        srcCell[2] = '\0';

        destCell[0] = moves.get();
        destCell[1] = moves.get();
        destCell[2] = '\0';

        moves.get();    // To remove next line character

        gotoxy(0, 0);

        if ((chessCordToIndex(srcCell, srcX, srcY) &&
             chessCordToIndex(destCell, destX, destY)) &&
            !isEmpty(board[srcY][srcX]) &&
            !hasSamePiece(board[srcY][srcX], board[destY][destX]) &&
            movePiece(board, srcX, srcY, destX, destY, turn, wCanCastle, bCanCastle, movedCells)) {

            moveCounter++;
            turn = (moveCounter % 2);

        }
        else {
            gotoxy(4, 25);

            SetConsoleTextAttribute(h, FOREGROUND_RED);
            cout << "INVALID MOVE" << endl;
            SetConsoleTextAttribute(h, 15);
            gotoxy(0, 0);
        }

        this_thread::sleep_for(chrono::milliseconds(800));

    }
}

/*
void saveGame(char fileName[], int board[][8]) {

    // add functionality to save board

    ofstream saveMoves;
    saveMoves.open(strcat(fileName, ".txt"));

    ifstream moveLog;
    moveLog.open("currentGameMoves.txt");

    char ch;

    while (!moveLog.eof()) {

        ch = moveLog.get();
        saveMoves << ch;

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

    int movedCells[8][8] = {0};

    int moveCounter = 0;
    int turn = 0;   // Keeps a track of the current player turn

    bool bCanCastle = true;
    bool wCanCastle = true;

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
            if (moveCounter < 1) {
                cout << getNextMove(currentMove) << endl;
            }
            else {
                cout << getNextMove(previousMove + " " + currentMove) << endl;
            }
            SetConsoleTextAttribute(h, 15);
            gotoxy(0, 0);

            if(kingInCheck(board)) {

                gotoxy(103, 2);

                SetConsoleTextAttribute(h, FOREGROUND_RED);
                cout << "BLACK IN CHECK" << endl;
            }
            else {
                gotoxy(103, 2);

                SetConsoleTextAttribute(h, 0);
                cout << "BLACK IN CHECK" << endl;
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