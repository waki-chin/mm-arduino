const socket = new WebSocket('ws://localhost:3000');

// Track our counts
let counts = {
    RED: 0,
    YELLOW: 0,
    BLUE: 0,
    GREEN: 0
};


socket.onmessage = function(event) {
    const color = event.data;

    if (counts.hasOwnProperty(color)) {
        counts[color]++;

        const elementId = `count-${color.toLowerCase()}`;
        document.getElementById(elementId).innerText = counts[color];
    }
};

socket.onopen = () => console.log("Connected to local server!");