// 引脚定义
#define TOUCH_PIN  4    // 触摸引脚 T0 (GPIO4)
#define LED_PIN    2    // 板载LED

// 触摸阈值 & 防抖时间
const int touchThreshold = 20;
const unsigned long debounceTime = 200;

// 呼吸灯参数
int brightness = 0;    // LED亮度 0~255
int fadeDirection = 1; // 增减方向：1=变亮，-1=变暗

// 档位控制：1慢 2中 3快
int speedLevel = 1;    // 默认1档
// 每个档位的步长（步长越大，呼吸越快）
const int stepLevel[] = {1, 3, 6}; 

// 触摸状态与防抖
bool lastTouched = false;
unsigned long lastDebounceTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // ====================== 1. 触摸检测 + 档位切换 ======================
  int touchVal = touchRead(TOUCH_PIN);
  bool isTouched = (touchVal < touchThreshold);

  // 防抖
  if (millis() - lastDebounceTime > debounceTime) {
    // 边缘检测：触摸按下瞬间
    if (!lastTouched && isTouched) {
      // 档位循环切换 1→2→3→1
      speedLevel++;
      if (speedLevel > 3) speedLevel = 1;
      
      Serial.print("当前档位：");
      Serial.println(speedLevel);
      
      lastDebounceTime = millis(); // 更新防抖时间
    }
  }
  lastTouched = isTouched; // 保存上一次状态

  // ====================== 2. PWM呼吸灯主逻辑 ======================
  // 根据当前档位获取步长
  int step = stepLevel[speedLevel - 1];
  
  brightness += fadeDirection * step; // 增减亮度
  analogWrite(LED_PIN, brightness);  // 输出PWM

  // 到达最亮/最暗时反转方向
  if (brightness <= 0 || brightness >= 255) {
    fadeDirection = -fadeDirection;
  }

  delay(10); // 基础小延时，保证平滑
}