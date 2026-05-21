// 引脚定义
#define TOUCH_PIN 4    // T0触摸引脚(GPIO4)
#define LED_PIN   2    // 板载LED引脚

// 触摸阈值（根据串口监视器调整）
const int touchThreshold = 20;
// 防抖时间（毫秒）
const unsigned long debounceTime = 200;

// 状态变量
bool ledState = false;          // LED当前状态
int lastTouchValue = 0;         // 上一次触摸值
bool lastTouched = false;       // 上一次是否触摸
unsigned long lastDebounceTime = 0;  // 上一次触发时间

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState);  // 初始熄灭
}

void loop() {
  // 读取当前触摸值
  int currentTouchValue = touchRead(TOUCH_PIN);
  // 判断当前是否触摸（值<阈值为触摸）
  bool currentTouched = (currentTouchValue < touchThreshold);

  // 串口输出调试
  Serial.print("触摸值：");
  Serial.print(currentTouchValue);
  Serial.print(" | 是否触摸：");
  Serial.println(currentTouched);

  // 软件防抖：未到防抖时间直接退出
  if (millis() - lastDebounceTime < debounceTime) {
    lastTouchValue = currentTouchValue;
    lastTouched = currentTouched;
    return;
  }

  // 边缘检测：上一次未触摸 → 当前触摸（按下瞬间）
  if (!lastTouched && currentTouched) {
    // 翻转LED状态
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    // 更新防抖时间
    lastDebounceTime = millis();
  }

  // 保存本次状态，供下一次对比
  lastTouchValue = currentTouchValue;
  lastTouched = currentTouched;
}