// Left, Right, Forward, Backward, U-turn 입력받은 후 2*3 array에서 해당 패턴 tactile stroke 방식과 순차 실행 비교
// l r f b u 는 tactile stroke, 4 6 8 2 5는 순차 실행
#include <Wire.h>
#include <drv2605.h>
#define TCAADDR 0x70
#include <Servo.h>
#define SPLIT_NUM 2

DRV2605 haptic;
Servo servoL, servoR;
int duration = 65;

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

void vibrate(int count, ...) {
  va_list args;
  va_start(args, count);
  int temp[count];

  for (int i=0; i<count; i++) {
    temp[i] = va_arg(args, int);
    tcaselect(temp[i]);
    haptic.drv2605Write(1, 52);
  }
  delay(duration);
  for (int i=0; i<count; i++) {
    tcaselect(temp[i]);
    haptic.drv2605Write(1, 0);
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

      movingTactile(4, 5, 6, 1, 2, 3, SPLIT_NUM*2);
      break;

    case 'B':
      Serial.println("Go Backward");

      movingTactile(1, 2, 3, 4, 5, 6, SPLIT_NUM*2);
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
    
    case '4':
      Serial.println("Turn Left");

      vibrate(2, 1, 4);
      delay(47.3-0.68*duration);
      vibrate(2, 2, 5);
      delay(47.3-0.68*duration);
      vibrate(2, 3, 6);
      break;

    case '6':
      Serial.println("Turn Right");

      vibrate(2, 3, 6);
      delay(47.3-0.68*duration);
      vibrate(2, 2, 5);
      delay(47.3-0.68*duration);
      vibrate(2, 1, 4);
      break;

    case '8':
      Serial.println("Go Forward");

      vibrate(3, 4, 5, 6);
      delay(47.3-0.68*duration);
      vibrate(3, 1, 2, 3);
      break;

    case '2':
      Serial.println("Go Backward");

      vibrate(3, 1, 2, 3);
      delay(47.3-0.68*duration);
      vibrate(3, 4, 5, 6);
      break;      
    
    case '5':
      Serial.println("U trun");

      vibrate(3, 6, 3, 2);
      delay(47.3-0.68*duration);
      vibrate(3, 3, 2, 1);
      delay(47.3-0.68*duration);
      vibrate(3, 2, 1, 4);
      break;
  }

}