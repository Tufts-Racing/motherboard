#include <Wire.h>
#include "pins.h"

uint8_t rpm = 99;           //rpm
uint8_t odo = 99;           //miles
uint8_t battery_temp = 128; //Farenheit
uint8_t t_pressure = 90;    //psi
int8_t  dir = 0;            //1 = forward, 0 = neutral, -1 = reverse
uint8_t spd = 10;           //speed in mph
uint8_t LV_SOC = 5;         //Low Volatage State of Charge
uint8_t HV_SOC = 7;         //Low Voltage State of Charge

uint8_t IMD_FLT = 0;        //IMD Fault state variable
uint8_t BMS_FLT = 0;        //BMS Fault state variable
uint8_t SEVCON_FLT = 0;     //Sevcon Fault
uint8_t BRAKE_FLT = 0;      //BOTS Fault
uint8_t TSMS_FLT = 0;       //TSMS Open
uint8_t COCKPIT_SW = 0;     //cockpit switch

boolean car_started = false;

const uint8_t WHEELADDR = 8;
const uint8_t NUM_BYTES_DIRECTION = 1;
const unsigned long Duration_Start_Noise = 1000; // milliseconds that start noise will run for, as well as delay before normal operation commence after nosie has ended

void transmit();
void readIn();
void playRTDS(void);
void setDir();

void setup() {
  Wire.begin(); // join i2c bus as master
  
  pinMode(SOC_IN, INPUT);
  pinMode(START_INPUT_IH, INPUT);
  pinMode(BMS_FLT_IH, INPUT);
  pinMode(SEVCON_FLT_IL, INPUT);
  pinMode(IMD_FLT_IN, INPUT);
  pinMode(COCKPIT_SW_IL, INPUT);
  pinMode(TSMS_IN, INPUT);
  pinMode(CONT_REQ_OUT, OUTPUT);
  pinMode(RTDS_OUT, OUTPUT);
  pinMode(FORWARD_OUT_L, OUTPUT);
  pinMode(REVERSE_OUT_L, OUTPUT);
  
  digitalWrite(FORWARD_OUT_L, HIGH);
  digitalWrite(REVERSE_OUT_L, HIGH);
    
  Serial.begin(9600);
  Serial.println("Setup done");


}
int i = 0;
void loop() {
  
  i++;
  if(i>500){
    Serial.println("----------------------------------------");
    Serial.println("Cockpit_sw");
    Serial.println(digitalRead(COCKPIT_SW_IL));
    Serial.println("BOTS");
    Serial.println(digitalRead(BOTS_IN));
    Serial.println("TSMS");
    Serial.println(digitalRead(TSMS_IN));
    Serial.println("IMD");
    Serial.println(digitalRead(IMD_FLT_IN));
    i=0;
  }
  
  readIn();
  transmit();
  //check to see if car requested to start
  if (car_started == true){
    //Serial.println("Started");
    //Serial.println(" ------- brake, cockpit switch --------");
    //Serial.println(digitalRead(BOTS_IN));
    //Serial.println(digitalRead(COCKPIT_SW_IL));
  }else{
    Serial.println("Waiting for Start Input");
  }
  
  if (digitalRead(START_INPUT_IH) && !car_started) //for case when cold start i.e. cockpit switch not tripped
  {   
      Serial.println("Start Input Received");    
     
      car_started = true;                 //set flag that car has been started
      digitalWrite(CONT_REQ_OUT, HIGH);   //set contact request high to close AIRs
      Serial.println("contact request on");
      delay(6000);                        //wait for precharge
      
      while(dir == 0){ 
        //while no direction input, car is in neutral, wait for drive direction input. NOTE:DRIVE_IN and REVERSE_IN are active low
        //spin until forward or reverese is selected
        Serial.println("Waiting for Drive or Reverse");
        readIn();
        transmit();
        if ( COCKPIT_SW && !BRAKE_FLT && car_started){ //cockpit switch tripped by driver after car has been started
          digitalWrite(CONT_REQ_OUT, LOW);        //disable AIRS via open drains
          Serial.println("contact request off: cockpit switch tripped");
          car_started = false;                    //revert car to cold start state to reinitiate precharge/RTDS 
          break;
        }
        if (BRAKE_FLT){ //BOTS switch tripped by driver after car has been started //TODO:://
          digitalWrite(CONT_REQ_OUT, LOW);        //disable AIRS via open drains
          Serial.println("contact request off: brake over travel switch tripped");
        }
      }
      readIn();
      transmit();
      if (car_started  &&  !BRAKE_FLT){
        playRTDS(Duration_Start_Noise);
      }
  }
  if (COCKPIT_SW && !BRAKE_FLT && car_started){ //cockpit switch tripped by driver after car has been started
    digitalWrite(CONT_REQ_OUT, LOW);        //disable AIRS via open drains
    Serial.println("contact request off: cockpit switch tripped");
    car_started = false;                    //revert car to cold start state to reinitiate precharge/RTDS 
  }
  if (BRAKE_FLT){ //cockpit switch tripped by driver after car has been started
    digitalWrite(CONT_REQ_OUT, LOW);        //disable AIRS via open drains
    Serial.println("contact request off: brake over travel switch tripped");
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
  Wire.write(COCKPIT_SW);
  Wire.endTransmission(true);    
  //Stop transmitting
}

void readIn() {
  //add i2c read from steering wheel for drive direction
  Wire.requestFrom(WHEELADDR, NUM_BYTES_DIRECTION);
  while(Wire.available())
  {
     dir = Wire.read();
  }
  
  IMD_FLT    = !digitalRead(IMD_FLT_IN);
  digitalWrite(52,IMD_FLT);
  BMS_FLT    = !digitalRead(BMS_FLT_IH);
  SEVCON_FLT = digitalRead(SEVCON_FLT_IL);
  BRAKE_FLT  = !digitalRead(BOTS_IN);
  TSMS_FLT   = !digitalRead(TSMS_IN);
  COCKPIT_SW = !digitalRead(COCKPIT_SW_IL);

  setDir();
}

void playRTDS(unsigned long delay_time){
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
      digitalWrite(FORWARD_OUT_L, LOW);
      digitalWrite(REVERSE_OUT_L, HIGH);
      break;
    /*case 0:
      digitalWrite(FORWARD_OUT_L, HIGH);
      digitalWrite(REVERSE_OUT, HIGH);
      break;  */
    case -1:
      digitalWrite(FORWARD_OUT_L, HIGH);
      digitalWrite(REVERSE_OUT_L, LOW);
      break;
    default:  //Safe check incase of fault
      digitalWrite(FORWARD_OUT_L, HIGH);
      digitalWrite(REVERSE_OUT_L, HIGH);
      break;
  }
}

/*
Ideas: Change dir -> direct, make checks on it for = 0, > 1, < 1 for fault? Issue: could create issues for accidental read high on 0.  
         Maybe 2 bytes?  Any bit of HON (high order nibble) = 1 means true?  If both then immediately request a second time?

*/
