#include <BfButton.h>
#include <Esp.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);

#define LOGO16_GLCD_HEIGHT 32
#define LOGO16_GLCD_WIDTH  128

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

const int keyNum = 5;

String keyArr[keyNum] = {
  "Hatari On/Speed",
  "Hatari Off",
  "Hatari Time",
  "Hatari Natural",
  "Hatari Swing"
};

uint64_t keyHArr[keyNum] = {
  0x0040BFB04F,
  0x0040BF8877,
  0x0040BF906F,
  0x0040BF807F,
  0x0040BFA05F
};


const uint16_t kIrLed = D0;
IRsend irsend(kIrLed);

int btnPin = D5; //GPIO #3-Push button on encoder
int DT = D4; //GPIO #4-DT on encoder (Output B)
int CLK = D3; //GPIO #5-CLK on encoder (Output A)
BfButton btn(BfButton::STANDALONE_DIGITAL, btnPin, true, LOW);
 
int counter = 0;
int aState;
int aLastState;  
int onHover = 1;//1-3
int hoverCounter = 0;
int remoteState = 0;//0=send 1=add 2=manage 3=selecting mode
 
void pressHandler (BfButton *btn, BfButton::press_pattern_t pattern) {
  switch (pattern) {
    case BfButton::SINGLE_PRESS:
      Serial.println("btnpress");
      irsend.sendNEC(keyHArr[counter/2]);
      break;
      
    case BfButton::DOUBLE_PRESS:
      break;
      
    case BfButton::LONG_PRESS:
      remoteState = 3;
      onHover = 0;
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,6);
      display.print("Mode: ");
      display.setTextColor(BLACK, WHITE);
      display.print("1");                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
      display.setTextColor(WHITE);
      display.println(" 2 3");
      display.println("Sending mode");
      display.display();
      break;
  }
}
void setup() {
  irsend.begin();
  pinMode(3, FUNCTION_3);
  pinMode(1, FUNCTION_3);
  Serial.begin(115200);
  pinMode(CLK,INPUT_PULLUP);
  pinMode(DT,INPUT_PULLUP);
  aLastState = digitalRead(CLK);
  
  //Button settings
  btn.onPress(pressHandler)
  .onDoublePress(pressHandler) // default timeout
  .onPressFor(pressHandler, 1000); // custom timeout for 1 second
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(6,2);
  display.println("Universal IR remote");
  display.setCursor(10,14);
  display.println("By Sriracha school");
  display.setCursor(15,25);
  display.println("Loading data...");
  display.display();
  delay(200);
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,6);
  display.println("Key: " + keyArr[counter/2]);
  display.println("Num: " + String(counter/2+1));
  display.display();
}
 
void loop() {
  btn.read();
  aState = digitalRead(CLK);
  if (aState != aLastState){   
    if(remoteState == 0){
      if (digitalRead(DT) != aState) { 
        counter++;
      }else {
        counter--;
      }
      if (counter >= (keyNum*2) ) {
        counter = 0;
      }
      if (counter < 0 ) {
        counter =(keyNum*2)-1;
      }
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,6);
      display.println("Key: " + keyArr[counter/2]);
      display.println("Num: " + String(counter/2+1));
      display.display();
    }else if(remoteState == 3){
      if (digitalRead(DT) != aState) { 
        onHover++;
      }else {
        onHover--;
      }
      if (onHover > 6) {
        onHover = 0;
      }
      if (onHover < 0 ) {
        onHover = 6;
      }
      if((onHover/2) == 1){
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0,6);
        display.print("Mode: ");
        display.setTextColor(BLACK, WHITE);
        display.print("1");
        display.setTextColor(WHITE);
        display.println(" 2 3");
        display.println("Sending mode");
        display.display();
      }else if((onHover/2) == 2){
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0,6);
        display.print("Mode: 1 ");
        display.setTextColor(BLACK, WHITE);
        display.print("2");
        display.setTextColor(WHITE);
        display.println(" 3");
        display.println("Adding mode");
        display.display();
      }else if((onHover/2) == 3){
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0,6);
        display.print("Mode: 1 2 ");
        display.setTextColor(BLACK, WHITE);
        display.println("3");
        display.setTextColor(WHITE);
        display.println("Management Mode");
        display.display();
      }
    }
  }   
  aLastState = aState;
}
