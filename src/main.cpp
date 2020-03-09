#include <Arduino.h>
#include "Thing.h"
#include "WebThingAdapter.h"

// TODO: Hardcode your wifi credentials here (and keep it private)
const char *ssid = "Broloc";
const char *password = "g9PomPTFfChR9eWemq";
const int controle = D8;

#if defined(LED_BUILTIN)
const int ledPin = LED_BUILTIN;
#else
const int ledPin = 13; // manually configure LED pin
#endif

WebThingAdapter *adapter;

const char *remoteTypes[] = {"OnOffSwitch", "Led", nullptr};
ThingDevice remote("remote", "Porte résidence", remoteTypes);
ThingProperty remoteOn("on", "", BOOLEAN, "OnOffProperty");

bool lastOn = false;

void setup(void)
{
  pinMode(ledPin, OUTPUT);
  pinMode(controle, OUTPUT);
  digitalWrite(ledPin, HIGH);
  digitalWrite(controle, HIGH);
  Serial.begin(115200);
  Serial.println("");
  Serial.print("Connecting to \"");
  Serial.print(ssid);
  Serial.println("\"");
#if defined(ESP8266) || defined(ESP32)
  WiFi.mode(WIFI_STA);
#endif
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  bool blink = true;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    digitalWrite(ledPin, blink ? LOW : HIGH); // active low led
    blink = !blink;
  }
  digitalWrite(ledPin, HIGH); // active low led

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  adapter = new WebThingAdapter("wemos_d1_mini", WiFi.localIP());

  remote.addProperty(&remoteOn);
  adapter->addDevice(&remote);
  adapter->begin();
  Serial.println("HTTP server started");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.print("/things/");
  Serial.println(remote.id);
}

void loop(void)
{
  adapter->update();
  bool on = remoteOn.getValue().boolean;
  digitalWrite(ledPin, on ? LOW : HIGH); // active low led
  digitalWrite(controle, on ?   HIGH : LOW);     // active low led
  if (on != lastOn)
  {
    Serial.print(remote.id);
    Serial.print(": ");
    Serial.println(on);
  }
  lastOn = on;
  if (lastOn == true)
  {
    delay(2000);
    ThingDataValue newValue;
    newValue.boolean = false;
    remoteOn.setValue(newValue);
  }
}