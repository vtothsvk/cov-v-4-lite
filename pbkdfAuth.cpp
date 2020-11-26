#include "pbkdfAuth.h"

int rDasAuth::makeKey(char *password, char *salt, int iterations){
    int ret;
    mbedtls_md_context_t sha1_ctx;
    const mbedtls_md_info_t *info_sha1;
    size_t plen = strlen(password);
    size_t slen = strlen(salt);
    const unsigned char *pwd = (unsigned char*)password;
    const unsigned char *slt = (unsigned char*)salt;

    mbedtls_md_init(&sha1_ctx);
    info_sha1 = mbedtls_md_info_from_type(MBEDTLS_MD_SHA1);
    if(info_sha1 == NULL){
        return -1;
    }

    if( (ret = mbedtls_md_setup(&sha1_ctx, info_sha1, 1)) != 0){
    	return ret;
    }

    ret = mbedtls_pkcs5_pbkdf2_hmac(&sha1_ctx, pwd, plen, slt, slen, iterations, 48, this -> authKey);
    if(ret){
        return ret;
    }

    mbedtls_md_free( &sha1_ctx);
    return 0;
}

int rDasAuth::makeBAuthKey(char *devId){
    char authKey_s[48];
    sprintf(authKey_s, "%s", this -> authKey);
    size_t mplen = 64;
    size_t inlen = 48;

    Base64 myPass;

    char* mypass64 = myPass.Encode(authKey_s, inlen, &mplen);
    strcpy(this -> bAuthKey, devId);
    strcat(this -> bAuthKey, ":");
    strcat(this -> bAuthKey, mypass64);
    
    mplen = 136;
    mypass64 = myPass.Encode(this -> bAuthKey, strlen(this -> bAuthKey), &mplen);
    strcpy(this -> bAuthKey, "Basic ");
    strcat(this -> bAuthKey, mypass64);
    printf("\n\rbauth key: %s\n\r\n\r", this -> bAuthKey);

    return 0;
}