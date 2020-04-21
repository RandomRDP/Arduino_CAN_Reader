#include <mcp2515.h>          //Library for using CAN Communication
#include "FastLED.h"

struct can_frame canMsg; 
MCP2515 HS_CAN(10);                 // SPI CS Pin 10 
MCP2515 MS_CAN(9);                  // SPI CS Pin 9
bool HS_CAN_MSG, MS_CAN_MSG = false;

#define NUM_LEDS 2
#define DATA_PIN 8
#define BRIGHTNESS 15
CRGB status_leds[NUM_LEDS];


void setup() {
  Serial.begin(115200);                //Begins Serial Communication at 38400 baudrate 

  FastLED.addLeds<WS2811, DATA_PIN, GRB>(status_leds, NUM_LEDS);
    
  HS_CAN.reset();                          
  HS_CAN.setBitrate(CAN_500KBPS,MCP_8MHZ); //Sets CAN at speed 500KBPS and Clock 8MHz 
  HS_CAN.setNormalMode();                  //Sets CAN at normal mode

  MS_CAN.reset();                          
  MS_CAN.setBitrate(CAN_125KBPS,MCP_8MHZ); //Sets CAN at speed 125KBPS and Clock 8MHz 
  MS_CAN.setNormalMode();              //Sets CAN at normal mode

  attachInterrupt(digitalPinToInterrupt(2), HS_CAN_ISR,FALLING);
  attachInterrupt(digitalPinToInterrupt(3), MS_CAN_ISR,FALLING);
  Serial.println(" Running "); 

}


void loop() {

  if (HS_CAN_MSG){
    HS_CAN_MSG = false;
    if (HS_CAN.readMessage(&canMsg) == MCP2515::ERROR_OK){ 
      printCanMsg("HS",&canMsg);   
    }
  }
  
  if (MS_CAN_MSG){
    MS_CAN_MSG = false;
    if (MS_CAN.readMessage(&canMsg) == MCP2515::ERROR_OK){ 
      printCanMsg("MS",&canMsg);   
    }
  }
  
}


void HS_CAN_ISR(){
  HS_CAN_MSG = true;
}

void MS_CAN_ISR(){
  MS_CAN_MSG = true;
}

void printCanMsg(char* name, struct can_frame *frame){
  Serial.print("<");
  Serial.print(name);
  Serial.print(";");
  if (frame->can_id < 256) {
    Serial.print("0");  
  }
  Serial.print(frame->can_id, HEX);
  Serial.print(";");
  for(int i=0;i<frame->can_dlc-1;i++) {  
    if (frame->data[i] < 16) {
      Serial.print("0");  
    }
    Serial.print(frame->data[i], HEX);
    Serial.print(",");
  }  
  if (frame->data[frame->can_dlc-1] < 16) {
      Serial.print("0");  
  }               
  Serial.print(frame->data[frame->can_dlc-1], HEX);             
  Serial.println(">");
}
