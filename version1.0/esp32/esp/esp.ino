#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "along";
const char* password = "thanhlong123";

WebServer server(80);

const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Bảng Điều Khiển Xe 4WD</title>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
  <style>
    body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; text-align: center; background-color: #1a1a1a; color: #fff; margin: 0; padding: 20px; }
    h2 { color: #00ffcc; text-shadow: 0 0 10px #00ffcc; }
    .d-pad { display: grid; grid-template-columns: repeat(3, 80px); grid-gap: 15px; justify-content: center; margin-top: 40px; }
    .btn { width: 80px; height: 80px; font-size: 24px; font-weight: bold; color: white; background: #333; border: 2px solid #555; border-radius: 15px; cursor: pointer; box-shadow: 0 5px 15px rgba(0,0,0,0.5); transition: 0.1s; display: flex; align-items: center; justify-content: center; }
    .btn:active { transform: scale(0.9); background: #555; }
    
    /* Màu sắc đặc trưng cho từng nút */
    #btn-up { background: #4CAF50; border-color: #4CAF50; }
    #btn-down { background: #f44336; border-color: #f44336; }
    #btn-left, #btn-right { background: #2196F3; border-color: #2196F3; }
    #btn-stop { background: #ff9800; border-color: #ff9800; border-radius: 50%; grid-column: 2; grid-row: 2; }
    
    /* Vị trí trên lưới (Grid) */
    .empty { background: transparent; border: none; box-shadow: none; pointer-events: none; }
    #btn-up { grid-column: 2; grid-row: 1; }
    #btn-left { grid-column: 1; grid-row: 2; }
    #btn-right { grid-column: 3; grid-row: 2; }
    #btn-down { grid-column: 2; grid-row: 3; }
  </style>
</head>
<body>
  <h2>HỆ THỐNG ĐIỀU KHIỂN ROBOT</h2>
  <div class="d-pad">
    <div class="empty"></div>
    <!-- Nút nhấn giữ: Khi nhấn thì chạy (onmousedown), thả tay ra thì dừng (onmouseup) -->
    <button id="btn-up" class="btn" onmousedown="sendCommand('F')" onmouseup="sendCommand('S')" ontouchstart="sendCommand('F')" ontouchend="sendCommand('S')">▲</button>
    <div class="empty"></div>
    <button id="btn-left" class="btn" onmousedown="sendCommand('L')" onmouseup="sendCommand('S')" ontouchstart="sendCommand('L')" ontouchend="sendCommand('S')">◀</button>
    <button id="btn-stop" class="btn" onclick="sendCommand('S')">■</button>
    <button id="btn-right" class="btn" onmousedown="sendCommand('R')" onmouseup="sendCommand('S')" ontouchstart="sendCommand('R')" ontouchend="sendCommand('S')">▶</button>
    <div class="empty"></div>
    <button id="btn-down" class="btn" onmousedown="sendCommand('B')" onmouseup="sendCommand('S')" ontouchstart="sendCommand('B')" ontouchend="sendCommand('S')">▼</button>
    <div class="empty"></div>
  </div>

  <script>
    function sendCommand(cmd) {
      fetch('/cmd?val=' + cmd);
    }
  </script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleCommand() {
  if (server.hasArg("val")) {
    String cmd = server.arg("val");
    Serial1.print(cmd); // Gửi lệnh sang Arduino
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

void setup() {
  Serial1.begin(9600, SERIAL_8N1, 20, 21); // Giao tiếp UART với Arduino
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/cmd", handleCommand);
  server.begin();
}

void loop() {
  server.handleClient();
}