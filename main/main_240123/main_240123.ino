#include <Wire.h>
#include <drv2605.h>
#include <math.h>
#include <string.h>

#define TCAADDR1 0x70
#define TCAADDR2 0x71
#define SPLIT_NUM 30      // # of stimuli

String input;
DRV2605 haptic;
int mode = 1;             // default : 3x3 patterns
int duration = 50;        // duration < 69.55
float x, y;
float d1, d2, d3;         // distance 1, 2, 3
int d1Idx, d2Idx, d3Idx;  // actuator index
float intensity1, intensity2, intensity3; // intensity of actuator 1, 2, 3

// drv2605 선택 및 활성화
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

// drv2605 비활성화
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

// 연결된 actuator 검사
void hapticInit(){
  for (int i = 0; i < 16; i++) {
    tcaselect(i);   // Loop through each connected displays on the I2C busess  
    if (haptic.init(false, true) != 0) Serial.println("init failed!");
    if (haptic.drv2605_AutoCal() != 0) Serial.println("auto calibration failed!");
    Serial.println("-------------------------------");

    delay(300);
  }
}

// index가 num인 actuator를 intensity의 강도로 활성화
void beginVibrate(int num, unsigned char intensity) {
  tcaselect(num);
  haptic.drv2605_RTP(intensity);
  tcaunselect(num);
}

// index가 num인 actuator를 비활성화
void endVibrate(int num) {
  tcaselect(num);
  haptic.drv2605_Play_Waveform(0);
  tcaunselect(num);
}

// 배열의 Idx -> 실제 actuator Idx로 변환
int arrIdxToActuatorIdx(int arrIdx, int mode) {
  switch (mode) {
    case 0:
      switch (arrIdx) {
        case 0:
          return 1;
        case 1:
          return 3;
        case 2:
          return 9;
        case 3:
          return 11;
        case 4:
          return 5;
      }
    case 1:
      if (arrIdx > 5)
        return arrIdx+3;
      else
        return arrIdx+1;
    
    case 2:
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
}

// (x,y)에서 모든 actuator와의 거리 계산하여, 가장 가까운 3개 actuator의 index 구하고, 거리 계산
float calcDistance(float x, float y, int& d1Idx, int& d2Idx, int& d3Idx, float& d1, float& d2, float& d3, int mode) {
  float d[16] = {0, };
  int size; // capacity
  int distanceMax;
  float min1=1000, min2=1000, min3=1000;
  int min1Idx, min2Idx, min3Idx;  // 가장 가까운 3점 배열에서의 index

  // mode 0, 1, 2 -> 2x2, 3x3, 4x4
  // (x,y)에서 9개의 점과의 거리 계산, d배열에 저장
  switch (mode) {
    case 0:
      size = 5;
      //distanceMax = 300; // 150*2
      for(int i=0; i<3; i+=2)
        for(int j=0; j<3; j+=2)
          d[i+j/2] = sqrt((x-150*(j-1))*(x-150*(j-1)) + (y+150*(i-1))*(y+150*(i-1)));
      d[4] = sqrt(x*x+y*y);
      break;

    case 1:
      size = 9;
      //distanceMax = 212; // 150*sqrt(2)
      for(int i=0; i<3; i++)
        for(int j=0; j<3; j++)
          d[i*3+j] = sqrt((x-150*(j-1))*(x-150*(j-1)) + (y+150*(i-1))*(y+150*(i-1)));
      break;  

    case 2:
      size = 16;
      //distanceMax = 300; // 100*sqrt(2)
      for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
          d[i*4+j] = sqrt((x+150-100*j)*(x+150-100*j) + (y-150+100*i)*(y-150+100*i));
      break;
  }

  // minimum 3 distances. min1 is lowest
  for (int i = 0; i < size; i++) {
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

  d1Idx = arrIdxToActuatorIdx(min1Idx, mode);
  d2Idx = arrIdxToActuatorIdx(min2Idx, mode);
  d3Idx = arrIdxToActuatorIdx(min3Idx, mode);
  d1 = min1; d2 = min2; d3 = min3;

  // Check minimum 3 values
  Serial.print(d1); Serial.print("("); Serial.print(min1Idx); Serial.print("), ");
  Serial.print(d2); Serial.print("("); Serial.print(min2Idx); Serial.print("), ");
  Serial.print(d3); Serial.print("("); Serial.print(min3Idx); Serial.println(")  ");
}

// 거리에 따른 강도 계산. 활성화되는 actuator의 거리는 d1.
float calcIntensity(float d1, float d2, float d3) {
  if (d1 < 0)
    return 0;
  else if (d1 == 0)
    return 127;
  else
    return 127*sqrt((1/d1)/(1/d1+1/d2+1/d3));
}

void tactileStroke(float x, float y) {
  calcDistance(x, y, d1Idx, d2Idx, d3Idx, d1, d2, d3, mode);
  beginVibrate(d1Idx, calcIntensity(d1, d2, d3));
  beginVibrate(d2Idx, calcIntensity(d2, d1, d3));
  beginVibrate(d3Idx, calcIntensity(d3, d1, d2));
  delay(duration);
  endVibrate(d1Idx);
  endVibrate(d2Idx);
  endVibrate(d3Idx);
  delay(47.3-0.68*duration);
}

void setup() {
  Serial.begin(9600);
  hapticInit(); // Initialize the motors 
  Serial.println("All drv connected");
}

void loop() {
  switch (Serial.read()) {
    case '0':
      mode = 0;
      Serial.println("Set mode 2x2");
      break;
    case '1':
      mode = 1;
      Serial.println("Set mode 3x3");
      break;
    case '2':
      mode = 2;
      Serial.println("Set mode 4x4");
      break;

    case 'L':
      for(int i=0; i<=SPLIT_NUM; i++) {
        x = -150+300*i/SPLIT_NUM;
        y = 0;
        tactileStroke(x, y);
      }
      Serial.println("------------------------------");
      break;

    case 'R':
      for(int i=0; i<=SPLIT_NUM; i++) {
        x = 150-300*i/SPLIT_NUM;
        y = 0;
        tactileStroke(x, y);
      }
      Serial.println("------------------------------");
      break;

    case 'F':
      for(int i=0; i<=SPLIT_NUM; i++) {
        x = 0;
        y = -150+300*i/SPLIT_NUM;
        tactileStroke(x, y);
      }
      Serial.println("------------------------------");
      break;

    case 'B':
      for(int i=0; i<=SPLIT_NUM; i++) {
        x = 0;
        y = 150-300*i/SPLIT_NUM;
        tactileStroke(x, y);
      }
      Serial.println("------------------------------");
      break;

    case 'O':
      for(int i=SPLIT_NUM; i>=0; i--) {
        float theta = 2*i*M_PI/SPLIT_NUM - M_PI/2;
        x = float(150*cos(theta));
        y = float(150*sin(theta));
        tactileStroke(x, y);
      }
      Serial.println("------------------------------");
      break;
    
    case 'r':
      for(int i=0; i<=SPLIT_NUM; i++) {
        y = -150+300*i/SPLIT_NUM;
        x = float(1/300*y*y-y-75);
        tactileStroke(x, y);
      }
      Serial.println("------------------------------");
      break;

    case 'l':
      for(int i=0; i<=SPLIT_NUM; i++) {
        y = -150+300*i/SPLIT_NUM;
        x = float(1/300*y*y+y-75);
        tactileStroke(x, y);
      }
      Serial.println("------------------------------");
      break;

    case 'P':
      for(int i=0; i<=SPLIT_NUM; i++) {
        if (i<=SPLIT_NUM/2) {
          x = -100;
          y = -150+300*i/SPLIT_NUM;
          tactileStroke(x, y);
        }
        else {
          float theta = 2*i*M_PI/SPLIT_NUM - M_PI/2;
          x = float(100*cos(theta));
          y = float(100*sin(theta)+50);
          tactileStroke(x, y);
        }
      }
      Serial.println("------------------------------");
      break;

    case 'S':
      for(int i=0; i<=SPLIT_NUM; i++) {
        y= -150+300*i/SPLIT_NUM;
        x = -150*sin(y*M_PI/150);
        tactileStroke(x, y);
      }
      Serial.println("------------------------------");
      break;
      
  }

}




