// 3*3 array일 때 원 모양 패턴 실행 - 각도 기준
#include <Wire.h>
#include <drv2605.h>
#include <math.h>

#define TCAADDR1 0x70
#define TCAADDR2 0x71
#define SPLIT_NUM 50

DRV2605 haptic;
int duration = 60;  // duration <= 69.55
float theta, x, y;
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
    theta = 2*i*M_PI/SPLIT_NUM;
    x = float(30*cos(theta));      // 30mm inputNum등분
    y = float(30*sin(theta));         // y=func

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

void iterateTactile(int a1, int a2, int a3, int iterNum) {
  for(int i=0; i<iterNum; i++) {
      beginVibrate(a1, 127);
      beginVibrate(a2, 127);
      beginVibrate(a3, 127);
      delay(duration);
      endVibrate(a1); endVibrate(a2); endVibrate(a3);
      delay(100);
  }
}

float calcDistance(float x, float y, int& d1Idx, int& d2Idx, int& d3Idx, float& d1, float& d2, float& d3) {
  float d[9];
  float min1=30, min2=30, min3=30;
  int min1Idx, min2Idx, min3Idx;
  for(int i=0; i<3; i++) {
    for(int j=0; j<3; j++) {
      d[i*3+j] = sqrt((x-30*(i-1))*(x-30*(i-1)) + (y+30*(j-1))*(y+30*(j-1)));
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

  // if (d2 > 29) {
  //   d2Idx = -1;
  //   d3Idx = -1;
  // }
  // else if (d3 > 29) {
  //   d3Idx = -1;
  // }



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

void loop() {
  int inputNum = Serial.parseInt();
  int inputTheta = map(inputNum, 0, 360, 0, SPLIT_NUM);
  if (Serial.available()) {
    Serial.println(inputTheta);
    for(int i=0; i<=inputTheta; i++) {
      beginVibrate(d1Idx[i], intensity1[i]);
      beginVibrate(d2Idx[i], intensity2[i]);
      beginVibrate(d3Idx[i], intensity3[i]);
      delay(duration);
      endVibrate(d1Idx[i]);
      endVibrate(d2Idx[i]);
      endVibrate(d3Idx[i]);
      delay(47.3-0.68*duration);
      Serial.println(i);
    }
    //iterateTactile(d1Idx[inputTheta], d2Idx[inputTheta], d3Idx[inputTheta], 5);
  }

}









