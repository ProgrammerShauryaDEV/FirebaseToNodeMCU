#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#include <DFRobot_DHT11.h>
DFRobot_DHT11 DHT;
#define DHT11_PIN 10
// Insert your network credentials
#define WIFI_SSID "SHAURYA" //OnePlus Nord CE 2
#define WIFI_PASSWORD "shaurya@12" //qpqpwowo

// Insert Firebase project API Key
#define API_KEY "AIzaSyCvJki3BGbZLjytyG8Qpcn4PUkLKOIlh8s"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://tryproject-2f59e-default-rtdb.firebaseio.com/"

#define Switch 8
#define Motor 5
#define Pump 6

//Define Firebase Data object
FirebaseData fbdo1;
FirebaseData fbdo2;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
int fanValue;
int pumpValue;
int switchValue;
bool signupOK = false;

int humi, temp;

void setup(){
  pinMode(Motor,OUTPUT);
  pinMode(Pump,OUTPUT);
  pinMode(Switch, INPUT);
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.print(WiFi.localIP());

  
  Serial.println();
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
 
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void runCooler(){
  if ((humi>=70 && temp >= 32) || (humi<=70 && humi >= 60 && temp <= 32 && temp >= 28 ) || (humi<=70 && humi >= 60 && temp >= 32 ) || (humi<=60 && temp <= 32 && temp >= 28 ) ){
    digitalWrite(Motor, LOW);
    digitalWrite(Pump,LOW);
    }
  else if ((temp <= 28 && humi <= 60) || (temp <= 28 && ( humi>60 || humi <= 70))){
      digitalWrite(Motor,HIGH);
      digitalWrite(Pump,HIGH);
    }
  else if ((temp<=28 && humi>=70)|| (temp<=32 & temp>=28 && humi>70)){
      digitalWrite(Motor,LOW);
      digitalWrite(Pump,HIGH);
      }
}

void loop(){

  switchValue = digitalRead(Switch);
  int appSwitch;
  if (Firebase.RTDB.getInt(&fbdo1, "/Component/Switch/")) {
    if (fbdo1.dataType() == "int") {
      appSwitch = fbdo1.intData();
      Serial.println(appSwitch);
      if (appSwitch == 1){
        runCooler();
      }
      else {
        digitalWrite(Motor,HIGH);
        digitalWrite(Pump,HIGH);
      }
    }
  }

  else if ( switchValue = 1){
    runCooler();    
  }
  else{
    digitalWrite(Motor,HIGH);
    digitalWrite(Pump,HIGH);
  }            
}
