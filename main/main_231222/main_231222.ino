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
int d1Idx, d2Idx, d3Idx;
float intensity1, intensity2, intensity3;

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

    delay(100);
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
  float min1=1000, min2=1000, min3=1000;
  int min1Idx, min2Idx, min3Idx;
  for(int i=0; i<3; i++) {
    for(int j=0; j<3; j++) {
      d[i*3+j] = sqrt((x-100*(j-1))*(x-100*(j-1)) + (y+100*(i-1))*(y+100*(i-1)));
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
  d1 = (min1 < 141) ? min1 : -1;
  d2 = (min2 < 141) ? min2 : -1;
  d3 = (min3 < 141) ? min3 : -1;

  // Check minimum 3 values
  Serial.print(d1); Serial.print("("); Serial.print(d1Idx); Serial.print("), ");
  Serial.print(d2); Serial.print("("); Serial.print(d2Idx); Serial.print("), ");
  Serial.print(d3); Serial.print("("); Serial.print(d3Idx); Serial.println(")");
}

float calcIntensity(float d1, float d2, float d3) {
  if (d1 < 0)
    return 0;
  else if (d1 == 0)
    return 127;
  else
    return 127*sqrt((1/d1)/(1/d1+1/d2+1/d3));
}

void loop() {

  switch (Serial.read()) {
    case 'L':
      for(int i=0; i<=SPLIT_NUM; i++) {
        x = -100+200*i/SPLIT_NUM;
        y = -100;
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
      break;

    case 'R':
      for(int i=0; i<=SPLIT_NUM; i++) {
        x = 100-200*i/SPLIT_NUM;
        y = -100;
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
      break;

    case 'F':
      for(int i=0; i<=SPLIT_NUM; i++) {
        x = -100;
        y = -100+200*i/SPLIT_NUM;
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
      break;

    case 'B':
      for(int i=0; i<=SPLIT_NUM; i++) {
        x = -100;
        y = 100-200*i/SPLIT_NUM;
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
      break;

    case 'U':
      for(int i=SPLIT_NUM; i>=0; i--) {
        int theta = 2*i*M_PI/SPLIT_NUM - M_PI/2;
        x = float(30*cos(theta));
        y = float(30*sin(theta));
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
      break;

  }
}









