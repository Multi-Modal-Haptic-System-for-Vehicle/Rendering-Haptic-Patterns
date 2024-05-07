// y에 해당하는 함수 모양 패턴 실행 - x값 기준
#include <Wire.h>
#include <drv2605.h>
#include <math.h>

#define TCAADDR1 0x70
#define TCAADDR2 0x71
#define SPLIT_NUM 30

DRV2605 haptic;
int duration = 50;  // duration <= 69.55
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
    x = 100;
    y = 0;

    Serial.print("("); Serial.print(x); Serial.print(", "); Serial.print(y); Serial.println(")");       

    calcDistance(x, y, d1IdxTemp, d2IdxTemp, d3IdxTemp, d1, d2, d3);
    d1Idx[i] = d1IdxTemp; intensity1[i] = calcIntensity(d1, d2, d3);
    d2Idx[i] = d2IdxTemp; intensity2[i] = calcIntensity(d2, d1, d3);
    d3Idx[i] = d3IdxTemp; intensity3[i] = calcIntensity(d3, d1, d2);

    Serial.print(d1); Serial.print("("); Serial.print(d1Idx[i]); Serial.print("), ");
    Serial.print(d2); Serial.print("("); Serial.print(d2Idx[i]); Serial.print("), ");
    Serial.print(d3); Serial.print("("); Serial.print(d3Idx[i]); Serial.println(")");
  }

}

// Initialize the displays 
void hapticInit(){
  for (int i = 0; i < 16; i++) {
    tcaselect(i);   // Loop through each connected displays on the I2C buses  
    if (haptic.init(false, true) != 0) Serial.println("init failed!");
    if (haptic.drv2605_AutoCal() != 0) Serial.println("auto calibration failed!");
    Serial.println("-------------------");

    delay(300);
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

void beginVibrate3(int num1, int num2, int num3, unsigned char intensity) {
  tcaselect(num1);
  tcaselect(num2);
  tcaselect(num3);

  haptic.drv2605_RTP(intensity);

  tcaunselect(num1);
  tcaunselect(num2);  
  tcaunselect(num3);
}

void endVibrate3(int num1, int num2, int num3) {
  tcaselect(num1);
  tcaselect(num2);
  tcaselect(num3);

  haptic.drv2605_Play_Waveform(0);

  tcaunselect(num1);
  tcaunselect(num2);
  tcaunselect(num3);
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

void loop() {

  switch (Serial.read()) {
    // 6번 actuator'만', 연산 수행 X
    case '1':
      for(int i=0; i<=SPLIT_NUM; i++) {
        beginVibrate(6, 127);
        delay(duration);
        endVibrate(6);
        delay(47.3-0.68*duration);
      }
      Serial.println("------------------------------");
      break;

    // 6번 actuator '위치'에, 미리 연산 수행
    case '2':
      for(int i=0; i<=SPLIT_NUM; i++) {
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

    // 6번 actuator '위치'에, loop 안에서 연산 수행 
    case '3':
      for(int i=0; i<=SPLIT_NUM; i++) {
        x = 100;
        y = 0;
        calcDistance(x, y, d1IdxTemp, d2IdxTemp, d3IdxTemp, d1, d2, d3);
        beginVibrate(d1IdxTemp, calcIntensity(d1, d2, d3));
        beginVibrate(d2IdxTemp, calcIntensity(d2, d1, d3));
        beginVibrate(d3IdxTemp, calcIntensity(d3, d1, d2));
        delay(duration);
        endVibrate(d1IdxTemp);
        endVibrate(d2IdxTemp);
        endVibrate(d3IdxTemp);
        delay(47.3-0.68*duration);
      }
      Serial.println("------------------------------");
      break;

    // 6번 actuator '위치'에, loop안에서 연산 수행
    case '4':
      for(int i=0; i<=SPLIT_NUM; i++) {
        beginVibrate3(d1Idx[i], d2Idx[i], d3Idx[i], 127);
        delay(duration);
        endVibrate3(d1Idx[i], d2Idx[i], d3Idx[i]);
        delay(47.3-0.68*duration);
      }
      Serial.println("------------------------------");
      break;
  }

}









