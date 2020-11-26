#ifndef pbkdfAuth_h
#define pbkdfAuth_h

#include "mbed.h"
#include "mbedtls/pkcs5.h"
#include "mbedtls/sha1.h"
#include "Base64.h"

class rDasAuth{
public:
    int makeKey(char* password, char* salt, int iterations);
    int makeBAuthKey(char* devId);
    unsigned char authKey[48];
    char bAuthKey[200];
};

#endif