#include <Wire.h>
#include "pins.h"

uint8_t rpm = 99;           //rpm
uint8_t odo = 99;           //miles
uint8_t battery_temp = 128; //Farenheit
uint8_t t_pressure = 90;    //psi
int8_t dir = 0;            //1 = forward, 0 = neutral, -1 = reverse
uint8_t spd = 10;           //speed in mph
uint8_t LV_SOC = 5;         //Low Volatage State of Charge
uint8_t HV_SOC = 7;         //Low Voltage State of Charge

uint8_t IMD_FLT = 0;        //IMD Fault state variable
uint8_t BMS_FLT = 0;        //BMS Fault state variable
uint8_t SEVCON_FLT = 0;     //Sevcon Fault
uint8_t BRAKE_FLT = 0;      //BOTS Fault
uint8_t TSMS_FLT = 0;       //TSMS Open
uint8_t COCKPIT_SW = 0;     //cockpit switch

uint8_t car_started = 0;

const uint8_t WHEELADDR = 8;

void transmit();
void readIn();
void playRTDS(void);
void setDir();

void setup() {
  Wire.begin(); // join i2c bus as master
  pinMode(SOC_IN, INPUT);
  pinMode(WHEEL3_IN, INPUT);
  pinMode(WHEEL2_IN, INPUT);
  pinMode(WHEEL1_IN, INPUT);
  pinMode(WHEEL0_IN, INPUT);
  pinMode(START_INPUT_IN, INPUT);
  pinMode(BMS_FLT_IN, INPUT);
  pinMode(SEVCON_FLT_IN, INPUT);
  pinMode(IMD_FLT_IN, INPUT);
  pinMode(COCKPIT_SW_IN, INPUT);
  pinMode(TSMS_IN, INPUT);

  pinMode(CONT_REQ_OUT, OUTPUT);
  pinMode(RTDS_OUT, OUTPUT);
  pinMode(FORWARD_OUT, OUTPUT);
  pinMode(REVERSE_OUT, OUTPUT);
  
  digitalWrite(FORWARD_OUT, HIGH);
  digitalWrite(REVERSE_OUT, HIGH);
  

 
  /*
  pinMode(START_INPUT_IN, INPUT);
  pinMode(CONT_REQ_OUT, OUTPUT);
  pinMode(DRIVE_IN, INPUT_PULLUP); //active low
  pinMode(REVERSE_IN, INPUT_PULLUP); //active low
  pinMode(IMD_FLT_IN, INPUT);
  pinMode(BMS_FLT_IN, INPUT);
  pinMode(SEVCON_FLT_IN, INPUT);
  pinMode(BOTS_IN, INPUT);

  pinMode(RTDS_OUT, OUTPUT); //initialize RTDS 
  */
  
  Serial.begin(9600);
  Serial.println("Setup done");


}
int i = 0;
void loop() {
  i++;
  if(i>500){
    Serial.println("----------------------------------------");
    Serial.println("Cockpit_sw");
    Serial.println(digitalRead(COCKPIT_SW_IN));
    Serial.println("BOTS");
    Serial.println(digitalRead(BOTS_IN));
    Serial.println("TSMS");
    Serial.println(digitalRead(TSMS_IN));
    i=0;
  }
  readIn();
  transmit();
  //check to see if car requested to start
  if(car_started){
    //Serial.println("Started");
    //Serial.println(" ------- brake, cockpit switch --------");
    //Serial.println(digitalRead(BOTS_IN));
    //Serial.println(digitalRead(COCKPIT_SW_IN));
  }else{
    Serial.println("Waiting for Start Input");
  }
  if (digitalRead(START_INPUT_IN) && car_started == 0) //for case when cold start i.e. cockpit switch not tripped
  {   
      Serial.println("Start Input Received");    
      car_started = 1; //set flag that car has been started
      digitalWrite(CONT_REQ_OUT, HIGH); //set contact request high to close AIRs
      Serial.println("contact request on");
      delay(6000); //wait for precharge
      while(dir == 0){ 
        Serial.println("Waiting for Drive or Reverse");
        readIn();
        transmit();
        // while no direction input, car is in neutral, wait for drive direction input. NOTE:DRIVE_IN and REVERSE_IN are active low
      //spin until forward or reverese is selected
      }
      readIn();
      transmit();
        
      playRTDS(1000);
  }
  if(!COCKPIT_SW && BRAKE_FLT && car_started){ //cockpit switch tripped by driver after car has been started
    digitalWrite(CONT_REQ_OUT, LOW); //disable AIRS via open drains
    Serial.println("contact request off");
    car_started = 0; //revert car to cold start state to reinitiate precharge/RTDS 
  }
  
}


void transmit() {
  Wire.beginTransmission(WHEELADDR); // transmit to device @ address 0x08
  
  //Wire.write(rpm);
  //Wire.write(odo);
  Wire.write(battery_temp);
  Wire.write(t_pressure);
  Wire.write(spd);
  Wire.write(LV_SOC);
  Wire.write(HV_SOC);

  Wire.write(IMD_FLT);
  Wire.write(BMS_FLT);
  Wire.write(SEVCON_FLT);
  Wire.write(BRAKE_FLT);

  Wire.endTransmission(true);    
  //Stop transmitting
}

void readIn() {
  //add i2c read from steering wheel for drive direction
  Wire.requestFrom(WHEELADDR, 1);
  while(Wire.available())
  {
     dir = Wire.read();
  }
  
  IMD_FLT = digitalRead(IMD_FLT_IN);
  BMS_FLT = digitalRead(BMS_FLT_IN);
  SEVCON_FLT = !digitalRead(SEVCON_FLT_IN);
  BRAKE_FLT = digitalRead(BOTS_IN);
  TSMS_FLT = digitalRead(TSMS_IN);
  COCKPIT_SW = digitalRead(COCKPIT_SW_IN);

  setDir();
}

void playRTDS(uint8_t delay_time){
  Serial.println("Playing RTDS DOOT DOOT");
  for(int i = 0; i < 4; i++){
    digitalWrite(RTDS_OUT, HIGH);
    delay(delay_time);
    digitalWrite(RTDS_OUT, LOW);
    delay(delay_time);
  }
  
}
void setDir(){
  switch (dir) {
    case 1:
      digitalWrite(FORWARD_OUT, LOW);
      digitalWrite(REVERSE_OUT, HIGH);
      break;
    case 0:
      digitalWrite(FORWARD_OUT, HIGH);
      digitalWrite(REVERSE_OUT, HIGH);
      break;
    case -1:
      digitalWrite(FORWARD_OUT, HIGH);
      digitalWrite(REVERSE_OUT, LOW);
      break;
  }
}



