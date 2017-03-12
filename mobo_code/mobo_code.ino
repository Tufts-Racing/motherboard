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

uint8_t IMD_FLT = 0;        //IMD Fault state variable
uint8_t BMS_FLT = 0;        //BMS Fault state variable
uint8_t SEVCON_FLT = 0;     //Sevcon Fault
uint8_t BRAKE_FLT = 0;      //BOTS Fault
uint8_t TSMS_FLT = 0;       //TSMS Open

uint8_t COCKPIT_SW = 0;     //cockpit switch

uint8_t uglow = 0;
uint8_t car_started = 0;

const uint8_t WHEELADDR = 8;

void transmit();
void readIn();
void playRTDS(void);

void setup() {
  Wire.begin(); // join i2c bus as master
  pinMode(START_INPUT_IN, INPUT);
  pinMode(CONT_REQ_OUT, OUTPUT);
  pinMode(DRIVE_IN, INPUT_PULLUP); //active low
  pinMode(REVERSE_IN, INPUT_PULLUP); //active low
  pinMode(IMD_FLT_IN, INPUT);
  pinMode(BMS_FLT_IN, INPUT);
  pinMode(SEVCON_FLT_IN, INPUT);
  pinMode(BOTS_IN, INPUT);
}

void loop() {



  readIn();
  transmit();
  //check to see if car requested to start
  if (digitalRead(START_INPUT_IN) && car_started == 0) //for case when cold start i.e. cockpit switch not tripped
  {
      car_started = 1; //set flag that car has been started
      digitalWrite(CONT_REQ_OUT, HIGH); //set contact request high to close AIRs
      delay(6000); //wait for precharge
      while(DRIVE_IN && REVERSE_IN){ // while no direction input, car is in neutral, wait for drive direction input. NOTE:DRIVE_IN and REVERSE_IN are active low
      //spin until forward or reverese is selected
      }
      digitalWrite(CONT_REQ_OUT, HIGH);
      delay(6000); //wait for precharge
      playRTDS();
  }
  if(!COCKPIT_SW && BRAKE_FLT && car_started == 1){ //cockpit switch tripped by driver after car has been started
    digitalWrite(CONT_REQ_OUT, LOW); //disable AIRS via open drains
    car_started = 0; //revert car to cold start state to reinitiate precharge/RTDS 
  }


  
}


void transmit() {
  Wire.beginTransmission(WHEELADDR); // transmit to device @ address 0x08
  
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
  
  IMD_FLT = digitalRead(IMD_FLT_IN);
  BMS_FLT = digitalRead(BMS_FLT_IN);
  SEVCON_FLT = digitalRead(SEVCON_FLT_IN);
  BRAKE_FLT = digitalRead(BOTS_IN);
  TSMS_FLT = digitalRead(TSMS_IN);
  COCKPIT_SW = digitalRead(COCKPIT_SW_IN);
  dir = getDirection();
  
}

void playRTDS(void){

  digitalWrite(RTDS_OUT, HIGH);
  delay(2000);
  digitalWrite(RTDS_OUT, LOW);

}

int getDirection(){

  uint8_t drive = digitalRead(DRIVE_IN);
  uint8_t reverse = digitalRead(REVERSE_IN);

  if(drive && reverse){
    return 1;
  }
  else if(reverse){
    return 0;
  }
  else{
    return 2;
  }


}

