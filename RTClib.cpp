#include "RTClib.h"


pcfRTC::pcfRTC(I2C *boardI2C){
    this -> rtcI2C = boardI2C;
    this -> last.tm_isdst = -1;
}

status_t pcfRTC::init(){
    char buffer = rtcinitReg;

    if(this -> rtcI2C -> write(rtcAddr, &buffer, 1, false)){
        return STATUS_TX_ERROR;
    }

    if(this -> rtcI2C -> read(rtcAddr, &buffer, 1, false)){
        return STATUS_RX_ERROR;
    }

    wait_ms(10);
    return STATUS_OK;
}

status_t pcfRTC::set(time_t t){
    struct tm ts;

    ts = *localtime(&t);

    char buffer[8];
    buffer[0] = rtctimeReg;
    buffer[1] = bin2bcd(ts.tm_sec);
    buffer[2] = bin2bcd(ts.tm_min);
    buffer[3] = bin2bcd(ts.tm_hour);
    buffer[4] = bin2bcd(ts.tm_mday);
    buffer[5] = bin2bcd(ts.tm_wday);
    buffer[6] = bin2bcd(ts.tm_mon);
    buffer[7] = bin2bcd(ts.tm_year);

    if(this -> rtcI2C -> write(rtcAddr, &buffer[0], 8, false)){
        return STATUS_TX_ERROR;
    }

    wait_ms(10);
    return STATUS_OK;
}
status_t pcfRTC::read(){
    char buffer[7];
    buffer[0] = rtctimeReg;

    if(this -> rtcI2C -> write(rtcAddr, &buffer[0], 1, false)){
        return STATUS_TX_ERROR;
    }
    
    if(this -> rtcI2C -> read(rtcAddr, &buffer[0], 8, false)){
        return STATUS_RX_ERROR;
    }

    this -> last.tm_sec = bcd2bin(buffer[0]&0x7F);
    this -> last.tm_min = bcd2bin(buffer[1]&0x7F);
    this -> last.tm_hour = bcd2bin(buffer[2]&0x3F);
    this -> last.tm_mday = bcd2bin(buffer[3]&0x3F);
    this -> last.tm_wday = bcd2bin(buffer[4]&0b111);
    this -> last.tm_mon = bcd2bin(buffer[5]&0x1F);
    this -> last.tm_year = bcd2bin(buffer[6]);

    //cout << "Reading rtc" << endl;
    wait_ms(10);
    return STATUS_OK;
}

void pcfRTC::printTime(){
    cout << this -> last.tm_sec << ", ";
    cout << this -> last.tm_min << ", ";
    cout << this -> last.tm_hour << ", ";
    cout << this -> last.tm_mday << ", ";
    cout << this -> last.tm_mon + 1 << ", ";
    cout << this -> last.tm_year + yrOffset << endl;
}

unsigned long pcfRTC::lastT(){
    return(mktime(&(this -> last)));
}