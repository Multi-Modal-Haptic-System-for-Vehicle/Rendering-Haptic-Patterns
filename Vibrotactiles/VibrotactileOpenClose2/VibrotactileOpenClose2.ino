// Open <-|->, Close ->|<-, Narrow /\, Wide \/, 패턴 실행 
#include <Wire.h>
#include <drv2605.h>
#include <math.h>

#define TCAADDR1 0x70
#define TCAADDR2 0x71
#define SPLIT_NUM 20

DRV2605 haptic;
int duration = 60;  // duration <= 69.55
float x, y;
float d1, d2, d3;
int d1IdxTemp, d2IdxTemp, d3IdxTemp;
int d1Idx[SPLIT_NUM], d2Idx[SPLIT_NUM], d3Idx[SPLIT_NUM];
float intensity1[SPLIT_NUM], intensity2[SPLIT_NUM], intensity3[SPLIT_NUM];

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

  for(int i=0; i<=SPLIT_NUM; i++) {
    x = float(60*i/SPLIT_NUM-30);    
    y = float(2*abs(x)-30);       

    calcDistance(x, y, d1IdxTemp, d2IdxTemp, d3IdxTemp, d1, d2, d3);
    d1Idx[i] = d1IdxTemp; intensity1[i] = calcIntensity(d1, d2, d3);
    d2Idx[i] = d2IdxTemp; intensity2[i] = calcIntensity(d2, d1, d3);
    d3Idx[i] = d3IdxTemp; intensity3[i] = calcIntensity(d3, d1, d2);
  }

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

float calcDistance(float x, float y, int& d1Idx, int& d2Idx, int& d3Idx, float& d1, float& d2, float& d3) {
  float d[9];
  float min1=100, min2=100, min3=100;
  int min1Idx, min2Idx, min3Idx;
  for(int i=0; i<3; i++) {
    for(int j=0; j<3; j++) {
      d[i*3+j] = sqrt((x-30*(j-1))*(x-30*(j-1)) + (y+30*(i-1))*(y+30*(i-1)));
    }
  }

  // minimum 3 distances. min1 is lowest
  for (int i = 0; i < 9; i++) {
    if (d[i] < min1) {
        min3 = min2;
        min3Idx = min2Idx;
        min2 = min1;
        min2Idx = min1Idx;
        min1 = d[i];
        min1Idx = i;
    } else if (d[i] < min2) {
        min3 = min2;
        min3Idx = min2Idx;
        min2 = d[i];
        min2Idx = i;
    } else if (d[i] < min3) {
        min3 = d[i];
        min3Idx = i;
    }
  }
  
  d1Idx = (min1Idx > 5) ? min1Idx+3 : min1Idx+1; 
  d2Idx = (min2Idx > 5) ? min2Idx+3 : min2Idx+1; 
  d3Idx = (min3Idx > 5) ? min3Idx+3 : min3Idx+1; 
  d1 = (min1 < 100) ? min1 : 0;
  d2 = (min2 < 100) ? min2 : 0;
  d3 = (min3 < 100) ? min3 : 0;

  // Check minimum 3 values
  Serial.print(d1); Serial.print("("); Serial.print(d1Idx); Serial.print("), ");
  Serial.print(d2); Serial.print("("); Serial.print(d2Idx); Serial.print("), ");
  Serial.print(d3); Serial.print("("); Serial.print(d3Idx); Serial.println(")");
}

float calcIntensity(float d1, float d2, float d3) {
  if (d1 == 0)
    return 127;
  else
    return 127*sqrt((1/d1)/(1/d1+1/d2+1/d3));
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

void loop() {

  switch (Serial.read()) {

    case '1':
      Serial.println("Open");
      openTactile(2, 5, 10, 1, 4, 9, 3, 6, 11, 10);
      break;

    case '2':
      Serial.println("Close");
      closeTactile(1, 4, 9, 3, 6, 11, 2, 5, 10, 10);
      break;

    case '3':
      Serial.println("Open");
      openTactile(4, 5, 6, 1, 2, 3, 9, 10, 11, 10);
      break;

    case '4':
      Serial.println("Close");
      closeTactile(1, 2, 3, 9, 10, 11, 4, 5, 6, 10);
      break;

    case 'N':
      for(int i=0; i<=SPLIT_NUM/2; i++) {
        beginVibrate(d1Idx[i], intensity1[i]);
        beginVibrate(d2Idx[i], intensity2[i]);
        beginVibrate(d3Idx[i], intensity3[i]);
        beginVibrate(d1Idx[SPLIT_NUM-i], intensity1[SPLIT_NUM-i]);
        beginVibrate(d2Idx[SPLIT_NUM-i], intensity2[SPLIT_NUM-i]);
        beginVibrate(d3Idx[SPLIT_NUM-i], intensity3[SPLIT_NUM-i]);        
        delay(duration);
        endVibrate(d1Idx[i]); 
        endVibrate(d2Idx[i]); 
        endVibrate(d3Idx[i]);
        endVibrate(d1Idx[SPLIT_NUM-i]);
        endVibrate(d2Idx[SPLIT_NUM-i]);
        endVibrate(d3Idx[SPLIT_NUM-i]);
        delay(47.3-0.68*duration);
        Serial.println(i);
      }
      break;

case 'W':
      for(int i=0; i<=SPLIT_NUM/2; i++) {
        beginVibrate(d1Idx[SPLIT_NUM/2 + i], intensity1[SPLIT_NUM/2 + i]);
        beginVibrate(d2Idx[SPLIT_NUM/2 + i], intensity2[SPLIT_NUM/2 + i]);
        beginVibrate(d3Idx[SPLIT_NUM/2 + i], intensity3[SPLIT_NUM/2 + i]);
        beginVibrate(d1Idx[SPLIT_NUM/2 - i], intensity1[SPLIT_NUM/2 - i]);
        beginVibrate(d2Idx[SPLIT_NUM/2 - i], intensity2[SPLIT_NUM/2 - i]);
        beginVibrate(d3Idx[SPLIT_NUM/2 - i], intensity3[SPLIT_NUM/2 - i]);        
        delay(duration);
        endVibrate(d1Idx[SPLIT_NUM/2 + i]); 
        endVibrate(d2Idx[SPLIT_NUM/2 + i]); 
        endVibrate(d3Idx[SPLIT_NUM/2 + i]);
        endVibrate(d1Idx[SPLIT_NUM/2 - i]);
        endVibrate(d2Idx[SPLIT_NUM/2 - i]);
        endVibrate(d3Idx[SPLIT_NUM/2 - i]);
        delay(47.3-0.68*duration);
        Serial.println(i);
      }
      break;
  }

}









