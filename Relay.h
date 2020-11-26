#ifndef Relay_h
#define Relay_h

#include "mbed.h"
#include "Relay_types.h"

/** A Relay module control interface, used for controlling individual RM channels
 * 
 * @note Synchronisation level: Thread safe
 * 
 * Example:
 * @code
 * Set up a Relay module, swhitch on a relay and monitor its state and current
 * #include "mbed.h"
 * #include "Relay.h"
 * 
 * #defime RM_CAN_ADDR  0x06
 * 
 */
class RMv3{
public:
    RMv3(int ADDR = RELAY_DEFAULT_CAN_ADDR);
    status_t relayCtrl(uint8_t relay, bool state);
    status_t getRelayStates(int timeout = DEFAULT_TIMEOUT);
    status_t getVIP(int timeout = DEFAULT_TIMEOUT);
    bool relayState(uint8_t relay);
    float relayCurrent(uint8_t relay);
    float voltage(void);
    float current(void);
    float power(void);

private:
    int moduleAddr;
    relay_t relay[4];
    float v,i,p;
};

#endif