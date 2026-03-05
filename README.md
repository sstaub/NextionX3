# NextionX3
A new, alternative and universal library to interact with Nextion HMI displays from Arduino and compatible MCUs. The library use the c++ std:: library, with string and vector libraries, so AVRs are not supported.

## General information
To be most universal, this library allows (in opposite to the official library) the use of multiple Nextion HMI displays connected to the same MCU under the condition to have enough hardware or software emulated serial ports (UARTs). 

You can use the Serial1, Serial2 and Serial3 ... ports to connect to the Nextion HMIs, while keeping the default Serial port free for debugging in the Serial Monitor of the Arduino IDE.

A simple debug function implemented for send and receive.

The library was tested with a Nextion Edge 4.3" display and a Raspberry Pi Pico.

For use with STM32duino you must declare the HardwareSerial object first e.g.

```cpp
//                     RX    TX
HardwareSerial Serial1(PA10, PA9);
```

## Features
- Dynamic memory handling
- New parsers, functions and callbacks for page and xy coordintes
- Special parser for user return codes
- Using std::string for text, for convert use `.c_str()` function
- Avoiding templates for better code reading
- New API

## Example

In the examples you will find a NextionX3.HMI file. This file file was created for the Nextion Edge 4.3" display but can simply modified with the Nextion editor. The example directory also contain a precompiled version NextioX3.tft file for upload with SD card and the used fonts.

![NextionX3 .hmi file](https://github.com/sstaub/NextionX3/blob/main/images/NextionX3.png?raw=true)
NextionX3 HMI demo file

The Nextionx3.ino shows the advantages of the library.

```cpp
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
	momentaryLed.attribute("bco", RED);
	}

void ledOff() {
	digitalWrite(LED_BUILTIN, LOW);
	momentaryLed.attribute("bco", BLACK);
	}

void ledToggle() {
	if (toggleButton.value() == 1) {
		digitalWrite(LED_BUILTIN, HIGH);
		toggleLed.attribute("bco", RED);
		}
	else if (toggleButton.value() == 0){
		digitalWrite(LED_BUILTIN, LOW);
		toggleLed.attribute("bco", BLACK);
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
	version.attribute("txt", "v.1.0.0");
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

```

# Documentation

## Special user return codes
The new user return codes allows you to send text and values in real time instead of using the time critical `get`function. 
The return code is handled in the Nextion `update()`function.
The callbacks handling is done in the Component object.
The example shows using this methods including the new keyboard layout and the build in handling.

### Return code 0x72 for txt
```
// for text t<x> this is handled in the keyboard Enter button in Touch Release at the end before `page loadpageid.val`
printh 72 // special code for text return
prints loadpageid.val,1 // current page
prints loadcmpid.val,1 // current component ID
prints input.txt,0 // text
printh ff ff ff // end of message

```
### Return code 0x73 for val
```
// for slider h0 this code must done in Touch Release and Move
printh 73 // special code for value return
prints dp,1 // page ID
prints h0.id,1 // component ID
prints h0.val,0 // value 4 byte
printh ff ff ff // end of message
```

## Nextion class

### Object Constructor
```cpp
Nextion
```

Creates a display object, this can used for multiple displays.

**Example**

```cpp
Nextion nextion;
```

### begin()
```cpp
void begin(HardwareSerial &serial = Serial1, uint16_t baud = 9600);
```
- **serial** pointer to Serial object you want to use, default `Serial1`
- **baud** the baud rate, default is 9600

Method to initialize the communication.<br>
This must done in the Arduino ```setup()``` function.

**Example**

```cpp
void setup() {
  nextion.begin(softSerial); // for use with Softserial
  nextion.begin(Serial2, 115200); // for use with Serial2 with higer baud rate
  nextion.begin(); // for use with default Serial1
  }
```

### update()
```cpp
void update();
```

This must done in the Arduino ```loop()``` function.

**Example**

```cpp
void loop() {
  nextion.update();
  }
```

### send()
```cpp
void send(string cmd);
```
- **cmd** command string

Send a raw command string to the display.

**Example**
```cpp
nextion.send("cir 50,50,20,WHITE");
```

## Page handling
`sendme` must set in PostInitialize tab of the page settings, to get page.

### page() set
```cpp
void page(uint8_t pageId);
```
- **pageId** page number

Set a current page.

**Example**
```cpp
nextion.page(1);
```

### pageCallback()
```cpp
void callbackPage(void (*onPage)());
```
- **onPage** callback function for page change

Set the callback function.

**Example**
```cpp
nextion.callbackPage(pageId);
```

### page() get
```cpp
uint8_t page();
```

Get the current page number using return code 0x66, this function should used inside the page callback.

**Example**
```cpp
void pageId() {
	Serial.print("Page ID: ");
	Serial.println(nextion.page());
	}
```

## Coordinate handling
`sendxy=1` must set in PostInitialize of the page setting.

### callbackTouch()
```cpp
void callbackTouch(void (*onTouch)());
```
- **onTouch** callback function for screen touch

Set the callback function for touching the display.

**Example**
```cpp
nextion.callbackTouch(touch);
```

### callbackRelease()
```cpp
void callbackRelease(void (*onRelease)());
```
- **onRelease** callback function for screen release
Set the callback function for release display.

**Example**
```cpp
nextion.callbackRelease(release);
```

### coordinateX(), coordinateY()
```cpp
uint16_t coordinateX();
uint16_t coordinateY();
```
Get the x and y coordinate.

**Example**
```cpp
void touch() {
	Serial.print("Touchscreen Coordinates X: ");
	Serial.print(nextion.coordinateX());
	Serial.print(" Y: ");
	Serial.println(nextion.coordinateY());
	}
void release() {
	Serial.println("Touchscreen released!");
	}
```

## Nextion graphic functions are documented at the end of the document!

## Component class

```cpp
Component(NexComm_t &nextion, uint8_t pageId, uint8_t objectId);
```
- **&nextion** pointer to Nextion object you want to use for
- **pageId** the page ID number for the component
- **objectId** the object ID number for the component

Creates a component object.

**Example**

```cpp
Component momentaryButton(nextion, 0, 2); // page 0, id 2
```

### callbackTouch()
```cpp
void callbackTouch(void (*onTouch)());
```
- **onTouch** callback function for touch an object

Add a callback function for the touch event.

**Example**

```cpp
momentaryButton.callbackTouch(ledOn);
```

### callbackRelease()
```cpp
void callbackRelease(void (*onRelease)());
```
- **onRelease** callback function for release an object

Add a callback function for the release event.

**Example**

```cpp
momentaryButton.callbackRelease(ledOff);
```

### callbackValue()
```cpp
void callbackValue(void (*onValue)());
```
- **onValue** callback function for value

Add a callback function for value change.

**Example**

```cpp
slider.callbackValue(sliderValue);
```

### callbackText()
```cpp
void callbackText(void (*onText)());
```
- **onText** callback function for text

Add a callback function for text change.

**Example**

```cpp
text.callbackText(textTxt);
```

### value() set
```cpp
void value(int32_t number);
```
- **number** argument value

Set a value.

**Example**

```cpp
number.value(5);
```

### value() get from user return code 0x73
```cpp
int32_t value();
```

Get the value from user return code 0x73.

**Example**

```cpp
void numberValue() {
	Serial.print("Number Value: ");
	Serial.println(number.value());
	}
```

### value() get
```cpp
int32_t getValue();
```

Get the value from return code 0x70.

**Example**

```cpp
int32_t valueNumber = number.getValue();
```

### text() set
```cpp
void text(string txt);
```
- **txt** text string

Set a text.

**Example**

```cpp
text.text("hello");
```

### text() get
```cpp
string text();
```

Get the text from user return code 0x72.

**Example**

```cpp
void textTxt() {
	Serial.print("Text: ");
	Serial.println(text.text().c_str());
	}
```

### getText()  
```cpp
string getText();
```

Get the text of the object by return code 0x71.

### Universal attribute() set
```cpp
void attributeNumber(string attr, int32_t number);
void attributeText(string attr, string text);
```
- **attr** attribute as a string
- **number** argument value
- **text** argument text

Set an attribute with a value or text.

**Example**

```cpp
number.attribute("val", 5);
text.attribute("txt", "v.1.0.0");
```

**Example**

```cpp
string textText = text.getText();
```

### Universal attribute() get
```cpp
int32_t attributeNumber(string attr);
string attributeText(string attr);
```
- **attr** attribute as a string

Get an attribute with a value or text.

**Example**

```cpp
int32_t valueNumber = number.attributeNumber("val", 5);
string textText = text.attributeText("txt", "v.1.0.0");
```


## Graphic Methods for *Nextion*

### Graphic Enumarations for text objects

```cpp
enum fill_t { // background fill modes
	CROP,
	SOLID,
	IMAGE,
	NOFILL
	};

enum alignhor_t { // horizontal alignment
	LEFT,
	CENTER,
	RIGHT
	};

enum alignver_t { // vertical alignment
	TOP,
	MIDDLE,
	BOTTOM
	}
```

### Colors

There are some colors predefined.

```cpp
#define BLACK      0x0000
#define BLUE       0x001F
#define RED        0xF800
#define GREEN      0x07E0
#define CYAN       0x07FF
#define MAGENTA    0xF81F
#define YELLOW     0xFFE0
#define LIGHT_GREY 0xBDF7
#define GREY       0x8430
#define DARK_GREY  0x4208
#define WHITE      0xFFFF
```

### color565()

```cpp
uint16_t color565(uint8_t red, uint8_t green, uint8_t blue);
```

Convert RGB 8bit values to the 16bit 565 format used by Nextion.


### cls()
```cpp
void cls(uint16_t color);
```

Clears the complete screen with a given color.

**Example**

```cpp
nextion.cls(BLACK);
```

### line()
```cpp
void line(uint16_t x1, uint16_t y1, int16_t x2, uint16_t y2, uint16_t color)
```
- **x1** start point x1
- **y1** start point y1
- **x2** end point x2
- **y1** end point y2
- **color** color in 565 16bit format

Draw a line.

**Example**

```cpp
nextion.line(50, 50, 100, 100, RED);
```

### circle()
```cpp
void circle(uint16_t x, uint16_t y, uint16_t radius, uint16_t color);
```
- **x** center point x
- **y** center point y
- **radius** circle radius
- **color** color in 565 16bit format

Draw a circle.

**Example**

```cpp
nextion.circle(200, 200, 50, BLUE);
```

### circleFilled()
```cpp
void circleFilled(uint16_t x, uint16_t y, uint16_t radius, uint16_t color);
```
- **x** center point x
- **y** center point y
- **radius** circle radius
- **color** color in 565 16bit format

Draw a filled circle.

**Example**

```cpp
nextion.circleFilled(200, 200, 50, BLUE);
```

### rectangle()
```cpp
void rectangle(uint16_t x, uint16_t y, int16_t width, uint16_t height, uint16_t color);
```
- **x** start point x
- **y** start point y
- **width** rectangle width
- **height** rectangle height
- **color** color in 565 16bit format

Draw a rectangle.

**Example**

```cpp
nextion.rectangle(50, 50, 150, 50, YELLOW);
```

### rectangleFilled()
```cpp
void rectangleFilled(uint16_t x, uint16_t y, int16_t width, uint16_t height, uint16_t color);
```
- **x** start point x
- **y** start point y
- **width** rectangle width
- **height** rectangle height
- **color** color in 565 16bit format

Draw a filled rectangle.

**Example**

```cpp
nextion.rectangleFilled(50, 50, 150, 50, YELLOW);
```

### text()
```cpp
void text(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t font, uint16_t colorfg, uint16_t colorbg, alignhor_t alignx, alignver_t aligny, fill_t fillbg, string text);
```
- **x** start point x
- **y** start point y
- **width** text area width
- **height** text area height
- **font** resource font number
- **colorfg** text foreground color in 565 16bit format
- **colorbg** text background color in 565 16bit format
- **alignx** text horizontal alignment LEFT / CENTER / RIGHT
- **aligny** text horizontal alignment TOP / MIDDLE / BOTTOM
- **fillbg** background fill mode CROP / SOLID / IMAGE / NONE
- **text** text string

Draw a filled rectangle.

**Example**

```cpp
nextion.text(50, 280, 200, 50, 1, WHITE, BLUE, CENTER, MIDDLE, SOLID, "Hello Nextion");
```

### picture()
```cpp
void picture(uint16_t x, uint16_t y, uint8_t id);
```
- **x** upper left x coordinate
- **y** upper left y coordinate
- **id** resource picture id

Draw a picture.

**Example**

```cpp
nextion.picture(320, 100, 0);
```

### pictureCrop()
```cpp
void pictureCrop(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t id);
```
- **x** upper left x coordinate
- **y** upper left y coordinate
- **width** crop width
- **height** crop height
- **id** resource picture id

Draw a croped picture, should only used with fullscreen picture!

**Example**

```cpp
nextion.pictureCrop(100, 100, 50, 50, 0);
```

### pictureCropX()
```cpp
void pictureCropX(uint16_t destx, uint16_t desty, uint16_t width, uint16_t height, uint16_t srcx, uint16_t srcy, uint8_t id);
```
- **destx** upper left x destination coordinate
- **desty** upper left y destination coordinate
- **width** crop width
- **height** crop height
- **srcx** upper left x source coordinate
- **srcy** upper left y source coordinate
- **id** resource picture id

Draw an extended croped picture.

**Example**

```cpp
nextion.pictureCropX(320, 160, 50, 50, 0, 0, 0);
```