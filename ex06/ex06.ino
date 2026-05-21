// 定义两路LED引脚
#define LED_A 18
#define LED_B 19

int val = 0;         // 基础亮度值
int dir = 1;         // 变化方向

void setup()
{
  pinMode(LED_A,OUTPUT);
  pinMode(LED_B,OUTPUT);
}

void loop()
{
  // A灯递增 0→255，B灯递减255→0
  analogWrite(LED_A, val);
  analogWrite(LED_B, 255 - val);

  // 亮度渐变
  val += dir;
  
  // 到达边界反转方向
  if(val >= 255 || val <= 0)
  {
    dir = -dir;
  }

  delay(8); // 控制渐变速度，数值越小切换越快
}