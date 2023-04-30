#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;
#define TRIGGER_PIN 33
#define ECHO_PIN 32

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

float duration = 0;
float distance = 0;

void setup() {
  Serial.begin(9600);  //Initialize serial
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
  // pieces of information in a channel.  Here, we write to field 1.
  int x = ThingSpeak.writeField(myChannelNumber, 1, distance, myWriteAPIKey);
  // - myChannelNumber: the ID number of the ThingSpeak channel to write to
  // - 1: the field number to write the data to
  // - distance: the value to write to the field (in this case, the distance measurement)
  // - myWriteAPIKey: the API key that provides write access to the ThingSpeak channel
  
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  // change the value
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  // Read the duration of the ultrasonic pulse from the sensor
  duration = pulseIn(ECHO_PIN, HIGH);

  // Calculating the distance
  distance= duration*0.034/2;
  // The speed of sound in air is approximately 340 m/s, or 0.034 cm/us.
  // The distance is calculated as half the total distance traveled by the sound wave: from the sensor to the object, and then back again.
  // Therefore, the distance is calculated as (speed of sound * duration) / 2, where duration is the time it took for the reflected sound wave to return to the sensor.
  // The resulting distance value is in centimeters.

  // Print the distance to the serial monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  delay(20000); // Wait 20 seconds to update the channel again
}
