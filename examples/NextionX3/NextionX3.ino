#include "Arduino.h"
#include "NextionX3.h"

Nextion nextion;

Component version(nextion, 0, 1);
Component number(nextion, 0, 5);
Component text(nextion, 0, 6);
Component momentaryButton(nextion, 0, 2);
Component momentaryLed(nextion, 0, 11);
Component toggleButton(nextion, 0, 12);
Component toggleLed(nextion, 0, 13);
Component slider(nextion, 0, 7);
Component checkbox(nextion, 0, 8);
Component xfloat(nextion, 0, 4);

uint32_t timing;
#define TIMER 5000

void ledOn() {
	digitalWrite(LED_BUILTIN, HIGH);
	momentaryLed.attributeNumber("bco", RED);
	}

void ledOff() {
	digitalWrite(LED_BUILTIN, LOW);
	momentaryLed.attributeNumber("bco", BLACK);
	}

void ledToggle() {
	if (toggleButton.value() == 1) {
		digitalWrite(LED_BUILTIN, HIGH);
		toggleLed.attributeNumber("bco", RED);
		}
	else if (toggleButton.value() == 0){
		digitalWrite(LED_BUILTIN, LOW);
		toggleLed.attributeNumber("bco", BLACK);
		}
	}

void check() {
	if (checkbox.value()) Serial.println("Checkbox True");
	else Serial.println("Checkbox False");
	}

void sliderValue() {
	Serial.print("Slider Value: ");
	Serial.println(slider.value());
	}

void numberValue() {
	Serial.print("Number Value: ");
	Serial.println(number.value());
	}

void xfloaValue() {
	Serial.print("XFloat Value: ");
	Serial.println(xfloat.text().c_str());
	}

void textTxt() {
	Serial.print("Text: ");
	Serial.println(text.text().c_str());
	}

void pageId() {
	Serial.print("Page ID: ");
	Serial.println(nextion.page());
	}

void touch() {
	Serial.print("Touchscreen Coordinates X: ");
	Serial.print(nextion.coordinateX());
	Serial.print(" Y: ");
	Serial.println(nextion.coordinateY());
	}

void release() {
	Serial.println("Touchscreen released!");
	}

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(9600);
	delay(1000);
	nextion.begin();
	nextion.callbackPage(pageId);
	nextion.callbackTouch(touch);
	nextion.callbackRelease(release);
	nextion.cls(BLACK);
	nextion.picture(320, 200, 0);
	delay(1000);
	nextion.page(0);
	version.attributeText("txt", "v.1.0.0");
	number.value(5);
	text.text("hello");
	momentaryButton.callbackTouch(ledOn);
	momentaryButton.callbackRelease(ledOff);
	toggleButton.callbackValue(ledToggle);
	slider.callbackValue(sliderValue);
	number.callbackValue(numberValue);
	xfloat.callbackText(xfloaValue);
	text.callbackText(textTxt);
	checkbox.callbackValue(check);
	timing = millis();
	}

void loop() {
	nextion.update();
	if (millis() > (timing + TIMER)) {
		int32_t valueNumber = number.getValue();
		int32_t valueSlider = slider.getValue();
		string textText = text.getText();
		xfloat.getValue();
		Serial.print("Text Field String: ");
		Serial.println(textText.c_str());
		Serial.print("Number Field Value: ");
		Serial.println(valueNumber);
		Serial.print("XFloat Field Value: ");
		Serial.println(xfloat.text().c_str());
		Serial.print("Slider Value: ");
		Serial.println(valueSlider);
		Serial.println();
		timing = millis();
		}
	}
