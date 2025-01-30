//#ifdef WIFIMANAGER
//#pragma once
#include <Arduino.h>
#include "mEEPROM.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <DNSServer.h>

#include <AsyncMqttClient.h>

#include "CANBUS.h"
#include "config.h"

class MQTTManagerClass {
    private:
        AsyncMqttClient* mqttClient;
        bool _provEnable;
        bool _needConfig;
        bool _wifiOK;
        bool _wifiEnabled = false;
        bool _dnsStarted = false;
        ConfigClass* config;
        CANBUS* inverter;
    public:
        MQTTManagerClass(ConfigClass&  _config, AsyncMqttClient& _mqttClient, CANBUS& _inverter) {
            _provEnable = false;
            config = &_config;
            mqttClient = &_mqttClient;
            inverter = &_inverter;
        }
        bool connectToMqtt();
        bool mqttPublish(String topic, String payload, bool retain);
        bool mqttPublish(const char* topic, const char* payload, bool retain);
        bool begin();
        void loop();
        bool isWiFiConnected();
        bool isMqttConnected();
        void onMqttConnect(bool sessionPresent);

        bool MQTTConnect();
        bool MQTTDisconnect();
        bool mqttEnabled = false;
        bool mqttInit = false;
        String GetIPAddr();
        wifi_mode_t GetMode();
        WiFiClient wifiClient;

        String GetWifiSSID();
        String GetWifiPass();
        String GetWifiHostName();
        String GetMQTTUser();
        String GetMQTTPass();
        String GetMQTTServerIP();
        String GetMQTTClientID();
        String GetMQTTTopic();
        String GetMQTTTopicData();
        String GetMQTTParameter();
        CANBUS* getInverter() { return inverter; }
        uint16_t GetMQTTPort();        
};

//#endif