#include <iostream>
#include <string>
#include <cctype>
#include "piece.h"
using namespace std;

//  Board printer

void printboard(char board[8][8]) {
    cout << endl;
    for (int i = 0; i < 8; i++) {
        cout << "  " << 8 - i << "  ";
        for (int j = 0; j < 8; j++) {
            cout << board[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl << "     ";
    for (char c = 'a'; c <= 'h'; c++)
        cout << c << " ";
    cout << endl << endl;
}

//  Parse "a2" -> row/col indices

bool parseSquare(const string &s, int &row, int &col) {
    if (s.size() < 2) return false;
    col = s[0] - 'a';
    row = 8 - (s[1] - '0');
    return col >= 0 && col < 8 && row >= 0 && row < 8;
}

//  Pawn promotion (auto-promote to queen)

void handlePromotion(char board[8][8]) {
    for (int c = 0; c < 8; c++) {
        if (board[0][c] == 'P') board[0][c] = 'Q';   // white pawn reached rank 8
        if (board[7][c] == 'p') board[7][c] = 'q';   // black pawn reached rank 1
    }
}

//  MAIN

int main() {
    char board[8][8];

    // initialise empty board
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            board[i][j] = '.';

    // pawns
    for (int i = 0; i < 8; i++) {
        board[1][i] = 'p';   // black pawns
        board[6][i] = 'P';   // white pawns
    }

    // black pieces
    board[0][0] = board[0][7] = 'r';
    board[0][1] = board[0][6] = 'n';
    board[0][2] = board[0][5] = 'b';
    board[0][3] = 'q';
    board[0][4] = 'k';

    // white pieces
    board[7][0] = board[7][7] = 'R';
    board[7][1] = board[7][6] = 'N';
    board[7][2] = board[7][5] = 'B';
    board[7][3] = 'Q';
    board[7][4] = 'K';

    printboard(board);

    bool isWhite = true;   // white goes first

    while (true) {

        // ── Check / Checkmate / Stalemate detection 
        int kingRow, kingCol;
        findKing(board, isWhite, kingRow, kingCol);
        bool inCheck = isSquareUnderAttack(board, kingRow, kingCol, isWhite);

        if (hasNoLegalMoves(board, isWhite)) {
            if (inCheck) {
                cout << "\n♚  CHECKMATE!  " << (isWhite ? "Black" : "White") << " wins!\n\n";
            } else {
                cout << "\nSTALEMATE! It's a draw.\n\n";
            }
            break;
        }

        if (inCheck)
            cout << "\n⚠  " << (isWhite ? "White" : "Black") << " is in CHECK!\n";

        // ── Prompt 
        cout << (isWhite ? "\nWhite" : "\nBlack")
             << "'s turn. Enter move (e.g. e2 e4): ";

        string fromStr, toStr;
        if (!(cin >> fromStr >> toStr)) break;   // EOF / pipe closed

        int fromRow, fromCol, toRow, toCol;
        if (!parseSquare(fromStr, fromRow, fromCol) || !parseSquare(toStr, toRow, toCol)) {
            cout << "Invalid square notation. Use format like e2 e4.\n";
            continue;
        }

        // ── Basic sanity checks
        char piece = board[fromRow][fromCol];

        if (piece == '.') {
            cout << "No piece on " << fromStr << ". Try again.\n";
            continue;
        }
        if (isWhite && islower(piece)) {
            cout << "That's a black piece. It's White's turn.\n";
            continue;
        }
        if (!isWhite && isupper(piece)) {
            cout << "That's a white piece. It's Black's turn.\n";
            continue;
        }

        // Validate move for the piece type
        char p = tolower(piece);
        bool valid = false;

        if      (p == 'p') valid = isValidPawnMove  (board, fromRow, toRow, fromCol, toCol, isWhite);
        else if (p == 'r') valid = isValidRookMove   (board, fromRow, toRow, fromCol, toCol);
        else if (p == 'n') valid = isValidKnightMove (board, fromRow, toRow, fromCol, toCol);
        else if (p == 'b') valid = isValidBishopMove (board, fromRow, toRow, fromCol, toCol);
        else if (p == 'k') valid = isValidKingMove   (board, fromRow, toRow, fromCol, toCol);
        else if (p == 'q') valid = isValidQueenMove  (board, fromRow, toRow, fromCol, toCol);

        if (!valid) {
            cout << "Invalid move for that piece. Try again.\n";
            continue;
        }

        // ── Make sure the move doesn't leave own king in check 
        if (moveLeavesKingInCheck(board, fromRow, fromCol, toRow, toCol, isWhite)) {
            cout << "That move leaves your King in check! Try again.\n";
            continue;
        }

        // Check if we are capturing the enemy king (should not normally happen with proper check detection, but kept as a safety net) ─
        char captured = board[toRow][toCol];
        if (captured == 'k' || captured == 'K') {
            board[toRow][toCol]     = board[fromRow][fromCol];
            board[fromRow][fromCol] = '.';
            printboard(board);
            cout << "\n\nCONGRATULATIONS " << (isWhite ? "WHITE" : "BLACK")
                 << "! YOU HAVE WON THE GAME.\n\n";
            break;
        }

        //  Apply the move 
        board[toRow][toCol]     = board[fromRow][fromCol];
        board[fromRow][fromCol] = '.';

        // pawn promotion (auto-queen)
        handlePromotion(board);

        printboard(board);
        isWhite = !isWhite;
    }

    return 0;
}