#ifndef Atlas_types_h
#define Atlas_types_h

#include <cstdint>

//Default sensor I2C adresses
#define PH_DEFAULT_ADDR     0x65
#define ORP_DEFAULT_ADDR    0x66
#define DO_DFEAULT_ADDR     0x67
#define EC_DEFAULT_ADDR      0x64

//CTRL registers
#define led_reg             0x05
#define ctrl_reg            0x06
#define phCal_request       0x0C
#define orpCal_request      0x0C
#define sCal_request        0x0E

//Calibration registers
#define phCal_reg           0x08
#define orpCal_reg          0x08
#define doCal_reg           0x08
#define sCal_reg            0x0A
#define doEC_reg            0x0A

#define phCal_confirm       0x0D

//Status registers
#define newRead_reg         0x07

//Read registers
#define phRead_reg          0x16
#define orpRead_reg         0x0E
#define doRead_reg          0x22
#define doSatura
#define sRead_reg           0x18
#define tdsRead_reg         0x1C
#define pssReag_reg         0x20

//Temp write registers
#define phTemp_reg_w        0x0E
#define doTemp_reg_w        0x12
#define sTemp_reg_w         0x10

//Temp read registers
#define phTemp_reg_r        0x12
#define doTemp_reg_r        0x1E
#define sTemp_reg_r         0x14

//Probe setting registers
#define sProbe_reg          0x08

//Controll macros
#define sleep       0
#define wake        1

//Calibration point macros
#define clr         1
#define low         2
#define mid         3
#define high        4
#define single      2
#define NA          0.00
#define dry         2
#define noO2        3
#define dualLow     4
#define dualHigh    5

//LED control macros
#define ON          1
#define OFF         0

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

typedef union atlas_data{
    char dataBuff[4];
    float data;
}atlas_data_t;

typedef union atlas_probe{
    char dataBuff[2];
    uint16_t data;
}atlas_probe_t;

typedef union atlas_calBuffer{
    unsigned long n_long;
    signed long n_slong;
    char buffer[4];
}calBuffer_t;

typedef struct atlas_registers{
    uint8_t readReg;
    uint8_t tempReg;
    uint8_t tempReg_r;
} atlas_registers_t;

#endif