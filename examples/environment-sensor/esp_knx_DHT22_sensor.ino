
/*
 * This is an example showing a simple environment sensor DHT22.
 * This sketch was tested on a ESP-01 and ESP-12N
 * "Serial.println("#################");" in void loop for testing 
 * and can be removed.
 */

#include <DHT.h>
#include "esp-knx-ip.h"

// WiFi config here
const char* ssid = "default";         // SSID
const char* pass = "";                // PASSWD

#define DHTPIN 2                      // DHT22 pin
#define DHTTYPE           DHT22       // DHT 22 (AM2302)
#define UPDATE_INTERVAL   10000       // This is the default send interval

unsigned long next_change = 0;
    
    float temp_old;
    float hum_old;
    unsigned long wasSendTemp;
    unsigned long wasSendHum;
    
config_id_t temp_ga, hum_ga;
config_id_t hostname_id;
config_id_t temp_rate_id;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  Serial.println("DHTxx test!");

  // Register the group addresses where to send information to and the other config
  temp_ga = knx.config_register_ga("Temperature");
  hum_ga = knx.config_register_ga("Humidity");
  hostname_id = knx.config_register_string("Hostname", 20, String("env"));
  temp_rate_id = knx.config_register_int("Send rate (ms)", UPDATE_INTERVAL);

  // Load previous values from EEPROM
  knx.load();
 
  // Init WiFi
  WiFi.hostname(knx.config_get_string(hostname_id));
  WiFi.begin(ssid, pass);

  Serial.println("");
  Serial.print("[Connecting]");
  Serial.print(ssid);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to wifi");
  Serial.println(WiFi.localIP());

  // Start knx
  knx.start();
}
    
    void loop() {
  // Process knx events, e.g. webserver
  knx.loop();
    
  unsigned long now = millis();
  unsigned long CycleInterval = knx.config_get_int(temp_rate_id);

  if (temp_old == 0.0) {
    temp_old = dht.readTemperature();
  }
  if (hum_old == 0.0) {
    hum_old =  dht.readHumidity();
    delay(1000);
  }

  if (next_change < now)
  {
    next_change = now + CycleInterval;
    
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    
    Serial.println("#################");
    Serial.print("T: ");
    Serial.print(temp);
    Serial.print("°C H: ");
    Serial.println(hum);
    Serial.println("#################");
      
    if ( (temp - temp_old > 0.3) || (temp_old - temp > 0.3))  {
        Serial.println("################# TEMP");
        Serial.println(temp - temp_old);
        Serial.println("#################");
        knx.write2ByteFloat(knx.config_get_ga(temp_ga), temp);
        temp_old = temp;
        wasSendTemp = now;
        }
   else if   ( now - wasSendTemp > CycleInterval * 10 ){
        knx.write2ByteFloat(knx.config_get_ga(temp_ga), temp);
        temp_old = temp;
        wasSendTemp = now;       
        Serial.println("################# TEMP");
        Serial.println(wasSendTemp);
        Serial.println("#################");
        }
   else {
        Serial.println("################# TEMP");
        Serial.println( "Not send Temp" );
        Serial.println("#################");    
        }
   if ( (hum - hum_old > 1.0) || (hum_old - hum > 1.0)) {
        Serial.println("################# HUM");
        Serial.println( hum - hum_old);
        Serial.println("#################");
        knx.write2ByteFloat(knx.config_get_ga(hum_ga), hum);
        hum_old = hum;  
        wasSendHum = now; 
        }
    else if ( now - wasSendHum > CycleInterval * 10 ){
        knx.write2ByteFloat(knx.config_get_ga(hum_ga), hum);
        hum_old = hum;  
        wasSendHum = now;
        Serial.println("################# HUM");
        Serial.println( wasSendHum );
        Serial.println("#################");
        }
   else {
        Serial.println("################# HUM");
        Serial.println( "Not send Hum" );
        Serial.println("#################");    
        }
    delay(CycleInterval/10);
  }
}
