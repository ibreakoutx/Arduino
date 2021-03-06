

//flag for saving data
bool shouldSaveConfig = false;

#include <Ticker.h>
Ticker flipper;

#define BLINK_RATE_FAST 1
#define BLINK_RATE_SLOW 0

void flip() {
  digitalWrite(STATUS_LED_PIN, LOW);
  delayMicroseconds(50000);
  digitalWrite(STATUS_LED_PIN, HIGH);  
}

void ledBlinkOn(int rate ) {
  if ( rate == BLINK_RATE_FAST )
   flipper.attach(0.4,flip);
  else
   flipper.attach(2.0,flip);
}

void ledBlinkOff() {
  flipper.detach();
}

//callback notifying us of the need to save config
void saveConfigCallback () {
  
  #ifdef DBG_PRINT
  Serial.println("Should save config");
  #endif
  
  shouldSaveConfig = true;
}

void configModeCallback (WiFiManager *myWiFiManager) {
  ledBlinkOn(BLINK_RATE_SLOW);
  
  #ifdef DBG_PRINT
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  Serial.println(myWiFiManager->getConfigPortalSSID());
  #endif
}

//Read saved parameters (sensor_id and iv_password) from Filesystem.
void readParam() {

  //read configuration from FS json
  #ifdef DBG_PRINT
  Serial.println("mounting FS...");
  #endif
  
  if (SPIFFS.begin()) {
    #ifdef DBG_PRINT
    Serial.println("mounted file system");
    #endif
    
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      #ifdef DBG_PRINT
      Serial.println("reading config file");
      #endif
      
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        #ifdef DBG_PRINT
        Serial.println("opened config file");
        #endif
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");

          strcpy(sensor_id, json["sensor_id"]);
          strcpy(iv_password, json["iv_password"]);
        } else {
          ;
          #ifdef DBG_PRINT
          Serial.println("failed to load json config");
          #endif
        }
      }
    }
  } else {
    ;
    #ifdef DBG_PRINT
    Serial.println("failed to mount FS");
    #endif
  }
  //end read
}

void configureWiFi() {

  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_sensor_id("sensor_id", "sensor id", sensor_id, 30);
  WiFiManagerParameter custom_iv_password("iv_password", "encrypt password", iv_password, 30);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  #ifdef DBG_PRINT
    wifiManager.setDebugOutput(true);
  #else
    wifiManager.setDebugOutput(false);
  #endif

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //Callback before entering configMode
  wifiManager.setAPCallback(configModeCallback);

  //set static ip
  //wifiManager.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
  
  //add all your parameters here
  wifiManager.addParameter(&custom_sensor_id);
  wifiManager.addParameter(&custom_iv_password);

  //reset settings - for testing
  //wifiManager.resetSettings();

  //set minimu quality of signal so it ignores AP's under that quality
  //defaults to 8%
  wifiManager.setMinimumSignalQuality();
  
  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager.setTimeout(120);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("AutoConnectAP", "password")) {
    #ifdef DBG_PRINT
    Serial.println("failed to connect and hit timeout");
    #endif
    ledBlinkOn(BLINK_RATE_FAST);
    delay(3000);
    ledBlinkOff();
    
    //reset and try again, or maybe put it to deep sleep
    ESP.deepSleep(0);
    //ESP.reset();
    delay(5000);
  }

  #ifdef DBG_PRINT
  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  #endif

  //read updated parameters
  strcpy(sensor_id, custom_sensor_id.getValue());
  strcpy(iv_password, custom_iv_password.getValue());

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    #ifdef DBG_PRINT
    Serial.println("saving config");
    #endif
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["sensor_id"] = sensor_id;
    json["iv_password"] = iv_password;

    if (SPIFFS.begin()) {
      File configFile = SPIFFS.open("/config.json", "w");
      if (!configFile) {
        ;
        #ifdef DBG_PRINT
        Serial.println("failed to open config file for writing");
        #endif
      }
      else {
        json.printTo(Serial);
        json.printTo(configFile);
        configFile.close();
        //end save
      }
    }
    else {
      ;
      #ifdef DBG_PRINT
      Serial.println("failed to mount SPIFFs");
      #endif
    }
  }

  #ifdef DBG_PRINT
  Serial.println("local ip");
  Serial.println(WiFi.localIP());
  #endif
  
}

