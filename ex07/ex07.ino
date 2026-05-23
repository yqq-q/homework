/*
 * ESP32 Web网页端无极调光器
 * 合并PWM呼吸灯与Web服务器功能，实现网页滑动条实时调节LED亮度
 */

#include <WiFi.h>
#include <WebServer.h>

// -------------------------- 请修改此处的WiFi信息 --------------------------
const char* ssid = "那咋了";       // 替换为你的WiFi名称
const char* password = "y17795758225";   // 替换为你的WiFi密码
// -----------------------------------------------------------------------

// LED与PWM配置（沿用你提供的呼吸灯配置）
const int ledPin = 2;                  // 板载LED引脚（GPIO2）
const int freq = 5000;                // PWM频率 5000Hz
const int resolution = 8;             // PWM分辨率 8位，对应取值范围0-255

// Web服务器对象
WebServer server(80);

// 当前亮度值，用于页面初始化显示
int currentBrightness = 127;          // 默认初始亮度为中间值

/**
 * 生成Web页面HTML，包含滑动条与交互JS
 */
String makePage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Web无极调光器</title>
  <style>
    html {
      font-family: Arial, sans-serif;
      text-align: center;
      margin: 0 auto;
      padding: 20px;
      max-width: 400px;
      background-color: #f5f5f5;
    }
    h1 {
      color: #333;
      margin-bottom: 30px;
      font-size: 1.8rem;
    }
    .slider-container {
      margin: 40px 0;
      background: white;
      padding: 30px 20px;
      border-radius: 12px;
      box-shadow: 0 2px 10px rgba(0,0,0,0.1);
    }
    .value-display {
      font-size: 2.5rem;
      font-weight: bold;
      color: #2196F3;
      margin: 0 0 30px 0;
    }
    .slider {
      -webkit-appearance: none;
      width: 100%;
      height: 16px;
      border-radius: 8px;
      background: #e0e0e0;
      outline: none;
    }
    .slider::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 36px;
      height: 36px;
      border-radius: 50%;
      background: #2196F3;
      cursor: pointer;
      transition: background 0.2s;
    }
    .slider::-webkit-slider-thumb:hover {
      background: #1976D2;
    }
    .slider::-moz-range-thumb {
      width: 36px;
      height: 36px;
      border-radius: 50%;
      background: #2196F3;
      cursor: pointer;
      border: none;
      box-shadow: 0 2px 5px rgba(0,0,0,0.2);
    }
    .tip {
      color: #666;
      font-size: 0.9rem;
      margin-top: 20px;
    }
  </style>
</head>
<body>
  <h1>ESP32 无极调光器</h1>
  <div class="slider-container">
    <div class="value-display">亮度: <span id="brightnessValue">)rawliteral" + String(currentBrightness) + R"rawliteral(</span></div>
    
    <!-- 滑动条控件，min=0, max=255 对应PWM的8位分辨率 -->
    <input 
      type="range" 
      min="0" 
      max="255" 
      value=")rawliteral" + String(currentBrightness) + R"rawliteral(" 
      step="1" 
      class="slider" 
      id="brightnessSlider"
    >
    <p class="tip">拖动滑动条即可实时调节LED亮度，支持手机/电脑浏览器访问</p >
  </div>

  <script>
    const slider = document.getElementById('brightnessSlider');
    const valueDisplay = document.getElementById('brightnessValue');

    // 监听滑动条的实时输入事件（拖动过程中持续触发，实现平滑实时控制）
    slider.addEventListener('input', function() {
      const value = this.value;
      // 更新页面上的亮度数值显示
      valueDisplay.textContent = value;
      
      // 异步发送GET请求到ESP32，不刷新页面
      fetch('/set?value=' + value)
        .catch(err => {
          // 简单的错误处理，不影响用户操作
          console.log('请求发送失败:', err);
        });
    });
  </script>
</body>
</html>
)rawliteral";
  return html;
}

/**
 * 处理根路径请求，返回Web页面
 */
void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

/**
 * 处理亮度设置请求，接收滑动条传来的数值并更新PWM
 */
void handleSet() {
  if (server.hasArg("value")) {
    // 获取URL中的value参数，转换为整数
    int value = server.arg("value").toInt();
    
    // 限制取值范围，防止非法输入
    if (value >= 0 && value <= 255) {
      currentBrightness = value;
      // 更新PWM占空比，调节LED亮度（新版ledcWrite API，直接通过引脚号写入）
      ledcWrite(ledPin, value);
      Serial.printf("已更新亮度值: %d\n", value);
    }
  }
  // 返回成功响应
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  Serial.println("正在初始化系统...");

  // 初始化PWM，绑定引脚、频率与分辨率（新版API，自动分配通道）
  ledcAttach(ledPin, freq, resolution);
  // 设置初始亮度
  ledcWrite(ledPin, currentBrightness);

  // 连接WiFi
  WiFi.begin(ssid, password);
  Serial.print("正在连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi连接成功!");
  Serial.print("请在浏览器中访问: http://");
  Serial.println(WiFi.localIP());

  // 注册Web路由
  server.on("/", handleRoot);        // 根页面
  server.on("/set", handleSet);      // 亮度设置接口

  // 启动Web服务器
  server.begin();
  Serial.println("Web服务器已启动，等待客户端连接...");
}

void loop() {
  // 处理Web客户端请求
  server.handleClient();
}