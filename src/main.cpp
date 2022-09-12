#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <ESP32Servo.h>
#include <Firebase_ESP_Client.h>
#include <addons/RTDBHelper.h>
#include <addons/TokenHelper.h>

#define API_KEY "knRkupn6SzK9n9hCBcXeepMlwO4XQJ3Dxq4h7w2a"
#define DATABASE_URL "mt-lab-6362e-default-rtdb.firebaseio.com"
#define USER_EMAIL ""
#define USER_PASSWORD ""

// Second core // 
TaskHandle_t Task1;

// Firebase Stuff //
FirebaseData stream;
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

String parentPath = "LCB-002/";
String childPath[12] = {"b1", "b2", "b3", "b4", "bob", "h1", "h2", "h3", "h4", "nb", "np", "push"};

// Servo Stuff //
Servo srv1;
Servo srv2;

int srv1pin = 32;
int srv2pin = 23;

// Wifi Stuff //
WiFiManager manager;

// Control Variables //
int h1 = 1;
int h2 = 1000;
int h3 = 1;
int h4 = 1000;

int b1 = 1;
int b2 = 1000;
int b3 = 1;
int b4 = 1000;

bool bob = true;
bool push = true;

int nbob = 2;
int npush = 2;

int condition;

// Other Variables //
bool dataChanged = false;

void streamCallback(MultiPathStream stream)
{
  size_t numChild = sizeof(childPath) / sizeof(childPath[0]);
  String loc;
  int value;

  for (size_t i = 0; i < numChild; i++)
  {
    if (stream.get(childPath[i]))
    {
      Serial.printf("path: %s, event: %s, type: %s, value: %s%s", stream.dataPath.c_str(), stream.eventType.c_str(), stream.type.c_str(), stream.value.c_str(), i < numChild - 1 ? "\n" : "");
      loc = stream.dataPath.c_str();
      value = stream.value.toInt();
      if (loc == "bob" || loc == "/bob")
      {
        
        if (value == 1)
        {
          bob = true;
        }else{
          bob = false;
        }
        
        Serial.print(F("Bob :")); Serial.println(bob);
      }  
      if (loc == "push" || loc == "/push")
      {
        if (value == 1)
        {
          push = true;
        }else{
          push = false;
        }
        Serial.print(F("Push : ")); Serial.println(push);
      }               
      if (loc == "b1" || loc == "/b1")
      {
        b1 = value;
        Serial.print(F("B1 Value : ")); Serial.println(b1);
      }
      if (loc == "b2" || loc == "/b2")
      {
        b2 = value;
        Serial.print(F("B2 Value : ")); Serial.println(b2);
      }
      if (loc == "b3" || loc == "/b3")
      {
        b3 = value;
        Serial.print(F("B3 Value : ")); Serial.println(b3);
      }
      if (loc == "b4" || loc == "/b4")
      {
        b4 = value;
        Serial.print(F("B4 Value : ")); Serial.println(b4);
      }
      if (loc == "h1" || loc == "/h1")
      {
        h1 = value;
        Serial.print(F("H1 Value : ")); Serial.println(h1);
      }
      if (loc == "h2" || loc == "/h2")
      {
        h2 = value;
        Serial.print(F("H2 Value : ")); Serial.println(h2);
      }
      if (loc == "h3" || loc == "/h3")
      {
        h3 = value;
        Serial.print(F("H3 Value : ")); Serial.println(h3);
      }
      if (loc == "h4"  || loc == "/h4")
      {
        h4 = value;
        Serial.print(F("H4 Value : ")); Serial.println(h4);
      }
      if (loc == "nb"  || loc == "/nb")
      {
        nbob = value;
        Serial.print(F("No. Of Bobs : ")); Serial.println(nbob);
      } 
      if (loc == "np"  || loc == "/np")
      {
        npush = value;
        Serial.print(F("No. of Push : ")); Serial.println(npush);
      }            
    }
  }

  Serial.println();

  if (dataChanged)
  {
    bob = Firebase.RTDB.getInt(&fbdo, F("LCB-001/bob"));
    Serial.println(bob);
    push = Firebase.RTDB.getInt(&fbdo, F("LCB-001/push"));
    Serial.println(push);
    dataChanged = false;
  }

}

void streamTimeoutCallback(bool timeout)
{
  if (timeout)
    Serial.println("stream timed out, resuming...\n");

  if (!stream.httpConnected())
    Serial.printf("error code: %d, reason: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
}

void headBob(int he1, int he2, int he3, int he4){
  for (int i = 40; i > 1; i -= 1) {
    srv1.write(i);
    delay(he1);
  }
  delay(he2);
  for (int i = 1; i < 40; i += 1) {
    srv1.write(i);
    delay(he3);
  }
  delay(he4);
}

void pushUp(int p1, int p2, int p3, int p4){
  for (int i = 1; i < 40; i += 1) {
    srv2.write(i);
    delay(p1);
  }
  delay(p2);
  for (int i = 40; i > 1; i -= 1) {
    srv2.write(i);
    delay(p3);
  }
  delay(p4);
}

void core2Task(void * pvParameters){
  for(;;){
     if (bob == true)
      {
        for (int i = 0; i < nbob; i++)
        {     
          headBob(h1, h2, h3, h4);
        }    
      }

      if (push == true)
      {
        for (int i = 0; i < npush; i++)
        {      
          pushUp(b1, b2, b3, b4);
        }    
      }       
  }
}

void setup() {

  // put your setup code here, to run once:
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  bool res;
  res = manager.autoConnect("LizBot");
  if (!res)
  {
    Serial.println(F("Failed to Connect"));
  }else{
    Serial.println(F("Connected"));
  }
  srv1.attach(srv2pin);
  srv2.attach(srv1pin);  

  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.signer.test_mode = true;
  config.token_status_callback = tokenStatusCallback;
  Firebase.reconnectWiFi(true);  
  Firebase.begin(&config, &auth);
  delay(1000);
   
  if (Firebase.ready())
  {
    FirebaseJson json;
    json.add("Online", true);
    Serial.printf("Set json... %s\n\n", Firebase.RTDB.updateNode(&fbdo, "LCB-001", &json) ? "ok" : fbdo.errorReason().c_str());
  }
  if (!Firebase.RTDB.beginMultiPathStream(&stream, parentPath)){
    Serial.printf("sream begin error, %s\n\n", stream.errorReason().c_str());
  }
  Firebase.RTDB.setMultiPathStreamCallback(&stream, streamCallback, streamTimeoutCallback);

  // xTaskCreatePinnedToCore(
  //                   core2Task,   /* Task function. */
  //                   "Task1",     /* name of task. */
  //                   10000,       /* Stack size of task */
  //                   NULL,        /* parameter of the task */
  //                   1,           /* priority of the task */
  //                   &Task1,      /* Task handle to keep track of created task */
  //                   0);

}

void loop() {
  // put your main code here, to run repeatedly:  
  if (bob == true)
      {
        for (int i = 0; i < nbob; i++)
        {     
          headBob(h1, h2, h3, h4);
        }    
      }

      if (push == true)
      {
        for (int i = 0; i < npush; i++)
        {      
          pushUp(b1, b2, b3, b4);
        }    
      }
}