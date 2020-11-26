#include "Atlas.h"
#include "mbed.h"
#include "drivers/I2C.h"
#include <cstdlib>
#include <iostream>

Atlas::Atlas(I2C *boardI2C, uint8_t ADDR){
    sensI2C = boardI2C;
    this -> last.data = 123.456;
    this -> temp.data = 20.0;
	this -> sensAddress = ADDR << 1;
}

status_t Atlas::ctrl(bool status){
    char message[2];
    message[0] = ctrl_reg;
    message[1] = status;

    if(this -> sensI2C -> write(this -> sensAddress, message, 2, false)){
        return STATUS_TX_ERROR;
    }

    ThisThread::sleep_for(10);
    return STATUS_OK;
}

status_t Atlas::ledCtrl(bool state){
	char message[2];
	message[0] = led_reg;
	message[1] = (state); 
	
	if(this -> sensI2C -> write(this -> sensAddress, message, 2, false)){
        return STATUS_TX_ERROR;
    }

    ThisThread::sleep_for(10);
    return STATUS_OK;
}

status_t Atlas::read_main(){
    if(isNewRead_a(this -> sensI2C, this -> sensAddress)){
        for(uint8_t i = 0; i<= 3; i++){
            char reg = (this -> registers.readReg) + i;
            if(this -> sensI2C -> write(this -> sensAddress, &reg, 1, false)){
                return STATUS_TX_ERROR;
            }

            if(this -> sensI2C -> read(this -> sensAddress, &(this -> last.dataBuff[3 - i]), 1, false)){
                return STATUS_RX_ERROR;
            }
        }
    }else {
        return STATUS_NO_DATA;
    }

    this -> last.data = (*(unsigned long *)&(this -> last.dataBuff));
    cout << this -> last.data << endl;

    ThisThread::sleep_for(10);
    return STATUS_OK;
}

status_t Atlas::tempWrite(float temp){
    char message[2];
    calBuffer_t sendBuffer;
    if(temp < 0.1) temp = 0.1;
    this -> setTemp = temp;
    sendBuffer.n_long = temp * 100;

    for(uint8_t i = 0; i <= 3; i++){
        message[0] = this -> registers.tempReg + i;
        message[1] = sendBuffer.buffer[3 - i];
        if(this -> sensI2C -> write(this -> sensAddress, &message[0], 2, false)){
            return STATUS_TX_ERROR;
        }
    }

    ThisThread::sleep_for(10);
    return STATUS_OK;
}

status_t Atlas::tempRead(){
    for(uint8_t i = 0; i<= 3; i++){
            char reg = (this -> registers.tempReg_r) + i;
            if(this -> sensI2C -> write(this -> sensAddress, &reg, 1, false)){
                return STATUS_TX_ERROR;
            }

            if(this -> sensI2C -> read(this -> sensAddress, &(this -> temp.dataBuff[3 - i]), 1, false)){
                return STATUS_RX_ERROR;
            }
        }

    this -> temp.data = (*(unsigned long *)&(this -> last.dataBuff))/100.0;
    ThisThread::sleep_for(10);
    return STATUS_OK;
}

void Atlas::setI2C(I2C *newI2C){
    this -> sensI2C = newI2C;
}

uint8_t Atlas::address(void){
	return(this -> sensAddress);
}

char* Atlas::lastReadingBuff(void){
	return(char*)(this -> last.dataBuff);
}

float Atlas::lastReading(void){
    return this -> last.data;
}

float Atlas::lastTemp(){
    return this -> setTemp;
}

status_t ezoPH::read(){
    if(isNewRead_a(this -> sensI2C, this -> sensAddress)){
        for(uint8_t i = 0; i<= 3; i++){
            char reg = (this -> registers.readReg) + i;
            if(this -> sensI2C -> write(this -> sensAddress, &reg, 1, false)){
                return STATUS_TX_ERROR;
            }

            if(this -> sensI2C -> read(this -> sensAddress, &(this -> last.dataBuff[3 - i]), 1, false)){
                return STATUS_RX_ERROR;
            }
        }
    }else {
        return STATUS_NO_DATA;
    }

    this -> last.data = (*(unsigned long *)&(this -> last.dataBuff))/1000.0;
    //this -> last.data /= 1000.0;
    ThisThread::sleep_for(10);
    return STATUS_OK;
}

status_t ezoPH::cal(uint8_t point, float n){
    char message[2];
    calBuffer_t sendBuffer;
    sendBuffer.n_long = n * 1000;

    for(uint8_t i = 0; i <= 3; i++){
        message [0] = phCal_reg + i;
        message[1] = sendBuffer.buffer[3 - i];

        this -> sensI2C -> write(this -> sensAddress, &message[0], 2, false);
    }

    message[0] = phCal_request;
    message[1] = point;
    if(this -> sensI2C -> write(this -> sensAddress, &message[0], 2, false)){
        return STATUS_TX_ERROR;
    }

    ThisThread::sleep_for(10);
    return STATUS_OK;
}

status_t ezoPH::calConfirm(){
    char buffer = phCal_confirm;

    if(this -> sensI2C -> write(this -> sensAddress, &buffer, 1, false)){
        return STATUS_TX_ERROR;
    }

    ThisThread::sleep_for(1);

    if(this -> sensI2C -> read(this -> sensAddress, &buffer, 1, false)){
        return STATUS_RX_ERROR;
    }

    ThisThread::sleep_for(10);
    return STATUS_OK;
}

status_t ezoORP::read(){
    if(isNewRead_a(this -> sensI2C, this -> sensAddress)){
        for(uint8_t i = 0; i<= 3; i++){
            char reg = (this -> registers.readReg) + i;
            if(this -> sensI2C -> write(this -> sensAddress, &reg, 1, false)){
                return STATUS_TX_ERROR;
            }

            if(this -> sensI2C -> read(this -> sensAddress, &(this -> last.dataBuff[3 - i]), 1, false)){
                return STATUS_RX_ERROR;
            }
        }
    }else {
        return STATUS_NO_DATA;
    }

    this -> last.data = (*(unsigned long *)&(this -> last.dataBuff))/10.0;
    ThisThread::sleep_for(10);
    return STATUS_OK;
}

status_t ezoORP::cal(uint8_t point, uint16_t n){
    char message[2];
    calBuffer_t sendBuffer;
    sendBuffer.n_slong = n * 10;

    for(uint8_t i = 0; i<= 3; i++){
        message[0] = orpCal_reg + i;
        message[1] = sendBuffer.buffer[3 - i];

        if(this -> sensI2C -> write(this -> sensAddress, &message[0], 2, false)){
            return STATUS_TX_ERROR;
        }
    }

    message[0] = orpCal_request;
    message[1] = point;

    if(this -> sensI2C -> write(this -> sensAddress, &message[0], 2, false)){
        return STATUS_TX_ERROR;
    }

    ThisThread::sleep_for(10);
    return STATUS_OK;
}

status_t ezoDO::read(){
    if(isNewRead_a(this -> sensI2C, this -> sensAddress)){
        for(uint8_t i = 0; i<= 3; i++){
            char reg = (this -> registers.readReg) + i;
            if(this -> sensI2C -> write(this -> sensAddress, &reg, 1, false)){
                return STATUS_TX_ERROR;
            }

            if(this -> sensI2C -> read(this -> sensAddress, &(this -> last.dataBuff[3 - i]), 1, false)){
                return STATUS_RX_ERROR;
            }
        }

        for(uint8_t i = 0; i<= 3; i++){
            char reg = (this -> registers.readReg) + 4 + i;
            if(this -> sensI2C -> write(this -> sensAddress, &reg, 1, false)){
                return STATUS_TX_ERROR;
            }

            if(this -> sensI2C -> read(this -> sensAddress, &(this -> saturation.dataBuff[3 - i]), 1, false)){
                return STATUS_RX_ERROR;
            }
        }
    }else {
        return STATUS_NO_DATA;
    }

    this -> last.data = (*(unsigned long *)&(this -> last.dataBuff))/100.0;
    this -> saturation.data = (*(unsigned long *)&(this -> saturation.dataBuff))/100.0;
    ThisThread::sleep_for(10);
    return STATUS_OK;
}

float ezoDO::lastSaturationReading(){
    return this -> saturation.data;
}

status_t ezoDO::cal(uint8_t point){
    char message [2];
    message[0] = doCal_reg;
    message[1] = point;
    if(this -> sensI2C -> write(this -> sensAddress, &message[0], 2, false)){
        return STATUS_TX_ERROR;
    }

    ThisThread::sleep_for(10);
    return STATUS_OK;
}

status_t ezoDO::writeEC(float EC){
     char message[2];
    calBuffer_t sendBuffer;
    sendBuffer.n_long = EC * 100;

    for(uint8_t i = 0; i <= 3; i++){
        message [0] = doEC_reg + i;
        message[1] = sendBuffer.buffer[3 - i];

        this -> sensI2C -> write(this -> sensAddress, &message[0], 2, false);
    }

    ThisThread::sleep_for(10);
    return STATUS_OK;
}

status_t ezoEC::read(){
    if(isNewRead_a(this -> sensI2C, this -> sensAddress)){
        for(uint8_t i = 0; i<= 3; i++){
            char reg = (this -> registers.readReg) + i;
            if(this -> sensI2C -> write(this -> sensAddress, &reg, 1, false)){
                return STATUS_TX_ERROR;
            }

            if(this -> sensI2C -> read(this -> sensAddress, &(this -> last.dataBuff[3 - i]), 1, false)){
                return STATUS_RX_ERROR;
            }
        }

        for(uint8_t i = 0; i<= 3; i++){
            char reg = tdsRead_reg + i;
            if(this -> sensI2C -> write(this -> sensAddress, &reg, 1, false)){
                return STATUS_TX_ERROR;
            }

            if(this -> sensI2C -> read(this -> sensAddress, &(this -> tds.dataBuff[3 - i]), 1, false)){
                return STATUS_RX_ERROR;
            }
        }
    }else {
        return STATUS_NO_DATA;
    }

    this -> last.data = (*(unsigned long *)&(this -> last.dataBuff)) / 100.0;
    this -> tds.data = (*(unsigned long *)&(this -> tds.dataBuff)) / 100.0;
    ThisThread::sleep_for(10);
    return STATUS_OK;
}

status_t ezoEC::cal(uint8_t point, uint32_t n){
    char message[2];
    calBuffer_t sendBuffer;
    sendBuffer.n_long = n * 100;

    for(uint8_t i = 0; i <= 3; i++){
        message [0] = sCal_reg + i;
        message[1] = sendBuffer.buffer[3 - i];

        this -> sensI2C -> write(this -> sensAddress, &message[0], 2, false);
    }

    message[0] = sCal_request;
    message[1] = point;
    if(this -> sensI2C -> write(this -> sensAddress, &message[0], 2, false)){
        return STATUS_TX_ERROR;
    }

    ThisThread::sleep_for(10);
    return STATUS_OK;
}

status_t ezoEC::setProbe(float probe){
    char message[2];
    uint16_t probe_int = probe * 100;
    memcpy(message, (unsigned long*)(&probe_int), 2);
    char reg = sProbe_reg;

    if(this -> sensI2C -> write(this -> sensAddress, &reg, 1, false)){
        return STATUS_TX_ERROR;
    }
    for(uint8_t i = 0; i <= 1; i++){
        if(this -> sensI2C -> write(this -> sensAddress, &message[1 - i], 1, false)){
            return STATUS_TX_ERROR;
        }
    }

    ThisThread::sleep_for(10);
    return STATUS_OK;
}

status_t ezoEC::readProbeSetting(){
    for(uint8_t i = 0; i<= 1; i++){
            char reg = sProbe_reg + i;
            if(this -> sensI2C -> write(this -> sensAddress, &reg, 1, false)){
                return STATUS_TX_ERROR;
            }

            if(this -> sensI2C -> read(this -> sensAddress, &(this -> probe.dataBuff[1 - i]), 1, false)){
                return STATUS_RX_ERROR;
            }
        }

    ThisThread::sleep_for(10);
    return STATUS_OK;
}

status_t ezoEC::readTDS(void){
    //if(isNewRead_a(this -> sensI2C, this -> sensAddress)){
        for(uint8_t i = 0; i<= 3; i++){
            char reg = tdsRead_reg + i;
            if(this -> sensI2C -> write(this -> sensAddress, &reg, 1, false)){
                return STATUS_TX_ERROR;
            }

            if(this -> sensI2C -> read(this -> sensAddress, &(this -> tds.dataBuff[3 - i]), 1, false)){
                return STATUS_RX_ERROR;
            }
        }
    //}else {
       // return STATUS_NO_DATA;
    //}

    this -> tds.data = (*(unsigned long *)&(this -> tds.dataBuff)) / 100.0;
    ThisThread::sleep_for(10);
    return STATUS_OK;
}

status_t ezoEC::readPSU(void){
    //if(isNewRead_a(this -> sensI2C, this -> sensAddress)){
        for(uint8_t i = 0; i<= 3; i++){
            char reg = pssReag_reg + i;
            if(this -> sensI2C -> write(this -> sensAddress, &reg, 1, false)){
                return STATUS_TX_ERROR;
            }

            if(this -> sensI2C -> read(this -> sensAddress, &(this -> pss.dataBuff[3 - i]), 1, false)){
                return STATUS_RX_ERROR;
            }
        }
    //}else {
        //return STATUS_NO_DATA;
    //}

    ThisThread::sleep_for(10);
    return STATUS_OK;
}

float ezoEC::probeSetting(){
    return this -> probe.data;
}

float ezoEC::lastTDS(void){
    return this -> tds.data;
}

float ezoEC::lastPSU(void){
    return this -> pss.data;
}

bool isNewRead_a(I2C *boardI2C, uint8_t ADDR){
    char message[2];
    char buffer;
    message[0] = newRead_reg;
    message[1] = 0;

    boardI2C -> write(ADDR, &message[0], 1, false);
    ThisThread::sleep_for(1);
    boardI2C -> read(ADDR, &buffer, 1, false);
    ThisThread::sleep_for(1);
    boardI2C -> write(ADDR, message, 2, false);

    ThisThread::sleep_for(10);
    return (bool)(buffer);
}