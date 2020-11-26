/* Slovak Diamond Group
 * Faculty of Electrical Engineering andd Information technology 
 * Slovak University of Technology
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef BDDv3_h
#define BDDv3_h

#include "mbed.h"
#include "BDDv3_types.h"

/** A BDD module control interface, used for communicating with and controlling BDD modules
 * 
 * @note Synchronisation level: Thread safe
 * 
 * Example:
 * @code
 * Set up a BDD module and monitor its operation
 * #include "mbed.h"
 * 
 * #define BDD_CAN_ADDR   0x06
 * #define polarityTime  60//s
 * #define dutyCycle    100//%
 * 
 * BDDv3 bdd(BDD_CAN_ADDR);
 * 
 * int main(void){
 *     bdd.setTime(ch1, polarityTime);
 *     bdd.setTime(ch2, polarityTime);
 *     bdd.setDuty(ch1, dutyCycle);
 *     bdd.setDuty(ch2, dutyCycle);
 * 
 *     while(true){
 *         bdd.getVIP(ch1, 100);
 *         bdd.getVIP(ch2, 100);
 *         
 *         printf("  ____  _____  _____         _                            _   __ \r\n");
 *         printf(" |  _ \|  __ \|  __ \       | |                          | | /_ |\r\n");
 *         printf(" | |_) | |  | | |  | |   ___| |__   __ _ _ __  _ __   ___| |  | |\r\n");
 *         printf(" |  _ <| |  | | |  | |  / __| '_ \ / _` | '_ \| '_ \ / _ \ |  | |\r\n");
 *         printf(" | |_) | |__| | |__| | | (__| | | | (_| | | | | | | |  __/ |  | |\r\n");
 *         printf(" |____/|_____/|_____/   \___|_| |_|\__,_|_| |_|_| |_|\___|_|  |_|\r\n\r\n");
 *         printf("Voltage: %.2f V\r\n", bdd.voltage(ch1));
 *         printf("Current: %.2f A\r\n", bdd.current(ch1));
 *         printf("Power:   %.2f W\r\n\r\n", bdd.power(ch1))
 *         
 *         printf("  ____  _____  _____         _                            _   ___  \r\n");
 *         printf(" |  _ \|  __ \|  __ \       | |                          | | |__ \ \r\n");
 *         printf(" | |_) | |  | | |  | |   ___| |__   __ _ _ __  _ __   ___| |    ) |\r\n");
 *         printf(" |  _ <| |  | | |  | |  / __| '_ \ / _` | '_ \| '_ \ / _ \ |   / / \r\n");
 *         printf(" | |_) | |__| | |__| | | (__| | | | (_| | | | | | | |  __/ |  / /_ \r\n");
 *         printf(" |____/|_____/|_____/   \___|_| |_|\__,_|_| |_|_| |_|\___|_|_| |____|\r\n\r\n");
 *         printf("Voltage: %.2f V\r\n", bdd.voltage(ch2));
 *         printf("Current: %.2f A\r\n", bdd.current(ch2));
 *         printf("Power:   %.2f W\r\n\r\n", bdd.power(ch2));
 *      
 *         ThisThread::sleep_for(1000);
 *     }
 * }
 * @endcode
 */
class BDDv3{
public:
    /** Creates an instance of a BDD module control object
     * 
     *  @param ADDR module address id (0x00 - 0x4F)
     */
    BDDv3(uint8_t ADDR = DEFAULT_CAN_ADDR);

    /** Set Polarity Time of the specified module channel
     * 
     *  @param channel channel id
     *  @param time new polarity time in seconds
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t setTime(bool channel, float time);

    /** Set PWM duty cycle of the specified channel
     * 
     *  @param chanel channel id
     *  @param duty dutyc cycle
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t setDuty(bool channel, uint8_t duty);

    /** Reads the currently set polarity time of both channels
     * 
     *  @param timeout timeout time
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t getTime(int timeout = DEFAULT_TIMEOUT);

    /** Reads the currently set PWM duty cycle of both channels
     * 
     *  @param timeout timeout time
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t getDuty(int timeout = DEFAULT_TIMEOUT);

    /** Reads specified PAC voltage, current and power readings
     * 
     *  @param pac PAC id
     *  @param timeout timeout time
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t getVIP(uint8_t pac, int = DEFAULT_TIMEOUT);

    /** Reads module ambient temperature and humidity
     * 
     *  @param timeout timeout time
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t getAmbientReading(int timeout = DEFAULT_TIMEOUT);

    /** Reads temperature of the channel driving IGBT transistors
     * 
     *  @param timeout timeout time
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t getTransistorTemp(int timeout = DEFAULT_TIMEOUT);

    /** Reads water flow
     * 
     *  @param timeout timeout time
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t getFlow(int timeout = DEFAULT_TIMEOUT);

    /** Reads tube pressure
     * 
     *  @param timeout timeout time
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t getPressure(int timeout = DEFAULT_TIMEOUT);

    /** Reads both of BDD modules channels current polarity
     *
     *  @param timeout timeout time
     *
     *  @returns
     *    0 if succesfull
     *    sstatus_code otherwise
     */
    status_t getPolarity(int timeout = DEFAULT_TIMEOUT);

    /** Returns polarity time of the specified channel
     * 
     *  @param chanel channel number
     */
    float polTime(bool channel);

    /** Returns duty cycle of the specified channel
     * 
     *  @param channel number
     */ 
    float dutyCycle(bool channel);

    /** Returns specified PAC voltage
     * 
     *  @param pac PAC id
     */ 
    float voltage(uint8_t pac);

    /** Returns specified PAC current
     * 
     *  @param pac PAC id
     */ 
    float current(uint8_t pac);

    /** Returns specified PAC power
     * 
     *  @param pac PAC id
     */ 
    float power(uint8_t pac);

    /** Returns specified channel IGBT transistor temperature
     * 
     *  @param channel channel id
     */ 
    float qTemp(bool channel);

    /** Returns module ambient temperature
     */ 
    float ambientTemp();

    /** Returns module ambient humidity
     */ 
    float ambientHum();

    /** Returns water flow
     */ 
    float flow();

private:
    uint8_t moduleAddr;
    bdd_channel_t channel[2];
    float lastAmbTemp, lastAmbHum, lastFlow, lastPressure, lastqTemp[2], v[4] = {0, 0, 0, 0}, i[4] = {0, 0, 0, 0}, p[4] = {0, 0, 0, 0};
};

#endif