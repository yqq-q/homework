const int ledPin = 18;
bool ledState = LOW;
unsigned long previousMillis = 0;

// 定义SOS信号模式：1=短闪，2=长闪，0=停顿
int sosPattern[] = {1,1,1,2,2,2,1,1,1,0};
int patternIndex = 0;
int patternLength = sizeof(sosPattern) / sizeof(sosPattern[0]);

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();
  long interval;

  // 根据模式设置亮灭时间
  if (sosPattern[patternIndex] == 1) {
    interval = 200;   // 短闪
  } else if (sosPattern[patternIndex] == 2) {
    interval = 600;   // 长闪
  } else {
    interval = 2000;  // 结束停顿
  }

  // 时间到，切换状态
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(ledPin, ledState);

    // 播放完一个状态，切换下一个SOS指令
    if (ledState == LOW) {
      patternIndex++;
      if (patternIndex >= patternLength) {
        patternIndex = 0; // 循环播放
      }
    }
  }
}