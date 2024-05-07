// Left, Right, Forward, Backward, U-turn 입력받은 후 2*3 array에서 해당 패턴 "Moving Tactile Stroke" 방식으로 실행
#include <Wire.h>
#include <drv2605.h>
#define TCAADDR1 0x70
#define TCAADDR2 0x71
#define SPLIT_NUM 5

DRV2605 haptic;
int duration = 60; // duration < 69.55

void tcaselect(uint8_t i2c_bus) {
  if (i2c_bus > 16) return;
  else if (i2c_bus > 7) {
    Wire.beginTransmission(TCAADDR2);
    i2c_bus -= 8;
  }
  else if (i2c_bus >= 0)
    Wire.beginTransmission(TCAADDR1);
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
  for (int i = 0; i < 16; i++) {
    tcaselect(i);   // Loop through each connected displays on the I2C buses  
    if (haptic.init(false, true) != 0) Serial.println("init failed!");
    if (haptic.drv2605_AutoCal() != 0) Serial.println("auto calibration failed!");
    Serial.println("----------------------------");
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

void iterateTactile(int a1, int a2, int a3, int b1, int b2, int b3, int iterNum) {
  for(int i=0; i<iterNum; i++) {
      beginVibrate(a1, 127);
      beginVibrate(a2, 127);
      beginVibrate(a3, 127);
      beginVibrate(b1, 127);
      beginVibrate(b2, 127);
      beginVibrate(b3, 127);
      delay(duration);
      endVibrate(a1); endVibrate(a2); endVibrate(a3);
      endVibrate(b1); endVibrate(b2); endVibrate(b3);
      delay(100);
  }
}

void loop() {

  switch (Serial.read()) {
    case 'L':
      Serial.println("Turn Left");

      movingTactile(1, 4, -1, 2, 5, -1, SPLIT_NUM);
      movingTactile(2, 5, -1, 3, 6, -1, SPLIT_NUM);
      break;

    case 'R':
      Serial.println("Turn Right");

      movingTactile(3, 6, -1, 2, 5, -1, SPLIT_NUM);
      movingTactile(2, 5, -1, 1, 4, -1, SPLIT_NUM);
      break;

    case 'F':
      Serial.println("Go Forward");

      movingTactile(4, 5, 6, 1, 2, 3, 20);
      //iterateTactile(1, 2, 3, -1, -1, -1, 3);
      break;

    case 'B':
      Serial.println("Go Backward");

      movingTactile(1, 2, 3, 4, 5, 6, 20);
      //iterateTactile(4, 5, 6, -1, -1, -1, 3);
      break;      
    
    case 'U':
      Serial.println("U trun");

      // 4 2 6
      // movingTactile(4, -1, -1, 2, -1, -1);
      // movingTactile(2, -1, -1, 6, -1, -1);
      
      // 1 2 3 6 5 4
      // movingTactile(1, -1, -1, 2, -1, -1, 5);
      // movingTactile(2, -1, -1, 3, -1, -1, 5);
      // movingTactile(3, -1, -1, 6, -1, -1, 5);
      // movingTactile(6, -1, -1, 5, -1, -1, 5);
      // movingTactile(5, -1, -1, 4, -1, -1, 5);
      
      // 6 3 2 1 4
      movingTactile(6, -1, -1, 3, -1, -1, 5);
      movingTactile(3, -1, -1, 2, -1, -1, 5);
      movingTactile(2, -1, -1, 1, -1, -1, 5);
      movingTactile(1, -1, -1, 4, -1, -1, 5);
      break;
    
    case 'S':
      Serial.println("STOP");
      iterateTactile(1, 2, 3, 4, 5, 6, 10);
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