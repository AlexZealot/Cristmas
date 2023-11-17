#include "GUI.h"
#include "../connect/UDPConnect.h"

GUIClass GUI;

device_t*   GUIClass::getDevice(uint8_t ID){
	for (uint8_t i = 0; i < DEV_COUNT; i++){
		if (_devices[i].id == ID) {
			_repaintDisplay = true;
			return &_devices[i];
		}
	}
	return NULL;
}

void GUIClass::init(){
	_devices[0].id = UDP_HALL_WINDOW;
	_devices[1].id = UDP_HALL_TV;
	_devices[2].id = UDP_CHILDREN;
	_devices[3].id = UDP_XMAS_TREE;

	for (uint8_t i = 0; i < DEV_COUNT; i++){
		_devices[i].connected = false;
		_devices[i].inSync = false;
		_devices[i].lastInSync = false;
		_devices[i].mode = 0;
		_devices[i].lastMode = 0;
		_devices[i].syncFlag = 0;
		_devices[i].lastSyncFlag = 0;
	}

	//pinMode(PIN_BACKLIGHT, OUTPUT);
	tft.begin();
	tft.setRotation(3);
	tft.setTextSize(1);
	blackScreen();
	setBacklight(128);
	_lastDraw = 0;
	touchCalibrate();	
}

void	GUIClass::showWiFiLogo(){
	// tft.fillSmoothCircle(120, 160, 3, TFT_BLUE, TFT_WHITE);
	// tft.drawSmoothArc(120, 160, 15, 12, 135, 225, TFT_BLUE, TFT_WHITE, false);
	// tft.drawSmoothArc(120, 160, 30, 27, 135, 225, TFT_BLUE, TFT_WHITE, false);
	// tft.drawSmoothArc(120, 160, 45, 42, 135, 225, TFT_BLUE, TFT_WHITE, false);

	tft.fillCircle(120, 160, 3, TFT_BLUE);
	tft.drawArc(120, 160, 15, 12, 135, 225, TFT_BLUE, TFT_WHITE, false);
	tft.drawArc(120, 160, 30, 27, 135, 225, TFT_BLUE, TFT_WHITE, false);
	tft.drawArc(120, 160, 45, 42, 135, 225, TFT_BLUE, TFT_WHITE, false);
}

const char * GUIClass::getDeviceName(uint8_t id){
	switch (id){
		case	UDP_HALL_WINDOW	:	   return "Окно в зале";
		case	UDP_HALL_TV		:	   return "Подсветка ТВ";
		case	UDP_CHILDREN	:	   return "Окно в детской";
		case	UDP_XMAS_TREE	:	   return "Ёлка";
		default					:	   return "";
	}
}


void		GUIClass::update(){
	if (millis() - _lastTouch > TOUCH_DELAY){
		if (!tft.getTouch(&_tx, &_ty, 175)){
			_tx = 0xFFFF;
			_ty = 0xFFFF;
		}
		/*Обработка нажатий*/
		switch (_display_state){
			case ds_dev_list:
				for (uint8_t i = 0; i < DEV_COUNT; i++){
					if ( (_tx > 215) && (_tx < 235) && (_ty > (65+i*75)) && (_ty < (85+i*75)) ){
						if ((!_devices[i].inSync) && (_devices[i].connected)){
							_display_state = ds_modes_list;
							_selected_dev = i;
							_repaintDisplay = true;
						}
					}

					if (_devices[i].id == UDP_XMAS_TREE){
						if ( (_tx > 110) && (_tx < 120) && (_ty > (48+i*75)) && (_ty < (58+i*75)) ){
							_devices[i].syncFlag ^= 0x02;
							UDPConnect.sendData(_devices[i]);
							_repaintDisplay = true;
						}

						if ( (_tx > 140) && (_tx < 150) && (_ty > (48+i*75)) && (_ty < (58+i*75)) ){
							_devices[i].syncFlag ^= 0x01;
							UDPConnect.sendData(_devices[i]);
							_repaintDisplay = true;
						}

						if ( (_tx > 165) && (_tx < 175) && (_ty > (48+i*75)) && (_ty < (58+i*75)) ){
							_devices[i].syncFlag ^= 0x04;
							UDPConnect.sendData(_devices[i]);
							_repaintDisplay = true;
						}										
					}
				}
				break;
			
			case ds_modes_list:
				for (uint8_t i = 0; i < MODES_COUNT; i++){
					//Костыль. Дисплей работает очень странно
					if ( (_tx > 5) && (_tx < 215) && (_ty > i*10.7) && (_ty < (i+1)*10.7) ){/*&& (_ty > (20+i*10)) && (_ty < (30+i*10)) ){*/
						_devices[_selected_dev].mode = i;
						UDPConnect.sendData(_devices[_selected_dev]);
						_display_state = ds_dev_list;
						_repaintDisplay = true;
						break;
					}
				}
				break;
		}
		_lastTouch = millis();
	}

	if (millis() - _last_adc > ADC_DELAY){
		_adc_val = analogRead(PIN_ADC);

		switch (_bat_state){
			case bs_full:
					if (_adc_val < 2375) _bat_state = bs_mid_h;
				break;

			case bs_mid_h:
					if (_adc_val < 2225) _bat_state = bs_mid_l;
					if (_adc_val > 2455) _bat_state = bs_full;
				break;

			case bs_mid_l:
					if (_adc_val < 2055) _bat_state = bs_low;
					if (_adc_val > 2375) _bat_state = bs_mid_h;
				break;

			case bs_low:
					if (_adc_val > 2205) _bat_state = bs_mid_l;
				break;
		}

		_last_adc = millis();
		if (_bat_state != _last_bat_state) _repaintDisplay = true;		
	}	

	/*Отрисовка*/
	if ((millis() - _lastDraw > DUDP) || (_repaintDisplay)){
		if (_last_display_state != _display_state){
			blackScreen();
		}

		/*отрисовка состояния батареи*/
		if ((_last_bat_state != _bat_state) || (_last_display_state != _display_state)){
			tft.fillRect(180, 5, 50, 10, TFT_BLACK);
			_last_bat_state = _bat_state;
			switch(_bat_state){
				case bs_full:
					tft.drawRect(180, 5, 50, 10, TFT_DARKGREEN);
					tft.fillRect(181, 6, 48, 8, TFT_GREEN);
					tft.setTextColor(TFT_DARKGREEN);
					break;
				case bs_mid_h:
					tft.drawRect(180, 5, 50, 10, makeRGB(0x20, 0x80, 0));
					tft.fillRect(181, 6, 36, 8, makeRGB(0x40, 0xFF, 0));
					tft.setTextColor(makeRGB(0x20, 0x80, 0));
					break;					
				case bs_mid_l:
					tft.drawRect(180, 5, 50, 10, makeRGB(0x80, 0x80, 0));
					tft.fillRect(181, 6, 24, 8, TFT_YELLOW);
					tft.setTextColor(makeRGB(0x80, 0x80, 0));
					break;
				case bs_low:
					tft.drawRect(180, 5, 50, 10, makeRGB(0x40, 0x00, 0x00));
					tft.fillRect(181, 6, 12, 8, TFT_RED);
					tft.setTextColor(makeRGB(0x40, 0x00, 0));
					break;
				default:
					tft.setTextColor(TFT_DARKGREY);
			}
			tft.drawString(WiFi.localIP().toString().c_str(), 6, 6, 1);
			_last_display_state = _display_state;	
		}

		switch (_display_state){
			case ds_dev_list:
				/*РИСУЕМ ИНТЕРФЕЙС*/
				for (uint8_t i = 0; i < DEV_COUNT; i++){
					drawDevice(_devices[i], 20+i*75);
				}
				break;
			
			case ds_modes_list:
				for (uint8_t i = 0; i < MODES_COUNT; i++){
					tft.drawRect(5, 20+i*10, 230, 10, makeRGB(0x20, 0x20, 0x20));
					tft.setTextColor((_devices[_selected_dev].mode == i)?TFT_GREEN:TFT_WHITE);
					tft.drawString(mode_names[i], 10, 21+i*10, 1);
				}
				break;
		}
		_lastDraw = millis();		
		_repaintDisplay = false;
	}

	/*Отследить тех, кто отвалился*/
	if (getState() == ds_dev_list)
		for (uint8_t i = 0; i < DEV_COUNT; i++){
			if (millis() - _devices[i].whenConnected > CONNECTED_DELAY){
				_devices[i].connected = false;
				_devices[i].inSync = 0;
				_devices[i].ip[0] = 0;
				_devices[i].ip[1] = 0;
				_devices[i].ip[2] = 0;
				_devices[i].ip[3] = 0;
				_devices[i].mode = 0;
				_devices[i].syncFlag = 0;
				_repaintDisplay = true;
			}
		}
}

void	GUIClass::blackScreen(){
	tft.fillScreen(TFT_BLACK);
}

uint16_t	GUIClass::makeRGB(uint8_t R, uint8_t G, uint8_t B){
	return (B >> 3) | ((G >> 2) << 5) | ((R >> 3) << 11);
}

void		GUIClass::drawDevice(device_t &d, uint16_t y_offs){
	uint16_t col 		= d.connected?TFT_WHITE:makeRGB(0x10, 0x10, 0x10);
	uint16_t txtCol 	= d.connected?TFT_GREEN:col;
	uint16_t modeCol 	= d.connected?makeRGB(0x80, 0x80, 0x80):col;
	uint16_t signColor 	= d.connected?makeRGB(0x40, 0x40, 0x40):col;
	uint16_t ctrlColor 	= d.connected?makeRGB(0x00, 0x20, 0x00):col;

	//Основной прямоугольник
	tft.drawRoundRect(0, y_offs, 240, 73, 5, col);

	//Название и IP-адрес
	tft.setTextColor(txtCol, TFT_BLACK);
	tft.drawString(getDeviceName(d.id), 	10, 	y_offs+10, 	1);
	if (d.connected != d.lastConnected)
		tft.fillRect(120, y_offs+10, 110, 10, TFT_BLACK);
	d.lastConnected = d.connected;
	tft.drawString(d.ip.toString().c_str(), 120, 	y_offs+10, 	1);

	//режим
	tft.setTextColor(signColor, TFT_BLACK);
	tft.drawString("Режим гирлянды:", 10, y_offs+30, 1);

	//синхронизация
	if (d.id == UDP_XMAS_TREE){
		if (d.syncFlag != d.lastSyncFlag){
			tft.fillRect(110, y_offs+25, 66, 20, TFT_BLACK);
			d.lastSyncFlag = d.syncFlag;
		}

		tft.drawString("Зал", 		120, y_offs+30, 1);
		if (d.syncFlag & 0x02)
			tft.fillRect(110, y_offs+28, 10, 10, txtCol);
		tft.drawRect(110, y_offs+28, 10, 10, signColor);

		tft.drawString("ТВ", 		150, y_offs+30, 1);
		if (d.syncFlag & 0x01)
			tft.fillRect(140, y_offs+28, 10, 10, txtCol);
		tft.drawRect(140, y_offs+28, 10, 10, signColor);

		tft.drawString("Детская", 	180, y_offs+30, 1);
		if (d.syncFlag & 0x04)
			tft.fillRect(165, y_offs+28, 10, 10, txtCol);
		tft.drawRect(165, y_offs+28, 10, 10, signColor);		
	}

	if ((d.lastMode != d.mode) || (d.lastInSync != d.inSync) || (d.syncFlag != d.lastSyncFlag)){
		tft.fillRect(5, y_offs+45, 230, 20, TFT_BLACK);
		d.lastMode = d.mode;
		d.lastInSync = d.inSync;
		d.lastSyncFlag = d.syncFlag;
	}

	tft.drawRect(5, y_offs+45, 230, 20, ctrlColor);
	tft.setTextColor(modeCol, TFT_BLACK);	
	tft.drawString(mode_names[d.mode], 15, y_offs+51, 1);

	if (!d.inSync){
		tft.drawRect(215, y_offs+45, 20, 20, ctrlColor);
		tft.setTextColor(signColor);
		tft.drawString(">", 220, y_offs+51, 1);
	}
}

void        GUIClass::touchCalibrate(){
	fs::File f;
	uint16_t calData[5];
	uint8_t calDataOK = 0;

	// check file system exists
	if (!SPIFFS.begin()) {
		SPIFFS.format();
		SPIFFS.begin();
	}

  	// check if calibration file exists and size is correct
	if (SPIFFS.exists(CALIBRATION_FILE)) {
		if (REPEAT_CAL){
      		// Delete if we want to re-calibrate
      		SPIFFS.remove(CALIBRATION_FILE);
    	} else {
      		f = SPIFFS.open(CALIBRATION_FILE, "r");
      		if (f) {
        		if (f.readBytes((char *)calData, 14) == 14)
          			calDataOK = 1;
        		f.close();
      		}
    	}
  	}

  	if (calDataOK && !REPEAT_CAL) {
    	// calibration data valid
    	tft.setTouch(calData);
  	} else {
    	// data not valid so recalibrate
    	tft.fillScreen(TFT_BLACK);
    	tft.setCursor(20, 0);
    	tft.setTextFont(2);
    	tft.setTextSize(1);
    	tft.setTextColor(TFT_WHITE, TFT_BLACK);

		tft.println("Touch corners as indicated");

		tft.setTextFont(1);
		tft.println();

		if (REPEAT_CAL) {
			tft.setTextColor(TFT_RED, TFT_BLACK);
			tft.println("Set REPEAT_CAL to false to stop this running again!");
		}

		tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

		tft.setTextColor(TFT_GREEN, TFT_BLACK);
		tft.println("Calibration complete!");

		// store data
		f = SPIFFS.open(CALIBRATION_FILE, "w");
		if (f) {
			f.write((const unsigned char *)calData, 14);
			f.close();
		}
	}	
}