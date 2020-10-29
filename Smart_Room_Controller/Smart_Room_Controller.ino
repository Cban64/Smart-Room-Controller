/*Project MidTerm
 * Description Theme: Room for Eldercare
 * Author Constance
 * Date: 10/26/2020
 * 
 */

#include <wemo.h>
#include <mac.h>
#include <Adafruit_BME280.h> 
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
#include <Ethernet.h>
#include <hue.h> //needs to be after ethernet
#include <OneButton.h>
#include <Encoder.h>

//OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET   4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//BME
Adafruit_BME280 bme;
float varTempC;
float varTempF;
int bmeStatus;

//Photo Diode
const int lavaLamp = 0; //used in place of circadian lamp
const int photoPin = A3;
int photoVal;
 

//buttons
bool buttonStateBlue;
bool buttonStateRed;
const int buttonPinBlue = 5;
const int buttonPinRed = 6;
OneButton buttonBlue (buttonPinBlue, false,false);// create object
OneButton buttonRed (buttonPinRed, false,false);// create object
const int coffeePot = 2;
const int whiteFan = 1;

//Encoder
const int pinR = 16;
const int pinG = 15;
const int pinSw = 14;
const int pinA = 23;
const int pinB= 22;
int encoderVal;
OneButton buttonSw (pinSw, false,false);
Encoder myEnc(pinB,pinA);
bool encoderButtonState;
const int hueLampOne = 1;
const int hueLampTwo = 2;
const int hueLampThree = 3;
const int hueLampFour = 4;
bool Hueon;
int HueBright = 250;

EthernetClient client;
bool ethernetStatus;

void setup() {
  // Intialize:diode,encoder,onebutton
  //BME,and OLED
  Serial.begin(9600);
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH); 
  
  //Start ethernet connection
  ethernetStatus = Ethernet.begin(mac);
  if (!ethernetStatus) {
    Serial.println("failed to configure Ethernet using DHCP");
    //no point in continueing 
    while(1);
  }
  //print your local IP address
  Serial.print("My IP address:");
  for (byte thisbyte = 0; thisbyte < 4; thisbyte++) {
    //print value of each byte of the IP address
    Serial.print(Ethernet.localIP()[thisbyte], DEC);
    if (thisbyte < 3) Serial.print(".");
  }
 

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds
  
  // Clear the buffer
  display.clearDisplay();  
  
  bmeStatus = bme.begin(0x76);
  if(bmeStatus==false){
    Serial.printf("failed to open BME"); 
  }
  pinMode(photoPin, INPUT);
  
  pinMode(buttonPinBlue,INPUT);
  pinMode(buttonPinRed,INPUT);
  pinMode(pinSw,INPUT);
  buttonBlue.attachLongPressStart(clickBlue);
  buttonRed.attachLongPressStart(clickRed);
  buttonSw.attachLongPressStart(clickHueYellow);
  buttonSw.attachLongPressStart(clickHueRed);
  buttonSw.attachClick(clickSw);
} //end setup

void loop() {
  buttonBlue.tick();  
  buttonRed.tick();
  buttonSw.tick();
  //Automatic fnx: diode/encoder
  //For lighting control
 
  //print temperature values to OLED
  showTemp();    // Draw 'stylized' characters
  
  readPhoto ();
  readEncoder();
  if (encoderButtonState==true){
      setHue (hueLampOne,true,HueYellow,HueBright);
      setHue (hueLampTwo,true,HueYellow,HueBright);
      setHue (hueLampThree,true,HueYellow,HueBright);
      setHue (hueLampFour,true,HueYellow,HueBright); 
    }
    else{
      if (encoderButtonState==true){
      setHue (hueLampOne,true,HueRed,HueBright);
      setHue (hueLampTwo,true,HueRed,HueBright);
      setHue (hueLampThree,true,HueRed,HueBright);
      setHue (hueLampFour,true,HueRed,HueBright); 
      } 
      else{
      setHue (hueLampOne,false,0,0);
      setHue (hueLampTwo,false,0,0);
      setHue (hueLampThree,false,0,0);
      setHue (hueLampFour,false,0,0);       
    }
}
void readEncoder(){
  HueBright = myEnc.read();
  //Serial.printf("encoderVal = %i \n",encoderVal);
   if(HueBright<0){  //dont allow to go below 0
    HueBright=0;  //not bigger than 95
  }
  if(HueBright>255){
    HueBright=255;
  }
}
void readPhoto (){
  //Over 15 circadian/lava lamp = off
  //Between 0-15 circadian/lava lamp = on
  photoVal = analogRead(photoPin);
 //Serial.printf("photoVal = %i \n",photoVal);
 if(photoVal<20) {
  switchON(lavaLamp);
 }
  else{
  switchOFF(lavaLamp);
 }
 
   
}

void showTemp(){    // Draw 'stylized' characters
  //Temp: sensor/display
  //Read value from sensors
  varTempC = bme.readTemperature();
  
 //convert Celsius to F 
  varTempF = ((varTempC*9)/5)+32;
  display.clearDisplay();    
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(3,0);             // Start at top-left corner      
  display.printf("Temp \n\n"); 
  display.printf("%0.2f \n",varTempF);  
  //Serial.printf("Temp = %0.2f \n",varTempF);   
  display.display();
  
}

  //Manual fnx: wemos/2buttons
  //comfort items (coffee/fan for demo)
  void clickBlue(){   
    buttonStateBlue=!buttonStateBlue;
    //Serial.printf ("click blueval = %i\n",buttonStateBlue); 
   if (buttonStateBlue==true){
    switchON(whiteFan);
   }
   else{
    switchOFF (whiteFan);
   }
  }
  void clickRed(){    
    buttonStateRed=!buttonStateRed;
   //Serial.printf ("click redval = %i\n",buttonStateRed); 
    if (buttonStateRed==true){
      switchON (coffeePot);
    }
    else{
      switchOFF (coffeePot);
    }
  }
  void clickSw(){
    encoderButtonState=!encoderButtonState;
    //Serial.printf ("click Swval = %i\n",encoderButtonState);
    
    
  }
  
  
