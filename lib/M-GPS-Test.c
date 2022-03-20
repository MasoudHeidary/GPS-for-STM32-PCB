/*
 * M-GPS-Test.c
 *
 *  Created on: Mar 12, 2022
 *      Author: Masoud Heidary
 *      gitHub: https://github.com/MasoudHeidary/
 *     License: MIT
 */
#include "M-GPS.h"

bool M_GPS_test(void) {
	return __test__M_GPS_updateTime()		&&
			__test__M_GPS_updateLocation()	&&
			__test__M_GPS_updateDate()		&&
			__test__M_GPS_translateGPRMC()	&&
			__test__M_GPS_translateGPGGA;
}

bool __test__M_GPS_translateGPRMC(void) {
	strcpy(__M_GPS_TranslatorBuf, "$GPRMC,123456.78,A,1234.56789,N,54321.12345,E,2.472, ,020322,");
	//------------------------------------time---------lat-----------lon---------speed---date---

	__M_GPS_translateGPRMC();

	// note: msc in here is 78, because of floating point error
	if (M_GPS.time.hur != 12 ||
		M_GPS.time.min != 34 ||
		M_GPS.time.sec != 56)
		return false;

	if ((long)(round(M_GPS.lat * 100000)) != 1257613)
		return false;

	if((long)(round(M_GPS.lon * 100000)) != 54335206)
		return false;

	if(M_GPS.spd != 4)
		return false;

	if(M_GPS.date.day != 02 ||
	   M_GPS.date.mon != 03 ||
	   M_GPS.date.yar != 22)
		return false;

	return true;
}

bool __test__M_GPS_translateGPGGA(void) {
	strcpy(__M_GPS_TranslatorBuf, "$GPGGA, , ,N, ,E,1,05, ,1369.5,M,-16.7,M");
	//------------------------------------------------NoS--height----hgo-----

	__M_GPS_translateGPGGA();

	if (M_GPS.NoS != 5)
		return false;

	if ((long)(round(M_GPS.hei * 10)) != 13695)
		return false;

	if((long)(round(M_GPS.hgo * 10)) != -167)
		return false;

	return true;
}

bool __test__M_GPS_updateTime(void) {
	double new_time = 123519.00;
	__M_GPS_updateTime(new_time);

	if (M_GPS.time.hur != 12 ||
		M_GPS.time.min != 35 ||
		M_GPS.time.sec != 19 ||
		M_GPS.time.msc != 00)
		return false;
	return true;
}

bool __test__M_GPS_updateLocation(void) {
	double new_location = 1234.54321;
	double returned_location = 0;

	__M_GPS_updateLocation(new_location, &returned_location);

	// returned_location == 12.57572
	if ((long)(round(returned_location * 100000)) != 1257572)
		return false;
	return true;
}


bool __test__M_GPS_updateDate(void) {
	uint32_t new_date = 120622;
	__M_GPS_updateDate(new_date);

	if (M_GPS.date.yar != 22 ||
		M_GPS.date.mon != 06 ||
		M_GPS.date.day != 12)
		return false;
	return true;
}
