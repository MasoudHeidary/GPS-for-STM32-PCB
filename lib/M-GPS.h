/*
 * M-GPS.h
 *
 *  Created on: Mar 12, 2022
 *      Author: Masoud Heidary
 *      gitHub: https://github.com/MasoudHeidary/
 *     License: MIT
 */

#ifndef INC_M_GPS_H_
#define INC_M_GPS_H_

#ifdef __cplusplus
extern "C" {
#endif

// todo: local timing need to complete
// todo: translator - reinit GPS buffer after finishing translation

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>		// use for floor
#include <stdbool.h>
#include <string.h>		// use for memset

#include "main.h"

/* defines ------------------------------------------------------------------*/
#define __M_GPS_BufLen 300
#define __M_GPS_TranslatorBufLen 100
#define __M_GPS_Knot2Km 1.85184

/* variables ------------------------------------------------------------------*/
UART_HandleTypeDef __M_GPS_UART;

char __M_GPS_Buf[__M_GPS_BufLen];
char __M_GPS_TranslatorBuf[__M_GPS_TranslatorBufLen];


uint8_t __M_GPS_localHur;
uint8_t __M_GPS_localMin;

/* structures ------------------------------------------------------------------*/
struct __M_GPS_Time{
	uint8_t hur;	// hour
	uint8_t min;	// minute
	uint8_t sec;	// second
	uint8_t msc;	// millisecond
};

struct __M_GPS_Date{
	uint8_t yar;
	uint8_t mon;
	uint8_t day;
};

struct {
	double lat;
	double lon;

	double hei;
	double hgo;

	uint8_t NoS;
	uint16_t spd;

	struct __M_GPS_Time time;
	struct __M_GPS_Date date;
} M_GPS;


/* public functions ------------------------------------------------------------------*/
// general functions
void M_GPS_init(UART_HandleTypeDef uart);
void M_GPS_bufInit(UART_HandleTypeDef uart);
void M_GPS_BufCplt(void);
void M_GPS_setLocalTime(uint8_t local_hour, uint8_t local_minute);

// get locations
double M_GPS_getLatitude(void);
double M_GPS_getLongitude(void);
double M_GPS_getHeight(void);
double M_GPS_getGeoHeight(void);

// get more information
uint16_t M_GPS_getSpeed(void);
uint8_t M_GPS_getNumberOfSatellites(void);

// get time
void M_GPS_getTimeString(char *str);
uint8_t M_GPS_getHour(void);
uint8_t M_GPS_getMinute(void);
uint8_t M_GPS_getSecond(void);
uint8_t M_GPS_getMilliSecond(void);

// get date
void M_GPS_getDateString(char *str);
uint8_t M_GPS_getDay(void);
uint8_t M_GPS_getMon(void);
uint8_t M_GPS_getYear(void);

/* private functions ------------------------------------------------------------------*/
void __M_GPS_translator(void);
void __M_GPS_translateGPRMC(void);
void __M_GPS_translateGPGGA(void);

void __M_GPS_updateTime(double);
void __M_GPS_updateLocation(double, double *);
void __M_GPS_updateDate(uint32_t);

/* tests ------------------------------------------------------------------*/
bool M_GPS_test();

bool __test__M_GPS_getLocalHour(void);
bool __test__M_GPS_updateLocation(void);
/*  ------------------------------------------------------------------*/


/*  ------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif /* INC_M_GPS_H_ */
