//#define _DEBUG_SKETCH_

#ifndef _DEBUG_SKETCH_

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

	Serial.println("Start WIFI");
	if (WiFiManager.init(false, 2, true, "XMasDisplay") == WiFi_Connected){
		Serial.println("WiFi ok");
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

#else

// This sketch is to test the touch controller, nothing is displayed
// on the TFT.  The TFT_eSPI library must be configured to suit your
// pins used. Make sure both the touch chip select and the TFT chip
// select are correctly defined to avoid SPI bus contention.

// Make sure you have defined a pin for the touch controller chip
// select line in the user setup file or you will see "no member"
// compile errors for the touch functions!

// It is a support and diagnostic sketch for the TFT_eSPI library:
// https://github.com/Bodmer/TFT_eSPI

// The "raw" (unprocessed) touch sensor outputs are sent to the
// serial port. Touching the screen should show changes to the x, y
// and z values. x and y are raw ADC readings, not pixel coordinates.

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

//====================================================================

void setup(void) {
  Serial.begin(115200);
  Serial.println("\n\nStarting...");

  tft.init();
}

//====================================================================

void loop() {

  uint16_t x, y;

  tft.getTouchRaw(&x, &y);
  
  Serial.printf("x: %i     ", x);

  Serial.printf("y: %i     ", y);

  Serial.printf("z: %i \n", tft.getTouchRawZ());

  delay(250);

}

//====================================================================

#endif