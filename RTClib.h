#ifndef RTClib_h
#define RTClib_h

#include "mbed.h"
#include "RTClib_tpyes.h"
#include <iostream>
#include <ctime>


/** An PCF real time clock controll interface
 * 
 * @note Synchronization level: Trhead safe
 *  
 * Example:
 * @code
 * Set up RTC time and date and periodically readd it and print it out
 * #include "mbed.h"
 * #include "RTClib.h"
 * #include <iostream>
 * 
 * I2C i2c(PB_7, PB_6);
 * 
 * pcfRTC rtc(&i2c);
 * 
 * int main(void){
 *     if(rtc.init()){
 *         cout << "ERROR: counldn't initialise RTC..."
 *     }
 *     
 *     time_t epoch = INSERT_UNIX_TIMESTAMP_HERE;
 *     rtc.set(epoch);
 * 
 *     while(true){
 *         rtc.read();
 *         rtc.printTime();
 *         ThisThread::sleep_for(1000);
 *     }
 * }
 * @endcode
 */
class pcfRTC{
public:
    /** Creates an instance of a PCF real time clock control interface
     * 
     *  @param *boardI2C poitner to an I2C interface
     */
    pcfRTC(I2C *boardI2C);

    /** Initialises the PCF real time clock IC
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t init(void);

    /** Sets RTC time to a given value
     * 
     *  @param t unix timestamp
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t set(time_t t);

    /** Reads current RTC time
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t read(void);

    /** Returns last unix timestamp read from the RTC
     */
    unsigned long lastT(void);

    /** Prints out last time read from the RTC
     * 
     *  @note format: sec, min, hour, day, month, year
     */
    void printTime(void);
private:
    I2C *rtcI2C;
    struct tm last;
};

/** Used to convet binary values to BCD
 *  
 *  @param val binary value
 * 
 *  @returns 
 *    val in BCD format
 */
static uint8_t bin2bcd (uint8_t val) { return (val + 6 * (val / 10)); }

/** Used to convert BCD values to binary
 * 
 *  @param val BCD value
 *  
 *  @returns 
 *    val in binary
 */
static uint8_t bcd2bin (uint8_t val) { return (val - 6 * (val >> 4)); }

#endif