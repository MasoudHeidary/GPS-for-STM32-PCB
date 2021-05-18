/*
 * GPS-M.h
 *
 *  Created on: Mar 21, 2020
 *      Author: MasoudHeidary
 *      Address:https://github.com/MasoudHeidary
 *      Gmail: MasoudHeidaryDeveloper@gmail.com
 */

#ifndef INC_GPS_M_H_
#define INC_GPS_M_H_

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////

#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

//////////////////////////////////////////////////////////////////////

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

#define UART huart2
#define UART_T USART2
#define RXBufferLen 500

#define GPSMainStringLen 80
#define GPSHeaderString "$GPGGA"

//////////////////////////////////////////////////////////////////////

char RXBuffer[RXBufferLen];
char GPSMainString[GPSMainStringLen];

//////////////////////////////////////////////////////////////////////
//public
    // for start GPS
    void GPSInit(void);

    // location
    float GPSLocationN(void);
    float GPSLocationE(void);

    // clock
    uint8_t GPSHour(void);
    uint8_t GPSMinute(void);
    uint8_t GPSSecond(void);

//private
void GPSClockRefresh(void);
void GPSRefresh(void);
void MakeGPSMainString(void);
float StrToFloat(char *Str);
uint8_t FindPoint(char *Str);
void Empthy(char *Str , int Len);
//////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif /* INC_GPS_M_H_ */
