#pragma once
#include "board.h"

int negamax(Board &b, int depth, int alpha, int beta, int color, int max_depth);
Move find_best_move(Board &b, int depth);