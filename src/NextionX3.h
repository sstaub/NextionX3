/*
MIT License

Copyright (c) 2026 Stefan Staub
Copyright (c) 2021 Thierry
Copyright (c) 2020 Athanasios Seitanis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef NEXTION_X3_H
#define NEXTION_X3_H

#include "Arduino.h"

#include <string>
#include <vector>
using namespace std;

#define DEBUG 0 // set 1 for debug send and receive

#define TIMEOUT 100

// color definitions
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

typedef enum Fill {
	CROP,
	SOLID,
	IMAGE,
	NOFILL
	} fill_t;

typedef enum AlignHor {
	LEFT,
	CENTER,
	RIGHT
	} alignhor_t;

typedef enum AlignVer {
	TOP,
	MIDDLE,
	BOTTOM
	} alignver_t;

class Nextion; // forward declararion
class Component; // forward declararion


/**
 * @brief Component Id declaration
 * 
 */
typedef union ComponentIdType {
	uint16_t guid;
	struct {
		uint8_t page;
		uint8_t object;
		};
	} componentId_t;

/**
 * @brief List Elements declaration
 * 
 */
typedef struct ListElement {
	uint16_t guid;
	Component *component;
	} listElement_t;

/**
 * @brief Nextion class declaration
 * 
 */
class Nextion {

	public:

		/**
		 * @brief Construct a new Nex Comm object
		 * 
		 */
		Nextion();

		/**
		 * @brief Start a serial com port
		 * 
		 * @tparam nextionSeriaType 
		 * @param baud serial baudrate, default 9600
		 */

		void begin(HardwareSerial &serial = Serial1, uint32_t baud = 9600);
		/**
		 * @brief Send a command string
		 * 
		 * @param cmd command string
		 */
		void send(string cmd);

		/**
		 * @brief Update the event loop
		 * 
		 */
		void update();

		/**
		 * @brief Set pageId
		 * 
		 * @param page number of the pageId
		 */
		void page(uint8_t pageId);

		/**
		 * @brief Get current pageID by return code 0x66, "sendme" must set in PostInitialize of the page setting
		 * 
		 * @return uint8_t page Id
		 */
		uint8_t page();

		/**
		 * @brief Add a page change callback
		 * 
		 * @param onPage callback function
		 */
		void callbackPage(void (*onPage)());

		/**
		 * @brief Add a touch callback
		 * 
		 * @param onTouch 
		 */
		void callbackTouch(void (*onTouch)());

		/**
		 * @brief Add a release callback
		 * 
		 * @param onRelease 
		 */
		void callbackRelease(void (*onRelease)());

		/**
		 * @brief Get the last touch x coordinate by return code 0x67, "sendxy=1" must set in PostInitialize of the page setting
		 * 
		 * @return uint16_t x coordinate
		 */
		uint16_t coordinateX();

		/**
		 * @brief Get the last touch y coordinate by return code 0x67, "sendxy=1" must set in PostInitialize of the page setting
		 * 
		 * @return uint16_t y coordinate
		 */
		uint16_t coordinateY();

		/**
		 * @brief Clear screen with a given color
		 * 
		 * @param color 
		 */
		void cls(uint16_t color);

		/**
		 * @brief Draw a line
		 * 
		 * @param x1 
		 * @param y1 
		 * @param x2 
		 * @param y2 
		 * @param color 
		 */
		void line(uint16_t x1, uint16_t y1, int16_t x2, uint16_t y2, uint16_t color);

		/**
		 * @brief Draw a circle
		 * 
		 * @param x 
		 * @param y 
		 * @param diameter 
		 * @param color 
		 */
		void circle(uint16_t x, uint16_t y, uint16_t radius, uint16_t color);

		/**
		 * @brief Draw a filled circle
		 * 
		 * @param x 
		 * @param y 
		 * @param diameter 
		 * @param color 
		 */
		void circleFilled(uint16_t x, uint16_t y, uint16_t radius, uint16_t color);

		/**
		 * @brief Draw a rectangle
		 * 
		 * @param x 
		 * @param y 
		 * @param width 
		 * @param height 
		 * @param color 
		 */
		void rectangle(uint16_t x, uint16_t y, int16_t width, uint16_t height, uint16_t color);

		/**
		 * @brief Draw a filled rectangle
		 * 
		 * @param x 
		 * @param y 
		 * @param width 
		 * @param height 
		 * @param color 
		 */
		void rectangleFilled(uint16_t x, uint16_t y, int16_t width, uint16_t height, uint16_t color);

		/**
		 * @brief Draw a text
		 * 
		 * @param x upper left x coordinate
		 * @param y upper left y coordinate
		 * @param width of text area
		 * @param height height of text area
		 * @param font resource font number
		 * @param colorfg foreground color
		 * @param colorbg background color
		 * @param alignx horizontal alignment LEFT, CENTER, RIGHT
		 * @param aligny vertical alignment TOP, CENTER, BOTTOM
		 * @param fillbg background fill mode CROP, SOLID, IMAGE, NOFILL
		 * @param text string content
		 */
		void text(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t font, uint16_t colorfg, uint16_t colorbg, alignhor_t alignx, alignver_t aligny, fill_t fillbg, string text);

		/**
		 * @brief Draw a picture
		 * 
		 * @param x upper left x coordinate
		 * @param y upper left y coordinate
		 * @param id resource picture id
		 */
		void picture(uint16_t x, uint16_t y, uint8_t id);

		/**
		 * @brief Draw a croped picture
		 * 
		 * @param x upper left x coordinate
		 * @param y upper left y coordinate
		 * @param width new width of the picture
		 * @param height new height of the picture
		 * @param id resource picture id
		 */
		void pictureCrop(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t id);

		/**
		 * @brief Draw a advanced croped picture
		 * 
		 * @param destx upper left x coordinate of desination
		 * @param desty upper left y coordinate of desination
		 * @param width new width of the picture
		 * @param height new height of the picture
		 * @param srcx upper left x coordinate of source
		 * @param srcy upper left y coordinate of source
		 * @param id resource picture id
		 */
		void pictureCropX(uint16_t destx, uint16_t desty, uint16_t width, uint16_t height, uint16_t srcx, uint16_t srcy, uint8_t id);

	protected:
		void addComponentList(Component *component);

	private:
		HardwareSerial *port;
		int32_t nextionValue();
		string nextionText();
		uint8_t indexByGuid(uint16_t guid);
		vector<uint8_t> buffer;
		uint8_t counterFF = 0;
		vector<listElement_t> element;
		uint8_t pageId;
		uint16_t x;
		uint16_t y;
		void (*onPage)() = nullptr;
		void (*onTouch)() = nullptr;
		void (*onRelease)() = nullptr;

		friend Component;
	};

/**
 * @brief Component class declaration
 * 
 */
class Component {

	public:

		/**
		 * @brief Construct a new Nex Comp object
		 * 
		 * @tparam NexComm_t 
		 */
		Component(Nextion &nextion, uint8_t pageId, uint8_t objectId);

		/**
		 * @brief Set the number value of an attribute
		 * 
		 * @param number 
		 */
		void attributeNumber(string attr, int32_t number);

		/**
		 * @brief Set the text of an attribute
		 * 
		 * @param text attibute text
		 */
		void attributeText(string attr, string text);

		/**
		 * @brief Get the number value of an attribute by return code 0x71
		 * 
		 * @param attr attribute
		 * @return int32_t attribute number value
		 */
		int32_t attributeNumber(string attr);

		/**
		 * @brief Get the text of an attribute by return code 0x70
		 * 
		 * @param attr attribute
		 * @return string 
		 */
		string attributeText(string attr);

		/**
		 * @brief Add a touch callback
		 * 
		 * @param onTouch callback function
		 */
		void callbackTouch(void (*onTouch)());

		/**
		 * @brief Add a release callback
		 * 
		 * @param onRelease callback function
		 */
		void callbackRelease(void (*onRelease)());

		/**
		 * @brief Add a value callback
		 * 
		 * @param onValue callback function
		 */
		void callbackValue(void (*onValue)());

		/**
		 * @brief Add a text callback
		 * 
		 * @param onText callback function
		 */
		void callbackText(void (*onText)());

		/**
		 * @brief Set the value of the object
		 * 
		 * @param number object value, 0xFFFFFFFF if not defined
		 */
		void value(int32_t number);

		/**
		 * @brief Get the stored value by return code 0x73
		 * 
		 * @return int32_t value
		 */
		int32_t value();
	
		/**
		 * @brief Get the value of the object by return code 0x71
		 * 
		 * @return int32_t value
		 */
		int32_t getValue();

		/**
		 * @brief Set the text of the object
		 * 
		 * @param txt object text
		 */
		void text(string txt);

		/**
		 * @brief Get the stored text by return code 0x72
		 * 
		 * @return string 
		 */
		string text();

		/**
		 * @brief Get the text of the object by return code 0x70
		 * 
		 * @return const char* text
		 */
		string getText();

	private:
		uint16_t guid();
		void callback(uint8_t event);
		Nextion *nextion;
		componentId_t myId;
		void (*onTouch)() = nullptr;
		void (*onRelease)() = nullptr;
		void (*onText)() = nullptr;
		void (*onValue)() = nullptr;
		bool status;
		int32_t val;
		string txt;

		friend Nextion;

	};

/**
 * @brief Converts rgb to 16bit color in 656 format
 * 
 * @param red 
 * @param green 
 * @param blue 
 * @return uint16_t 16bit color
 */
uint16_t color565(uint8_t red, uint8_t green, uint8_t blue);

#endif