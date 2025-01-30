#include <WiFi.h>
#include <Arduino.h>
#include "mEEPROM.h"

extern "C" {
	#include "freertos/FreeRTOS.h"
	#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>
#include <config.h>
#include <MQTTManager.h>

/*
   Wait time in Loop
   this determines how many frames are send to MQTT
   if wait time is e.g. 10 minutes, we will send only every 10 minutes to MQTT
   Note: only the last incoming block will be send; all previous blocks will be discarded
   Wait time is in seconds
   Waittime of 1 or 0 means every received packet will be transmitted to MQTT
   Packets during OTA or OneWire will be discarded
*/
int VE_WAIT_TIME = 1; // in s
// Change from start up screen to normal after
int VE_STARTUP_TIME = 30;
// Update LCD Screen
int VE_LCD_REFRESH = 1;

int VE_MQTT_RECONNECT = 5;

int VE_LOOP_TIME = 5;

ConfigClass config;
AsyncMqttClient mqttClient;
CANBUS inverter;
MQTTManagerClass mqttManagerClass(config, mqttClient, inverter);

//mEEPROM pref;
bool mqttEnabled = false;
char buffer[10];

bool sendUpdateMQTTData()
{
   if (mqttClient.connected())
   {
    
    String sTopic = mqttManagerClass.GetMQTTTopic();
    mqttManagerClass.mqttPublish((wifiManager.GetMQTTTopic() + "/Param/EnablePYLONTECH").c_str(), (Inverter.EnablePylonTech() == true) ? "ON" : "OFF",true);
    mqttManagerClass.mqttPublish((wifiManager.GetMQTTTopic() + "/Param/ForceCharge").c_str(), (Inverter.ForceCharge() == true) ? "ON" : "OFF" , true);  
    mqttManagerClass.mqttPublish((wifiManager.GetMQTTTopic() + "/Param/DischargeEnable").c_str(), (Inverter.DischargeEnable() == true && Inverter.ManualAllowDischarge()) ? "ON" : "OFF" , true); 
    mqttManagerClass.mqttPublish((wifiManager.GetMQTTTopic() + "/Param/ChargeEnable").c_str(), (Inverter.ChargeEnable() == true && Inverter.ManualAllowCharge()) ? "ON" : "OFF" , true); 
    return true;
  } 
  else  
    return false; 
}

//
// Send VE data from passive mode to MQTT
//
bool sendVE2MQTT() {
  String sTopic = mqttManagerClass.GetMQTTTopic();
  for (int i = 0; i < veHandle.FrameLength(); i++) {
    String key = veHandle.veName[i];
    if (key.length() == 0)
      break; // stop is no key is here.
    String value = veHandle.veValue[i];
    String topic = wifiManager.GetMQTTTopic() + "/" + key;
    if (mqttClient.connected()) {
      topic.replace("#", ""); // # in a topic is a no go for MQTT
      value.replace("\r\n", "");
      if (mqttManagerClass.mqttPublish(topic.c_str(), value.c_str(),true)) {
        log_i("MQTT message sent succesfully: %s: \"%s\"", topic.c_str(), value.c_str());
      } else {
        log_e("Sending MQTT message failed: %s: %s", topic.c_str(), value.c_str());
      }
    } 
  }

  // Send Voltage
  sprintf (buffer, "%u", Inverter.BattVoltage());
  mqttManagerClass.mqttPublish((sTopic + "/V").c_str(),buffer,false);
  // Send Current
  sprintf (buffer, "%i", Inverter.BattCurrentmA());
  mqttManagerClass.mqttPublish((sTopic + "/I").c_str(),buffer,false);
  // Send SOC
  sprintf (buffer, "%i", Inverter.BattSOC());
  mqttManagerClass.mqttPublish((sTopic + "/SOC").c_str(),buffer,false);


  // Publish Json with more details in.
  mqttManagerClass.mqttPublish(mqttManagerClass.GetMQTTTopicData().c_str(),generateDatatoJSON(false).c_str(),false);
  return true;
}
