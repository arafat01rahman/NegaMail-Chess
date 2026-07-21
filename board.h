#pragma once
#include <vector>
#include <iostream>

enum Piece
{
    EMPTY = 0,
    W_PAWN = 1,
    W_KNIGHT = 2,
    W_BISHOP = 3,
    W_ROOK = 4,
    W_QUEEN = 5,
    W_KING = 6,
    B_PAWN = -1,
    B_KNIGHT = -2,
    B_BISHOP = -3,
    B_ROOK = -4,
    B_QUEEN = -5,
    B_KING = -6
};

struct Board
{
    int squares[128];
    int en_passant_square;
    int castling_rights;
};

enum MoveFlag
{
    MOVE_NORMAL,
    MOVE_PROMOTION,
    MOVE_ENPASSANT,
    MOVE_CASTLING,
    MOVE_DOUBLE_PAWN_PUSH
};

struct Move
{
    int from;
    int to;
    int promotion;
    MoveFlag flag;
};

const int knight_offset[8] = {-33, -31, -18, -14, 14, 18, 31, 33};
const int king_offset[8] = {-1, -15, -17, 1, 15, 16, 17};
const int bishop_offset[8] = {-15, -17, 15, 17};
const int rook_offset[8] = {-16, -1, 1, 16};
const int queen_offset[8] = {-17, -16, -15, -1, 1, 15, 16, 17};

void clear_board(Board &b);
int type_of(int piece);
int color_of(int piece);
bool is_on_board(int sq);
void init_starting_position(Board &b);
char piece_to_char(int piece);
void print_board(Board &b);
void print_move(const Move &m);
void generate_pawn_moves(const Board &b, int color, std::vector<Move> &moves);
void generate_knight_moves(const Board &b, int from, int color, std::vector<Move> &moves);
void generate_king_moves(const Board &b, int from, int color, std::vector<Move> &moves);
void generate_slider_moves(const Board &b, int from, int color, const int *offsets, int dir_offsets, std::vector<Move> &moves);
void generate_moves(const Board &b, int color, std::vector<Move> &moves);