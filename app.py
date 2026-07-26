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
import signal

app = Flask(__name__, static_folder='static')
CORS(app)

# Engine configuration
ENGINE_PATH = './engine'
ENGINE_TIMEOUT = 5  # seconds

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
    depth = data.get('depth', 4)
    
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
        
        # Send UCI commands
        commands = [
            'uci\n',
            'isready\n',
            f'position fen {fen}\n',
            f'go depth {depth}\n'
        ]
        
        for cmd in commands:
            engine.stdin.write(cmd)
            engine.stdin.flush()
        
        # Read engine output
        bestmove = None
        score = 0
        
        for line in engine.stdout:
            if line.startswith('bestmove'):
                parts = line.split()
                bestmove = parts[1] if len(parts) > 1 else None
                break
            elif line.startswith('info score cp'):
                # Parse score for display
                parts = line.split()
                for i, part in enumerate(parts):
                    if part == 'cp' and i + 1 < len(parts):
                        try:
                            score = int(parts[i + 1])
                        except:
                            pass
        
        engine.terminate()
        engine.wait(timeout=2)
        
        return jsonify({
            'move': bestmove,
            'score': score,
            'fen': fen,
            'depth': depth
        })
        
    except subprocess.TimeoutExpired:
        engine.kill()
        return jsonify({'error': 'Engine timeout'}), 500
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/analyze', methods=['POST'])
def analyze():
    """Get multiple moves with scores (for analysis)"""
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
        
        commands = [
            'uci\n',
            'isready\n',
            f'position fen {fen}\n',
            f'go depth {depth} movetime 1000\n'
        ]
        
        for cmd in commands:
            engine.stdin.write(cmd)
            engine.stdin.flush()
        
        # Collect info lines
        moves = []
        for line in engine.stdout:
            if line.startswith('info'):
                # Parse multi-line info
                pass
            elif line.startswith('bestmove'):
                parts = line.split()
                bestmove = parts[1] if len(parts) > 1 else None
                engine.terminate()
                return jsonify({
                    'bestmove': bestmove,
                    'moves': moves
                })
        
        engine.terminate()
        return jsonify({'moves': moves})
        
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