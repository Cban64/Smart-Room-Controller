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
int status;
const int photoPin = A3;
int photoVal;
 

//buttons
//OneButton button1 (buttonPin, false,false);// create object
bool buttonState;

EthernetClient client;

void setup() {
  // Intialize:diode,encoder,onebutton
  //BME,and OLED

  Serial.begin(9600);

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
  
  status = bme.begin(0x76);
  if(status==false){
    Serial.printf("failed to open BME"); 
  }
  pinMode(photoPin, INPUT);
} //end setup

void loop() {
  //Automatic fnx: diode/encoder
  //For lighting control
 
  //print temperature values to OLED
  showTemp();    // Draw 'stylized' characters
  //Read photo
  readPhoto ();
}

void readPhoto (){
  //over 300/daylight 1-100/evening
  //daylight=brightwhite  eveining=brightorange
  photoVal = analogRead(photoPin);
 Serial.printf("photoVal = %i \n",photoVal);  
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
  Serial.printf("Temp = %0.2f \n",varTempF);   
  display.display();
  delay(2000);
}

  //Manual fnx: wemos/2buttons
  //comfort items (coffee/fan for demo)
  
