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
#ifndef Atlas_h
#define Atlas_h

#include "mbed.h"
#include "Atlas_types.h"

class Atlas{

public:
    /** Creates an instance of general OEM board control object
     * 
     *  @param *boardI2C pointer to the modules I2C interface
     *  @param ADDR OEM board I2C address
     * 
     *  Constructor being explicitly declared to be inheritable
     */
	explicit Atlas(I2C *boardI2C, uint8_t ADDR); //explicit declaration of constructor alowing it to be inherited

    /** General read method used for reading OEM board registers
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t read_main(void);

    /** Sends current measurement temperature to the OEM board
     * 
     *  @param temp temperature being sent
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t tempWrite(float temp);

    /** Reads currently set temperature compensation
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t tempRead(void);

    /** Sets OEM board operation mode
     * 
     *  @param status wake/sleep
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t ctrl(bool status);

    /** Sets OEM board I2C address
     * 
     *  @param ADDR desired address //see OEM board datasheet for ADDR limitations
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
	void setAddress(uint8_t ADDR);

    /** OEM board LED enable control
     * 
     *  @param state Enable/Disable
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
	status_t ledCtrl(bool state);

    /** Changes control objects I2C interface
     * 
     *  @param *newI2C pointer to the new I2C interface
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    void setI2C(I2C *newI2C);

    /** Returns OEM board currently set I2C addres
     */
	uint8_t address(void);

    /** Returns OEM boards last reading buffer
     */
    char* lastReadingBuff(void);

    /** Returns OEM boards last reading
     */
	float lastReading(void);

    /** Returns OEM boards last temperature compensation setting
     */
    float lastTemp(void);

    //test data storage
    float lastPH;
	
protected:
    uint8_t sensAddress;
    I2C *sensI2C;
    atlas_data_t last;
    atlas_data_t temp;
    atlas_registers_t registers;
    float setTemp;
};

/** An Atlas Scientific pH OEM sensor board control interface
 * 
 * @note Synchronization level: Trhead safe
 *  
 * Example:
 * @code
 * Set up a pH OEM board and periodically read pH and print it out
 * #include "mbed.h"
 * #include "Atlas.h"
 * #include <iostream>
 * 
 * I2C i2c(PB_7, PB_6);
 * 
 * ezoPH ph(&i2c, PH_DEFAULT_ADDR);
 * 
 * int main(void){
 *     ph.ctrl(wake);
 * 
 *     while(true){
 *         ThisThread::sleep_for(420); //it takes 420ms for the OEM board to take a reading
 *         ph.read();
 *         cout << "pH: " << ph.lastReading() << endl;
 *     }
 * }
 * @endcode
 */
class ezoPH: public Atlas{

public:
    /** Creates an instance of a pH OEM board control object
     * 
     *  @param *boardI2C pointer to the modules I2C interface
     *  @param ADDR OEM board I2C address
     * 
     *  @note Constructor being inherited from Atlas class and modified for the pH OEM board;
     */
    explicit ezoPH(I2C *boardI2C, uint8_t ADDR = PH_DEFAULT_ADDR) : Atlas(boardI2C, ADDR) {
        this -> registers.readReg = phRead_reg;
        this -> registers.tempReg = phTemp_reg_w;
        this -> registers.tempReg_r = phTemp_reg_r;
    }

    /** Gets last ph OEM board reading
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t read(void);

    /** Calibrate the ph OEM board at the sepicifed point to a given pH value
     * 
     *  @param point calibration point
     *  @param n calibration value
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t cal(uint8_t point, float n = NA);

private:

    /** Confirm calibrated points
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t calConfirm(void);
};

/** An Atlas Scientific ORP OEM sensor board control interface
 * 
 * @note Synchronization level: Trhead safe
 *  
 * Example:
 * @code
 * Set up a ORP OEM board and periodically read ORP and print it out
 * #include "mbed.h"
 * #include "Atlas.h"
 * #include <iostream>
 * 
 * I2C i2c(PB_7, PB_6);
 * 
 * ezoORP orp(&i2c, ORP_DEFAULT_ADDR);
 * 
 * int main(void){
 *     orp.ctrl(wake);
 * 
 *     while(true){
 *         ThisThread::sleep_for(420); //it takes 420ms for the OEM board to take a reading
 *         orp.read();
 *         cout << "ORP: " << orp.lastReading() <<  " mV" << endl;
 *     }
 * }
 * @endcode
 */
class ezoORP: public Atlas{

public:

    /** Creates an instance of a ORP OEM board control object
     * 
     *  @param *boardI2C pointer to the modules I2C interface
     *  @param ADDR OEM board I2C address
     * 
     *  @note Constructor being inherited from Atlas class and modified for the ORP OEM board;
     */
    explicit ezoORP(I2C *boardI2C, uint8_t ADDR = ORP_DEFAULT_ADDR) : Atlas(boardI2C, ADDR) { 
        this -> registers.readReg = orpRead_reg;
        this -> registers.tempReg = 0x00;
        this -> registers.tempReg_r = 0x00;
    }

    /** Reads the ORP OEM board last ORP reading
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t read(void);

    /** Calibrate the ORP OEM board at the sepicifed point to a given pH value
     * 
     *  @param point calibration point
     *  @param n calibration value
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t cal(uint8_t point, uint16_t n);
};

/** An Atlas Scientific DO OEM sensor board control interface
 * 
 * @note Synchronization level: Trhead safe
 *  
 * Example:
 * @code
 * Set up a DO OEM board and periodically read absolute DO and DO saturation and print them out
 * #include "mbed.h"
 * #include "Atlas.h"
 * #include <iostream>
 * 
 * I2C i2c(PB_7, PB_6);
 * 
 * ezoDO o2(&i2c, DO_DEFAULT_ADDR);
 * 
 * int main(void){
 *     o2.ctrl(wake);
 * 
 *     while(true){
 *         ThisThread::sleep_for(420); //it takes 420ms for the OEM board to take a reading
 *         o2.read();
 *         cout << "DO: " << o2.lastReading() << " mg/l" << endl;
 *         cout << "DO: " << o2.lastSaturationReading() << "%" << endl;
 *     }
 * }
 * @endcode
 */
class ezoDO: public Atlas{

public:
    /** Creates an instance of a DO OEM board control object
     * 
     *  @param *boardI2C pointer to the modules I2C interface
     *  @param ADDR OEM board I2C address
     * 
     *  @note Constructor being inherited from Atlas class and modified for the DO OEM board;
     */
    explicit ezoDO(I2C *boardI2C, uint8_t ADDR = DO_DFEAULT_ADDR) : Atlas(boardI2C, ADDR) {
        this -> registers.readReg = doRead_reg;
        this -> registers.tempReg = doTemp_reg_w;
        this -> registers.tempReg_r = doTemp_reg_r;
    }

    /** Reads the DO OEM boards last DO reading (both DO absolute and DO saturation)
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t read(void);

    /** Calibrates the DO OEM board at a given point
     * 
     *  @param point dry/zero
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t cal(uint8_t point);

    /** Sets DO OEM boards electrical conductivity calibration value
     * 
     *  @param EC electrical conductivity value
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t writeEC(float EC);

    /** Returns the last DO saturation reading
     */
    float lastSaturationReading(void);

    private:
    atlas_data_t saturation;
};

/** An Atlas Scientific EC OEM sensor board control interface
 * 
 * @note Synchronization level: Trhead safe
 *  
 * Example:
 * @code
 * Set up a EC OEM board and periodically read EC and TDS and print them out
 * #include "mbed.h"
 * #include "Atlas.h"
 * #include <iostream>
 * 
 * I2C i2c(PB_7, PB_6);
 * 
 * ezoEC ec(&i2c, EC_DEFAULT_ADDR);
 * 
 * int main(void){
 *     ec.ctrl(wake);
 * 
 *     while(true){
 *         ThisThread::sleep_for(420); //it takes 420ms for the OEM board to take a reading
 *         ec.read();
 *         cout << "EC: " << ec.lastReading() << " uS" << endl;
 *         cout << "TDS: " << o2.lastTDS() << " uS" << endl;
 *     }
 * }
 * @endcode
 */
class ezoEC: public Atlas{

public:
    /** Creates an instance of a EC OEM board control object
     * 
     *  @param *boardI2C pointer to the modules I2C interface
     *  @param ADDR OEM board I2C address
     * 
     *  @note Constructor being inherited from Atlas class and modified for the EC OEM board;
     */
    explicit ezoEC(I2C *boardI2C, uint8_t ADDR = EC_DEFAULT_ADDR) : Atlas(boardI2C, ADDR) { 
        this -> registers.readReg = sRead_reg;
        this -> registers.tempReg = sTemp_reg_w;
        this -> registers.tempReg_r = sTemp_reg_r;
    }

    /** Reads the EC OEM boards last EC and TDS reading
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t read(void);

    /** Sets probe type calibration setting
     * 
     *  @param probe probe type
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t setProbe(float probe);

    /** Reads last probe setting
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t readProbeSetting(void);

    /** Reads last TDS reading
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    [[depreached ("included in ezoEC::read()")]] status_t readTDS(void);

    /** Reads lsast PSS reading
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t readPSU(void);

    /** Calibrates the EC OEM board at a specific point to a given value
     * 
     *  @param point caclibration point
     *  @param n calibration value
     * 
     *  @returns
     *    0 if succesfull
     *    status_code otherwise
     */
    status_t cal(uint8_t point, uint32_t n);

    /** Returns the last probe setting
     */
    float probeSetting(void);

    /** Returns the laste TDS reading
     */
    float lastTDS(void);

    /** Returns the last PSU reading
     */
    float lastPSU(void);

private:
    atlas_data_t tds;
    atlas_data_t pss;
    atlas_probe_t probe;
};

/** Checks whether a new reading is availible
 * 
 *  @returns
 *    1 true
 *    0 false
 */
bool isNewRead_a(I2C *boardI2C, uint8_t ADDR);

#endif