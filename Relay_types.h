#ifndef Relay_types_h
#define Relay_types_h

#include <cstdlib>

#define CAN_devType              0x0010
#define RELAY_DEFAULT_CAN_ADDR   0x0006

#ifndef DEFAULT_TIEMOUT
#define DEFAULT_TIMEOUT     200
#endif

#define relayCtrlId 0x10
#ifndef PACId
#define PACId       0x20
#endif

#define R1Id    0x08
#define R2Id    0x04
#define RIDmask 0x0C
#define stateId 0x02
#ifndef Rb
#define Rb      0x01
#endif

#ifndef status_type
#define status_type 
typedef enum status{
    STATUS_OK = 0,
    STATUS_NO_DATA = -1000,
    STATUS_TX_ERROR = -1001,
    STATUS_RX_ERROR = -1002,
    STATUS_TX_TIMEOUT = -1003,
    STATUS_RX_TIMEOUT = -1004,
    STATUS_UNKNOWN = -1005
}status_t;
#endif

typedef struct relay_ch{
    bool state = false;
    float i = 0.0;
}relay_t;

#endif