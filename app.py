"""
NegaMail-Chess API Server
Hosted on Render.com - Free 24/7 Chess Engine API
Author: ARAFAT
"""

from flask import Flask, request, jsonify, send_from_directory
from flask_cors import CORS
import subprocess
import os
import json
import time
import select
import signal

app = Flask(__name__, static_folder='static')
CORS(app)

# Engine configuration
ENGINE_PATH = './engine'
ENGINE_TIMEOUT = 5  # seconds (search timeout)

@app.route('/')
def home():
    """Serve the chess board UI"""
    return send_from_directory('static', 'index.html')

@app.route('/move', methods=['POST'])
def get_move():
    """
    Get best move from engine
    Expects: {"fen": "start", "depth": 4}
    Returns: {"move": "e2e4", "score": 20}
    """
    data = request.json
    fen = data.get('fen', 'start')
    depth = data.get('depth', 4)  # default depth 4, but we may reduce to 3 for speed

    try:
        # Start engine process
        engine = subprocess.Popen(
            [ENGINE_PATH],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            bufsize=1
        )

        def write_cmd(cmd):
            engine.stdin.write(cmd + '\n')
            engine.stdin.flush()

        # --- UCI Handshake ---
        write_cmd('uci')
        # Wait for uciok
        for line in engine.stdout:
            if line.startswith('uciok'):
                break

        write_cmd('isready')
        for line in engine.stdout:
            if line.startswith('readyok'):
                break

        # --- Send position and go ---
        write_cmd(f'position fen {fen}')
        write_cmd(f'go depth {depth}')

        # --- Read result with timeout ---
        bestmove = None
        score = 0
        start_time = time.time()
        timeout = ENGINE_TIMEOUT

        while time.time() - start_time < timeout:
            # Check if stdout has data
            if select.select([engine.stdout], [], [], 0.1)[0]:
                line = engine.stdout.readline()
                if line.startswith('bestmove'):
                    parts = line.split()
                    if len(parts) > 1:
                        bestmove = parts[1]
                    break
                elif line.startswith('info score cp'):
                    parts = line.split()
                    for i, p in enumerate(parts):
                        if p == 'cp' and i+1 < len(parts):
                            try:
                                score = int(parts[i+1])
                            except:
                                pass
            else:
                # No output yet, continue
                continue
        else:
            # Timeout reached
            engine.terminate()
            engine.wait(timeout=2)
            return jsonify({'error': 'Engine timeout'}), 500

        engine.terminate()
        engine.wait(timeout=2)

        if bestmove is None:
            return jsonify({'error': 'No move found'}), 500

        return jsonify({
            'move': bestmove,
            'score': score,
            'fen': fen,
            'depth': depth
        })

    except subprocess.TimeoutExpired:
        engine.kill()
        return jsonify({'error': 'Engine killed due to timeout'}), 500
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/analyze', methods=['POST'])
def analyze():
    """Get multiple moves with scores (for analysis) - optional"""
    data = request.json
    fen = data.get('fen', 'start')
    depth = data.get('depth', 4)

    try:
        engine = subprocess.Popen(
            [ENGINE_PATH],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            bufsize=1
        )

        def write_cmd(cmd):
            engine.stdin.write(cmd + '\n')
            engine.stdin.flush()

        write_cmd('uci')
        for line in engine.stdout:
            if line.startswith('uciok'):
                break

        write_cmd('isready')
        for line in engine.stdout:
            if line.startswith('readyok'):
                break

        write_cmd(f'position fen {fen}')
        write_cmd(f'go depth {depth} movetime 1000')

        # Collect bestmove and score
        bestmove = None
        for line in engine.stdout:
            if line.startswith('bestmove'):
                parts = line.split()
                if len(parts) > 1:
                    bestmove = parts[1]
                break
            # Could also parse multipv info

        engine.terminate()
        engine.wait(timeout=2)

        return jsonify({'bestmove': bestmove})

    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/health')
def health():
    """Health check for Render"""
    return jsonify({'status': 'ok', 'engine': 'NegaMail-Chess'})

@app.errorhandler(404)
def not_found(e):
    return jsonify({'error': 'Not found'}), 404

if __name__ == '__main__':
    port = int(os.environ.get('PORT', 5000))
    app.run(host='0.0.0.0', port=port, debug=False)