#include <INA226.h>


INA226 INA(0x40);

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
  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.print("INA226_LIB_VERSION: ");
  Serial.println(INA226_LIB_VERSION);
  Wire.setSCL(PB8);
  Wire.setSDA(PB9);
  Wire.begin();
  if (!INA.begin() )
  {
    Serial.println("could not connect. Fix and Reboot");
  }
  INA.setMaxCurrentShunt(1, 0.002);
}


void loop()
{
  Serial.println("\nBUS\tSHUNT\tCURRENT\tPOWER");
  
  for (int i = 0; i < 20; i++)
  {
    float bv = INA.getBusVoltage();
    float sv = INA.getShuntVoltage_mV();    
    float cmA = INA.getCurrent_mA();    
    float pmW = INA.getPower_mW();

     Serial.print(bv);
    Serial.print("\t");
    Serial.print(sv);
    Serial.print("\t");
    Serial.print(cmA);
    Serial.print("\t");
    Serial.print(pmW);
    Serial.println();
    delay(1000);
  }
  
}
