# File Transfer API for Raspberry Pi and Mobile App

This project provides a **Node.js API** for the mobile application and a **Python script** for the Raspberry Pi to enable file transfer. The Raspberry Pi hosts a specific directory accessible over Wi-Fi, allowing file access via a mobile app.

## 1. Node.js API for the Mobile App
This API fetches files from the Raspberry Pi when the mobile app is connected.

### Install Dependencies
Run the following commands on your server:
```bash
npm init -y
npm install express cors
```

### API Code (`server.js`)
```javascript
const express = require("express");
const cors = require("cors");
const fs = require("fs");
const path = require("path");

const app = express();
const PORT = 3000;
const DIRECTORY = "/home/pi/Desktop/shared"; // Change to your desired directory

app.use(cors());

// List files in the directory
app.get("/files", (req, res) => {
    fs.readdir(DIRECTORY, (err, files) => {
        if (err) {
            return res.status(500).json({ error: "Unable to access directory" });
        }
        res.json({ files });
    });
});

// Serve files
app.get("/files/:filename", (req, res) => {
    const filePath = path.join(DIRECTORY, req.params.filename);
    if (fs.existsSync(filePath)) {
        res.sendFile(filePath);
    } else {
        res.status(404).json({ error: "File not found" });
    }
});

app.listen(PORT, () => {
    console.log(`Server running at http://<RaspberryPi_IP>:${PORT}`);
});
```

### Run the API
```bash
node server.js
```

## 2. Raspberry Pi Script (Python)
This script makes a directory (`/home/pi/Desktop/shared`) accessible over the mobile app.

### Install Flask
```bash
pip3 install flask flask-cors
```

### Python Server (`file_server.py`)
```python
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
```

### Run the Python Script
```bash
python3 file_server.py
```

## 3. How to Access Files on Mobile App
- Call `http://<RaspberryPi_IP>:5000/files` to **list available files**.
- Call `http://<RaspberryPi_IP>:5000/files/<filename>` to **download a file**.

This setup ensures **only the specific directory is accessible** while keeping it lightweight and simple. 🚀

