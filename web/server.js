const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

app.use(express.static('site'));


const ARDUINO_PORT = 'COM3'; 

const port = new SerialPort({
    path: ARDUINO_PORT,
    baudRate: 9600,
    autoOpen: true
});

const parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));

port.on('open', () => console.log(`Connected to Arduino on ${ARDUINO_PORT}`));
port.on('error', (err) => console.log('Serial Port Error (Is the Arduino plugged in?): ', err.message));


parser.on('data', (data) => {
    const color = data.trim().toUpperCase(); 
    console.log(`Arduino identified: ${color}`);

    wss.clients.forEach((client) => {
        if (client.readyState === WebSocket.OPEN) {
            client.send(color);
        }
    });
});


server.listen(3000, () => {
    console.log('Website is running at: http://localhost:3000');
});