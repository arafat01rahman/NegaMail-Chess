#include "search.h"
#include "evaluate.h"
#include <vector>
#include "board.h" 


int negamax(Board &b, int depth, int alpha, int beta, int color, int max_depth)
{
    // Base case for recursion (Leaf node)
    if (depth == 0)
    {
        return color * evaluate(b);
    }

    std::vector<Move> moves;
    generate_legal_moves(b, b.side_to_move, moves);

    if (moves.empty())
    {
        if (is_in_check(b, b.side_to_move))
        {
            return -100000 + (max_depth - depth);
        }

        return 0; // stalemate
    }

    int best = -1000000;
    for (const Move &m : moves)
    {
        // make the move
        UndoInfo u = make_move(b, m);
        // recursive negamax here
        int score = -negamax(b, depth - 1, -beta, -alpha, -color, max_depth);

        // unmake the move after each test
        unmake_move(b, m, u);

        // Update best score
        if (score > best)
            best = score;

        // Alpha-beta pruning
        if (score > alpha)
            alpha = score;

        if (alpha >= beta)
            break;
    }
    return best;
}

extern bool stop_search;

Move find_best_move(Board &b, int depth)
{
    int color = b.side_to_move;
    std::vector<Move> moves;
    generate_legal_moves(b, b.side_to_move, moves);

    if (moves.empty())
        return Move{-1, -1, 0, MOVE_NORMAL};

    Move best_move = moves[0];
    int best_score = -1000000;

    for (const Move &m : moves)
    {
        if (stop_search)
            break;

        UndoInfo u = make_move(b, m);
        int score = -negamax(b, depth - 1, -1000000, 1000000, -color, depth);
        unmake_move(b, m, u);

        if (score > best_score)
        {
            best_score = score;
            best_move = m;
        }
    }

    return best_move;
}