#include "NextionX3.h"

const string finishSend = "\xFF\xFF\xFF";

Nextion::Nextion() {}

void Nextion::begin(HardwareSerial &serial, uint32_t baud) {
	port = &serial;
	port->begin(baud);
	}

void Nextion::send(string cmd) {
	#if DEBUG == 1
		Serial.print("Send: ");
		Serial.println(cmd.c_str());
	#endif
	port->print(cmd.c_str()); 
	port->print(finishSend.c_str());
	}

void Nextion::update() {
	componentId_t component;
	while (port->available() && counterFF < 3) {
		uint8_t inputByte = port->read();
		if (inputByte != 0xFF) buffer.push_back(inputByte);
		if (inputByte == 0xFF) counterFF++;
		}
	if (counterFF == 3) {
		#if DEBUG == 1 
			Serial.print("Received:");
			for (uint8_t i = 0; i < buffer.size(); i++) {
				Serial.print(" ");
				Serial.print("0x");
				Serial.print(buffer[i], HEX);
				}
			Serial.println("");
		#endif
		counterFF = 0;
		if (buffer[0] == 0x65) { // 
			component.page = buffer[1];
			component.object = buffer[2];
			uint8_t listpos = indexByGuid(component.guid);
			if (listpos < element.size()) element[listpos].component->callback(buffer[3]);
			}
		else if (buffer[0] == 0x66) { // page change
			pageId = buffer[1];
			if (onPage != nullptr) onPage();
			}
		else if (buffer[0] == 0x67) {
			x = buffer[1] << 8;
			x += buffer[2];
			y = buffer[3] << 8;
			y += buffer[4];
			if (buffer[5] == 0) {
				if (onRelease != nullptr) onRelease();
				}
			else if (buffer[5] == 1) {
				if (onTouch != nullptr) onTouch();
				}
			}
		else if (buffer[0] == 0x72) { // formatted string return
			string text = "";
			for (int i = 3; i < (int)buffer.size(); i++) {
				text += (char)buffer[i];
				}
			component.page = buffer[1];
			component.object = buffer[2];
			uint8_t listpos = indexByGuid(component.guid);
			if (listpos < element.size()) {
				element[listpos].component->txt = text;
				element[listpos].component->callback(2);
				}
			}
		else if (buffer[0] == 0x73) { // formatted value return
			int32_t value;
			value = buffer[6];
			value <<= 8;
			value |= buffer[5];
			value <<= 8;
			value |= buffer[4];
			value <<= 8;
			value |= buffer[3];
			component.page = buffer[1];
			component.object = buffer[2];
			uint8_t listpos = indexByGuid(component.guid);
			if (listpos < element.size()) {
				element[listpos].component->val = value;
				element[listpos].component->callback(3);
				}
			}
		buffer.clear();
		}
	}

void Nextion::addComponentList(Component *component) {
	listElement_t myComponent;
	myComponent.component = component;
	myComponent.guid = component->guid();
	if (indexByGuid(myComponent.guid) == 0xFF) {
		element.push_back(myComponent);
		}
	}

int32_t Nextion::nextionValue() {
	bool endOfCommandFound;
	bool error = false;
	uint8_t tempChar = 0;
	uint8_t buffer[4];
	memset(buffer, 0, sizeof(buffer));
	int32_t value = 0;
	uint8_t inputByte = 0;
	uint32_t timer = millis();  
	while (port->available() < 8) {
		if ((millis() - timer) > TIMEOUT) {
			error = true;
	 		break;
			}
		}
	if (port->available() > 7) {
		inputByte = port->read();
		timer = millis();
		while (inputByte != 0x71) {
			if (port->available()) inputByte = port->read();
			if((millis() - timer) > TIMEOUT) {
				error = true;
				break;
				}   
			}
		if (inputByte == 0x71) {
			for (int i = 0; i < 4; i++) buffer[i] = port->read();
			int endBytes = 0;
			timer = millis();
			while (endOfCommandFound == false) {
				tempChar = port->read();
				if (tempChar == 0xFF) {
					endBytes++ ;
					if (endBytes == 3) endOfCommandFound = true;
					}
				else {
					error = true;
					break;
					}
				if ((millis() - timer) > TIMEOUT){
					error = true;
					break;
					}
				}
			}
		}
	if (endOfCommandFound == true) {
		value = buffer[3];
		value <<= 8;
		value |= buffer[2];
		value <<= 8;
		value |= buffer[1];
		value <<= 8;
		value |= buffer[0];
		}
	else error = true;
	if (error) value = 0xFFFFFFFF;
	return value;
	}

string Nextion::nextionText() {
	bool endOfCommandFound = false;
	bool error = false;
	uint8_t tempChar = 0;
	string buf = "";
	uint8_t inputByte;
	uint32_t timer = millis();
	while (port->available() < 4) {
		if ((millis() - timer) > TIMEOUT){
			error = true;
			break;
			}
		}
	if (port->available() > 3) {
		inputByte = port->read();
		timer = millis();
		while (inputByte != 0x70) {
			if (port->available()) inputByte = port->read();
			if((millis() - timer) > TIMEOUT) {
				error = true;
				break;
				}   
			}
		if (inputByte == 0x70) {
			int endBytes = 0;
			timer = millis();
			while (endOfCommandFound == false) {
				if (port->available()) {
					tempChar = port->read();
					if (tempChar == 0xFF) {
						endBytes++ ;
						if (endBytes == 3) endOfCommandFound = true;
						}
					else {
						buf += tempChar;
						}
					}
				if ((millis() - timer) > TIMEOUT){
					error = true;
					break;
					}
				}
			}
		}
	if (error) buf = "Error";
	return buf;
	}

uint8_t Nextion::indexByGuid(uint16_t guid) {
	uint8_t index = 0xFF;
	for (uint8_t i = 0; i < element.size(); i++) {
		if (element[i].guid == guid) {
			index = i;
			break;
			}
		}
	return index;
	}

void Nextion::page(uint8_t pageId) {
	string message = "page " + to_string(pageId);
	send(message);
	}

uint8_t Nextion::page() {
	return pageId;
	}

void Nextion::callbackPage(void (*onPage)() = nullptr) {
	this->onPage = onPage;
	}

void Nextion::callbackTouch(void (*onTouch)() = nullptr) {
	this->onTouch = onTouch;
	}

void Nextion::callbackRelease(void (*onRelease)() = nullptr) {
	this->onRelease = onRelease;
	}

uint16_t Nextion::coordinateX() {
	return x;
	}

uint16_t Nextion::coordinateY() {
	return y;
	}

void Nextion::cls(uint16_t color) {
	string message = "cls " + to_string(color);
	send(message);
	}

void Nextion::line(uint16_t x1, uint16_t y1, int16_t x2, uint16_t y2, uint16_t color) {
	string message = "line " + to_string(x1) + ',' + to_string(y1) + ',' + to_string(x2) + ',' + to_string(y2) + ',' + to_string(color);
	send(message);
	}

void Nextion::rectangle(uint16_t x, uint16_t y, int16_t width, uint16_t height, uint16_t color) {
	string message = "draw " + to_string(x) + ',' + to_string(y) + ',' + to_string(x + width) + ',' + to_string(y + height) + ',' + to_string(color);
	send(message);
	}


void Nextion::rectangleFilled(uint16_t x, uint16_t y, int16_t width, uint16_t height, uint16_t color) {
	string message = "fill " + to_string(x) + ',' + to_string(y) + ',' + to_string(width) + ',' + to_string(height) + ',' + to_string(color);
	send(message);
	}

void Nextion::circle(uint16_t x, uint16_t y, uint16_t radius, uint16_t color) {
	string message = "cir " + to_string(x) + ',' + to_string(y) + ',' + to_string(radius) + ',' + to_string(color);
	send(message);
	}

void Nextion::circleFilled(uint16_t x, uint16_t y, uint16_t radius, uint16_t color) {
	string message = "cirs " + to_string(x) + ',' + to_string(y) + ',' + to_string(radius) + ',' + to_string(color);
	send(message);
	}

void Nextion::text(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t font, uint16_t colorfg, uint16_t colorbg, alignhor_t alignx, alignver_t aligny, fill_t fillbg, string text) {
	string message = "xstr " + to_string(x) + ',' + to_string(y) + ',' + to_string(width) + ',' + to_string(height) + ',' + to_string(font) + ',' + to_string(colorfg) + ',' + to_string(colorbg) + ',' + to_string(alignx) + ',' + to_string(aligny) + ',' + to_string(fillbg) + ','  + '\"' + text + '\"';
	send(message);
	}

void Nextion::picture(uint16_t x, uint16_t y, uint8_t id) {
	string message = "pic " + to_string(x) + ',' + to_string(y) + ',' + to_string(id);
	send(message);
	}

void Nextion::pictureCrop(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t id) {
	string message = "picq " + to_string(x) + ',' + to_string(y) + ',' + to_string(width) + ',' + to_string(height) + ',' + to_string(id);
	send(message);
	}

void Nextion::pictureCropX(uint16_t destx, uint16_t desty, uint16_t width, uint16_t height, uint16_t srcx, uint16_t srcy, uint8_t id) {
	string message = "xpic " + to_string(destx) + ',' + to_string(desty) + ',' + to_string(width) + ',' + to_string(height) + ',' + to_string(srcx) + ',' + to_string(srcy) + ',' + to_string(id);
	send(message);
	}


Component::Component(Nextion &nextion, uint8_t pageId, uint8_t objectId) {
	this->nextion = &nextion;
	myId.page = pageId;
	myId.object = objectId;
	nextion.addComponentList(this);
	}

uint16_t Component::guid() {
	return myId.guid;
	}

void Component::callbackTouch(void (*onTouch)() = nullptr) {
	this->onTouch = onTouch;
	}

void Component::callbackRelease(void (*onRelease)() = nullptr) {
	this->onRelease = onRelease;
	}

void Component::callbackValue(void (*onValue)() = nullptr) {
	this->onValue = onValue;
	}

void Component::callbackText(void (*onText)() = nullptr) {
	this->onText = onText;
	}

void Component::attributeNumber(string attr, int32_t number) {
	string message = "p[";
	message += to_string(myId.page);
	message += "].b[";
	message += to_string(myId.object);
	message += "].";
	message += attr;
	message += "=";
	message += to_string(number);
	nextion->send(message);
	}

void Component::attributeText(string attr, string text) {
	string message = "p[";
	message += to_string(myId.page);
	message += "].b[";
	message += to_string(myId.object);
	message += "].";
	message += attr;
	message += "=\"";
	message += text;
	message += "\"";
	nextion->send(message);
	}

int32_t Component::attributeNumber(string attr) {
	string message = "get p[";
	message += to_string(myId.page);
	message += "].b[";
	message += to_string(myId.object);
	message += "].";
	message += attr;
	nextion->send(message);
	int32_t returnValue = nextion->nextionValue();
	return returnValue;
	}

string Component::attributeText(string attr) {
	string message = "get p[";
	message += to_string(myId.page);
	message += "].b[";
	message += to_string(myId.object);
	message += "].";
	message += attr;
	nextion->send(message);
	string returnText = nextion->nextionText();
	return returnText;
	}

void Component::value(int32_t number) {
	attributeNumber("val", number);
	}

int32_t Component::value() {
	return val;
	}

int32_t Component::getValue() {
	return attributeNumber("val");
	}

void Component::text(string txt) {
	attributeText("txt", txt);
	}

string Component::text() {
	return txt;
	}

string Component::getText() {
	return attributeText("txt");
	}

void Component::callback(uint8_t event) {
	switch (event) {
		case 0:
			if (onRelease != nullptr) onRelease();
			break;
		case 1:
			if (onTouch != nullptr) onTouch();
			break;
		
		case 2:
			if (onText != nullptr) onText();
			break;
		case 3:
			if (onValue != nullptr) onValue();
			break;
		}
	}

uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) {
	red >>= 3;
	green >>= 2;
	blue >>= 3;
	return (red << 11) | (green << 5) | blue;
	}
