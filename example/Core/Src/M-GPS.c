/*
 * M-GPS.c
 *
 *  Created on: Mar 12, 2022
 *      Author: Masoud Heidary
 *      gitHub: https://github.com/MasoudHeidary/
 *     License: MIT
 */

#include "M-GPS.h"


/**
 * @breif setting parameters to zero
 *
 * @param uart get uart port that GPS is connected on
 *
 * @retval None
 */
void M_GPS_init(UART_HandleTypeDef uart) {
	// set everything to zero
	M_GPS.lat = 0;
	M_GPS.lon = 0;
	M_GPS.hei = 0;
	M_GPS.spd = 0;

	__M_GPS_updateDate(0);

	__M_GPS_updateTime(0);

	M_GPS_setLocalTime(0, 0);

	// send data to GPS module to set communication speed
	uint8_t _ = '$';
	HAL_UART_Transmit(&uart, &_, 1, 300);
}


/**
 * @breif initialize GPS buffer to save incoming data
 *
 * @param None
 *
 * @retval None
 */
void M_GPS_bufInit(UART_HandleTypeDef uart){
	memset(__M_GPS_Buf, 0, __M_GPS_BufLen);
	HAL_UART_Receive_DMA(&uart, (uint8_t*) __M_GPS_Buf, __M_GPS_BufLen);
}


/**
 * @breif setting local time to use local time in addition to UTC
 *
 * @param local_hour can be negative or positive
 * @param local_minute can be negative or positive
 *
 * @retval None
 */
void M_GPS_setLocalTime(signed char local_hour, signed char local_minute) {
	__M_GPS_localHur = local_hour;
	__M_GPS_localMin = local_minute;
}


/**
 * @breif split sentences from buffer from $ to $ and replace blank parameters with space
 * and send sentences to translator
 * *** This function should call in "HAL_UART_RxCpltCallback" interrupt
 *
 * @param None
 *
 * @retval None
 */
void M_GPS_bufCplt(void) {
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


/**
 * @breif try to translate by different translator, after success translating
 * all needed parameters restart GPS buffer to incoming data
 *
 * @param None
 *
 * @retval None
 */
void __M_GPS_translator(void) {
	__M_GPS_translateGPRMC();
	__M_GPS_translateGPGGA();
}


/**
 * @breif obtain parameters from $GPRMC
 *
 * @param None
 *
 * @retval None
 */
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


/**
 * @breif obtain parameters from $GPGGA
 *
 * @param None
 *
 * @retval None
 */
void __M_GPS_translateGPGGA(void) {
	char number_of_satelite[5] = "";
	char height[10] = "";
	char height_of_geoid[10] = "";

	sscanf(__M_GPS_TranslatorBuf,
				"$GPGGA,"
				"%*[0-9|.| ],"
				"%*[0-9|.| ], N,"
				"%*[0-9|.| ], E, 1,"
				"%[0-9|.| ],"
				"%*[0-9|.| ],"
				"%[0-9|.| |-], M,"
				"%[0-9|.| |-], M,",
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
 * @brief update saved time in M_GPS structure base on newTime
 *
 * @param newTime in "hhmmss.ss" format
 *
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
 * @breif convert hybrid location to geographical degree and save in given location
 *
 * @param newLocation in dddmm.m* format
 * @param location
 *
 * @retval None
 */
void __M_GPS_updateLocation(double newLocation, double *location) {
	int int_loc = (int) newLocation;
	double deg = (int)(int_loc / 100.0);
	double min = newLocation - deg * 100.0;

	*location = deg + min / 60.0;
}


/**
 * @breif update saved date in M_GPS structure base on newDate
 *
 * @param newDate in "ddmmyy" format
 *
 * @retval None
 */
void __M_GPS_updateDate(uint32_t newDate) {
	M_GPS.date.day = (uint8_t)(newDate / 10000);
	newDate %= 10000;

	M_GPS.date.mon = (uint8_t)(newDate / 100);
	newDate %= 100;

	M_GPS.date.yar = (uint8_t)(newDate);
}


/* get value functions ------------------------------------------------------------------*/
// get locations
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

// get more information
uint16_t M_GPS_getSpeed(void) {
	return M_GPS.spd;
}

uint8_t M_GPS_getNumberOfSatellites(void) {
	return M_GPS.NoS;
}

// get time
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

// get local time
// todo: add functions
// {getLocalTimeString, getLocalHour, getLocalMinute}

// get date
void M_GPS_getDateString(char *str) {
	sprintf(str, "20%d/%d/%d",
			M_GPS.date.yar,
			M_GPS.date.mon,
			M_GPS.date.day);
}

uint8_t M_GPS_getDay(void) {
	return M_GPS.date.day;
}

uint8_t M_GPS_getMon(void) {
	return M_GPS.date.mon;
}

uint8_t M_GPS_getYear(void) {
	return M_GPS.date.yar;
}

