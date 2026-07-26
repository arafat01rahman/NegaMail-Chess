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
 * 
 * Date: Sun 26 JULY 2026
 * License: Open source (specify your license)
 */

#pragma once

#include "board.h"      
#include "search.h"     
#include "evaluate.h"   
#include <string>       

// ============================================================
// UCI Helper Functions
// ============================================================

/**
 * Convert a 0x88 square index to UCI format
 * @param sq 0x88 square index (0-127, only even indices valid)
 * @return String like "e4", "a1", "h8"
 */
std::string square_to_uci(int sq);

/**
 * Convert a UCI square string to 0x88 index
 * @param s UCI square string (e.g., "e4")
 * @return 0x88 square index
 */
int uci_to_square(const std::string& s);

/**
 * Convert internal Move struct to UCI format
 * @param m Internal Move structure
 * @return UCI string like "e2e4", "e7e8q" for promotions
 */
std::string move_to_uci(const Move& m);

/**
 * Convert UCI string to internal Move struct using brute-force
 * Generates all legal moves and finds the matching one
 * @param uci UCI move string (e.g., "e2e4", "e7e8q")
 * @param b Current board state (non-const for move generation)
 * @return Internal Move structure, or invalid move (-1,-1) if not found
 */
Move uci_to_move(const std::string& uci, Board& b);

/**
 * Parse a FEN string and set up the board
 * @param b Board to modify
 * @param fen FEN string (e.g., "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
 */
void parse_fen(Board& b, const std::string& fen);

/**
 * Main UCI event loop
 * Reads commands from stdin and responds via stdout
 * Handles: uci, isready, position, go, stop, quit
 */
void uci_loop();

// ============================================================
// Global State
// ============================================================

/**
 * Global flag to stop the search mid-execution
 * Set by "stop" command, checked by find_best_move()
 */
extern bool stop_search;