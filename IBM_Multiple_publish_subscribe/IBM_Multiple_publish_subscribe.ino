

#include <ESP8266WiFi.h>
#include <PubSubClient.h> 
#include <ArduinoJson.h> 

//-------- Customise these values -----------
const char* ssid = "OnePlus";
const char* password = "subham123";

#define ORG "w3zu71"
#define DEVICE_TYPE "NodeMcu"
#define DEVICE_ID "demoDevice"
#define TOKEN "Xa5!fEa4n-nsZ&vn?Y"
//-------- Customise the above values --------

char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

const char eventTopic[] = "iot-2/evt/status/fmt/json";
const char cmdTopic[] = "iot-2/cmd/led/fmt/json";

const char eventTopic1[] = "iot-2/evt/dataUS/fmt/json";
const char cmdTopic1[] = "iot-2/cmd/ultrasonic/fmt/json";


WiFiClient wifiClient;
void callback(char* topic, byte* payload, unsigned int payloadLength) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  for (int i = 0; i < payloadLength; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(D0, HIGH);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(D0, LOW);  // Turn the LED off by making the voltage HIGH
  }

}
PubSubClient client(server, 1883, callback, wifiClient);

int publishInterval = 5000; // 5 seconds//Send adc every 5sc
long lastPublishMillis;

void setup() {
  Serial.begin(9600); Serial.println();
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(D0,OUTPUT);
  wifiConnect();
  mqttConnect();
}

void loop() {
  if (millis() - lastPublishMillis > publishInterval) {
    publishData();
    lastPublishMillis = millis();
  }

  if (!client.loop()) {
    mqttConnect();
  }
}

void wifiConnect() {
  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("nWiFi connected, IP address: "); Serial.println(WiFi.localIP());

}

void mqttConnect() {
  if (!!!client.connected()) {
    Serial.print("Reconnecting MQTT client to "); Serial.println(server);
    while (!!!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
    if (client.subscribe(cmdTopic)) {
      Serial.println("subscribe to responses OK");
    } else {
      Serial.println("subscribe to responses FAILED");
    }
    Serial.println();
  }
}


void publishData() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);

  String payload = "{\"d\":{\"adc\":";
  payload += String(sensorValue, DEC);
  payload += "}}";

  Serial.print("Sending 1st payload: "); Serial.println(payload);

  if (client.publish(eventTopic, (char*) payload.c_str())) {
    Serial.println("1st Publish OK");
  } else {
    Serial.println("1st Publish FAILED");
  }

  Serial.print("Sending 2nd payload: "); Serial.println(payload);

  if (client.publish(eventTopic1, (char*) payload.c_str())) {
    Serial.println("2nd Publish OK");
  } else {
    Serial.println("2nd Publish FAILED");
  }
  
}





