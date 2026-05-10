#include "piece.h"
#include <cmath>
#include <ctype.h>
#include <algorithm>
using namespace std;

//  PAWN

bool isValidPawnMove(char board[8][8], int fromRow, int toRow, int fromCol, int toCol, bool isWhite) {
    // direction: white moves up (row decreases), black moves down (row increases)
    int dir        = isWhite ? -1 : 1;
    int startRow   = isWhite ?  6 :  1;   // row index where pawns begin

    // Diagonal capture 
    if (toRow == fromRow + dir && abs(toCol - fromCol) == 1) {
        char target = board[toRow][toCol];
        if (target == '.') return false;                      // nothing to capture
        if (isWhite  && islower(target)) return true;         // white captures black piece
        if (!isWhite && isupper(target)) return true;         // black captures white piece
        return false;                                         // can't capture own piece
    }

    // Straight moves (column must stay same)
    if (toCol != fromCol) return false;

    // one step forward
    if (toRow == fromRow + dir) {
        return board[toRow][toCol] == '.';
    }

    // two steps forward from starting row
    if (fromRow == startRow && toRow == fromRow + 2 * dir) {
        // both squares ahead must be empty
        return board[fromRow + dir][fromCol] == '.' && board[toRow][toCol] == '.';
    }

    return false;
}

//  ROOK

bool isValidRookMove(char board[8][8], int fromRow, int toRow, int fromCol, int toCol) {
    // can't capture own piece
    if ((isupper(board[fromRow][fromCol]) && isupper(board[toRow][toCol])) ||
        (islower(board[fromRow][fromCol]) && islower(board[toRow][toCol])))
        return false;

    // must move along a rank or file
    if (fromRow != toRow && fromCol != toCol) return false;
    if (fromRow == toRow && fromCol == toCol) return false;  // no move at all

    if (fromRow == toRow) {
        // horizontal – check for pieces between
        for (int c = min(fromCol, toCol) + 1; c < max(fromCol, toCol); c++)
            if (board[fromRow][c] != '.') return false;
        return true;
    }

    // vertical – check for pieces between
    for (int r = min(fromRow, toRow) + 1; r < max(fromRow, toRow); r++)
        if (board[r][fromCol] != '.') return false;
    return true;
}

//  KNIGHT

bool isValidKnightMove(char board[8][8], int fromRow, int toRow, int fromCol, int toCol) {
    // can't capture own piece
    if ((isupper(board[fromRow][fromCol]) && isupper(board[toRow][toCol])) ||
        (islower(board[fromRow][fromCol]) && islower(board[toRow][toCol])))
        return false;

    int dr = abs(fromRow - toRow);
    int dc = abs(fromCol - toCol);
    // valid knight deltas: (1,2) or (2,1)
    return (dr == 1 && dc == 2) || (dr == 2 && dc == 1);
}

//  BISHOP

bool isValidBishopMove(char board[8][8], int fromRow, int toRow, int fromCol, int toCol) {
    // can't capture own piece
    if ((isupper(board[fromRow][fromCol]) && isupper(board[toRow][toCol])) ||
        (islower(board[fromRow][fromCol]) && islower(board[toRow][toCol])))
        return false;

    int dr = abs(fromRow - toRow);
    int dc = abs(fromCol - toCol);
    if (dr != dc || dr == 0) return false;   // must be diagonal and actually move

    // step directions
    int rowStep = (toRow > fromRow) ? 1 : -1;
    int colStep = (toCol > fromCol) ? 1 : -1;

    int r = fromRow + rowStep;
    int c = fromCol + colStep;
    while (r != toRow) {                     // walk the diagonal, check for blockers
        if (board[r][c] != '.') return false;
        r += rowStep;
        c += colStep;
    }
    return true;
}

//  KING  (one step in any direction)

bool isValidKingMove(char board[8][8], int fromRow, int toRow, int fromCol, int toCol) {
    // can't capture own piece
    if ((isupper(board[fromRow][fromCol]) && isupper(board[toRow][toCol])) ||
        (islower(board[fromRow][fromCol]) && islower(board[toRow][toCol])))
        return false;

    int dr = abs(fromRow - toRow);
    int dc = abs(fromCol - toCol);
    // must move exactly 1 step in any direction (including diagonal)
    return dr <= 1 && dc <= 1 && (dr + dc) > 0;
}

//  QUEEN  (rook + bishop combined)

bool isValidQueenMove(char board[8][8], int fromRow, int toRow, int fromCol, int toCol) {
    // can't capture own piece
    if ((isupper(board[fromRow][fromCol]) && isupper(board[toRow][toCol])) ||
        (islower(board[fromRow][fromCol]) && islower(board[toRow][toCol])))
        return false;

    // queen = rook movement OR bishop movement
    // We temporarily treat it as both without the own-piece check (already done above)
    return isValidRookMove(board, fromRow, toRow, fromCol, toCol) ||
           isValidBishopMove(board, fromRow, toRow, fromCol, toCol);
}

//  UTILITY: find king of a given color

bool findKing(char board[8][8], bool isWhite, int &kingRow, int &kingCol) {
    char kingChar = isWhite ? 'K' : 'k';
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            if (board[r][c] == kingChar) {
                kingRow = r; kingCol = c;
                return true;
            }
    return false;   // king not on board (shouldn't happen in a normal game)
}

//  Is a square under attack by the opponent?

bool isSquareUnderAttack(char board[8][8], int kingRow, int kingCol, bool kingIsWhite) {
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            char piece = board[r][c];
            if (piece == '.') continue;

            // only enemy pieces can attack
            bool pieceIsWhite = isupper(piece);
            if (pieceIsWhite == kingIsWhite) continue;

            char p = tolower(piece);

            // We need a dummy board where the destination is a capturable piece
            // for the validator to not reject it as empty.  We do the check
            // directly with the move-validator functions which look at the board.
            if (p == 'p' && isValidPawnMove(board, r, kingRow, c, kingCol, pieceIsWhite))
                return true;
            if (p == 'r' && isValidRookMove(board, r, kingRow, c, kingCol))
                return true;
            if (p == 'n' && isValidKnightMove(board, r, kingRow, c, kingCol))
                return true;
            if (p == 'b' && isValidBishopMove(board, r, kingRow, c, kingCol))
                return true;
            if (p == 'q' && isValidQueenMove(board, r, kingRow, c, kingCol))
                return true;
            if (p == 'k' && isValidKingMove(board, r, kingRow, c, kingCol))
                return true;
        }
    }
    return false;
}

//  Does making a move leave our own king in check?

bool moveLeavesKingInCheck(char board[8][8], int fromRow, int fromCol, int toRow, int toCol, bool isWhite) {
    // Make the move on a temporary board
    char temp[8][8];
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            temp[r][c] = board[r][c];

    temp[toRow][toCol]     = temp[fromRow][fromCol];
    temp[fromRow][fromCol] = '.';

    int kingRow, kingCol;
    findKing(temp, isWhite, kingRow, kingCol);
    return isSquareUnderAttack(temp, kingRow, kingCol, isWhite);
}

//  Does the given side have ANY legal move?
//  (used to detect checkmate vs stalemate)

bool hasNoLegalMoves(char board[8][8], bool isWhite) {
    for (int fr = 0; fr < 8; fr++) {
        for (int fc = 0; fc < 8; fc++) {
            char piece = board[fr][fc];
            if (piece == '.') continue;
            if ((bool)isupper(piece) != isWhite) continue;  // not our piece

            char p = tolower(piece);

            for (int tr = 0; tr < 8; tr++) {
                for (int tc = 0; tc < 8; tc++) {
                    if (fr == tr && fc == tc) continue;

                    bool legal = false;
                    if (p == 'p') legal = isValidPawnMove(board, fr, tr, fc, tc, isWhite);
                    else if (p == 'r') legal = isValidRookMove(board, fr, tr, fc, tc);
                    else if (p == 'n') legal = isValidKnightMove(board, fr, tr, fc, tc);
                    else if (p == 'b') legal = isValidBishopMove(board, fr, tr, fc, tc);
                    else if (p == 'q') legal = isValidQueenMove(board, fr, tr, fc, tc);
                    else if (p == 'k') legal = isValidKingMove(board, fr, tr, fc, tc);

                    if (legal && !moveLeavesKingInCheck(board, fr, fc, tr, tc, isWhite))
                        return false;   // found at least one legal move
                }
            }
        }
    }
    return true;   // no legal move found
}