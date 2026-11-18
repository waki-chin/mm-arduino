# M&M Color Sorter

A real-time web dashboard that counts M&Ms sorted by an Arduino color sensor over USB serial.

## How It Works
- **Arduino:** Identifies each M&M color (`RED`, `YELLOW`, `BLUE`, `GREEN`) and sends it over serial (`9600` baud).
- **Node.js Backend:** Reads the serial port (`express`, `serialport`) and broadcasts updates via WebSockets (`ws`).
- **Frontend:** Vanilla HTML/CSS/JS dashboard displaying live counts at `http://localhost:3000`.

## Running Locally
1. Plug in the Arduino and update `ARDUINO_PORT` in the server file if needed.
2. Install dependencies and start the server:
   ```bash
   npm install
   node server.js
