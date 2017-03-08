#include <Wire.h>
#include "pins.h"

uint8_t rpm = 99;           //rpm
uint8_t odo = 99;           //miles
uint8_t battery_temp = 128; //Farenheit
uint8_t t_pressure = 90;    //psi
uint8_t dir = 1;            //0 = forward, 1 = neutral, 2 = reverse
uint8_t spd = 10;           //speed in mph
uint8_t LV_SOC = 5;         //Low Volatage State of Charge
uint8_t HV_SOC = 7;         //Low Voltage State of Charge

uint8_t IMD_FLT = 0;
uint8_t BMS_FLT = 0;
uint8_t SEVCON_FLT = 0;
uint8_t BRAKE_FLT = 0;
uint8_t TMSM_FLT = 0;

uint8_t START_INPUT = 0;
uint8_t COCKPIT_SW = 0;

uint8_t uglow = 0;


const uint8_t WHEELADDR = 8;


void setup() {
  Wire.begin(); // join i2c bus as master
}

void loop() {
  
  transmit();
  readIn();


  //
  START_INPUT = digitalRead(START_INPUT_IN);
  if (START_INPUT == HIGH)
  {
      digitalWrite(CONT_REQ_OUT, HIGH);
      
      Wire.requestFrom(WHEELADDR, 2);
      dir = Wire.read();
      uglow = Wire.read();
      
  }
  
  
}

void transmit() {
  Wire.beginTransmission(WHEELADDR); // transmit to device #8
  
  //Wire.write(rpm);
  //Wire.write(odo);
  Wire.write(battery_temp);
  Wire.write(t_pressure);
  Wire.write(dir);
  Wire.write(spd);
  Wire.write(LV_SOC);
  Wire.write(HV_SOC);

  Wire.write(IMD_FLT);
  Wire.write(BMS_FLT);
  Wire.write(SEVCON_FLT);
  Wire.write(BRAKE_FLT);
  
  Wire.endTransmission();    
  //Stop transmitting
}

void readIn() {
  
  uint8_t IMD_FLT = digitalRead(IMD_FLT_IN);
  uint8_t BMS_FLT = digitalRead(BMS_FLT_IN);
  uint8_t SEVCON_FLT = digitalRead(IMD_FLT_IN);
  uint8_t BRAKE_FLT = digitalRead(BOTS_IN);
  uint8_t TMSM_FLT = digitalRead(TSMS_IN);
  
  uint8_t START_INPUT = digitalRead(START_INPUT_IN);
  uint8_t COCKPIT_SW = digitalRead(COCKPIT_SW_IN);
}

