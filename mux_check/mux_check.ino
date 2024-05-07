/**
 * TCA9548 I2CScanner.pde -- I2C bus scanner for Arduino
 *
 * Based on code c. 2009, Tod E. Kurt, http://todbot.com/blog/
 *
 */

#include "Wire.h"
extern "C" { 
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}

#define TCAADDR1 0x70
#define TCAADDR2 0x71
#define TCAADDR3 0x72

void tcaselect(uint8_t i2c_bus) {
  if (i2c_bus > 15) {
    Wire.beginTransmission(TCAADDR3);
    i2c_bus -= 16;
  }
  else if (i2c_bus > 7) {
    Wire.beginTransmission(TCAADDR2);
    i2c_bus -= 8;
  }
  else if (i2c_bus >= 0){
    Wire.beginTransmission(TCAADDR1);
  }
  else
    return;

  Wire.write(1 << i2c_bus);
  Wire.endTransmission();   
}


// standard Arduino setup()
void setup()
{
    while (!Serial);
    delay(1000);

    Wire.begin();
    
    Serial.begin(9600);
    Serial.println("\nTCAScanner ready!");
    
    for (uint8_t t=0; t<24; t++) {
      tcaselect(t);
      Serial.print("TCA Port #"); Serial.println(t);

      for (uint8_t addr = 0; addr<=127; addr++) {
        if (addr == TCAADDR1 || addr == TCAADDR2 || addr == TCAADDR3) continue;
      
        uint8_t data;
        if (! twi_writeTo(addr, &data, 0, 1, 1)) {
          if(addr == 90)
           Serial.print("Found I2C 0x");  Serial.println(addr,HEX);
        }
      }
    }
    Serial.println("\ndone");
}

void loop() 
{
}