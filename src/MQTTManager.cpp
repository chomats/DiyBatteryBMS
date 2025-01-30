
//#ifdef WIFIMANAGER
#include "MQTTManager.h"

String sServer;
String sSubscribe;
String sTopic;
String sTopicData;
String sUser;
String sPass;
TimerHandle_t mqttReconnectTimer;

MQTTManagerClass* instanceMqtt;

void _connectToMqtt(TimerHandle_t xTimer) {
    instanceMqtt->connectToMqtt();
}

void _onMqttConnect(bool sessionPresent) {
  
  instanceMqtt->onMqttConnect(sessionPresent);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
    log_d("Message received. Topic: %s, Payload: %s",topic,payload);
    
    String sTopic = String(topic);
    String prefixTopic = instanceMqtt->GetMQTTTopic();
    String message = String(payload,len);
    CANBUS* Inverter = instanceMqtt->getInverter();

    if (sTopic == (prefixTopic + "/set/DischargeCurrent")) {
        Inverter->SetDischargeCurrent(message.toInt());
    }
    else if (sTopic == (prefixTopic + "/set/ChargeVoltage")) {
    if (message.toInt() > 0) {
        Inverter->SetChargeVoltage(message.toInt());
    }
    }
    else if (sTopic == prefixTopic + "/set/ChargeCurrent") {
        Inverter->SetChargeCurrent(message.toInt());
    }
    else if (sTopic == prefixTopic + "/set/ForceCharge") {
        bool forcecharge = (message == "ON") ? true : false;
        Inverter->ForceCharge((message == "ON") ? true : false);
        log_d("Force charge set to: %d", forcecharge);
    }
    else if (sTopic == prefixTopic + "/set/DischargeEnable") {
        Inverter->ManualAllowDischarge((message == "ON") ? true : false); 
    }
    else if (sTopic == prefixTopic + "/set/ChargeEnable") {
        Inverter->ManualAllowCharge((message == "ON") ? true : false); 
    }
    else if (sTopic == prefixTopic + "/set/EnablePYLONTECH") {
        Inverter->EnablePylonTech((message == "ON") ? true : false); 
    }
    
}


void MQTTManagerClass::onMqttConnect(bool sessionPresent) {
  // log_d("Connected to MQTT.");
  //Lcd.Data.MQTTConnected.setValue(true);
  sSubscribe = config->mqttTopic;
  if (!sSubscribe.endsWith("/"))
    sSubscribe += "/";


  mqttClient->subscribe((sSubscribe + "set/ChargeEnable").c_str(),2);
  mqttClient->subscribe((sSubscribe + "set/DischargeEnable").c_str(),2);
  mqttClient->subscribe((sSubscribe + "set/ForceCharge").c_str(),2);
  mqttClient->subscribe((sSubscribe + "set/EnablePYLONTECH").c_str(),2);
  mqttClient->subscribe((sSubscribe + "set/ChargeVoltage").c_str(),2);
  mqttClient->subscribe((sSubscribe + "set/ChargeCurrent").c_str(),2);
  
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    // log_d("Disconnected from MQTT.");
    // Lcd.Data.MQTTConnected.setValue(false);

    if (WiFi.isConnected()) {
        xTimerStart(mqttReconnectTimer, 0);
    }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  log_d("Subscribe acknowledged. packetId: %d qos: %d", packetId, qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  log_d("Unsubscribe acknowledged.");
}

void onMqttPublish(uint16_t packetId) {
   log_d("Publish acknowledged. packetId: %d", packetId);
}

void WiFiEvent(WiFiEvent_t event) {
    log_d("[WiFi-event] event: %d\n", event);
    switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
        log_d("WiFi connected, IP Address: %s",WiFi.localIP().toString());
        instanceMqtt->connectToMqtt();
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        log_d("WiFi lost connection");
        xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
        break;
    }
}


bool MQTTManagerClass::begin()
{
    IPAddress ipaddr;
    
    instanceMqtt = this;
    if (config->mqttClientID.length() > 1 && (ipaddr.fromString(config->mqttServer) 
    || config->mqttServer.length() >= 2) && config->mqttPort > 20 && config->mqttPort < 65535)
    {
        mqttEnabled = true;
    }
    else {
        log_d("MQTT details stored are not valid.");
        log_d("MQTT Server IP: %s, MQTT Port %d, MQTT Client ID: %s",config->mqttServer,config->mqttPort, config->mqttClientID);
        return false;
    }


    sServer = config->mqttServer;
    sUser = config->mqttUser;
    sPass = config->mqttPass;
    sTopic = config->mqttTopic;
    sTopicData = config->mqttTopic + "/Data";
    uint16_t mqttPort = (uint16_t) config->mqttPort;
    IPAddress _ip;
    bool useIP = false;
    
    mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(_connectToMqtt));

    WiFi.onEvent(WiFiEvent);

    mqttClient->onConnect(_onMqttConnect);
    mqttClient->onDisconnect(onMqttDisconnect);
    mqttClient->onSubscribe(onMqttSubscribe);
    mqttClient->onUnsubscribe(onMqttUnsubscribe);
    mqttClient->onMessage(onMqttMessage);
    mqttClient->onPublish(onMqttPublish);

    if (sServer.length()>0)
    {
        if (_ip.fromString(sServer))
        {
            mqttEnabled = true;
            log_d("Using IP Address method to connect.");
            mqttClient->setServer(ipaddr,mqttPort);
        }
        else {
            log_d("Using Hostname method to connect.");
            mqttClient->setServer(sServer.c_str(),mqttPort);
        }

    }
    else return false;

    return true;
}

bool MQTTManagerClass::connectToMqtt() {
    if (!mqttEnabled) return false;
    log_i("Connecting to MQTT...");
    //if (sUser.length()>0)
    log_i("Using User: %s, Password: %s",config->mqttUser, config->mqttPass);
    //mqttClient->setCredentials(sUser.c_str(),sPass.c_str());
    mqttClient->setCredentials(config->mqttUser.c_str(),config->mqttPass.c_str());
    mqttClient->connect();
    return mqttClient->connected();
}

bool MQTTManagerClass::isMqttConnected() {
    return mqttClient->connected();
}

bool MQTTManagerClass::MQTTDisconnect() {
    return false;
}

bool MQTTManagerClass::mqttPublish(String topic, String payload, bool retain)
{
  return mqttPublish(topic.c_str(),payload.c_str(),retain);
}

bool MQTTManagerClass::mqttPublish(const char* topic, const char* payload, bool retain)
{
  if (mqttClient->connected())
    return mqttClient->publish(topic,0,retain,payload);
  else 
    return false;
    
}

bool MQTTManagerClass::isWiFiConnected()
{
    
    if (WiFi.getMode() == WIFI_MODE_STA)
        return WiFi.isConnected();
    else
        return false;
}

bool MQTTManagerClass::MQTTConnect()
{
    return this->connectToMqtt();
}

void MQTTManagerClass::loop()
{
    //if (_dnsStarted)         
    //  _dnsserver.processNextRequest();
}

String MQTTManagerClass::GetIPAddr()
{

    if (WiFi.getMode() == WIFI_MODE_AP)
        return WiFi.softAPIP().toString();
    else if (WiFi.getMode() == WIFI_MODE_STA)
        return WiFi.localIP().toString();
    else 
        return "?";
}

wifi_mode_t MQTTManagerClass::GetMode()
{
    return WiFi.getMode();
}

String MQTTManagerClass::GetWifiSSID(){ return config->wifiSSID; }
String MQTTManagerClass::GetWifiPass(){ return config->wifiPass; }
String MQTTManagerClass::GetWifiHostName(){ return config->wifiHostName; }
String MQTTManagerClass::GetMQTTUser() { return config->mqttUser; }
String MQTTManagerClass::GetMQTTPass() { return config->mqttPass; }
String MQTTManagerClass::GetMQTTServerIP(){ return config->mqttServer; }
String MQTTManagerClass::GetMQTTClientID(){ return config->mqttClientID; }
String MQTTManagerClass::GetMQTTTopic(){ return config->mqttTopic; }
String MQTTManagerClass::GetMQTTTopicData(){ return sTopicData; }
String MQTTManagerClass::GetMQTTParameter(){ return config->mqttParameter; }
uint16_t MQTTManagerClass::GetMQTTPort(){ return config->mqttPort; }

//#endif