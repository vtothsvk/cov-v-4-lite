#ifndef RTClib_types_h
#define RTClib_types_h

#include <cstdint>

#ifndef RTC_LIB_YEAR_OFFSET
#define RTC_LIB_YEAR_OFFSET
#define yrOffset 1900
#endif

#define rtcAddr     (0x68 << 1)
#define rtcinitReg     0x02
#define rtctimeReg     0x03

#ifndef status_type
#define status_type
typedef enum status{
    STATUS_OK = 0,
    STATUS_NO_DATA = -1000,
    STATUS_TX_ERROR = -1001,
    STATUS_RX_ERROR = -1002,
    STATUS_RESERVED = -1003,
    STATUS_RESERVED2 = -1004,
    STATUS_UNKNOWN = -1005
}status_t;
#endif

#endif