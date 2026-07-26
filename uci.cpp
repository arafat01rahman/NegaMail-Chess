/*
 * UCI (Universal Chess Interface) Protocol Implementation
 * The UCI protocol is a standard interface for chess engines to communicate
 * with GUIs (Graphical User Interfaces) like Cute Chess, Arena, and Fritz.
 *
 * Based on the UCI protocol specification:
 * https://www.uci-protocol.org/
 *
 * Implementation features:
 * - Full UCI command parsing (uci, isready, position, go, stop, quit)
 * - FEN position parsing
 * - Move conversion between internal format and UCI format
 * - Brute-force UCI to Move conversion via legal move generation
 *
 * Author: ARAFAT
 * Date: Sun 26 JULY 2026
 * License: Open source
 */

/*
 * UCI (Universal Chess Interface) Protocol Implementation
 * ...
 * Author: ARAFAT
 */

#include "uci.h"
#include <sstream>
#include <cctype>

bool stop_search = false;

std::string square_to_uci(int sq)
{
    int file = sq & 7;
    int rank = sq >> 4;
    return std::string(1, 'a' + file) + std::string(1, '1' + rank);
}

int uci_to_square(const std::string &s)
{
    int file = s[0] - 'a';
    int rank = s[1] - '1';
    return rank * 16 + file;
}

std::string move_to_uci(const Move &m)
{
    std::string uci = square_to_uci(m.from) + square_to_uci(m.to);
    if (m.flag == MOVE_PROMOTION)
    {
        if (m.promotion == W_QUEEN || m.promotion == B_QUEEN)
            uci += 'q';
        else if (m.promotion == W_ROOK || m.promotion == B_ROOK)
            uci += 'r';
        else if (m.promotion == W_BISHOP || m.promotion == B_BISHOP)
            uci += 'b';
        else if (m.promotion == W_KNIGHT || m.promotion == B_KNIGHT)
            uci += 'n';
    }
    return uci;
}

Move uci_to_move(const std::string &uci, Board &b)
{
    std::vector<Move> moves;
    generate_legal_moves(b, b.side_to_move, moves);

    int from = uci_to_square(uci.substr(0, 2));
    int to = uci_to_square(uci.substr(2, 2));

    int promo = 0;
    if (uci.length() == 5)
    {
        char p = uci[4];
        int color = b.side_to_move;
        if (p == 'q')
            promo = (color == 1) ? W_QUEEN : B_QUEEN;
        else if (p == 'r')
            promo = (color == 1) ? W_ROOK : B_ROOK;
        else if (p == 'b')
            promo = (color == 1) ? W_BISHOP : B_BISHOP;
        else if (p == 'n')
            promo = (color == 1) ? W_KNIGHT : B_KNIGHT;
    }

    for (const Move &m : moves)
    {
        if (m.from == from && m.to == to)
        {
            if (uci.length() == 5)
            {
                if (m.flag == MOVE_PROMOTION && m.promotion == promo)
                    return m;
            }
            else
            {
                return m;
            }
        }
    }
    return Move{-1, -1, 0, MOVE_NORMAL};
}

void parse_fen(Board &b, const std::string &fen)
{
    // If fen is "startpos", just set up the starting position
    if (fen == "startpos")
    {
        init_starting_position(b);
        return;
    }

    clear_board(b);
    std::istringstream iss(fen);
    std::string piece_placement;
    iss >> piece_placement;

    int rank = 7;
    int file = 0;
    for (char c : piece_placement)
    {
        if (c == '/')
        {
            rank--;
            file = 0;
        }
        else if (isdigit(c))
        {
            file += (c - '0');
        }
        else
        {
            int piece = 0;
            switch (c)
            {
            case 'P':
                piece = W_PAWN;
                break;
            case 'N':
                piece = W_KNIGHT;
                break;
            case 'B':
                piece = W_BISHOP;
                break;
            case 'R':
                piece = W_ROOK;
                break;
            case 'Q':
                piece = W_QUEEN;
                break;
            case 'K':
                piece = W_KING;
                break;
            case 'p':
                piece = B_PAWN;
                break;
            case 'n':
                piece = B_KNIGHT;
                break;
            case 'b':
                piece = B_BISHOP;
                break;
            case 'r':
                piece = B_ROOK;
                break;
            case 'q':
                piece = B_QUEEN;
                break;
            case 'k':
                piece = B_KING;
                break;
            }
            if (piece != 0)
            {
                int sq = rank * 16 + file;
                b.squares[sq] = piece;
            }
            file++;
        }
    }

    std::string side, castling, ep;
    if (!(iss >> side))
        side = "w";
    if (!(iss >> castling))
        castling = "-";
    if (!(iss >> ep))
        ep = "-";

    b.side_to_move = (side == "w") ? 1 : -1;

    b.castling_rights = 0;
    if (castling.find('K') != std::string::npos)
        b.castling_rights |= 1;
    if (castling.find('Q') != std::string::npos)
        b.castling_rights |= 2;
    if (castling.find('k') != std::string::npos)
        b.castling_rights |= 4;
    if (castling.find('q') != std::string::npos)
        b.castling_rights |= 8;

    // Guard against empty ep string
    if (ep.empty() || ep == "-")
    {
        b.en_passant_square = -1;
    }
    else
    {
        b.en_passant_square = uci_to_square(ep);
    }
}

void uci_loop()
{
    Board b;
    std::string line;

    while (std::getline(std::cin, line))
    {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "uci")
        {
            std::cout << "id name NegaMail-Chess" << std::endl;
            std::cout << "id author ARAFAT" << std::endl;
            std::cout << "uciok" << std::endl;
            std::flush(std::cout);
        }
        else if (token == "isready")
        {
            std::cout << "readyok" << std::endl;
            std::flush(std::cout);
        }
        else if (token == "position")
        {
            iss >> token;
            if (token == "startpos")
            {
                init_starting_position(b);
                if (iss >> token && token == "moves")
                {
                    std::string move_str;
                    while (iss >> move_str)
                    {
                        Move m = uci_to_move(move_str, b);
                        if (m.from != -1)
                            make_move(b, m);
                    }
                }
            }
            else if (token == "fen")
            {
                std::string fen;
                for (int i = 0; i < 6; i++)
                {
                    std::string part;
                    if (!(iss >> part))
                        break;
                    fen += part;
                    if (i < 5)
                        fen += " ";
                }
                parse_fen(b, fen);
                if (iss >> token && token == "moves")
                {
                    std::string move_str;
                    while (iss >> move_str)
                    {
                        Move m = uci_to_move(move_str, b);
                        if (m.from != -1)
                            make_move(b, m);
                    }
                }
            }
        }
        else if (token == "go")
        {
            int depth = 4;
            while (iss >> token)
            {
                if (token == "depth")
                    iss >> depth;
                else if (token == "movetime")
                {
                    int mt;
                    iss >> mt;
                }
                else if (token == "infinite")
                { /* ignore */
                }
            }
            stop_search = false;
            Move best = find_best_move(b, depth);
            if (best.from != -1)
                std::cout << "bestmove " << move_to_uci(best) << std::endl;
            else
                std::cout << "bestmove (none)" << std::endl;
            std::flush(std::cout);
        }
        else if (token == "stop")
        {
            stop_search = true;
        }
        else if (token == "ucinewgame")
        {
            // Could reset TT here
        }
        else if (token == "quit")
        {
            break;
        }
    }
}