#include "BDDv3.h"
#include <iostream>
#include "moduleCan.h"
#include "Relay.h"


RMv3::RMv3(int ADDR){
    moduleCAN.frequency(250000);
    this -> moduleAddr = ADDR;
}

status_t RMv3::relayCtrl(uint8_t relay, bool state){
    uint32_t canId = (CAN_devType << 17)|(this -> moduleAddr << 11)|(relayCtrlId)|(relay << 2)|((uint8_t)state << 1);

    if(!(moduleCAN.write(CANMessage(canId, "\0", 1, CANData, CANExtended)))){
        moduleCAN.mode(CAN::Reset);
        moduleCAN.mode(CAN::Normal);

        return STATUS_TX_ERROR;
    }

    wait_ms(100);
    return STATUS_OK;
}

status_t RMv3::getRelayStates(int timeout){
    uint32_t canId = (CAN_devType << 17)|(this -> moduleAddr << 11)|(relayCtrlId)|(Rb);

    if(!(moduleCAN.write(CANMessage(canId, "\0", 1, CANData, CANExtended)))){
        moduleCAN.mode(CAN::Reset);
        moduleCAN.mode(CAN::Normal);

        return STATUS_TX_ERROR;
    }

    int elapsed = 0;
    CANMessage buffer;

    while(elapsed <= timeout){
        if(moduleCAN.read(buffer)){
            for(uint8_t i = 0; i < 4; i++){
                this -> relay[i].state = buffer.data[i];
                this -> relay[i].i = ((float)buffer.data[i+4])/10;
            }

            return STATUS_OK;
        }

        wait_ms(1);
        elapsed++;
    }

    return STATUS_RX_TIMEOUT;
}

status_t RMv3::getVIP(int timeout){
    uint32_t canId = (CAN_devType << 17)|(this -> moduleAddr << 11)|(PACId);

    if(!(moduleCAN.write(CANMessage(canId, "\0", 1, CANData, CANExtended)))){
        moduleCAN.mode(CAN::Reset);
        moduleCAN.mode(CAN::Normal);

        return STATUS_TX_ERROR;
    }

    int elapsed = 0;
    CANMessage buffer;

    while(elapsed <= timeout){
        if(moduleCAN.read(buffer)){
            this -> v = *(float*)&buffer.data[0];
            this -> i = *(float*)&buffer.data[4];
            this -> p = v * i;

            return STATUS_OK;
        }

        wait_ms(1);
        elapsed++;
    }

    return STATUS_RX_TIMEOUT;
}

bool RMv3::relayState(uint8_t relay){
    return this -> relay[relay].state;
}

float RMv3::relayCurrent(uint8_t relay){
    return this -> relay[relay].i;
}

float RMv3::voltage(){
    return this -> v;
}

float RMv3::current(){
    return this -> i;
}

float RMv3::power(){
    return this -> p;
}

BDDv3::BDDv3(uint8_t ADDR){
    moduleCAN.frequency(250000);
    this -> moduleAddr = ADDR;
}

status_t BDDv3::setTime(bool channel, float time){
    char buffer[4];
    int canId = (CAN_TYPE << 9)|(this -> moduleAddr << 6)|(polTimeId)|(((uint8_t)channel) << 1);
    memcpy(&buffer[0], &time, 4);

    if(!(moduleCAN.write(CANMessage(canId, &buffer[0], 4, CANData, CANStandard)))){
        moduleCAN.mode(CAN::Reset);
        moduleCAN.mode(CAN::Normal);

        return STATUS_TX_ERROR;
    }

    wait_ms(10);
    return STATUS_OK;
}
 
status_t BDDv3::setDuty(bool channel, uint8_t duty){
    char buffer[4];
    float dutyCycle = ((float)duty)/100;
    int canId = (CAN_TYPE << 9)|(this -> moduleAddr << 6)|(channel << 1 );
    memcpy(&buffer[0], &dutyCycle, 4);

    if(!(moduleCAN.write(CANMessage(canId, &buffer[0], 4, CANData, CANStandard)))){
        moduleCAN.mode(CAN::Reset);
        moduleCAN.mode(CAN::Normal);

        return STATUS_TX_ERROR;
    }

    wait_ms(10);
    return STATUS_OK;
}

status_t BDDv3::getTime(int timeout){
    int canId = (CAN_TYPE << 9)|(this -> moduleAddr << 6)|(polTimeId)|(Rb);

    if(!(moduleCAN.write(CANMessage(canId, "\0", 1, CANData, CANStandard)))){
        moduleCAN.mode(CAN::Reset);
        moduleCAN.mode(CAN::Normal);

        return STATUS_TX_ERROR;
    }

    int elapsed = 0;
    CANMessage buffer;

    while(elapsed <= timeout){
        if(moduleCAN.read(buffer)){
            this -> channel[ch1].polTime = *(float*)&buffer.data[0];
            this -> channel[ch2].polTime = *(float*)&buffer.data[4];

            return STATUS_OK;
        }
        wait_ms(1);
        elapsed++;
    }

    return STATUS_RX_TIMEOUT;
}

status_t BDDv3::getDuty(int timeout){
    int canId = (CAN_TYPE << 9)|(this -> moduleAddr << 6)|(DCId)|(Rb);

    if(!(moduleCAN.write(CANMessage(canId, "\0", 1, CANData, CANStandard)))){
        moduleCAN.mode(CAN::Reset);
        moduleCAN.mode(CAN::Normal);

        return STATUS_TX_ERROR;
    }

    int elapsed = 0;
    CANMessage buffer;

    while(elapsed <= timeout){
        if(moduleCAN.read(buffer, 0)){
            this -> channel[ch1].dutyCycle = *(float*)&buffer.data[0];
            this -> channel[ch2].dutyCycle = *(float*)&buffer.data[4];

            return STATUS_OK;
        }
        wait_ms(1);
        elapsed++;
    }

    return STATUS_RX_TIMEOUT;
}

status_t BDDv3::getVIP(uint8_t pac, int timeout){
    int canId = (CAN_TYPE << 9)|(this -> moduleAddr << 6)|(PACId)|(pac&0b11);

    if(!(moduleCAN.write(CANMessage(canId, "\0", 1, CANData, CANStandard)))){
        moduleCAN.mode(CAN::Reset);
        moduleCAN.mode(CAN::Normal);

        return STATUS_TX_ERROR;
    }

    int elapsed = 0;
    CANMessage buffer;
    
    while(elapsed <= timeout){
        if(moduleCAN.read(buffer, 0)){
            this -> v[pac&0b11] = *(float*)&buffer.data[0];
            this -> i[pac&0b11] = *(float*)&buffer.data[4];
            this -> p[pac&0b11] = v[pac&0b11] * i[pac&0b11];

            return STATUS_OK;
        }

        wait_ms(1);
        elapsed++;
    }

    return STATUS_RX_TIMEOUT;
}

status_t BDDv3::getAmbientReading(int timeout){
    int canId = (CAN_TYPE << 9)|(this -> moduleAddr << 6)|(SHTCId);

    if(!(moduleCAN.write(CANMessage(canId, "\0", 1, CANData, CANStandard)))){
        moduleCAN.mode(CAN::Reset);
        moduleCAN.mode(CAN::Normal);

        return STATUS_TX_ERROR;
    }

    int elapsed = 0;
    CANMessage buffer;

    while(elapsed <= timeout){
        if(moduleCAN.read(buffer, 0)){
            this -> lastAmbTemp = *(float*)&buffer.data[0];
            this -> lastAmbHum = *(float*)&buffer.data[4];

            return STATUS_OK;
        }

        wait_ms(1);
        elapsed++;
    }

    return STATUS_RX_TIMEOUT;
}

status_t BDDv3::getFlow(int timeout){
    int canId = (CAN_TYPE << 9)|(this -> moduleAddr << 6)|(SHTCId);

    if(!(moduleCAN.write(CANMessage(canId, "\0", 1, CANData, CANStandard)))){
        moduleCAN.mode(CAN::Reset);
        moduleCAN.mode(CAN::Normal);
        
        return STATUS_TX_ERROR;
    }

    int elapsed = 0;
    CANMessage buffer;

    while(elapsed <= (timeout + FLOW_DELAY)){
        if(moduleCAN.read(buffer, 0)){
            this -> lastFlow = *(float*)&buffer.data[0];

            return STATUS_OK;
        }

        wait_ms(1);
        elapsed++;
    }

    return STATUS_RX_TIMEOUT;
}

status_t BDDv3::getPressure(int timeout){
    int canId = (CAN_TYPE << 9)|(this -> moduleAddr << 6)|(PressureId);

    if(!(moduleCAN.write(CANMessage(canId, "\0", 1, CANData, CANStandard)))){
        moduleCAN.mode(CAN::Reset);
        moduleCAN.mode(CAN::Normal);

        return STATUS_TX_ERROR;
    }

    int elapsed = 0;
    CANMessage buffer;

    while(elapsed <= timeout){
        if(moduleCAN.read(buffer, 0)){
            this -> lastPressure = *(float*)&buffer.data[0];

            return STATUS_OK;
        }

        wait_ms(1);
        elapsed++;
    }

    return STATUS_RX_TIMEOUT;
}

status_t BDDv3::getPolarity(int timeout){
    int canId = (CAN_TYPE << 9)|(this -> moduleAddr << 6)|(PolarityId);

    if(!(moduleCAN.write(CANMessage(canId, "\0", 1, CANData, CANStandard)))){
        moduleCAN.mode(CAN::Reset);
        moduleCAN.mode(CAN::Normal);

        return STATUS_TX_ERROR;
    }

    int elapsed = 0;
    CANMessage buffer;

    while(elapsed <= timeout){
        if(moduleCAN.read(buffer, 0)){
            this -> channel[0].polarity = buffer.data[0];
            this -> channel[1].polarity = buffer.data[1];

            return STATUS_OK;
        }

        wait_ms(1);
        elapsed++;
    }

    return STATUS_RX_TIMEOUT;
}

float BDDv3::polTime(bool channel){
    return this -> channel[channel].polTime;
}

float BDDv3::dutyCycle(bool channel){
    return this -> channel[channel].dutyCycle;
}

float BDDv3::voltage(uint8_t pac){
    return this -> v[pac&0b11];
}

float BDDv3::current(uint8_t pac){
    return this -> i[pac&0b11];
}

float BDDv3::power(uint8_t pac){
    return this -> p[pac&0b11];
}

float BDDv3::ambientTemp(){
    return this -> lastAmbTemp;
}

float BDDv3::ambientHum(){
    return this -> lastAmbHum;
}

float BDDv3::flow(){
    return this -> lastFlow;
}