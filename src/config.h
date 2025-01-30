#pragma once
#include <Arduino.h>

#ifndef CONFIG_H
#define CONFIG_H

/*
   Copyright (c) 2022 Simon Jones

   Free to use in personal projects and modify for your own use, no permission for 
   selling or commericalising this code in this project.
   
   The copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/
#define SYSLOG 1

#define initBattChargeVoltage 55600       // Battery Charge Voltage sent to inverter
#define initBattFullVoltage 0             // Battery Full Voltage - keep SOC from 100% until this value.
#define initBattDischargeVoltage 45000    // Battery discharge voltage, not currently used
#define initBattChargeCurrent 0           // in mA, 
#define initBattDischargeCurrent 0        // in mA 
#define initBattCapacity 0                // used for charge limits when batteries becoming full.
#define initLowSOCLimit 0                 // Stop discharge limit (defauly only)
#define initHighSOCLimit 100              // Stop Charge above this limit (default only)
#define initSlowSOCCharge1 100
#define initSlowSOCCharge2 100
#define initSlowSOCDivider1 0
#define initSlowSOCDivider2 0

// To use strict PYLONTECH Protocol enable below
//#define USE_PYLONTECH

//
// Use OneWire temperature sensors 
//
//#define USE_ONEWIRE

#ifdef USE_ONEWIRE
#define ONEWIRE_PIN 22
/*
   define the wait time between 2 attempts to send one wire data
   300000 = every 5 minutes
*/
int OW_WAIT_TIME = 10; // in s
time_t last_ow;
#endif


/*
   WiFi parameters
*/
#define WIFIMANAGER

#ifdef USE_ONEWIRE
String MQTT_ONEWIRE = "/Temp/OneWire";
#endif

#define AppCore 1
#define SysCore 0

class ConfigClass {
    public:
        String wifiSSID = "Freebox-1F4155-2.4";
        String wifiPass = "zdknwq5hwfqkv72rqv65t2";
        String mqttServer = "192.168.1.61";
        String wifiHostName = "smartshunt.local";
        String mqttUser = "mosch";
        String mqttPass = "Mmdp-3325";
        uint16_t mqttPort = 1883;
        String mqttClientID = "diy-smartshunt-bms";
        String mqttTopic = "diy-smartshunt-bms";
        String mqttParameter = "/Param";
        String ntpServer = "192.168.1.254";
};

#endif