#ifndef BDDv3_types_h
#define BDDv3_types_h

#include <cstdlib>

#define CAN_TYPE            0x01
#define DEFAULT_CAN_ADDR    0x06

#define DEFAULT_TIMEOUT     200
#define FLOW_DELAY          1000

#define DCId        0x00
#define polTimeId   0x08
#define HBId        0x10
#define SHTCId      0x18
#define IGBTId      0x1C
#define FlowId      0x1E
#define PressureId  0x1F
#define PACId       0x20
#define PolarityId  0x24

#define Rb  1

#define ch1 0
#define ch2 1

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

typedef struct BDD_v3_channel{
    float dutyCycle, polTime;
    bool polarity;
}bdd_channel_t;

#endif