// Left, Right, Forward, Backward, U-turn 입력받은 후 3*3 array에서 해당 패턴 실행
// 7, 8, 9번 drv는 9, 10, 11로
#include <Wire.h>
#include <drv2605.h>
#include <Servo.h>
#define TCAADDR1 0x70
#define TCAADDR2 0x71
#define SPLIT_NUM 5

DRV2605 haptic;
Servo myservo;
int duration = 60; // duration < 69.55
int angle;

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

void tcaunselect(uint8_t i2c_bus) {
  if (i2c_bus > 7) {
    Wire.beginTransmission(TCAADDR2);
    i2c_bus -= 8;
  }
  else if (i2c_bus >= 0)
    Wire.beginTransmission(TCAADDR1);
  Wire.write(0);
  Wire.endTransmission();
}


void setup() {
  Serial.begin(9600);
  myservo.attach(9);
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
  tcaunselect(num);
}

void endVibrate(int num) {
  tcaselect(num);
  haptic.drv2605_Play_Waveform(0);
  tcaunselect(num);
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

      movingTactile(1, 4, 9, 2, 5, 10, SPLIT_NUM);
      movingTactile(2, 5, 10, 3, 6, 11, SPLIT_NUM);
      break;

    case 'R':
      Serial.println("Turn Right");

      movingTactile(3, 6, 11, 2, 5, 10, SPLIT_NUM);
      movingTactile(2, 5, 10, 1, 4, 9, SPLIT_NUM);
      break;

    case 'F':
      Serial.println("Go Forward");

      movingTactile(9, 10, 11, 4, 5, 6, SPLIT_NUM);
      movingTactile(4, 5, 6, 1, 2, 3, SPLIT_NUM);
      //iterateTactile(1, 2, 3, -1, -1, -1, 3);
      break;

    case 'B':
      Serial.println("Go Backward");

      movingTactile(1, 2, 3, 4, 5, 6, SPLIT_NUM);
      movingTactile(4, 5, 6, 9, 10, 11, SPLIT_NUM);
      //iterateTactile(4, 5, 6, -1, -1, -1, 3);
      break;      
    
    case 'U':
      Serial.println("U trun");
      
      // 11 6 2 4 9
      movingTactile(11, -1, -1, 6, -1, -1, 5);
      movingTactile(6, -1, -1, 2, -1, -1, 5);
      movingTactile(2, -1, -1, 4, -1, -1, 5);
      movingTactile(4, -1, -1, 9, -1, -1, 5);
      break;
    
    case 'S':
      Serial.println("STOP");
      iterateTactile(1, 2, 3, 4, 5, 6, 10);
      break;
    
    case '4':
      Serial.println("Getting out of left traffic lane");
      iterateTactile(3, 6, 11, -1, -1, -1, 3);
      break;
    case '6':
      Serial.println("Getting out of right traffic lane");
      iterateTactile(1, 4, 9, -1, -1, -1, 3);
      break;
    case '/':
      Serial.println("Stop all drv");
      iterateTactile(1, 2, 3, 4, 5, 6, 1);
      iterateTactile(9, 10, 11, -1, -1, -1, 1);
      break;     
  }


}