#pragma once
#include <string>
using namespace std;

bool isValidPawnMove(char board[8][8], int fromRow, int toRow, int fromCol, int toCol, bool isWhite);
bool isValidRookMove(char board[8][8], int fromRow, int toRow, int fromCol, int toCol);
bool isValidKnightMove(char board[8][8], int fromRow, int toRow, int fromCol, int toCol);
bool isValidBishopMove(char board[8][8], int fromRow, int toRow, int fromCol, int toCol);
bool isValidKingMove(char board[8][8], int fromRow, int toRow, int fromCol, int toCol);
bool isValidQueenMove(char board[8][8], int fromRow, int toRow, int fromCol, int toCol);

// Returns true if the square (kingRow, kingCol) is under attack by the opponent
bool isSquareUnderAttack(char board[8][8], int kingRow, int kingCol, bool kingIsWhite);

// Returns true if making the move from->to would leave own king in check
bool moveLeavesKingInCheck(char board[8][8], int fromRow, int fromCol, int toRow, int toCol, bool isWhite);

// Find king position of the given color
bool findKing(char board[8][8], bool isWhite, int &kingRow, int &kingCol);

// Returns true if the given side has no legal moves (used for checkmate/stalemate)
bool hasNoLegalMoves(char board[8][8], bool isWhite);