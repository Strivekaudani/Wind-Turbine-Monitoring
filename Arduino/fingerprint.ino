#include <HardwareSerial.h>
#include <Adafruit_Fingerprint.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h> 
#include <ESP32Servo.h>
#include <WiFi.h>
#include <HTTPClient.h> 
#include "time.h"

const char* ssid       = "DOUBLE C";
const char* password   = "987651234";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

const byte numRows= 4;          //number of rows on the keypad
const byte numCols= 4;          //number of columns on the keypad
char keypressed;
char code[]={'1','2','3','4'}; //Passcode needed you can change it just keep the format "4 digits as char array"
char c[4];                     //Array to get the code from the user
int ij;

//keymap defines the key pressed according to the row and columns just as appears on the keypad
char keymap[numRows][numCols]= 
{
{'1', '2', '3', 'A'}, 
{'4', '5', '6', 'B'}, 
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};
byte rowPins[numRows] = {12,14,27,26}; //Rows 0 to 3 //if you modify your pins you should modify this too
byte colPins[numCols]= {25,33,32,13}; //Columns 0 to 3
//initializes an instance of the Keypad class
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

#define LED_PIN 23
#define BUTTON_PIN 5
byte lastButtonState = LOW;
byte ledState = HIGH;
byte buttonState = 1;

#define TRIG_PIN 2 // ESP32 pin GIOP23 connected to Ultrasonic Sensor's TRIG pin
#define ECHO_PIN 15 // ESP32 pin GIOP22 connected to Ultrasonic Sensor's ECHO pin
float duration_us, distance_cm;

#define analog_pin 35
float sensor_value;  //variable to store sensor value
#define notDrunk 700   // Define max value that we consider not Drunk
#define Drunk 2000  // Define min value that we consider drunk

Servo myservo;  // create servo object to control a servo
// 16 servo objects can be created on the ESP32

int pos = 0;    // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
// Possible PWM GPIO pins on the ESP32-S2: 0(used by on-board button),1-17,18(used by on-board LED),19-21,26,33-42
#if defined(ARDUINO_ESP32S2_DEV)
int servoPin = 17;
#else
int servoPin = 19;
#endif

HardwareSerial mySerial(2);   //Fingerprint sensor wiring RX 3, TX 2 
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;

int counter = 0;
int finger_num = 0;
String isDrunk;

void setup()
{
    Serial.begin(9600); 
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(1000);
      Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the WiFi network");

    
    Serial.println("MQ3 warming up!");
    delay(2000);
     
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
   
    Serial.begin (9600);
    pinMode(TRIG_PIN, OUTPUT); // configure the trigger pin to output mode
    pinMode(ECHO_PIN, INPUT);// configure the echo pin to input mode

    

    lcd.init();  
    lcd.init();
    // Print a message to the LCD.
    lcd.backlight();
    lcd.setCursor(3,0);
    lcd.print("STANDBY");
    delay(2000);
  
    Serial.begin(9600);
    while (!Serial);  // For Yun/Leo/Micro/Zero/...
    delay(100);
    Serial.println("\n\nAdafruit finger detect test");
  
    // set the data rate for the sensor serial port
    finger.begin(57600);
    delay(5);
    if (finger.verifyPassword())
    {
      Serial.println("Found fingerprint sensor!");
    } 
    else 
    {
      Serial.println("Did not find fingerprint sensor :(");
      while (1) { delay(1); }
    }

    Serial.println(F("Reading sensor parameters"));
    finger.getParameters();
    Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
    Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
    Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
    Serial.print(F("Security level: ")); Serial.println(finger.security_level);
    Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
    Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
    Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
  
    finger.getTemplateCount();

    if (finger.templateCount == 0) 
    {
      Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
    }
    else 
    {
      Serial.println("Waiting for valid finger...");
      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
    }

//    Serial.begin(9600);
//    
//    //connect to WiFi
//    Serial.printf("Connecting to %s ", ssid);
//    WiFi.begin(ssid, password);
//    while (WiFi.status() != WL_CONNECTED) {
//        delay(500);
//        Serial.print(".");
//    }
//    Serial.println(" CONNECTED");
//    
     //init and get the time
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      printLocalTime();
//  
//    //disconnect WiFi as it's no longer needed
//    WiFi.disconnect(true);
//    WiFi.mode(WIFI_OFF);
}

void loop()                            // run over and over again
{
    getFingerprintID();
  
  
//  delay(5000);  //Send a request every 10 seconds

  
  keypressed = myKeypad.getKey();      //Reading the buttons typed by the keypad
  if(keypressed == 'A')                //If it's 'A' it triggers "Adding new template" sequence
  {               
      ij=0;                            //ij is set to 0
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("ENTER PASSWORD");
      getPassword();                   //Getting the passcode function
      if(ij==4)
      {                       //If the passcode is correct we can start enrolling new finger template
        finger.getTemplateCount();
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("SENSOR CONTAINS");
        lcd.setCursor(0,1);
        lcd.print(finger.templateCount);
        lcd.setCursor(5,1);
        lcd.print("TEMPLATES");
        delay(3000);
        Enrolling();              //Enrolling function
        delay(2000);
        lcd.clear();
      }
      else
      {                    //If the code is wrong we can't add new users (templates)
        lcd.setCursor(0,0);
        lcd.print("WRONG CODE");
        delay(2000);
        lcd.clear();
      } 
  }
  if(keypressed == 'B')        //As the keypad is meant to stay inside the 'B' button opens the door lock
  {                            //Because the system I used needs an electrical signal to open
      ij=0;                      //ij is set to 0
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("ENTER PASSWORD");
      getPassword();                   //Getting the passcode function
      if(ij==4)
      {                            //If the passcode is correct we can start enrolling new finger template
        Deleting();                 //Enrolling function
        delay(2000);
        lcd.clear();
      }
      else
      {                    //If the code is wrong we can't add new users (templates)
        lcd.setCursor(0,0);
        lcd.print("WRONG CODE");
        delay(2000);
        lcd.clear();
      }
  }

  printLocalTime();
  lcd.setCursor(0,1);
  lcd.print("PLACE FINGER"); //Constant showing message          
  delay(1000);      //don't ned to run this at full speed.
}

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("----------------");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  
      lcd.setCursor(0,0);
      lcd.print(&timeinfo, "%B %d %H:%M ");
//      lcd.setCursor(0,1);
//      lcd.print(&timeinfo, "%d %Y %H:%M:%S");
}

void send_to_server()
{
    if(WiFi.status()== WL_CONNECTED)
    {   //Check WiFi connection status
  
         String postData, f_num_string;
         f_num_string = String(finger_num);
         postData = f_num_string + "," + isDrunk;
         Serial.println(postData);
     
     
         HTTPClient http;   
      
         http.begin("http://192.168.137.130:8090/post");
         http.addHeader("Content-Type", "text/plain");             
      
         int httpResponseCode = http.POST(postData);
         String payload = http.getString(); 
      
         if(httpResponseCode>0)
         {
          Serial.println(httpResponseCode); 
         }else
         {
          Serial.println("Error on sending POST");
         }
         http.end();  //Free resources

         finger_num = 0;
         isDrunk = "";
  
    }
     else
    {
      Serial.println("Error in WiFi connection");   
    }
}


uint8_t getFingerprintID() 
{
  uint8_t p = finger.getImage();
  switch (p)
  {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) 
  {
    case FINGERPRINT_OK:
      lcd.clear();
      lcd.print("Image taken");
      delay(2000);
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      lcd.clear();
      lcd.print("Image too messy");
      delay(2000);
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.clear();
      lcd.print("Communication error");
      delay(2000);
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      lcd.init();
      lcd.setCursor(0,0);
      lcd.print("Could not find ");
      lcd.setCursor(0,1);
      lcd.print("fingerprint features");
      delay(2000);
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      lcd.init();
      lcd.setCursor(0,0);
      lcd.print("Could not find ");
      lcd.setCursor(0,1);
      lcd.print("fingerprint features");
      delay(2000);
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      lcd.clear();
      lcd.print("Unknown error");
      delay(2000);
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  lcd.init();
  lcd.setCursor(0,0);
  lcd.print("REMOVE FINGER");
  delay(1000);
  p = finger.fingerSearch(); 
  if (p == FINGERPRINT_OK)
  {
    Serial.println("Found a print match!");
    lcd.init();
    lcd.setCursor(3,0);
    lcd.print("FINGERPRINT");
    lcd.setCursor(3,1);
    lcd.print("MATCHED");
    delay(2000);

    lcd.init();
    lcd.setCursor(3,0);
    lcd.print("PROCEED TO");
    lcd.setCursor(3,1);
    lcd.print("ALCOHOL TEST");
    delay(1000);
    
    lcd.init();
    lcd.setCursor(1,0);
    lcd.print("PLEASE BREATH");
    delay(5000);
    
    alcohol_testing();
    if (sensor_value >= Drunk)
    {
      isDrunk = "YES";
      digitalWrite(LED_PIN, HIGH);
      delay(500);
      digitalWrite(LED_PIN, LOW);
      delay(500);
      lcd.init();
      lcd.setCursor(1,0);
      lcd.print("YOU CAN'T ENTER");
      lcd.setCursor(1,1);
      lcd.print("YOU ARE DRUNK");
      lcd.print("ID: ");
      lcd.setCursor(13,1);
      lcd.print(finger.fingerID); //And the ID of the finger template
      finger_num = finger.fingerID;
      delay(2000);
      lcd.init();
      lcd.setCursor(5,1);
      lcd.print("NEXT");
      send_to_server();
    }
    else
    {
      isDrunk = "NO";
      lcd.init();
      lcd.setCursor(3,0);
      lcd.print("WELCOME TO ");
      lcd.setCursor(3,1);
      lcd.print("WORK");        //Printing a message for the recognized template
      lcd.setCursor(9,1);
      lcd.print("ID: ");
      lcd.setCursor(13,1);
      lcd.print(finger.fingerID); //And the ID of the finger template
      finger_num = finger.fingerID;
      delay(1000);
      lcd.init();
      lcd.setCursor(3,0);
      lcd.print("LOGGED AT");
      printLocalTime();
      lcd.init();
      lcd.setCursor(3,0);
      lcd.print("OPENING GATE");
      send_to_server();
      servo_open();
      sonic();
    
      while (distance_cm >= 15)
      {
        lcd.init();
        lcd.setCursor(3,0);
        lcd.print("PLEASE ENTER");
        sonic(); 
      }
     
    
      while (distance_cm < 8)
      {
        sonic();
      }
      lcd.init();
      lcd.setCursor(3,0);
      lcd.print("CLOSING GATE");
      delay(1000);
      digitalWrite(LED_PIN, HIGH);
      delay(1000);
      digitalWrite(LED_PIN, LOW);
      delay(1000);
      servo_close();
    }
          
  } 
      
        
  else if (p == FINGERPRINT_PACKETRECIEVEERR) 
  {
    lcd.clear();
    lcd.print("Communication error");
    delay(2000);
    Serial.println("Communication error");
    return p;
  } 
 else if (p == FINGERPRINT_NOTFOUND) 
 {
    counter++;
    Serial.println("Did not find a match");
    lcd.clear();                     //And here we write a message or take an action for the denied template
    lcd.print("ACCESS DENIED");
    delay(2000);
    

    if (counter==3)
    {
      counter = 0;
      byte lastButtonState = LOW;
      byte ledState = HIGH;
      byte buttonState = 1;
      digitalWrite(LED_PIN, HIGH);
      while(buttonState)
      {
        Serial.println("Did not find a match");
        buttonState = digitalRead(BUTTON_PIN);
        if (buttonState != lastButtonState && buttonState == lastButtonState) 
            lastButtonState = buttonState;
            if (buttonState == LOW) 
            {
              ledState = (ledState == LOW) ? HIGH: LOW;
              digitalWrite(LED_PIN, LOW);
            }      
      }
    }
    else
    {   
      lcd.clear();
      return p;
    }
    lcd.clear();
      return p;
 } 
  else 
  {
    lcd.clear();
    lcd.print("Communication error");
    delay(2000);
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  return finger.fingerID;
}

//Getting password (code) function, it gets the characters typed
//and store them in c[4] array
void getPassword(){
   for (int i=0 ; i<4 ; i++){
    c[i]= myKeypad.waitForKey();
    lcd.setCursor(i,1);
    lcd.print("*");
   }
   lcd.clear();
   for (int j=0 ; j<4 ; j++){ //comparing the code entred with the code stored
    if(c[j]==code[j])
     ij++;                    //Everytime the char is correct we increment the ij until it reaches 4 which means the code is correct
   }                          //Otherwise it won't reach 4 and it will show "wrong code" as written above
}

//The Enrolling and getFingerprintEnroll functions are mainly based on the "Enroll" example code from the library
//Only the modifications will be commented, return to the example to see each step in details, as here it's shortened

uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  lcd.clear();
  lcd.print("DELETE ID:"); //Message to print for every step
  lcd.setCursor(10,0);
  lcd.print(id);
  
  p = finger.deleteModel(id);
  
  if (p == FINGERPRINT_OK) 
  {
    Serial.println("Deleted!");
    lcd.clear();                //if both images are gotten without problem we store the template as the Id we entred
    lcd.setCursor(0,0);
    lcd.print("DELETED");    //Print a message after storing and showing the ID where it's stored
    lcd.setCursor(0,1);
    lcd.print("ID: ");
    lcd.setCursor(5,1);
    lcd.print(id);
    delay (1000);
  } 
  else if (p == FINGERPRINT_PACKETRECIEVEERR)
  {
    lcd.clear();
    lcd.print("Communication error");
    delay(2000);
    Serial.println("Communication error");
    return p;
  } 
  else if (p == FINGERPRINT_BADLOCATION) 
  {
    Serial.println("Could not delete in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) 
  {
    Serial.println("Error writing to flash");
    return p;
  } 
  else 
  {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
    return p;
  }   
}

void Deleting()  
{
  keypressed = NULL;
  lcd.clear();
  lcd.print("DELETE OLD USER");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("USER ID:");
  id = readnumber();                           //This function gets the Id it was meant to get it from the Serial monitor but we modified it
  if (id == 0) 
  {                                            // ID #0 not allowed, try again!
     return;
  }
  Serial.print("Deleting ID #");
  Serial.println(id);
  lcd.clear();
  lcd.print("Deleting ID #");
  lcd.setCursor(14,1);
  lcd.print(id);
  delay(2000);
  deleteFingerprint(id);
}

void Enrolling()  
{
  keypressed = NULL;
  lcd.clear();
  lcd.print("ENROLL NEW USER");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ENTER NEW ID");
  id = readnumber();                           //This function gets the Id it was meant to get it from the Serial monitor but we modified it
  if (id == 0) 
  {                                           // ID #0 not allowed, try aga
    return;
     
  }
  while (!getFingerprintEnroll() );
}

//Enrolling function only the modifications are commented
uint8_t getFingerprintEnroll() 
{
  int p = -1;
  lcd.clear();
  lcd.print("Enroll ID:"); //Message to print for every step
  lcd.setCursor(10,0);
  lcd.print(id);
  lcd.setCursor(0,1);
  lcd.print("PLACE FINGER"); //First step
  while (p != FINGERPRINT_OK) 
  {
    p = finger.getImage();
    switch (p) 
    {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
   }
 }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) 
  {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_IMAGEMESS:
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      return p;
    case FINGERPRINT_FEATUREFAIL:
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      return p;
    default:
      return p;
  } 
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("REMOVE FINGER"); //After getting the first template successfully
  lcd.setCursor(0,1);
  lcd.print("PLEASE !");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) 
  {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("PLACE SAME "); //We launch the same thing another time to get a second template of the same finger
  lcd.setCursor(0,1);
  lcd.print("FINGER PLEASE");
  while (p != FINGERPRINT_OK) 
  {
    p = finger.getImage();
    switch (p) 
    {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
   }
 }
  // OK success!
  p = finger.image2Tz(2);
  switch (p) 
  {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_IMAGEMESS:
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      return p;
    case FINGERPRINT_FEATUREFAIL:
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      return p;
    default:
      return p;
  } 
  Serial.print("Creating model for #");  Serial.println(id);
  p = finger.createModel();
  if (p == FINGERPRINT_OK) 
  {
  } 
  else if (p == FINGERPRINT_PACKETRECIEVEERR) 
  {
    return p;
  } 
  else if (p == FINGERPRINT_ENROLLMISMATCH) 
  {
    return p;
  } 
  else
  {
    return p;
  }    
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) 
  {
    lcd.clear();                //if both images are gotten without problem we store the template as the Id we entred
    lcd.setCursor(0,0);
    lcd.print("STORED IN");    //Print a message after storing and showing the ID where it's stored
    lcd.setCursor(0,1);
    lcd.print("ID: ");
    lcd.setCursor(5,1);
    lcd.print(id);
    delay(3000);
  }
  else if (p == FINGERPRINT_PACKETRECIEVEERR) 
  {
    return p;
  } 
  else if (p == FINGERPRINT_BADLOCATION) 
  {
    return p;
  } 
  else if (p == FINGERPRINT_FLASHERR) 
  {
    return p;
  } 
  else 
  {
    return p;
  } 
  return true;  
}

//This function gets the ID number as 3 digits format like 001 for ID 1
//And return the number to the enrolling function
uint8_t readnumber(void) 
{
  uint8_t num = 0;
  while (num == 0) 
  {
     char keey = myKeypad.waitForKey();
     int  num1 = keey-48;
     lcd.setCursor(0,1);
     lcd.print(num1);
     keey = myKeypad.waitForKey();
     int  num2 = keey-48;
     lcd.setCursor(1,1);
     lcd.print(num2);
     keey = myKeypad.waitForKey();
     int  num3 = keey-48;
     lcd.setCursor(2,1);
     lcd.print(num3);
     delay(1000);
     num=(num1*100)+(num2*10)+num3;
     keey=NO_KEY;
  }
   return num;
}

void servo_open()
{                                         
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    myservo.setPeriodHertz(50);    // standard 50 hz servo
    myservo.attach(servoPin, 1000, 2000); // attaches the servo on pin 18 to the servo object
    for (pos = 0; pos <= 180; pos += 1) 
    { 
      myservo.write(pos);    // tell servo to go to position in variable 'pos'
      delay(50);             // waits 15ms for the servo to reach the position
   }       
}

void sonic()
{
  // generate 10-microsecond pulse to TRIG pin
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration_us = pulseIn(ECHO_PIN, HIGH);    // measure duration of pulse from ECHO pin
  distance_cm = 0.017 * duration_us;        // calculate the distance
  Serial.print("distance: ");               // print the value to Serial Monitor
  Serial.print(distance_cm);
  Serial.println(" cm");
//  delay(500); 
}

void alcohol_testing()
{
  sensor_value = analogRead(analog_pin);
  Serial.print("Sensor Value: ");
  Serial.print(sensor_value); 
  if (sensor_value < notDrunk) 
  {
    Serial.println("  |  Status: Not Drunk");
    lcd.init();
    lcd.setCursor(3,0);
    lcd.print("NOT DRUNK");
  } 
  else if (sensor_value >= notDrunk && sensor_value < Drunk) 
  {
    Serial.println("  |  Status: Drinking but within legal limits");
    lcd.init();
    lcd.setCursor(3,0);
    lcd.print("WITHIN LIMIT");
  } 
  else if (sensor_value >= Drunk)
  {
    Serial.println("  |  Status: DRUNK"); 
  } 
   delay(2000); 
}

void servo_close()
{
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    myservo.setPeriodHertz(50);    // standard 50 hz servo
    myservo.attach(servoPin, 1000, 2000); // attaches the servo on pin 18 to the servo object
    for (pos = 180; pos >= 0; pos -= 1) 
    { 
      myservo.write(pos);    // tell servo to go to position in variable 'pos'
      delay(50);             // waits 15ms for the servo to reach the position
    }
    lcd.init();
    lcd.setCursor(3,0);
    lcd.print("NEXT");
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() 
{
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}
