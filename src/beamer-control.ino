#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <Espanol.h>
#include <SoftwareSerial.h>
#include "beamer-control.h"

const byte rxPin = 13;
const byte txPin = 15;

// set up a new serial object
SoftwareSerial mySerial (rxPin, txPin);

#define OTA_PASSWORD "bugsbunny"

const String ssid     = "HaSi-Kein-Internet-Legacy";
const String password = "bugsbunny";
const String broker   = "mqtt.hasi";
int port              = 1883;

int lock = 0;

#define STR_LEN 100

void setup()
{
	Serial.begin(9600);
    mySerial.begin(9600);

    Espanol.setDebug(true);
    Espanol.begin(ssid, password, "beamercontrol", broker, port);

	Espanol.subscribe("hasi/beamercontrol");

	Espanol.setCallback([](char *topic, byte *payload, unsigned int length) {
		int i;

		String payload_string = "";
		for(i=0;i<length;i++){
			payload_string += char(payload[i]);
		}

		Serial.println(payload_string);
		for(i=0;i<NUM_CMDS;i++){
			if(payloads[i] == payload_string){
				mySerial.print(codes[i]);
				Serial.println(codes[i]);
				break;
			}
		}

		if(i<NUM_CMDS){
			Espanol.publish("hasi/beamercontrol/response","Success");
		}
		else{
			Espanol.publish("hasi/beamercontrol/response","Command unknown");
		}
    });

    ArduinoOTA.setHostname("beamercontrol");
    ArduinoOTA.setPassword(OTA_PASSWORD);
    ArduinoOTA.begin();
}

void loop()
{
    ArduinoOTA.handle();
    Espanol.loop();
}

