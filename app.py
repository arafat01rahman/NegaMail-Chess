"""
NegaMail-Chess API Server
Hosted on Render.com - Free 24/7 Chess Engine API
Author: ARAFAT
"""

from flask import Flask, request, jsonify, send_from_directory
from flask_cors import CORS
import subprocess
import os
import time
import select
import signal
import sys

app = Flask(__name__, static_folder='static')
CORS(app)

# Absolute path to engine binary (runs from repo root)
ENGINE_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'engine')
ENGINE_TOTAL_TIMEOUT = 10.0   # seconds – increased for free tier

# ----------------------------------------------------------------------
# Helper: read a line from a stream with a timeout
# ----------------------------------------------------------------------
def read_line_with_timeout(stream, deadline):
    """
    Read one line from `stream` using select() to avoid blocking.
    Raises TimeoutError if deadline is reached, or RuntimeError if EOF.
    """
    while time.time() < deadline:
        rlist, _, _ = select.select([stream], [], [], 0.1)
        if rlist:
            line = stream.readline()
            if not line:          # EOF – process died
                raise RuntimeError("Engine process exited unexpectedly (EOF)")
            return line
    raise TimeoutError("Timed out waiting for engine output")

# ----------------------------------------------------------------------
# Helper: send a command and wait for a specific prefix
# ----------------------------------------------------------------------
def send_and_wait(engine, cmd, expected_prefix, deadline):
    """
    Write `cmd` to engine.stdin, then read lines until one starts with
    `expected_prefix`.  Raises on timeout or EOF.
    """
    engine.stdin.write(cmd + '\n')
    engine.stdin.flush()
    while time.time() < deadline:
        line = read_line_with_timeout(engine.stdout, deadline)
        if line.startswith(expected_prefix):
            return line
    raise TimeoutError(f"Engine did not respond with '{expected_prefix}'")

# ----------------------------------------------------------------------
# Main move endpoint
# ----------------------------------------------------------------------
@app.route('/move', methods=['POST'])
def get_move():
    data = request.json
    fen = data.get('fen', 'start')

    engine = None
    try:
        # Start engine with stderr merged into stdout to avoid deadlock
        engine = subprocess.Popen(
            [ENGINE_PATH],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,   # <-- KEY FIX
            text=True,
            bufsize=1
        )

        deadline = time.time() + ENGINE_TOTAL_TIMEOUT

        app.logger.info(f"Starting UCI handshake for fen={fen}")

        # ---- UCI handshake ----
        send_and_wait(engine, 'uci', 'uciok', deadline)
        app.logger.info("Received uciok")
        send_and_wait(engine, 'isready', 'readyok', deadline)
        app.logger.info("Received readyok")

        # ---- Position ----
        engine.stdin.write(f'position fen {fen}\n')
        engine.stdin.flush()

        # ---- Search with time control (3 seconds max) ----
        engine.stdin.write(f'go movetime 3000\n')
        engine.stdin.flush()
        app.logger.info("Search started (movetime 3000ms)")

        # ---- Read bestmove ----
        bestmove = None
        score = 0
        while time.time() < deadline:
            line = read_line_with_timeout(engine.stdout, deadline)
            if line.startswith('bestmove'):
                parts = line.split()
                if len(parts) > 1:
                    bestmove = parts[1]
                app.logger.info(f"Received bestmove: {bestmove}")
                break
            elif line.startswith('info score cp'):
                parts = line.split()
                for i, p in enumerate(parts):
                    if p == 'cp' and i+1 < len(parts):
                        try:
                            score = int(parts[i+1])
                        except ValueError:
                            pass

        if bestmove is None:
            raise TimeoutError("No 'bestmove' received from engine")

        return jsonify({
            'move': bestmove,
            'score': score,
            'fen': fen,
            'depth': 0
        })

    except (TimeoutError, RuntimeError) as e:
        app.logger.error(f"Engine error: {e}")
        # If we have any leftover output, log it
        if engine and engine.stdout:
            try:
                leftover = engine.stdout.read()
                if leftover:
                    app.logger.error(f"Engine leftover output: {leftover}")
            except Exception:
                pass
        return jsonify({'error': str(e)}), 500

    except Exception as e:
        app.logger.exception("Unexpected error in /move")
        return jsonify({'error': str(e)}), 500

    finally:
        if engine:
            engine.terminate()
            try:
                engine.wait(timeout=2)
            except subprocess.TimeoutExpired:
                engine.kill()
                engine.wait()

# ----------------------------------------------------------------------
# Analyze endpoint (optional) – same pattern
# ----------------------------------------------------------------------
@app.route('/analyze', methods=['POST'])
def analyze():
    data = request.json
    fen = data.get('fen', 'start')

    engine = None
    try:
        engine = subprocess.Popen(
            [ENGINE_PATH],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            bufsize=1
        )

        deadline = time.time() + ENGINE_TOTAL_TIMEOUT
        send_and_wait(engine, 'uci', 'uciok', deadline)
        send_and_wait(engine, 'isready', 'readyok', deadline)

        engine.stdin.write(f'position fen {fen}\n')
        engine.stdin.flush()
        engine.stdin.write(f'go movetime 2000\n')
        engine.stdin.flush()

        bestmove = None
        while time.time() < deadline:
            line = read_line_with_timeout(engine.stdout, deadline)
            if line.startswith('bestmove'):
                parts = line.split()
                if len(parts) > 1:
                    bestmove = parts[1]
                break

        return jsonify({'bestmove': bestmove})

    except (TimeoutError, RuntimeError) as e:
        app.logger.error(f"Analyze error: {e}")
        return jsonify({'error': str(e)}), 500

    except Exception as e:
        app.logger.exception("Unexpected error in /analyze")
        return jsonify({'error': str(e)}), 500

    finally:
        if engine:
            engine.terminate()
            try:
                engine.wait(timeout=2)
            except subprocess.TimeoutExpired:
                engine.kill()
                engine.wait()

# ----------------------------------------------------------------------
# Health and error handlers
# ----------------------------------------------------------------------
@app.route('/health')
def health():
    return jsonify({'status': 'ok', 'engine': 'NegaMail-Chess'})

@app.errorhandler(404)
def not_found(e):
    return jsonify({'error': 'Not found'}), 404

# ----------------------------------------------------------------------
# HOME ROUTE – serves index.html from static/ folder
# ----------------------------------------------------------------------
@app.route('/')
def home():
    return app.send_static_file('index.html')

if __name__ == '__main__':
    port = int(os.environ.get('PORT', 5000))
    app.run(host='0.0.0.0', port=port, debug=False)