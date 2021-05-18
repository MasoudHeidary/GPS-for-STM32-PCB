/*
 * GPS-M.c
 *
 *  Created on: Mar 21, 2020
 *      Author: MasoudHeidary
 *      Address:https://github.com/MasoudHeidary
 *      Gmail: MasoudHeidaryDeveloper@gmail.com
 */

#include "GPS-M.h"

char NMinuteString[8] = { 0 }, EMinuteString[8] = { 0 };
int LocalHour = 0, LocalMinute = 0, LocalSecond = 0;
float NLocation = 0, ELocation = 0;
int ClockHour = 0, ClockMinute = 0, ClockSecond = 0;
int NDegree = 0, EDegree = 0;

//start GPS
void GPSInit() {
	HAL_UART_Receive_DMA(&UART, (uint8_t *) RXBuffer, RXBufferLen);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == UART_T) {
		GPSRefresh();
		Empthy(RXBuffer, RXBufferLen);
		HAL_UART_Receive_DMA(&UART, (uint8_t *) RXBuffer, RXBufferLen);
	}
}

float GPSLocationN(void) {
	return NLocation;
}
float GPSLocationE(void) {
	return ELocation;
}
uint8_t GPSHour(void) {
	return (uint8_t) (LocalHour);
}
uint8_t GPSMinute(void) {
	return (uint8_t) (LocalMinute);
}
uint8_t GPSSecond(void) {
	return (uint8_t) (LocalSecond);
}

//change Clock with GMT
void GPSClockRefresh() {
	LocalHour = ClockHour + 4;
	LocalMinute = ClockMinute + 30;
	LocalSecond = ClockSecond;
	if (LocalMinute >= 60) {
		LocalHour++;
		LocalMinute -= 60;
	}
	if (LocalHour >= 24) {
		LocalHour -= 24;
	}
}

//change Buffer to useful string
void MakeGPSMainString() {
	char *Str = (char *) malloc(sizeof(char) * GPSMainStringLen);
	Str = strstr(RXBuffer, GPSHeaderString);
	strncpy(GPSMainString, Str, GPSMainStringLen);
}

//Refresh and make everything
void GPSRefresh() {
	MakeGPSMainString();

	sscanf(GPSMainString, "$GPGGA,%2d%2d%2d.00,%2d%8s,N,%3d%8s,E", &ClockHour,
			&ClockMinute, &ClockSecond, &NDegree, NMinuteString, &EDegree,
			EMinuteString);

	GPSClockRefresh();
	NLocation = (float) (NDegree) + StrToFloat(NMinuteString) / 60.0;
	ELocation = (float) (EDegree) + StrToFloat(EMinuteString) / 60.0;
}
//----------------------------------------------------------------------------------------------
//For StrToFloat
uint8_t FindPoint(char *Str) {
	uint8_t counter = 0;
	do {
		counter++;
	} while (Str[counter] != '.');
	return counter;
}

//change String To Float
float StrToFloat(char *Str) {
	int point = (int) FindPoint(Str);
	int M = 0, L = 0;

	sscanf(Str, "%d", &M);

	char help[10] = { 0 };
	strcpy(help, &(Str[point + 1]));

	sscanf(help, "%d", &L);
	int LLen = strlen(help);

	return (float) M + ((float) (L) * (float) pow(10, 0 - LLen));
}

//Empthy a String
void Empthy(char *Str, int Len) {
	for (int i = 0; i < Len; i++)
		Str[i] = 0;
}
