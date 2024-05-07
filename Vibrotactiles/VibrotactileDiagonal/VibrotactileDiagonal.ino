// Left, Right, Forward, Backward, U-turn 입력받은 후 2*3 array에서 해당 패턴 "Moving Tactile Stroke" 방식으로 실행
#include <Wire.h>
#include <drv2605.h>
#define TCAADDR 0x70
#define SPLIT_NUM 5

DRV2605 haptic;
int duration = 60; // duration < 69.55

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

    delay(200);
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

void movingTactile(int a1, int a2, int b1, int b2, int splitNum) {
  for(int i=0; i<splitNum; i++) {
      beginVibrate(a1, 127 - 127*i/splitNum);
      beginVibrate(a2, 127 - 127*i/splitNum);
      beginVibrate(b1, 127*i/splitNum);
      beginVibrate(b2, 127*i/splitNum);
      delay(duration);
      endVibrate(a1); endVibrate(a2);
      endVibrate(b1); endVibrate(b2);
      delay(47.3-0.68*duration);
  }
}

void loop() {

  switch (Serial.read()) {
    case '4':
      Serial.println("Turn Left");

      movingTactile(1, 4, 2, 5, SPLIT_NUM);
      movingTactile(2, 5, 3, 6, SPLIT_NUM);
      break;

    case '6':
      Serial.println("Turn Right");

      movingTactile(3, 6, 2, 5, SPLIT_NUM);
      movingTactile(2, 5, 1, 4, SPLIT_NUM);
      break;

    case '7':
      Serial.println("Turn Left");
      // 45 23
      // movingTactile(2, 4, 3, 5, SPLIT_NUM);

      // 4 5 2 3
      movingTactile(4, -1, 5, -1, SPLIT_NUM);
      movingTactile(5, -1, 2, -1, SPLIT_NUM);
      movingTactile(2, -1, 3, -1, SPLIT_NUM);

      break;

    case '9':
      Serial.println("Turn Right");
      
      // 56 21
      // movingTactile(4, 6, 1, 3, SPLIT_NUM);

      // 5 6 2 1
      movingTactile(5, -1, 6, -1, SPLIT_NUM);
      movingTactile(6, -1, 2, -1, SPLIT_NUM);
      movingTactile(2, -1, 1, -1, SPLIT_NUM);

      break;
  }


}