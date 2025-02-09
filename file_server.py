# this is for the raspbery pi 
from flask import Flask, jsonify, send_from_directory
from flask_cors import CORS
import os

app = Flask(__name__)
CORS(app)

DIRECTORY = "/home/pi/Desktop/shared"

@app.route('/files', methods=['GET'])
def list_files():
    try:
        files = os.listdir(DIRECTORY)
        return jsonify({"files": files})
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/files/<filename>', methods=['GET'])
def get_file(filename):
    try:
        return send_from_directory(DIRECTORY, filename)
    except Exception as e:
        return jsonify({"error": "File not found"}), 404

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
