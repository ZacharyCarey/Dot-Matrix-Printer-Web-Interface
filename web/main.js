var websocket;

function init() {
    websocket = new WebSocket("ws://" + window.location.host + ":81");
    document.getElementById("ws_state").textContent = "CONNECTING";

    websocket.onopen = function(){ document.getElementById("ws_state").textContent = "CONNECTED" };
    websocket.onclose = function(){ document.getElementById("ws_state").textContent = "CLOSED" };
    websocket.onerror = function(){ alert("websocket error " + this.url) };

    websocket.onmessage = ws_onmessage;
}

function ws_onmessage(e_msg) {
    e_msg = e_msg || window.event;
    alert("msg : " + e_msg.data);
}

function fetchTemperature() {
    fetch("/temperature")
        .then(response => {
            if (response.ok) {
                return response.text();
            }
            throw new Error("Failed to fetch data.");
        })
        .catch((error) => {
            return "error";
        })
        .then(data => {
            document.getElementById("temperature").textContent = data;
        });
}

var state = false;
var btn = document.getElementById("button");
btn.onclick = function() {
    // alert("button was clicked");
    state = !state;
    if (state) {
        btn.className = "button";
        btn.textContent = "ON";
        if (websocket.readyState == 1) {
            websocket.send("led_on");
        }
    } else {
        btn.className = "button button2";
        btn.textContent = "OFF";
        if (websocket.readyState == 1) {
            websocket.send("led_off");
        }
    }
};

const textinput = document.getElementById('inputtext');
const charCountLabel = document.getElementById('charCountLabel');
function sendText() {
    // Get the value of the textarea
    const text = textinput.value;
    const normalized = text.normalize("NFD");
    const asciiOnly = normalized.replace(/[^\x00-\x7F]/g, "");

    // Print it to the console
    //console.log(text);
    websocket.send(asciiOnly);
}
textinput.addEventListener('input', function() {
    const charCount = textinput.value.length;
    charCountLabel.textContent = `Characters: ${charCount}`;
});

fetchTemperature();
setInterval(fetchTemperature, 4000); // Update temp every 4 seconds
window.onload = init;