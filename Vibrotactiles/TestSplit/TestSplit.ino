// 3*3 array일 때 원 모양 패턴 실행 - 각도 기준
#include <Wire.h>
#include <drv2605.h>
#include <math.h>

#define TCAADDR1 0x70
#define TCAADDR2 0x71
#define SPLIT_NUM 60
#define SPLIT_NUM2 10

DRV2605 haptic;
int duration = 65;  // duration <= 69.55
float theta, x, y;
float d1, d2, d3;
int d1IdxTemp, d2IdxTemp, d3IdxTemp;
int d1Idx[SPLIT_NUM/2], d2Idx[SPLIT_NUM/2], d3Idx[SPLIT_NUM/2];
int d1Idx2[SPLIT_NUM/2], d2Idx2[SPLIT_NUM/2], d3Idx2[SPLIT_NUM/2];
float intensity1[SPLIT_NUM/2], intensity2[SPLIT_NUM/2], intensity3[SPLIT_NUM/2];
float intensity12[SPLIT_NUM/2], intensity22[SPLIT_NUM/2], intensity32[SPLIT_NUM/2];

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

  for(int i=0; i<=SPLIT_NUM/2; i++) {
    theta = 2*i*M_PI/SPLIT_NUM - M_PI/2;
    x = float(10*cos(theta));
    y = float(10*sin(theta));

    calcDistance(x, y, d1IdxTemp, d2IdxTemp, d3IdxTemp, d1, d2, d3);
    d1Idx[i] = d1IdxTemp; intensity1[i] = calcIntensity(d1, d2, d3);
    d2Idx[i] = d2IdxTemp; intensity2[i] = calcIntensity(d2, d1, d3);
    d3Idx[i] = d3IdxTemp; intensity3[i] = calcIntensity(d3, d1, d2);
  }

  for(int i=0; i<=SPLIT_NUM/2; i++) {
    theta = 2*i*M_PI/SPLIT_NUM;
    x = float(10*cos(theta));
    y = float(10*sin(theta));

    calcDistance(x, y, d1IdxTemp, d2IdxTemp, d3IdxTemp, d1, d2, d3);
    d1Idx2[i] = d1IdxTemp; intensity12[i] = calcIntensity(d1, d2, d3);
    d2Idx2[i] = d2IdxTemp; intensity22[i] = calcIntensity(d2, d1, d3);
    d3Idx2[i] = d3IdxTemp; intensity32[i] = calcIntensity(d3, d1, d2);
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
      d[i*3+j] = sqrt((x-10*(j-1))*(x-10*(j-1)) + (y+10*(i-1))*(y+10*(i-1)));
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

int moveLeft(int a) {
  if (a==3 || a==6 || a==11)
    return -1;
  else
    return a+1;
}

int moveRight(int a) {
  if (a==1 || a==4 || a==9)
    return -1;
  else
    return a-1;
}

int moveDown(int a) {
  if (a < 4)
    return a+3;
  else if (a < 7)
    return a+5;
  else
    return -1;
}

int moveUp(int a) {
  if (a < 4)
    return -1;
  else if (a < 7)
    return a-3;
  else
    return a-5;
}

int rotateLeft(int a) {
  if (a == 1) 
    return 9;
  else if (a == 2)
    return 4;
  else if (a == 3)
    return 1;
  else if (a == 4)
    return 10;
  else if (a == 5)
    return 5;
  else if (a == 6)
    return 2;
  else if (a == 9)
    return 11;
  else if (a == 10)
    return 6;
  else if (a == 11)
    return 3;
}

int rotateRight(int a) {
  if (a == 1)
    return 3;
  else if (a == 2)
    return 6;
  else if (a == 3)
    return 11;
  else if (a == 4)
    return 2;
  else if (a == 5)
    return 5;
  else if (a == 6)
    return 10;
  else if (a == 9)
    return 1;
  else if (a == 10)
    return 4;
  else if (a == 11)
    return 9;
}


void loop() {
  switch (Serial.read()) {

    case '9':
      for(int i=0; i<=SPLIT_NUM/2; i++) {
        beginVibrate(d1Idx[i], intensity1[i]);
        beginVibrate(d2Idx[i], intensity2[i]);
        beginVibrate(d3Idx[i], intensity3[i]);
        delay(duration);
        endVibrate(d1Idx[i]);
        endVibrate(d2Idx[i]);
        endVibrate(d3Idx[i]);
        delay(47.3-0.68*duration);
      }
      for(int i=0; i<=SPLIT_NUM/2; i++) {
        beginVibrate(d1Idx2[i], intensity12[i]);
        beginVibrate(d2Idx2[i], intensity22[i]);
        beginVibrate(d3Idx2[i], intensity32[i]);
        delay(duration);
        endVibrate(d1Idx2[i]);
        endVibrate(d2Idx2[i]);
        endVibrate(d3Idx2[i]);
        delay(47.3-0.68*duration);
      }
      break;

    case 'a':
      for(int i=SPLIT_NUM; i>=0; i--) {
        beginVibrate(d1Idx[i], intensity1[i]);
        beginVibrate(d2Idx[i], intensity2[i]);
        beginVibrate(d3Idx[i], intensity3[i]);
        delay(duration);
        endVibrate(d1Idx[i]);
        endVibrate(d2Idx[i]);
        endVibrate(d3Idx[i]);
        delay(47.3-0.68*duration);
      }
      break;


  }

}