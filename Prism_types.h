#ifndef Prism_types_h
#define Prism_types_h

#define Standardn3  0
#define Standardn2  1
#define Standardn1  2
#define Standard    3
#define Standardp1  4
#define Standardp2  5
#define Start       6
#define Vacation    7
#define Cottage     8
#define Manual      9


#ifndef WiFi_config
#define WiFi_config

//#define WiFi_SSID   "Bajo"
//#define WiFi_PASS   "Diamond123"
#define WiFi_SSID   "Fyzika je lahka"
#define WiFi_PASS   "serenada12"

#endif

#include <cstdint>

typedef struct Prism_data{
uint8_t operationMode = Standardn1;
bool isConnected = false;
long wqltMeasureInterval = 20000, BDDvipMeasureInterval = 2000;
long T1[10] = {45000L, 60000L, 45000L, 60000L, 80000L, 90000L, 180000L, 60000L, 120000L, 20000L};
long T2[10] = {180000L, 240000L, 180000L, 240000L, 300000L, 360000L, 180000L, 180000L, 120000L, 20000L};
long T3[10] = {495000L, 300000L, 75000L, 60000L, 20000L, 0, 0, 660000L, 660000L, 20000L};
long T4 = 10000L;
}prism_data_t;
#endif