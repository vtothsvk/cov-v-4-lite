#include "mbed.h"
#include "EthernetInterface.h"
#include "Prism_types.h"
#include "Atlas.h"
#include "BDDv3.h"
#include "RTClib.h"
#include "https_request.h"
#include "Relay.h"
#include "stm32-uid.h"
#include "pbkdfAuth.h"
#include "Base64.h"
#include "picojson.h"
#include <cstdlib>
#include <Cstring>
#include <iostream>

//RTC//
#define now  1601052960

//Relay modules
#define rm220   0
#define rm5     1
#define rm12    2

//Relays
#define blower      0
#define twoWayValve 1
#define reserved1   2
#define reserved2   3

#define slugRefreshRel  3

//Phases
#define standbyPhase     0
#define circulationPhase 1
#define aereationPhase   2

const char ssl_cert[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIEkjCCA3qgAwIBAgIQCgFBQgAAAVOFc2oLheynCDANBgkqhkiG9w0BAQsFADA/\n"
    "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n"
    "DkRTVCBSb290IENBIFgzMB4XDTE2MDMxNzE2NDA0NloXDTIxMDMxNzE2NDA0Nlow\n"
    "SjELMAkGA1UEBhMCVVMxFjAUBgNVBAoTDUxldCdzIEVuY3J5cHQxIzAhBgNVBAMT\n"
    "GkxldCdzIEVuY3J5cHQgQXV0aG9yaXR5IFgzMIIBIjANBgkqhkiG9w0BAQEFAAOC\n"
    "AQ8AMIIBCgKCAQEAnNMM8FrlLke3cl03g7NoYzDq1zUmGSXhvb418XCSL7e4S0EF\n"
    "q6meNQhY7LEqxGiHC6PjdeTm86dicbp5gWAf15Gan/PQeGdxyGkOlZHP/uaZ6WA8\n"
    "SMx+yk13EiSdRxta67nsHjcAHJyse6cF6s5K671B5TaYucv9bTyWaN8jKkKQDIZ0\n"
    "Z8h/pZq4UmEUEz9l6YKHy9v6Dlb2honzhT+Xhq+w3Brvaw2VFn3EK6BlspkENnWA\n"
    "a6xK8xuQSXgvopZPKiAlKQTGdMDQMc2PMTiVFrqoM7hD8bEfwzB/onkxEz0tNvjj\n"
    "/PIzark5McWvxI0NHWQWM6r6hCm21AvA2H3DkwIDAQABo4IBfTCCAXkwEgYDVR0T\n"
    "AQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8EBAMCAYYwfwYIKwYBBQUHAQEEczBxMDIG\n"
    "CCsGAQUFBzABhiZodHRwOi8vaXNyZy50cnVzdGlkLm9jc3AuaWRlbnRydXN0LmNv\n"
    "bTA7BggrBgEFBQcwAoYvaHR0cDovL2FwcHMuaWRlbnRydXN0LmNvbS9yb290cy9k\n"
    "c3Ryb290Y2F4My5wN2MwHwYDVR0jBBgwFoAUxKexpHsscfrb4UuQdf/EFWCFiRAw\n"
    "VAYDVR0gBE0wSzAIBgZngQwBAgEwPwYLKwYBBAGC3xMBAQEwMDAuBggrBgEFBQcC\n"
    "ARYiaHR0cDovL2Nwcy5yb290LXgxLmxldHNlbmNyeXB0Lm9yZzA8BgNVHR8ENTAz\n"
    "MDGgL6AthitodHRwOi8vY3JsLmlkZW50cnVzdC5jb20vRFNUUk9PVENBWDNDUkwu\n"
    "Y3JsMB0GA1UdDgQWBBSoSmpjBH3duubRObemRWXv86jsoTANBgkqhkiG9w0BAQsF\n"
    "AAOCAQEA3TPXEfNjWDjdGBX7CVW+dla5cEilaUcne8IkCJLxWh9KEik3JHRRHGJo\n"
    "uM2VcGfl96S8TihRzZvoroed6ti6WqEBmtzw3Wodatg+VyOeph4EYpr/1wXKtx8/\n"
    "wApIvJSwtmVi4MFU5aMqrSDE6ea73Mj2tcMyo5jMd6jmeWUHK8so/joWUoHOUgwu\n"
    "X4Po1QYz+3dszkDqMp4fklxBwXRsW10KXzPMTZ+sOPAveyxindmjkW8lGy+QsRlG\n"
    "PfZ+G6Z6h7mjem0Y+iWlkYcV4PIWL1iwBi8saCbGS5jN2p8M+X+Q7UNKEkROb3N6\n"
    "KOqkqm57TH2H3eDJAkSnh6/DNFu0Qg==\n"
    "-----END CERTIFICATE-----";
// EOC

// Booting //
void printOperationMode(void);
int atlasInit();
int makeAuthKey();

//Cascade Thread//
void circulation();
void aereation();
void standby();
void slugRefresh();
void modeChange(uint8_t cycle);

//wqltThread//
void wqlt();

//BDD measurement//
void bddMeasure();

//HTTP Thread//
void HTTPmain();
int event(char* devId, const char* logger, const char* type, float val);

//I2C interfaces//
I2C wbi2c(PB_7, PB_6);

//Event Queues//
EventQueue cascadeQueue;
EventQueue wqltQueue;
EventQueue httpQueue;

//Threads//
Thread cascadeThread;
Thread wqltThread;
Thread httpThread(osPriorityNormal, 32 * 1024);

//Ethernet interface//
//EthernetInterface* net;
NetworkInterface* net;
TLSSocket* socket;

//Relay modules//
RMv3 rm[3]{RMv3(RELAY_DEFAULT_CAN_ADDR), RMv3(RELAY_DEFAULT_CAN_ADDR + 1), RMv3(RELAY_DEFAULT_CAN_ADDR + 2)};

//BDD modules//
BDDv3 bdd;

//Prism variables//
volatile prism_data_t prism;
volatile int phaseId;
volatile uint8_t phase;
volatile float lastTemp;
volatile uint8_t newOpMode = 0;
volatile bool isOpModeChangePending = false;

static char logger[30];
static char type[30];

//HTTP Basic Auth//
//static char devId[] = "5a53379e-0821-439e-a913-1be8577acf79";
static char devId[] = "11ac4dcb-6a50-42d8-9491-ebab0c4433de";
char hwKey2[24];
rDasAuth my;
Base64 myPass;
STM32uid devHWkey;

//Atlas OEMs//
ezoPH   ph(&wbi2c);
ezoORP  orp(&wbi2c);
ezoDO   o2(&wbi2c);
ezoEC   ec(&wbi2c);

int main()
{
    cout << "Booting Prims v3.9..." << endl;
    cascadeThread.start(callback(&cascadeQueue, &EventQueue::dispatch_forever));
    wqltThread.start(callback(&wqltQueue, &EventQueue::dispatch_forever));
    //httpThread.start(callback(&httpQueue, &EventQueue::dispatch_forever));

    wqltQueue.call(&atlasInit);

    /*
    net = EthernetInterface::get_default_instance();

    cout << "Connecting..." << endl;
    net -> connect();
    const char *mac = net -> get_mac_address(); 
    cout << "MAC: " << mac << endl;

    //socket -> open(net);
    set_time(now);

    // Show the network address ma
    //const char *ip = net -> get_ip_address();
    //cout << "IP address: " << (ip ? ip : "No IP") << endl;
    makeAuthKey();
    */
    set_time(now);

    cascadeQueue.call(&slugRefresh);

    while (true) {
        //bdd.getVIP(1);
        wqltQueue.call(&wqlt);
        bddMeasure();
        ThisThread::sleep_for(20000);
    }
}

void circulation(){
    if(phase == standbyPhase){
        phase++;
        cout << "Circulation phase commenced..." << endl;

        //rm[rm220].relayCtrl(blower, ON);
        //rm[rm220].relayCtrl(twoWayValve, OFF);
        //rm[rm220].relayCtrl(reserved1, ON);

        rm[rm220].relayCtrl(blower, ON);
        rm[rm220].relayCtrl(twoWayValve, OFF);
        rm[rm220].relayCtrl(reserved1, ON);

        //sprintf(logger, "Relay-blower");
        //sprintf("Voltage", "Voltage");
        //httpQueue.call(&event, devId, "Relay-blower", "Voltage", 1);
        //sprintf(logger, "Relay-2wayValve");
        //sprintf("Voltage", "Voltage");
        //httpQueue.call(&event, devId, "Relay-2wayValve", "Voltage", 0);
        //sprintf(logger, "Relay-reserved");
        //sprintf("Voltage", "Voltage");
        //httpQueue.call(&event, devId, "Relay-reserved", "Voltage", 1);

        ThisThread::sleep_for(prism.T1[prism.operationMode]);

        cout << "Circulation phase ending..." << endl;
        if(isOpModeChangePending){
            cascadeQueue.call(&modeChange, 0);
        }else{
            cout << "normal a-call" << endl;
            phaseId = cascadeQueue.call(&aereation);
        }
    }
}

void aereation(){
    if(phase == circulationPhase){
        phase++;
        cout << "Aereation phase commenced..." << endl;

        //rm[rm220].relayCtrl(blower, ON);
        //rm[rm220].relayCtrl(twoWayValve, ON);
        //rm[rm220].relayCtrl(reserved1, OFF);

        rm[rm220].relayCtrl(blower, ON);
        rm[rm220].relayCtrl(twoWayValve, ON);
        rm[rm220].relayCtrl(reserved1, OFF);

        //static char logger[20] = "Relay-blower";
        //static char "Voltage"[20] = "Voltage";

        //sprintf(logger, "Relay-blower");
        //sprintf("Voltage", "Vlotage");
        //httpQueue.call(&event, devId, "Relay-blower", "Voltage", 1);
        //sprintf(logger, "Relay-2wayValve");
        //sprintf("Voltage", "Vlotage");
        //httpQueue.call(&event, devId, "Relay-2wayValve", "Voltage", 1);
        //sprintf(logger, "Relay-reserved");
        //sprintf("Voltage", "Vlotage");
        //httpQueue.call(&event, devId, "Relay-reserved", "Voltage", 0);

        ThisThread::sleep_for(prism.T2[prism.operationMode]);

        cout << "Aereation phase ending..." << endl;
        if(isOpModeChangePending){
            cascadeQueue.call(&modeChange, 1);
        }else{
            cout << "normal sb-call" << endl;
            phaseId = cascadeQueue.call(&standby);
        }
    }
}

void standby(){
    if(phase == aereationPhase){
        phase = standbyPhase;
        cout << "Standing by..." << endl;

        //rm[rm220].relayCtrl(blower, OFF);
        //rm[rm220].relayCtrl(twoWayValve, OFF);
        //rm[rm220].relayCtrl(reserved1, OFF);

        rm[rm220].relayCtrl(blower, OFF);
        rm[rm220].relayCtrl(twoWayValve, OFF);
        rm[rm220].relayCtrl(reserved1, OFF);

        //static char logger[20] = "Relay-blower";
        //static char "Voltage"[20] = "Voltage";

        //sprintf(logger, "Relay-blower");
        //sprintf("Voltage", "Voltage");
        //httpQueue.call(&event, devId, "Relay-blower", "Voltage", 0);
        //sprintf(logger, "Relay-2wayValve");
        //sprintf("Voltage", "Voltage");
        //httpQueue.call(&event, devId, "Relay-2wayValve", "Voltage", 0);
        //sprintf(logger, "Relay-reserved");
        //sprintf("Voltage", "Voltage");
        //httpQueue.call(&event, devId, "Relay-reserved", "Voltage", 0);

        wqltQueue.call(&wqlt);

        ThisThread::sleep_for(prism.T3[prism.operationMode] - prism.T4);

        if(isOpModeChangePending){
            cascadeQueue.call(&modeChange, 2);
        }else{
            cout << "normal refresh call" << endl;
            phaseId = cascadeQueue.call(&slugRefresh);
        }
    }
}

void slugRefresh(){
    if(phase == standbyPhase){
        cout << "Refreshing slug..." << endl;

        //rm[rm220].relayCtrl(0, ON);
        //rm[rm220].relayCtrl(3, ON);

        //rm[rm220].relayCtrl(blower, ON);
        //rm[rm220].relayCtrl(reserved2, ON);

        //static char logger[20] = "Relay-blower";
        //static char "Voltage"[20] = "Voltage";

        //sprintf(logger, "Relay-blower");
        //sprintf("Voltage", "Voltage");
        //httpQueue.call(&event, devId, "Relay-blower", "Voltage", 1);
        //sprintf(logger, "Relay-2wayValve");
        //sprintf("Voltage", "Voltage");
        //httpQueue.call(&event, devId, "Relay-2wayValve", "Voltage", 0);
        //sprintf(logger, "Relay-slug");
        //sprintf("Voltage", "Voltage");
        //httpQueue.call(&event, devId, "Relay-slug", "Voltage", 1);

        ThisThread::sleep_for(prism.T4);

        //rm[rm220].relayCtrl(3, OFF);
        rm[rm220].relayCtrl(reserved2, OFF);
        //sprintf(logger, "Relay-slug");
        //sprintf("Voltage", "Voltage");
        httpQueue.call(&event, devId, "Relay-slug", "Voltage", 0);

        if(isOpModeChangePending){
            cascadeQueue.call(&modeChange, 3);
        }else{
            cout << "normal c call" << endl;
            phaseId = cascadeQueue.call(&circulation);
        }
    }
}

void modeChange(uint8_t cycle){
    prism.operationMode = newOpMode;
    printOperationMode();
    isOpModeChangePending = false;
    cout << "akafuka" << endl;
    switch(cycle){
        case 0:
            cascadeQueue.call(&aereation);
            break;
        case 1: 
            cascadeQueue.call(&standby);
            break;
        case 2:
            cascadeQueue.call(&slugRefresh);
            break;
        case 3:
            cascadeQueue.call(&circulation);
            break;
    }
    cout << "fundaluka" << endl;
}

void wqlt(){
    status_t status;
    cout << "Measuring WQLT..." << endl;
    //led2 = 1;
    status = ph.ctrl(wake);
    if(orp.ctrl(wake)){
        //error_status_handler(status);
        cout << "Atlas Error" << endl;
    }
    o2.ctrl(wake);
    ec.ctrl(wake);

    ph.ledCtrl(ON);
    orp.ledCtrl(ON);
    o2.ledCtrl(ON);
    ec.ledCtrl(ON);

    ThisThread::sleep_for(650);

    float wTemp = 20;
    //probe[0] -> convertTemperature(true, DS1820::all_devices);
    //wTemp = probe[0] -> temperature();

    if((wTemp <= 120)&&(wTemp >= 0)){
        lastTemp = wTemp;
    }

    cout << "Water temp is: " << lastTemp + 0 << " C" << endl;

    ec.tempWrite(lastTemp);
    o2.tempWrite(lastTemp);

    ph.read();
    orp.read();
    ec.read();
    o2.writeEC(ec.lastReading());
    o2.read();

    ph.ledCtrl(OFF);
    orp.ledCtrl(OFF);
    o2.ledCtrl(OFF);
    ec.ledCtrl(OFF);

    ph.ctrl(sleep);
    orp.ctrl(sleep);
    o2.ctrl(sleep);
    ec.ctrl(sleep);

    cout << "Done" << endl;
    cout << "Publishing WQLT dadta..." << endl;
    
    //static char logger[20] = "WQLT_pH";
    //static char type[20] = "Voltage";

    //httpQueue.call(&event, devId, "WQLT_pH", "Voltage", ph.lastReading());
    //sprintf(logger, "WQLT_ORP");
    //httpQueue.call(&event, devId, "WQLT_ORP", "Voltage", orp.lastReading());
    //sprintf(logger, "WQLT_EC");
    //httpQueue.call(&event, devId, "WQLT_EC", "Voltage", ec.lastReading());
    //sprintf(logger, "WQLT_TDS");
    //httpQueue.call(&event, devId, "WQLT_TDS", "Voltage", ec.lastTDS());
    //sprintf(logger, "WQLT_DO-relative");
    //httpQueue.call(&event, devId, "WQLT_DO-relative", "Voltage", o2.lastReading());
    //sprintf(logger, "WQLT_DO-absolute");
    //httpQueue.call(&event, devId, "WQLT_DO-absolute", "Voltage", o2.lastSaturationReading());
}

void bddMeasure(){
    long epoch = -1;

    cout << "BDD meas..." << endl;

    cout << bdd.getVIP(0) << endl;
    cout << bdd.getVIP(1) << endl;

    //static char logger[20] = "BDD_CH1";
    //static char type[20] = "Voltage";


    //httpQueue.call(&event, devId, "BDD_CH1", "Voltage", bdd.voltage(0));
    //sprintf(logger, "BDD_CH2");
    cout << "BDD V: " << bdd.voltage(1) + 0.00 << endl;
    //httpQueue.call(&event, devId, "BDD_CH2", "Voltage", bdd.voltage(1));
    //sprintf(logger, "Humidity");
    //httpQueue.call(&event, devId, "BDD_CH2", "Humidity", bdd.current(1));
    //sprintf(logger, "BDD_CH1");
    //httpQueue.call(&event, devId, "BDD_CH1", "Humidity", bdd.current(0));
}

int atlasInit(){
    status_t status;

    cout << "Initialising Atlas OEM boards..." << endl;

    if((status = ph.ctrl(wake))){
        cout << "Failed to initialise Atlas Scientific OEMs!" << endl;
        return -1;
    }
    if((status = orp.ctrl(wake))){
        cout << "Failed to initialise Atlas Scientific OEMs!" << endl;
        return -1;
    }
    if((status = o2.ctrl(wake))){
        cout << "Failed to initialise Atlas Scientific OEMs!" << endl;
        return -1;
    }
    if((status = ec.ctrl(wake))){
        cout << "Failed to initialise Atlas Scientific OEMs!" << endl;
        return -1;
    }

    ph.ledCtrl(ON);
    orp.ledCtrl(ON);
    o2.ledCtrl(ON);
    ec.ledCtrl(ON);

    ThisThread::sleep_for(500);

    ph.ledCtrl(OFF);
    orp.ledCtrl(OFF);
    o2.ledCtrl(OFF);
    ec.ledCtrl(OFF);

    ph.ctrl(sleep);
    orp.ctrl(sleep);
    o2.ctrl(sleep);
    ec.ctrl(sleep);
    return 0;
}

int makeAuthKey(){
    devHWkey.get(hwKey2);
    /*cout << "UID: ";
    for(uint8_t i = 0; i < 12; i++){
        char c[3];
        printf("%02X", hwKey2[i]);
        sprintf(&hwKey2[2*i], "%02x", hwKey2[i+12]);
    }*/
    strcpy(hwKey2, "cb1f3abf5ffabcf99fabcdfa");
    my.makeKey(devId, hwKey2, 10000);
    my.makeBAuthKey(devId);

    return 0;
}

int event(char *devId, const char *logger, const char *type, float val){
    char body[2000];

    cout << "publishin to : " << logger << " - " << type << endl;

    unsigned long epoch = (unsigned long)time(NULL);

    sprintf(body, "[{\"LoggerMacAddress\": \"%s\", \"LoggerName\": \"%s\", \"Timestamp\": %ld, \"MeasuredData\": [{\"Type\": \"%s\", \"Name\": \"%s\", \"Value\": %f}], \"ServiceData\": [], \"DebugData\": [], \"DeviceId\": \"%s\"}]", logger, logger, epoch, type, type, val, devId);

    HttpsRequest* request = new HttpsRequest(net, ssl_cert, HTTP_POST, "https://fei.edu.r-das.sk:51415/api/v1/Auth");
    request -> set_header("Authorization", "Basic MTFhYzRkY2ItNmE1MC00MmQ4LTk0OTEtZWJhYjBjNDQzM2RlOm5KMUxuRUtYdlpzaU5rTjJHckwva2lrNGR6S0FaZHUxWUlzM2plSkxrZmRZVGg1dE9xUHpweDg3aUo1N1c1MTc=");
    request -> set_header("Content-Type", "application/json");
    HttpResponse* response = request->send(body, strlen(body));

    cout << response->get_body_as_string() << endl;

    delete request;

    return 0;
}

void printOperationMode(){
    cout << "Operation mode: ";

    switch(prism.operationMode){
        case Standardn3:
            cout << "Standard -3" << endl;
        break;

        case Standardn2:
            cout << "Standard -2" << endl;
        break;

        case Standardn1:
            cout << "Standard -1" << endl;
        break;

        case Standard:
            cout << "Standard" << endl;
        break;

        case Standardp1:
            cout << "Standard  +1" << endl;
        break;

        case Standardp2:
            cout << "Standard +2" << endl;
        break;

        case Start:
            cout << "Start" << endl;
        break;

        case Vacation:
            cout << "Vacation" << endl;
        break;

        case Cottage:
            cout << "Cottage" << endl;
        break;

        case Manual:
            cout << "Manual" << endl;
        break;

        default:
            cout << "N/A" << endl;
        break;
    }
}