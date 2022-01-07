/******************************************************//**
 * @file main.cpp
 * @author Gabriele Baldi (you@domain.com)
 * @brief 
 * 
 * Manage the controller main and the action derived 
 * from the comunication with othier nterfaces, including 
 * IA software
 * 
 * @version 0.1
 * @date 2021-12-16
 **********************************************************/

/**
 * Header
 */
#include "main.h"

StaticJsonDocument<200> doc;
bool flag = false;

/**
 * Variables definition
 */
// Counter variables
bool count = false;
unsigned long counter = 0;
bool startCount = false;
// Timing variables
unsigned long counterMillis = 0;
unsigned long previousMillis = 0;

/**
 * Main Functions
 */
/**
 * @brief 
 * Initialize Serial, Serial 1 for serial communication
 * and the Ethernet with Thingsboard and FastAPI 
 */
void setup() 
{
  Serial.begin(115200);
  while(!Serial){}
  Serial1.begin(9600);
  Serial1.setTimeout(10000);
  initEthernet();
  initTBClient();
}

/**
 * @brief 
 * VMR controller loop - executes functions to fulfill
 * the VMR process requirements
 */
void loop() 
{
  counterMillis = millis();
  // If the connection goes down try to reconnect
  if(Ethernet.linkStatus() != 1 || !mqtt_client.connected())
  {
    reconnect();
  }
  mqtt_client.loop();
  // Process cycle: take the input and decide which output
  incomingSerial();
  selectAction();
  // Asynchrounous waiting for Jetson Nano
  if(count == true)
  {
    if (startCount == true)
    {
      previousMillis = millis();
      counterMillis = millis();
      startCount = false;
    }
    if(counterMillis - previousMillis >= 20000UL)
    {
      previousMillis = counterMillis;
      inputJson["key"] = "VMRController";
      inputJson["value"] = "WARNING";
    }
  }
}

/**
 * Other Functions
 */
/**
 * @brief 
 * Select the action to be made, based on the 
 * incoming data from the serial communication
 */
void selectAction()
{
  // Copy inputJson into outputJson and decides which action has to be taken
  if(inputJson.isNull())
  {
    return;
  }
  String message;
  String key = String((const char*)inputJson["key"]);
  String value = String((const char*)inputJson["value"]);
  if(inputJson.size() < 2)
  {
    Serial.println();
    Serial.println("ERROR");
    return;
  }
  outputJson["key"] = inputJson["key"];
  outputJson["value"] = inputJson["value"];

  // Actions to be done before the ON input from Thingsboard
  if(key.equals("IASoftware") && flag_start == false)
  {
    if(value.equals("ON"))
    {
      // Controllo componenti ed invio messaggio
      Serial.println("\nComponents OK");
    }
  }
  // Actions of the overall process(After that the ON button has been pressed)
  // IA Software messages
  else if(key.equals("IASoftware") && flag_start == true)
  {
    outputJson["Code"] = String((const char*)inputJson["code"]);
    outputJson["vm_x"] = String((const char*)inputJson["vm_x"]);
    outputJson["vm_y"] = String((const char*)inputJson["vm_y"]);
    outputJson["shape"] = inputJson["shape"]; 
    outputJson["detail"] = inputJson["detail"];
    serializeJson(outputJson,message);
    Serial.println("\nMessage sent: ");
    serializeJson(outputJson,Serial);
    httpRequest(message);
    count = false;
    counter = 0;
    if(value.equals("OK"))
    { 
      Serial.println("Waiting for glass slide removal");
      delay(5000);
      Serial.println("Glass slide removed");
      delay(1000);
      ///////////////////////////////////////////////////////////////////////////////////
      // INPECO Interface waiting cycle
      //unsigned long time = 0;
      //unsigned long time_now = 0;
      //time = millis();
      // while(!glassRemoved){ time_now = millis(); if(time_now-time >= 10000){WARNING}}
      // if(digitalRead == OK) 
      // Controllo sensori
      // IF ERROR
      //Gestire errore {gestione errore; return;} 
      ///////////////////////////////////////////////////////////////////////////////////
    } 
    else if(value.equals("ERROR"))
    {
      flag_start = false;
    } 
    // Turn off the platform
    mqtt_client.publish("v1/devices/me/attributes", "{\"param\":\"false\"}");
    // Make sure that the buffer is empty
    while(Serial1.available()){Serial1.read();}
  }
  // VMR Controller messages
  else if(key.equals("VMRController")  && flag_start == true)
  {
    if(value.equals("START"))
    {
      Serial.println("Turning on Lights"); 
      delay(1000);
      Serial.println("Turning on background monitor");
      delay(1000);      
      Serial.println("Waiting for glass slide");
      delay(1000);
      ///////////////////////////////////////////////////////////////////////////////////
      // INPECO Interface waiting cycle
      //unsigned long time = 0;
      //unsigned long time_now = 0;
      //time = millis();
      // while(!glassArrived){ time_now = millis(); if(time_now-time >= 10000){WARNING}}
      // if(digitalRead == OK) 
      // Controllo sensori
      // IF ERROR
      //Gestire errore {gestione errore; return;}
      ///////////////////////////////////////////////////////////////////////////////////
      outputJson["SliceCode"] = inputJson["SliceCode"]; //ALLA SERIALE
      serializeJson(outputJson,message);
      outgoingSerial(message);
      count = true;
      startCount = true;
    }
    else if(value.equals("WARNING"))
    {   
        outputJson["Code"] = "21502";
        outputJson["detail"] = "No%20response%20from%20IA%20Software";
        serializeJson(outputJson,message);
        httpRequest(message);
        counter = 0;
    }
  }
  inputJson.clear();
  outputJson.clear(); 
}

/**
 * @brief 
 * Tries to reconnect to Ethernet and Thingsboard
 */
void reconnect() 
{
  Serial.println("Not connected");
  // Loop until we're reconnected, both to Ethernet and then Thingsboard server
  while(!mqtt_client.connected()) 
  {
    if(!Ethernet.linkStatus())
    {
      Serial.println("Connecting to Ethernet");
      initEthernet();
    }
      initTBClient();
  }
}
