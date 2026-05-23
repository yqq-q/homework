#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "那咋了";
const char* password = "y17795758225";
WebServer server(80);

int getTouchValue() {
  return touchRead(T0);
}

String getHTML() {
  String html = "<!DOCTYPE html>";
  html += "<html lang='zh-CN'>";
  html += "<head><meta charset='UTF-8'>";
  html += "<title>实时传感器</title>";
  html += "<style>";
  html += "body{background:#1a1a1a;color:#fff;text-align:center;font-size:24px;}";
  html += ".val{font-size:80px;color:#00bfff;}";
  html += "</style>";
  html += "<script>";
  // 使用最稳定的传统AJAX写法
  html += "function updateSensor(){";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.open('GET', '/data', true);";
  html += "  xhr.onreadystatechange = function(){";
  html += "    if(xhr.readyState == 4 && xhr.status == 200){";
  html += "      document.getElementById('v').innerText = xhr.responseText;";
  html += "    }";
  html += "  };";
  html += "  xhr.send();";
  html += "}";
  // 每100毫秒更新一次
  html += "setInterval(updateSensor, 100);";
  html += "</script>";
  html += "</head>";
  html += "<body>";
  html += "<h1>ESP32 触摸传感器实时仪表盘</h1>";
  html += "<div id='v' class='val'>0</div>";
  html += "<p>手指靠近引脚 → 数值减小 | 手指离开 → 数值恢复</p >";
  html += "</body></html>";
  return html;
}

void handleRoot() {
  server.send(200, "text/html", getHTML());
}

void handleData() {
  int val = getTouchValue();
  server.send(200, "text/plain", String(val));
}

void setup() {
  Serial.begin(115200);
  delay(10);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi 连接成功!");
  Serial.print("访问地址: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();

  // 触摸值打印
  Serial.print("触摸值: ");
  Serial.println(touchRead(T0));
}