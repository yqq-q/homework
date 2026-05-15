/ 定义LED引脚
const int ledPin = 18;
// LED状态
bool ledState = LOW;
// 记录上一次切换时间
unsigned long previousMillis = 0;
// 闪烁间隔 1000ms = 1秒
const long interval = 1000;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // 获取当前时间
  unsigned long currentMillis = millis();

  // 到达间隔时间，切换LED
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
  }
}