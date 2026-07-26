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

#include "uci.h"
#include <sstream>
#include <cctype>

// ============================================================
// Global State Definition
// ============================================================

/**
 * Global stop flag definition
 * Initialized to false at program start
 */
bool stop_search = false;

// ============================================================
// UCI Helper Functions
// ============================================================

/**
 * Convert square index to UCI string
 *
 * Example:
 *   sq=0   -> "a1"  (0x88 square 0 = a1)
 *   sq=68  -> "e4"  (0x88 square 68 = e4)
 *
 * @param sq 0x88 square index
 * @return UCI formatted square string
 */
std::string square_to_uci(int sq)
{
    int file = sq & 7;
    int rank = sq >> 4;
    return std::string(1, 'a' + file) + std::string(1, '1' + rank);
}

/**
 * Convert UCI string to 0x88 square index
 *
 * Example:
 *   "a1" -> 0   (0x88 square 0 = a1)
 *   "e4" -> 68  (0x88 square 68 = e4)
 *
 * @param s UCI square string
 * @return 0x88 square index
 */
int uci_to_square(const std::string &s)
{
    int file = s[0] - 'a';
    int rank = s[1] - '1';
    return rank * 16 + file;
}

/**
 * Convert internal Move to UCI string
 *
 * Examples:
 *   Move(e2, e4)      -> "e2e4"
 *   Move(e7, e8, QUEEN) -> "e7e8q"
 *
 * @param m Internal Move structure
 * @return UCI formatted move string
 */
std::string move_to_uci(const Move &m)
{
    std::string uci = square_to_uci(m.from) + square_to_uci(m.to);

    // Add promotion piece if it's a promotion
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

/**
 * Convert UCI string to internal Move using brute-force matching
 *
 * This is the safest way to convert UCI moves because:
 * 1. It handles promotions automatically
 * 2. It correctly identifies castling (by checking board state)
 * 3. It works for en passant captures
 *
 * The brute-force approach is fine because this is only called
 * a few times per game (when parsing the "position moves" command).
 *
 * @param uci UCI move string
 * @param b Current board state (modified during move generation)
 * @return Internal Move, or invalid move if not found
 */
Move uci_to_move(const std::string &uci, Board &b)
{
    // Generate all legal moves from current position
    std::vector<Move> moves;
    generate_legal_moves(b, b.side_to_move, moves);

    // Parse the UCI string
    int from = uci_to_square(uci.substr(0, 2));
    int to = uci_to_square(uci.substr(2, 2));

    // Check for promotion
    int promo = 0;
    if (uci.length() == 5)
    {
        char p = uci[4];
        int color = (b.side_to_move == 1) ? 1 : -1; // 1=white, -1=black

        if (p == 'q')
            promo = (color == 1) ? W_QUEEN : B_QUEEN;
        else if (p == 'r')
            promo = (color == 1) ? W_ROOK : B_ROOK;
        else if (p == 'b')
            promo = (color == 1) ? W_BISHOP : B_BISHOP;
        else if (p == 'n')
            promo = (color == 1) ? W_KNIGHT : B_KNIGHT;
    }

    // Find matching move by comparing from, to, and promotion
    for (const Move &m : moves)
    {
        if (m.from == from && m.to == to)
        {
            if (uci.length() == 5)
            {
                if (m.flag == MOVE_PROMOTION && m.promotion == promo)
                {
                    return m;
                }
            }
            else
            {
                return m;
            }
        }
    }

    // Move not found - return invalid move
    return Move{-1, -1, 0, MOVE_NORMAL};
}

/**
 * Parse FEN string and populate board
 *
 * FEN format: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
 *
 * This implementation handles:
 * - Piece placement
 * - Side to move
 * - Castling rights
 * - En passant square
 *
 * @param b Board to populate
 * @param fen FEN string
 */
void parse_fen(Board &b, const std::string &fen)
{
    clear_board(b);

    std::istringstream iss(fen);
    std::string piece_placement;
    iss >> piece_placement;

    int rank = 7; // Start from rank 8 (top)
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
            if (c == 'P')
                piece = W_PAWN;
            else if (c == 'N')
                piece = W_KNIGHT;
            else if (c == 'B')
                piece = W_BISHOP;
            else if (c == 'R')
                piece = W_ROOK;
            else if (c == 'Q')
                piece = W_QUEEN;
            else if (c == 'K')
                piece = W_KING;
            else if (c == 'p')
                piece = B_PAWN;
            else if (c == 'n')
                piece = B_KNIGHT;
            else if (c == 'b')
                piece = B_BISHOP;
            else if (c == 'r')
                piece = B_ROOK;
            else if (c == 'q')
                piece = B_QUEEN;
            else if (c == 'k')
                piece = B_KING;

            int sq = rank * 16 + file;
            b.squares[sq] = piece;
            file++;
        }
    }

    // Parse side to move
    std::string side;
    iss >> side;
    b.side_to_move = (side == "w") ? 1 : -1;

    // Parse castling rights
    std::string castling;
    iss >> castling;
    b.castling_rights = 0;
    if (castling.find('K') != std::string::npos)
        b.castling_rights |= 1;
    if (castling.find('Q') != std::string::npos)
        b.castling_rights |= 2;
    if (castling.find('k') != std::string::npos)
        b.castling_rights |= 4;
    if (castling.find('q') != std::string::npos)
        b.castling_rights |= 8;

    // Parse en passant
    std::string ep;
    iss >> ep;
    if (ep != "-")
    {
        b.en_passant_square = uci_to_square(ep);
    }
    else
    {
        b.en_passant_square = -1;
    }

    // Ignore halfmove and fullmove clocks
}

/**
 * Main UCI event loop
 *
 * This is the heart of the UCI protocol implementation.
 * It reads commands from stdin (one line at a time) and
 * responds via stdout.
 *
 * Supported commands:
 *   uci          - Identify engine and send options
 *   isready      - Confirm engine is ready
 *   position     - Set up a position (startpos or fen)
 *   go           - Start search
 *   stop         - Stop search
 *   quit         - Exit engine
 *
 * This loop runs until "quit" is received or stdin is closed.
 */
void uci_loop()
{
    Board b;
    std::string line;

    while (std::getline(std::cin, line))
    {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        // ============================================================
        // Command: uci
        // ============================================================
        // Purpose: Identify engine to GUI
        // Response: id name, id author, uciok
        if (token == "uci")
        {
            std::cout << "id name NegaMail-Chess" << std::endl;
            std::cout << "id author ARAFAT" << std::endl;
            std::cout << "uciok" << std::endl;
            std::flush(std::cout);
        }

        // ============================================================
        // Command: isready
        // ============================================================
        // Purpose: Check if engine is ready
        // Response: readyok
        else if (token == "isready")
        {
            std::cout << "readyok" << std::endl;
            std::flush(std::cout);
        }

        // ============================================================
        // Command: position
        // ============================================================
        // Purpose: Set up board position
        // Format: position startpos [moves ...]
        //         position fen <fen> [moves ...]
        else if (token == "position")
        {
            iss >> token; // "startpos" or "fen"

            // Task 4.5.3: position startpos
            if (token == "startpos")
            {
                init_starting_position(b);

                // Task 4.5.4: position startpos moves ...
                if (iss >> token && token == "moves")
                {
                    std::string move_str;
                    while (iss >> move_str)
                    {
                        Move m = uci_to_move(move_str, b);
                        if (m.from != -1)
                        {
                            make_move(b, m);
                        }
                    }
                }
            }

            // Task 4.5.5: position fen (Satisfactory)
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

                // Check for moves after fen
                if (iss >> token && token == "moves")
                {
                    std::string move_str;
                    while (iss >> move_str)
                    {
                        Move m = uci_to_move(move_str, b);
                        if (m.from != -1)
                        {
                            make_move(b, m);
                        }
                    }
                }
            }
        }

        // ============================================================
        // Command: go
        // ============================================================
        // Purpose: Start search for best move
        // Format: go depth <n> | movetime <ms> | infinite
        else if (token == "go")
        {
            int depth = 4; // Default depth

            // Parse options
            while (iss >> token)
            {
                if (token == "depth")
                {
                    iss >> depth;
                }
                else if (token == "movetime")
                {
                    // Task 4.5.6: movetime (Satisfactory)
                    int movetime;
                    iss >> movetime;
                    // For now, ignore movetime
                }
                else if (token == "infinite")
                {
                    // Task 4.5.9: infinite (Satisfactory)
                    // For now, ignore
                }
            }

            // Reset stop flag
            stop_search = false;

            // Find and output best move
            Move best = find_best_move(b, depth);

            // Task 4.6.4: Print bestmove
            if (best.from != -1)
            {
                std::cout << "bestmove " << move_to_uci(best) << std::endl;
            }
            else
            {
                // Check if checkmate or stalemate
                std::vector<Move> legal_moves;
                generate_legal_moves(b, b.side_to_move, legal_moves);
                if (is_in_check(b, b.side_to_move))
                {
                    std::cout << "bestmove (checkmate)" << std::endl;
                }
                else
                {
                    std::cout << "bestmove (stalemate)" << std::endl;
                }
            }
            std::flush(std::cout);
        }

        // ============================================================
        // Command: stop
        // ============================================================
        // Purpose: Stop the current search
        // Effect: Sets stop_search flag, which is checked in search
        else if (token == "stop")
        {
            stop_search = true;
        }

        // ============================================================
        // Command: ucinewgame
        // ============================================================
        // Purpose: Start a new game (reset transposition table)
        // Currently a placeholder for Stage 5
        else if (token == "ucinewgame")
        {
            // For Stage 5: reset transposition table
            // For now, do nothing
        }

        // ============================================================
        // Command: quit
        // ============================================================
        // Purpose: Exit the engine
        // Effect: Breaks the main loop
        else if (token == "quit")
        {
            break;
        }

        // Unknown command - silently ignore
        else
        {
            // Ignore unknown commands
        }
    }
}