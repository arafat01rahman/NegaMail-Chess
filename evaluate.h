#pragma once

#include "board.h"

extern const int PAWN_VALUE;
extern const int KNIGHT_VALUE;
extern const int BISHOP_VALUE;
extern const int ROOK_VALUE;
extern const int QUEEN_VALUE;
extern const int KING_VALUE;


extern const int PST_PAWN[128];
extern const int PST_KNIGHT[128];
extern const int PST_BISHOP[128];
extern const int PST_ROOK[128];
extern const int PST_QUEEN[128];
extern const int PST_KING[128];

int evaluate(const Board &b);
