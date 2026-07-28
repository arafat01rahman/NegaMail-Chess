"""
NegaMail-Chess API Server (Pure Python edition)
No external engine – uses python-chess and a simple minimax AI.
"""

from flask import Flask, request, jsonify, send_from_directory
from flask_cors import CORS
from werkzeug.exceptions import HTTPException
import chess
import time
import os

app = Flask(__name__, static_folder='static')
CORS(app)

# ---------- Evaluation function (material + piece-square tables) ----------
PIECE_VALUES = {
    chess.PAWN: 100,
    chess.KNIGHT: 320,
    chess.BISHOP: 330,
    chess.ROOK: 500,
    chess.QUEEN: 900,
    chess.KING: 20000,
}

# Piece-square tables (simplified from your C++ version)
# Index: square 0..63 (a1=0, h1=7, a2=16, ...)
# We'll just use basic PST for simplicity.
PAWN_PST = [
    0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5,  5, 10, 25, 25, 10,  5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5, -5,-10,  0,  0,-10, -5,  5,
    5, 10, 10,-20,-20, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0
]
KNIGHT_PST = [
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
]
BISHOP_PST = [
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
]
ROOK_PST = [
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10, 10, 10, 10, 10,  5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
    0,  0,  0,  5,  5,  0,  0,  0
]
QUEEN_PST = [
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
    -5,  0,  5,  5,  5,  5,  0, -5,
    0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
]
KING_PST = [
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
    20, 20,  0,  0,  0,  0, 20, 20,
    20, 30, 10,  0,  0, 10, 30, 20
]

PST = {
    chess.PAWN: PAWN_PST,
    chess.KNIGHT: KNIGHT_PST,
    chess.BISHOP: BISHOP_PST,
    chess.ROOK: ROOK_PST,
    chess.QUEEN: QUEEN_PST,
    chess.KING: KING_PST,
}

def evaluate(board):
    """Return score from White's perspective."""
    if board.is_checkmate():
        return -999999 if board.turn == chess.WHITE else 999999
    if board.is_stalemate() or board.is_insufficient_material():
        return 0

    score = 0
    # Material + PST
    for color in (chess.WHITE, chess.BLACK):
        multiplier = 1 if color == chess.WHITE else -1
        for piece_type in PIECE_VALUES:
            squares = board.pieces(piece_type, color)
            for sq in squares:
                # PST index: for black we mirror vertically (rank 7 - rank)
                if color == chess.WHITE:
                    idx = sq
                else:
                    rank = chess.square_rank(sq)
                    file = chess.square_file(sq)
                    idx = (7 - rank) * 8 + file
                score += multiplier * (PIECE_VALUES[piece_type] + PST[piece_type][idx])
    return score

# ---------- Minimax with alpha‑beta ----------
def minimax(board, depth, alpha, beta, maximizing):
    if depth == 0 or board.is_game_over():
        return evaluate(board)

    if maximizing:
        max_eval = -999999
        for move in board.legal_moves:
            board.push(move)
            eval = minimax(board, depth-1, alpha, beta, False)
            board.pop()
            max_eval = max(max_eval, eval)
            alpha = max(alpha, eval)
            if beta <= alpha:
                break
        return max_eval
    else:
        min_eval = 999999
        for move in board.legal_moves:
            board.push(move)
            eval = minimax(board, depth-1, alpha, beta, True)
            board.pop()
            min_eval = min(min_eval, eval)
            beta = min(beta, eval)
            if beta <= alpha:
                break
        return min_eval

def find_best_move(board, depth=3):
    """Returns the best move in UCI format."""
    if board.is_game_over():
        return None

    is_white_to_move = board.turn == chess.WHITE
    best_move = None
    if is_white_to_move:
        best_score = -999999
        compare = lambda score, best: score > best
    else:
        best_score = 999999
        compare = lambda score, best: score < best

    for move in board.legal_moves:
        board.push(move)
        score = minimax(board, depth-1, -999999, 999999, not is_white_to_move)
        board.pop()
        if compare(score, best_score):
            best_score = score
            best_move = move
    return best_move

# ---------- Flask endpoints ----------
@app.route('/move', methods=['POST'])
def get_move():
    data = request.get_json(silent=True)
    if not isinstance(data, dict):
        return jsonify({'error': 'Invalid JSON body'}), 400

    fen = data.get('fen', chess.STARTING_FEN)
    depth = data.get('depth', 3)
    try:
        depth = int(depth)
    except (TypeError, ValueError):
        depth = 3
    depth = max(1, min(depth, 4))  # keep engine search bounded

    try:
        board = chess.Board(fen)
    except ValueError:
        return jsonify({'error': 'Invalid FEN'}), 400

    if board.is_game_over():
        return jsonify({'error': 'Game over'}), 400

    start = time.time()
    move = find_best_move(board, depth)
    elapsed = time.time() - start

    if move is None:
        return jsonify({'error': 'No legal moves'}), 400

    return jsonify({
        'move': move.uci(),
        'score': evaluate(board),  # score from current position (before moving)
        'time': round(elapsed, 3),
        'fen': fen
    })

@app.route('/analyze', methods=['POST'])
def analyze():
    # Optional – same as /move but returns more info
    data = request.json
    fen = data.get('fen', chess.STARTING_FEN)
    board = chess.Board(fen)
    if board.is_game_over():
        return jsonify({'error': 'Game over'}), 400
    move = find_best_move(board, 3)
    return jsonify({'bestmove': move.uci() if move else None})

@app.route('/debug-engine')
def debug_engine():
    # Not needed anymore, but keep for compatibility
    return jsonify({'status': 'pure Python engine', 'engine': 'NegaMail-AI'})

@app.route('/health')
def health():
    return jsonify({'status': 'ok', 'engine': 'NegaMail-AI (Python)'})

@app.errorhandler(404)
def not_found(e):
    return jsonify({'error': 'Not found'}), 404

@app.errorhandler(Exception)
def handle_exception(e):
    if isinstance(e, HTTPException):
        return jsonify({'error': e.description}), e.code
    app.logger.exception('Unhandled exception')
    return jsonify({'error': 'Internal server error'}), 500

@app.route('/')
def home():
    return app.send_static_file('index.html')

if __name__ == '__main__':
    port = int(os.environ.get('PORT', 5000))
    app.run(host='0.0.0.0', port=port, debug=False)