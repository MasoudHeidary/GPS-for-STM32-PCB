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
//	return _testUpdateTime();
	return __test__M_GPS_updateLocation();
//	return true;
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
	if ((long)(returned_location * 100000) != 1257572)
		return false;
	return true;
}
