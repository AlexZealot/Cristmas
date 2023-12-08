#include <Arduino.h>
#include "WiFiManager.h"
#include "gui/GUI.h"
#include "connect/UDPConnect.h"
#include <SPI.h>
#include <SPIFFS.h>

void setup() {
	WiFiManager.setCallback([](bool on){
		GUI.setBacklight(on?128:0);
	});

	GUI.init();
	GUI.blackScreen();
	GUI.setBacklight(128);
	GUI.showWiFiLogo();	

	if (WiFiManager.init(false, 2, true, "XMasDisplay") == WiFi_Connected){
		UDPConnect.init();
		GUI.blackScreen();
	}		

	GUI.setBacklight(128);
}

void loop() {
	if (WiFiManager.update() == WiFi_Connected){
		UDPConnect.update();
		GUI.update();
	}	
}