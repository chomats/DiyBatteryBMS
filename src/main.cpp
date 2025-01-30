#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "config.h"


ConfigClass config;

// Replace with your network credentials
const char* ssid     = config.wifiSSID.c_str();
const char* password = config.wifiPass.c_str();
const char* poolServerName = config.ntpServer.c_str();

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, poolServerName);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  log_d("Connecting to %s",ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    log_d(".");
  }
  // Print local IP address
  log_d("WiFi connected - IP address: %s", WiFi.localIP().toString());

// Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(3600);
}
void loop() {
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  // log_d("formattedDate: %s", formattedDate.substring(0, formattedDate.length()-1));

  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  log_d("DATE: %s", dayStamp);

  // Extract time
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  log_d("HOUR: %s", timeStamp);
  delay(1000);
}