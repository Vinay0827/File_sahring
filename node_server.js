//for mobile api 
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
