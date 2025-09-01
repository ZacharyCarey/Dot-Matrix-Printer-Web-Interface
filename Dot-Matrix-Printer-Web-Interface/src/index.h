#ifndef WEBPAGE_H
#define WEBPAGE_H

const char* webpage = R"=====(<!DOCTYPE html><html><head><title>ESP32 Web Interface</title><style>
            .button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer; }
            .button2 { background-color: #555555; }
        </style></head><body><h1>ESP32 Web Interface</h1><p> WebSocket : <span id=ws_state style=color:blue;>null</span><br></p><p> Temperature: <span style="color: red;"><span id=temperature>Loading...</span> &#8451;</span></p><p><button class="button button2" id=button name=button value=enter>OFF</button></p><script>var websocket;function init(){websocket=new WebSocket("ws://"+window.location.host+":81");document.getElementById("ws_state").textContent="CONNECTING";websocket.onopen=function(){document.getElementById("ws_state").textContent="CONNECTED"};websocket.onclose=function(){document.getElementById("ws_state").textContent="CLOSED"};websocket.onerror=function(){alert("websocket error "+this.url)};websocket.onmessage=ws_onmessage;}
function ws_onmessage(e_msg){e_msg=e_msg||window.event;alert("msg : "+e_msg.data);}
function fetchTemperature(){fetch("/temperature").then(response=>{if(response.ok){return response.text();}
throw new Error("Failed to fetch data.");}).catch((error)=>{return"error";}).then(data=>{document.getElementById("temperature").textContent=data;});}
var state=false;var btn=document.getElementById("button");btn.onclick=function(){state=!state;if(state){btn.className="button";btn.textContent="ON";if(websocket.readyState==1){websocket.send("led_on");}}else{btn.className="button button2";btn.textContent="OFF";if(websocket.readyState==1){websocket.send("led_off");}}};fetchTemperature();setInterval(fetchTemperature,4000);window.onload=init;</script></body></html>)=====";

#endif