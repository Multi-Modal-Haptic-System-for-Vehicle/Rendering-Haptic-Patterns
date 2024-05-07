// 1번과 2번 2개의 진동모터 사이 몇개의 자극으로 나눌지 입력받은 후 직선 패턴 실행
#include <Wire.h>
#include <drv2605.h>
#define TCAADDR 0x70

DRV2605 haptic;
int duration = 50;

void tcaselect(uint8_t i2c_bus) {
  if (i2c_bus > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i2c_bus);
  Wire.endTransmission();
}

void setup() {
  Serial.begin(9600);
  hapticInit(); // Initialize the motors 
  Serial.println("All drv connected");
}

// Initialize the displays 
void hapticInit(){
  for (int i = 0; i < 8; i++) {
    tcaselect(i);   // Loop through each connected displays on the I2C buses
    if (haptic.init(false, true) != 0) Serial.println("init failed!");
    if (haptic.drv2605_AutoCal() != 0) Serial.println("auto calibration failed!");
    Serial.println("-------------------");

    delay(500);
  }
}

void beginVibrate(int num, unsigned char intensity) {
  tcaselect(num);
  haptic.drv2605_RTP(intensity);
}

void endVibrate(int num) {
  tcaselect(num);
  haptic.drv2605_Play_Waveform(0);
}

void movingTactile(int a1, int a2, int a3, int b1, int b2, int b3, int splitNum) {
  for(int i=0; i<splitNum; i++) {
      beginVibrate(a1, 127 - 127*i/splitNum);
      beginVibrate(a2, 127 - 127*i/splitNum);
      beginVibrate(a3, 127 - 127*i/splitNum);
      beginVibrate(b1, 127*i/splitNum);
      beginVibrate(b2, 127*i/splitNum);
      beginVibrate(b3, 127*i/splitNum);
      delay(duration);
      endVibrate(a1); endVibrate(a2); endVibrate(a3);
      endVibrate(b1); endVibrate(b2); endVibrate(b3);
      delay(47.3-0.68*duration);
  }
}

void loop() {
  int inputNum = Serial.parseInt();
  if(Serial.available()) {
    Serial.println(inputNum);

    movingTactile(1, 4, -1, 2, 5, -1, inputNum);
    movingTactile(2, 5, -1, 3, 6, -1, inputNum);
  }

}