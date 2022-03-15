/*
 * M-GPS.c
 *
 *  Created on: Mar 12, 2022
 *      Author: Masoud Heidary
 *      gitHub: https://github.com/MasoudHeidary/
 *     License: MIT
 */

#include "M-GPS.h"

void M_GPS_init(UART_HandleTypeDef uart) {
	__M_GPS_UART = uart;

	// set everything to zero
	M_GPS.lat = 0;
	M_GPS.lon = 0;
	M_GPS.hei = 0;
	M_GPS.spd = 0;

	__M_GPS_updateDate(0);

	__M_GPS_updateTime(0);

	M_GPS_setLocalTime(0, 0);

	M_GPS_bufInit();
}

void M_GPS_bufInit(void){
	HAL_UART_Receive_DMA(&__M_GPS_UART, (uint8_t*) __M_GPS_Buf, __M_GPS_BufLen);
}

void M_GPS_setLocalTime(uint8_t local_hour, uint8_t local_minute) {
	__M_GPS_localHur = local_hour;
	__M_GPS_localMin = local_minute;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	__M_GPS_bufferSpliter();

	M_GPS_bufInit();
//	HAL_UART_Receive_DMA(&UART, (uint8_t*) Buf, BufLen);
}

// split buffer from $ to $ and send to translator to translation
void __M_GPS_bufferSpliter(void) {
	uint16_t ptr = 0;
	for (uint16_t i = 0; i < __M_GPS_BufLen; i++) {
		if (__M_GPS_Buf[i] == '$') {
			__M_GPS_translator();
			ptr = 0;
		}

		__M_GPS_TranslatorBuf[ptr++] = __M_GPS_Buf[i];
		if (__M_GPS_Buf[i] == ',')
			__M_GPS_TranslatorBuf[ptr++] = ' ';
	}
}

void __M_GPS_translator(void) {
	__M_GPS_translateGPRMC();
	__M_GPS_translateGPGGA();
}

void __M_GPS_translateGPRMC(void) {
	char time[20] = "";
	char lat[20] = "";	// latitude
	char lon[20] = "";	// longitude
	char speed[10] = "";
	char date[10] = "";

	sscanf(__M_GPS_TranslatorBuf,
			"$GPRMC,"
			"%[0-9|.| ], A,"
			"%[0-9|.| ], N,"
			"%[0-9|.| ], E,"
			"%[0-9|.| ],"
			"%*[0-9|.| ],"
			"%[0-9|.| ]",
			time, lat, lon, speed, date);


	double _ = atof(time);
	if (_ != 0)
		__M_GPS_updateTime(_);

	_ = atof(lat);
	if (_ != 0)
		__M_GPS_updateLocation(_, &M_GPS.lat);

	_ = atof(lon);
	if (_ != 0)
		__M_GPS_updateLocation(_, &M_GPS.lon);

	_ = atof(speed);
	if (_ != 0)
		M_GPS.spd = (uint16_t)(_ * __M_GPS_Knot2Km);

	_ = atoi(date);
	if (_ != 0)
		__M_GPS_updateDate((uint16_t)_);
}

void __M_GPS_translateGPGGA(void) {
	char number_of_satelite[5] = "";
	char height[10] = "";
	char height_of_geoid[10] = "";

	sscanf(__M_GPS_TranslatorBuf,
				"$GPRMC,"
				"%*[0-9|.| ],"
				"%*[0-9|.| ], N,"
				"%*[0-9|.| ], E, 1,"
				"%[0-9|.| ],"
				"%*[0-9|.| ],"
				"%[0-9|.| ], M,"
				"%[0-9|.| ], M,",
				number_of_satelite, height, height_of_geoid);

	double _ = atoi(number_of_satelite);
	if (_ != 0)
		M_GPS.NoS = (uint8_t)_;

	_ = atof(height);
	if(_ != 0)
		M_GPS.hei = _;

	_ = atof(height_of_geoid);
	if(_ != 0)
		M_GPS.hgo = _;
}

/**
 * @brief update time in structure base on new gived time
 *
 * @param newTime specify new time
 * @retval None
 */
void __M_GPS_updateTime(double newTime) {
	double dec_time = newTime - floor(newTime);
	int int_time = (int) (newTime);

	M_GPS.time.msc = (uint8_t) (dec_time * 100);

	M_GPS.time.hur = (int_time / 10000);
	int_time %= 10000;

	M_GPS.time.min = (uint8_t) (int_time / 100);
	int_time %= 100;

	M_GPS.time.sec = (uint8_t) (int_time);
}

/**
 * @breif convert location format to degree then update location by reference
 *
 * @param newLocation
 * @param location	reference that newLocation gonna save on it
 *
 * @retval None
 */
void __M_GPS_updateLocation(double newLocation, double *location) {
	int int_loc = (int) newLocation;
	double deg = int_loc / 100;
	double min = int_loc - deg * 100;

	*location = deg + min / 60;
}


void __M_GPS_updateDate(uint16_t newDate) {
	M_GPS.date.day = (uint8_t)(newDate / 10000);
	newDate %= 10000;

	M_GPS.date.mon = (uint8_t)(newDate / 100);
	newDate %= 100;

	M_GPS.date.day = (uint8_t)(newDate);
}


/* get value functions ------------------------------------------------------------------*/

double M_GPS_getLatitude(void) {
	return M_GPS.lat;
}

double M_GPS_getLongitude(void) {
	return M_GPS.lon;
}

double M_GPS_getHeight(void) {
	return M_GPS.hei;
}

double M_GPS_getGeoHeight(void) {
	return M_GPS.hgo;
}

uint8_t M_GPS_getNumberOfSatellites(void) {
	return M_GPS.NoS;
}

void M_GPS_getTimeString(char *str) {
	sprintf(str, "%d:%d:%d.%d",
			M_GPS.time.hur,
			M_GPS.time.min,
			M_GPS.time.sec,
			M_GPS.time.msc);
}

uint8_t M_GPS_getHour(void) {
	return M_GPS.time.hur;
}

uint8_t M_GPS_getMinute(void) {
	return M_GPS.time.min;
}

uint8_t M_GPS_getSecond(void) {
	return M_GPS.time.sec;
}

uint8_t M_GPS_getMilliSecond(void) {
	return M_GPS.time.msc;
}






