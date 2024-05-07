// 2*3 array에서 <-|-> , ->|<- 두가지 패턴
#include <Wire.h>
#include <drv2605.h>
#define TCAADDR1 0x70
#define TCAADDR2 0x71
#define SPLIT_NUM 10

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
  hapticInit(); // Initialize the motors 
  Serial.println("All drv connected");
}

// Initialize the displays 
void hapticInit(){
  for (int i = 0; i < 16; i++) {
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
  tcaunselect(num);
}

void endVibrate(int num) {
  tcaselect(num);
  haptic.drv2605_Play_Waveform(0);
  tcaunselect(num);
}

void openTactile(int a1, int a2, int a3, int b1, int b2, int b3, int b4, int b5, int b6, int splitNum) {
  for(int i=0; i<splitNum; i++) {
      beginVibrate(a1, 127 - 127*i/splitNum);
      beginVibrate(a2, 127 - 127*i/splitNum);
      beginVibrate(a3, 127 - 127*i/splitNum);
      beginVibrate(b1, 127*i/splitNum);
      beginVibrate(b2, 127*i/splitNum);
      beginVibrate(b3, 127*i/splitNum);
      beginVibrate(b4, 127*i/splitNum);
      beginVibrate(b5, 127*i/splitNum);
      beginVibrate(b6, 127*i/splitNum);
      delay(duration);
      endVibrate(a1); endVibrate(a2); endVibrate(a3);
      endVibrate(b1); endVibrate(b2); endVibrate(b3); endVibrate(b4); endVibrate(b5); endVibrate(b6);
      delay(47.3-0.68*duration);
  }
}

void closeTactile(int a1, int a2, int a3, int a4, int a5, int a6, int b1, int b2, int b3, int splitNum) {
  for(int i=0; i<splitNum; i++) {
      beginVibrate(a1, 127 - 127*i/splitNum);
      beginVibrate(a2, 127 - 127*i/splitNum);
      beginVibrate(a3, 127 - 127*i/splitNum);
      beginVibrate(a4, 127 - 127*i/splitNum);
      beginVibrate(a5, 127 - 127*i/splitNum);
      beginVibrate(a6, 127 - 127*i/splitNum);
      beginVibrate(b1, 127*i/splitNum);
      beginVibrate(b2, 127*i/splitNum);
      beginVibrate(b3, 127*i/splitNum);
      delay(duration);
      endVibrate(a1); endVibrate(a2); endVibrate(a3); endVibrate(a4); endVibrate(a5); endVibrate(a6);
      endVibrate(b1); endVibrate(b2); endVibrate(b3); 
      delay(47.3-0.68*duration);
  }
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

  switch (Serial.read()) {
    case 'O':
      Serial.println("Open");

      openTactile(2, 5, 10, 1, 4, 9, 3, 6, 11, SPLIT_NUM);
      break;

    case 'C':
      Serial.println("Close");

      closeTactile(1, 4, 9, 3, 6, 11, 2, 5, 10, SPLIT_NUM);
      break;

    case 'N':
      Serial.println("Narrow");

      movingTactile(9, 11, -1, 2, -1, -1, SPLIT_NUM);

    case 'W':
      Serial.println("Wide");

      movingTactile(2, -2, -1, 9, 11, -1, SPLIT_NUM);
  }


}