//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.0
#include <Wire.h>
#include <LiquidCrystal_AIP31068_I2C.h>

LiquidCrystal_AIP31068_I2C lcd((0x7C >> 1),16,2);  // set the LCD address to 0x3E for a 20 chars and 4 line display
void setup();
void loop();

int main(void){
  setup();
  loop();
}
void setup()
{
  HAL_Init();  // 显式初始化 HAL 库（可选）
  SystemClock_Config();  // 确保时钟配置正确
  // 在 setup() 或类似初始化函数中调用一次
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // 启用 DWT
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;            // 启用周期计数器
  DWT->CYCCNT = 0;   

  Wire.setSCL(PB8);
  Wire.setSDA(PB9);
 
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.setCursor(3,0);
  lcd.print("Hello, world!");
  lcd.setCursor(2,1);
  lcd.print("Ywrobot Arduino!");
   lcd.setCursor(0,2);
  lcd.print("Arduino LCM IIC 2004");
   lcd.setCursor(2,3);
  lcd.print("Power By Ec-yuan!");
}


void loop()
{
}