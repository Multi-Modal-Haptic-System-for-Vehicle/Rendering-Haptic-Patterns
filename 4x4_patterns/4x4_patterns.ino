#include <Wire.h>
#include <drv2605.h>
#include <math.h>

#define TCAADDR1 0x70
#define TCAADDR2 0x71
#define TCAADDR3 0x72
#define SPLIT_NUM 30

DRV2605 haptic;
int duration = 30;        // duration <= 69.55
float x, y;
float d1, d2, d3;         // distance 1, 2, 3
int d1Idx, d2Idx, d3Idx;  // actuator index
float intensity1, intensity2, intensity3; // intensity of actuator 1, 2, 3

void tcaselect(uint8_t i2c_bus) {
  if (i2c_bus > 15) {
    Wire.beginTransmission(TCAADDR3);
    i2c_bus -= 16;
  }
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
  if (i2c_bus > 15) {
    Wire.beginTransmission(TCAADDR3);
    i2c_bus -= 16;
  }
  else if (i2c_bus > 7) {
    Wire.beginTransmission(TCAADDR2);
    i2c_bus -= 8;
  }
  else if (i2c_bus >= 0)
    Wire.beginTransmission(TCAADDR1);
  Wire.write(0);
  Wire.endTransmission();
}

// 연결된 actuator 검사
void hapticInit(){
  for (int i = 0; i < 24; i++) {
    tcaselect(i);   // Loop through each connected displays on the I2C buses  
    if (haptic.init(false, true) != 0) Serial.println("init failed!");
    if (haptic.drv2605_AutoCal() != 0) Serial.println("auto calibration failed!");
    Serial.println("-------------------------------");

    delay(300);
  }
}

// num의 index 가진 actuator, intensity의 강도로 활성화
void beginVibrate(int num, unsigned char intensity) {
  tcaselect(num);
  haptic.drv2605_RTP(intensity);
  tcaunselect(num);
}

// num의 index 가진 actuator, 비활성화
void endVibrate(int num) {
  tcaselect(num);
  haptic.drv2605_Play_Waveform(0);
  tcaunselect(num);
}

// 배열의 Idx -> 실제 액추에이터 Idx로 변환
int arrIdxToActuatorIdx(int arrIdx) {
  switch (arrIdx) {
    case 0:
      return 1;
    case 1:
      return 2;
    case 2:
      return 18;
    case 3:
      return 3;
    case 4:
      return 4;
    case 5:
      return 5;
    case 6:
      return 19;
    case 7:
      return 6;
    case 8:
      return 20;
    case 9:
      return 21;
    case 10:
      return 22;
    case 11:
      return 23;
    case 12:
      return 9;
    case 13:
      return 10;
    case 14:
      return 24;
    case 15:
      return 11;
  }

   
}

// (x,y)에서 모든 actuator와의 거리 계산하여, 가장 가까운 3개의 actuator index와 거리 계산
float calcDistance(float x, float y, int& d1Idx, int& d2Idx, int& d3Idx, float& d1, float& d2, float& d3) {
  float d[16];
  float min1=1000, min2=1000, min3=1000;
  int min1Idx, min2Idx, min3Idx;
  for(int i=0; i<4; i++) {
    for(int j=0; j<4; j++) {
      d[i*4+j] = sqrt((x-100*(j-1))*(x-100*(j-1)) + (y+100*(i-3))*(y+100*(i-3)));
    }
  }

  // minimum 3 distances. min1 is lowest
  for (int i = 0; i < 16; i++) {
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

  d1Idx = arrIdxToActuatorIdx(min1Idx); 
  d2Idx = arrIdxToActuatorIdx(min2Idx); 
  d3Idx = arrIdxToActuatorIdx(min3Idx); 
  d1 = (min1 < 71) ? min1 : -1;
  d2 = (min2 < 71) ? min2 : -1;
  d3 = (min3 < 71) ? min3 : -1;

  // Check minimum 3 values
  Serial.print(d1); Serial.print("("); Serial.print(d1Idx); Serial.print("), ");
  Serial.print(d2); Serial.print("("); Serial.print(d2Idx); Serial.print("), ");
  Serial.print(d3); Serial.print("("); Serial.print(d3Idx); Serial.println(")  ");
}

float calcIntensity(float d1, float d2, float d3) {
  if (d1 < 0)
    return 0;
  else if (d1 == 0)
    return 127;
  else
    return 127*sqrt((1/d1)/(1/d1+1/d2+1/d3));
}

void setup() {
  Serial.begin(9600);
  hapticInit(); // Initialize the motors 
  Serial.println("All drv connected");
}

void loop() {
  switch (Serial.read()) {
    case 'L':
      for(int i=0; i<=SPLIT_NUM; i++) {
        x = -100+200*i/SPLIT_NUM;
        y = 0;
        calcDistance(x, y, d1Idx, d2Idx, d3Idx, d1, d2, d3);
        beginVibrate(d1Idx, calcIntensity(d1, d2, d3));
        beginVibrate(d2Idx, calcIntensity(d2, d1, d3));
        beginVibrate(d3Idx, calcIntensity(d3, d1, d2));
        delay(duration);
        endVibrate(d1Idx);
        endVibrate(d2Idx);
        endVibrate(d3Idx);
        delay(47.3-0.68*duration);
      }
      Serial.println("------------------------------");
      break;

    case 'R':
      for(int i=0; i<=SPLIT_NUM; i++) {
        x = 100-200*i/SPLIT_NUM;
        y = 0;
        calcDistance(x, y, d1Idx, d2Idx, d3Idx, d1, d2, d3);
        beginVibrate(d1Idx, calcIntensity(d1, d2, d3));
        beginVibrate(d2Idx, calcIntensity(d2, d1, d3));
        beginVibrate(d3Idx, calcIntensity(d3, d1, d2));
        delay(duration);
        endVibrate(d1Idx);
        endVibrate(d2Idx);
        endVibrate(d3Idx);
        delay(47.3-0.68*duration);
      }
      Serial.println("------------------------------");
      break;

    case 'F':
      for(int i=0; i<=SPLIT_NUM; i++) {
        x = 0;
        y = -100+200*i/SPLIT_NUM;
        calcDistance(x, y, d1Idx, d2Idx, d3Idx, d1, d2, d3);
        beginVibrate(d1Idx, calcIntensity(d1, d2, d3));
        beginVibrate(d2Idx, calcIntensity(d2, d1, d3));
        beginVibrate(d3Idx, calcIntensity(d3, d1, d2));
        delay(duration);
        endVibrate(d1Idx);
        endVibrate(d2Idx);
        endVibrate(d3Idx);
        delay(47.3-0.68*duration);
      }
      Serial.println("------------------------------");
      break;

    case 'B':
      for(int i=0; i<=SPLIT_NUM; i++) {
        x = 0;
        y = 100-200*i/SPLIT_NUM;
        calcDistance(x, y, d1Idx, d2Idx, d3Idx, d1, d2, d3);
        beginVibrate(d1Idx, calcIntensity(d1, d2, d3));
        beginVibrate(d2Idx, calcIntensity(d2, d1, d3));
        beginVibrate(d3Idx, calcIntensity(d3, d1, d2));
        delay(duration);
        endVibrate(d1Idx);
        endVibrate(d2Idx);
        endVibrate(d3Idx);
        delay(47.3-0.68*duration);
      }
      Serial.println("------------------------------");
      break;

    case 'U':
      for(int i=SPLIT_NUM; i>=0; i--) {
        int theta = 2*i*M_PI/SPLIT_NUM - M_PI/2;
        x = float(100*cos(theta));
        y = float(100*sin(theta));
        calcDistance(x, y, d1Idx, d2Idx, d3Idx, d1, d2, d3);
        beginVibrate(d1Idx, calcIntensity(d1, d2, d3));
        beginVibrate(d2Idx, calcIntensity(d2, d1, d3));
        beginVibrate(d3Idx, calcIntensity(d3, d1, d2));
        delay(duration);
        endVibrate(d1Idx);
        endVibrate(d2Idx);
        endVibrate(d3Idx);
        delay(47.3-0.68*duration);
      }
      Serial.println("------------------------------");
      break;
    
    case 'r':
      for(int i=0; i<=SPLIT_NUM; i++) {
        y = -100+200*i/SPLIT_NUM;
        x = float(-0.005*y*y-y+50);
        calcDistance(x, y, d1Idx, d2Idx, d3Idx, d1, d2, d3);
        beginVibrate(d1Idx, calcIntensity(d1, d2, d3));
        beginVibrate(d2Idx, calcIntensity(d2, d1, d3));
        beginVibrate(d3Idx, calcIntensity(d3, d1, d2));
        delay(duration);
        endVibrate(d1Idx);
        endVibrate(d2Idx);
        endVibrate(d3Idx);
        delay(47.3-0.68*duration);
      }
      Serial.println("------------------------------");
      break;

    case 'l':
      for(int i=0; i<=SPLIT_NUM; i++) {
        y = -100+200*i/SPLIT_NUM;
        x = float(0.005*y*y+y-50);
        calcDistance(x, y, d1Idx, d2Idx, d3Idx, d1, d2, d3);
        beginVibrate(d1Idx, calcIntensity(d1, d2, d3));
        beginVibrate(d2Idx, calcIntensity(d2, d1, d3));
        beginVibrate(d3Idx, calcIntensity(d3, d1, d2));
        delay(duration);
        endVibrate(d1Idx);
        endVibrate(d2Idx);
        endVibrate(d3Idx);
        delay(47.3-0.68*duration);
      }
      Serial.println("------------------------------");
      break;
  }
}




