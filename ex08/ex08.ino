#include <WiFi.h>
#include <WebServer.h>

// ==================== 配置信息（修改为你的WiFi） ====================
const char* ssid = "那咋了";
const char* password = "y17795758225";

// ==================== 硬件定义 ====================
#define LED_PIN 2          // 板载LED引脚
#define TOUCH_PIN T0       // 触摸引脚 (GPIO4)
#define TOUCH_THRESHOLD 40 // 触摸触发阈值（根据实际调整）

// ==================== 全局变量（系统状态） ====================
bool isArmed = false;    // 布防状态：false=未布防，true=已布防
bool isAlarming = false; // 报警状态：false=正常，true=报警中

// Web服务器对象，端口80
WebServer server(80);

// ==================== 网页HTML代码 ====================
String getHTML() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><meta charset='UTF-8'><title>ESP32 安防报警器</title>";
  // 网页样式美化
  html += "<style>";
  html += "body{text-align:center; font-family:Arial; margin-top:50px;}";
  html += ".btn{width:200px; height:80px; font-size:25px; margin:20px; border:none; border-radius:10px; cursor:pointer;}";
  html += ".arm{background-color:red; color:white;}";
  html += ".disarm{background-color:green; color:white;}";
  html += ".status{font-size:30px; margin:30px;}";
  html += "</style></head>";
  
  html += "<body><h1>ESP32 物联网安防报警器</h1>";
  // 显示当前系统状态
  html += "<div class='status'>系统状态：";
  if(isAlarming){
    html += "<span style='color:red;'>⚠️ 报警中！</span>";
  }else if(isArmed){
    html += "<span style='color:orange;'>✅ 已布防</span>";
  }else{
    html += "<span style='color:green;'>🛡️ 已撤防</span>";
  }
  html += "</div>";
  
  // 布防/撤防按钮（添加了onclick点击事件）
  html += "<button class='btn arm' onclick='location.href=\"/arm\"'>布防 (Arm)</button>";
  html += "<button class='btn disarm' onclick='location.href=\"/disarm\"'>撤防 (Disarm)</button>";
  html += "</body></html>";
  return html;
}

// ==================== 服务器路由处理 ====================
// 根目录：显示网页
void handleRoot() {
  server.send(200, "text/html", getHTML());
}

// 布防路由
void handleArm() {
  isArmed = true;
  isAlarming = false; // 布防时重置报警状态
  server.send(200, "text/html", getHTML());
}

// 撤防路由
void handleDisarm() {
  isArmed = false;
  isAlarming = false; // 撤防时停止报警
  digitalWrite(LED_PIN, LOW); // 熄灭LED
  server.send(200, "text/html", getHTML());
}

// ==================== 触摸检测函数 ====================
void checkTouchSensor() {
  // 仅在【已布防且未报警】时检测触摸
  if(isArmed && !isAlarming) {
    int touchValue = touchRead(TOUCH_PIN);
    // 触摸值低于阈值 → 触发报警
    if(touchValue < TOUCH_THRESHOLD) {
      isAlarming = true;
      Serial.println("⚠️ 触摸触发！启动报警！");
    }
  }
}

// ==================== 报警控制函数 ====================
void alarmControl() {
  if(isAlarming) {
    // 高频闪烁：50ms亮，50ms灭（狂闪）
    digitalWrite(LED_PIN, HIGH);
    delay(50);
    digitalWrite(LED_PIN, LOW);
    delay(50);
  }
}

// ==================== 初始化 ====================
void setup() {
  Serial.begin(115200);
  
  // 初始化LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // 初始化WiFi
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接成功！");
  Serial.print("IP地址：");
  Serial.println(WiFi.localIP()); // 串口打印IP，用于网页访问
  
  // 注册服务器路由
  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  
  // 启动服务器
  server.begin();
  Serial.println("Web服务器已启动！");
}

// ==================== 主循环 ====================
void loop() {
  server.handleClient(); // 处理网页请求
  checkTouchSensor();   // 检测触摸信号
  alarmControl();       // 控制报警LED
}