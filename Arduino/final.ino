#include <Adafruit_Fingerprint.h>
#include <TinyGPS.h>

#define mySerial Serial1
#define espSerial Serial2

//float lat = 0.00;
//float lon = 0.00;
TinyGPS gps;
String str = "";
int finger_scan = 1;
int send_distance;
int image_taken = 0;

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

int counter = 0;
const unsigned int solenoid_lock = 3;
const unsigned int buzzer = 6;
const unsigned short trig = 13;
const unsigned short echo = 12;

void setup()
{
  pinMode(solenoid_lock, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  Serial.begin(115200);
  espSerial.begin(115200);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) 
  {
    Serial.println("Found fingerprint sensor!");
  } else 
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

//  Serial2.begin(4800); // connect gps sensor
  delay(2000);
}

void loop()                     // run over and over again
{
  getFingerprintID();
  delay(50);            //don't ned to run this at full speed.

  unsigned long duration;
  int distance;

  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(2);
  digitalWrite(trig, LOW);

  duration = pulseIn(echo, HIGH);
  float _time = duration/2;
  float _speed = 0.03475;
  distance = _time * _speed;
  distance = 20 - distance;

  Serial.print(distance);
  Serial.println("cm");

//  get_gps_data();
  if(image_taken ==1)
  {
      str = String(distance)+String(",")+String(finger_scan);
    //  Serial.println(str);
      espSerial.println(str);
      finger_scan = 0;
  }
  
  delay(950);
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  image_taken = 0;
  switch (p) {
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
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      image_taken = 1;
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    digitalWrite(solenoid_lock, HIGH);
    delay(5000);
    digitalWrite(solenoid_lock, LOW);
    
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) 
  {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) 
  {
    Serial.println("Did not find a match");
    counter++;
     if (counter==3)
    {
      counter = 0;
      for(int i = 0; i < 3; i++)
      {
        tone(buzzer, 453);
        delay(1000);
        noTone(buzzer);
        delay(1000);   
      } 
    }
    
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  finger_scan = finger.fingerID;
  return finger.fingerID;
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

//void get_gps_data()
//{
//   if(Serial3.available())
//   { // check for gps data
//      if(gps.encode(Serial3.read()))// encode gps data
//      { 
//        gps.f_get_position(&lat,&lon); // get latitude and longitude
//    
//        Serial.print("Position: ");
//        
//        //Latitude
//        Serial.print("Latitude: ");
//        Serial.print(lat,6);
//        
//        Serial.print(",");
//        
//        //Longitude
//        Serial.print("Longitude: ");
//        Serial.println(lon,6); 
//      }
//   }
//}
