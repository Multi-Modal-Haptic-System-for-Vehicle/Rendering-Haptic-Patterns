#include "Wire.h"
#include "Adafruit_DRV2605.h"

#define TCAADDR 0x70
#define SPLIT_NUM 30      // # of stimuli

int duration = 50;        // duration < 69.55
float x, y;
float d1, d2, d3;         // distance 1, 2, 3
int d1Idx, d2Idx, d3Idx;  // actuator index
float intensity1, intensity2, intensity3; // intensity of actuator 1, 2, 3

Adafruit_DRV2605 drv[7];

void tcaselect(uint8_t i) {
  if (i>7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

void tcaunselect(uint8_t i) {
  if (i>7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(0);
  Wire.endTransmission();
}

void setup() {
  Wire.begin();
  Serial.begin(115200);

  for (int i=1; i<7; i++) {
    tcaselect(i);
    drv[i].begin();
    drv[i].useLRA();
    drv[i].setMode(DRV2605_MODE_AUTOCAL);
  }


  Serial.println("\nTCA Scanner ready!");
  Serial.println("\ndone");
}

void beginVibrate(int num, unsigned char intensity) {
  tcaselect(num);
  drv[num].setMode(DRV2605_MODE_REALTIME);
  drv[num].setRealtimeValue(intensity);
  tcaunselect(num);
}

void endVibrate(int num) {
  tcaselect(num);
  drv[num].setRealtimeValue(0);
  drv[num].setMode(DRV2605_MODE_AUTOCAL);
  tcaunselect(num);
}

int arrIdxToActuatorIdx(int arrIdx) {

    return arrIdx+1;
}

float calcDistance(float x, float y, int& d1Idx, int& d2Idx, int& d3Idx, float& d1, float& d2, float& d3) {
  float d[6];
  float min1=1000, min2=1000, min3=1000;
  int min1Idx, min2Idx, min3Idx;  // 가장 가까운 3점 배열에서의 index
  for(int i=0; i<3; i++) {
    for(int j=0; j<2; j++) {      // (x,y)에서 6개의 점과의 거리 계산, d배열에 저장
      d[i*2+j] = sqrt((x-100*(2*j-1))*(x-100*(2*j-1)) + (y+100*(i-1))*(y+100*(i-1)));
    }
  }

  // minimum 3 distances. min1 is lowest
  for (int i = 0; i < 6; i++) {
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
  d1 = (min1 < 150) ? min1 : -1;
  d2 = (min2 < 150) ? min2 : -1;
  d3 = (min3 < 150) ? min3 : -1;

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

void loop() {
  //Serial.println(Serial.read());

  while(Serial.available())
    switch (Serial.read()) {
      case '1':
        beginVibrate(1, 127);
        delay(duration);
        endVibrate(1);
      break;

      case '2':
        beginVibrate(2, 127);
        delay(duration);
        endVibrate(2);
      break;

      case '3':
        beginVibrate(3, 127);
        delay(duration);
        endVibrate(3);
      break;

      case '4':
        beginVibrate(4, 127);
        delay(duration);
        endVibrate(4);
      break;

      case '5':
        beginVibrate(5, 127);
        delay(duration);
        endVibrate(5);
      break;

      case '6':
        beginVibrate(6, 127);
        delay(duration);
        endVibrate(6);
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
        delay(300);
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
    }


  // while(Serial.available())  
  // {
  //   input = Serial.readStringUntil('\n');

  //   if(input.equals("1"))
  //   {
  //     beginVibrate(1, 127);
  //     delay(1000);
  //     endVibrate(1);
  //   }

  // }
 
}

