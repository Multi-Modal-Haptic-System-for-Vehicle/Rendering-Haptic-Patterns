// Left, Right, Forward, Backward, U-turn 입력받은 후 2*3 array에서 해당 패턴 "Moving Tactile Stroke" 방식으로 실행
#include <Wire.h>
#include <drv2605.h>
#define TCAADDR 0x70
#define SPLIT_NUM 30

DRV2605 haptic;
int duration = 65; // duration < 69.55

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

void modifiedMovingTactile(int a1, int a2, int b1, int b2, int c1, int c2, int splitNum) {
  for(int i=0; i<splitNum; i++) {
      beginVibrate(a1, 127 - 127*i/splitNum);
      beginVibrate(a2, 127 - 127*i/splitNum);
      beginVibrate(b1, 127);
      beginVibrate(b2, 127);
      beginVibrate(c1, 127*i/splitNum);
      beginVibrate(c2, 127*i/splitNum);
      delay(duration);
      endVibrate(a1); endVibrate(a2); 
      endVibrate(b1); endVibrate(b2); 
      endVibrate(c1); endVibrate(c2);
      delay(47.3-0.68*duration);
  }
}

void iterateTactile(int a1, int a2, int a3, int a4, int a5, int a6, int iterNum) {
  for(int i=0; i<iterNum; i++) {
      beginVibrate(a1, 127);
      beginVibrate(a2, 127);
      beginVibrate(a3, 127);
      beginVibrate(a4, 127);
      beginVibrate(a5, 127);
      beginVibrate(a6, 127);
      delay(duration);
      endVibrate(a1); endVibrate(a2); endVibrate(a3);
      endVibrate(a4); endVibrate(a5); endVibrate(a6);
      delay(100);
  }
}

void loop() {

  switch (Serial.read()) {
    case 'L':
      Serial.println("Turn Left");
      modifiedMovingTactile(1, 4, 2, 5, 3, 6, SPLIT_NUM);
      break;

    case 'R':
      Serial.println("Turn Right");
      modifiedMovingTactile(3, 6, 2, 5, 1, 4, SPLIT_NUM);
      break;

    case '4':
      Serial.println("Getting out of left traffic lane");
      iterateTactile(3, 6, -1, -1, -1, -1, 3);
      break;
    case '6':
      Serial.println("Getting out of right traffic lane");
      iterateTactile(1, 4, -1, -1, -1, -1, 3);
      break;
  }


}