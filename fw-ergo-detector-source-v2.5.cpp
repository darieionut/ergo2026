#include <avr/wdt.h>

#include <Arduino.h>

#ifndef MAINCONFIG_H
#define MAINCONFIG_H

#define APP_VERSION 25

#define DEVICE_TYPE 1

#define WATCHDOG_ENABLED true

#define SERIAL_BAUDRATE 19200

#define DEFAULT_DEVICE_ID 65000

#define DEFAULT_RECEIVER_ADDRESS 1

#define DISABLE_CC1101_RECEIVE

#define DEBUG_ENABLED false

#endif

#ifndef LEDCONFIG_H
#define LEDCONFIG_H

#define BLINK_INTERVAL_MS 300

#define LED_COLOR_NONE 0
#define LED_COLOR_RED 1

#define LED_STATUS_COUNT 5

#define LED_STATUS_NONE 0
#define LED_STATUS_OK 1
#define LED_STATUS_WARNING 2
#define LED_STATUS_ERROR 3
#define LED_STATUS_COMM_ERROR 4

#endif

#ifndef CC1101CONFIG_H
#define CC1101CONFIG_H

#define CC1101_Interrupt 1    
#define CC1101_GDO2 3         

#define CC1101_MAX_BUFFER_SIZE 30       
#define CC1101_MAX_TX_BUFFER_SIZE 15    
#define CC1101_MAX_RX_BUFFER_SIZE 100   

#define WRITE_BURST              0x40
#define READ_SINGLE              0x80
#define READ_BURST               0xC0

#define CC1101_CONFIG_REGISTER   READ_SINGLE
#define CC1101_STATUS_REGISTER   READ_BURST

#define CC1101_PATABLE           0x3E        
#define CC1101_TXFIFO            0x3F        
#define CC1101_RXFIFO            0x3F        

#define CC1101_SRES              0x30        
#define CC1101_SFSTXON           0x31        

#define CC1101_SXOFF             0x32        
#define CC1101_SCAL              0x33        

#define CC1101_SRX               0x34        
#define CC1101_STX               0x35        

#define CC1101_SIDLE             0x36        
#define CC1101_SWOR              0x38        

#define CC1101_SPWD              0x39        
#define CC1101_SFRX              0x3A        
#define CC1101_SFTX              0x3B        
#define CC1101_SWORRST           0x3C        
#define CC1101_SNOP              0x3D        

#define CC1101_IOCFG2            0x00        
#define CC1101_IOCFG1            0x01        
#define CC1101_IOCFG0            0x02        
#define CC1101_FIFOTHR           0x03        
#define CC1101_SYNC1             0x04        
#define CC1101_SYNC0             0x05        
#define CC1101_PKTLEN            0x06        
#define CC1101_PKTCTRL1          0x07        
#define CC1101_PKTCTRL0          0x08        
#define CC1101_ADDR              0x09        
#define CC1101_CHANNR            0x0A        
#define CC1101_FSCTRL1           0x0B        
#define CC1101_FSCTRL0           0x0C        
#define CC1101_FREQ2             0x0D        
#define CC1101_FREQ1             0x0E        
#define CC1101_FREQ0             0x0F        
#define CC1101_MDMCFG4           0x10        
#define CC1101_MDMCFG3           0x11        
#define CC1101_MDMCFG2           0x12        
#define CC1101_MDMCFG1           0x13        
#define CC1101_MDMCFG0           0x14        
#define CC1101_DEVIATN           0x15        
#define CC1101_MCSM2             0x16        
#define CC1101_MCSM1             0x17        
#define CC1101_MCSM0             0x18        
#define CC1101_FOCCFG            0x19        
#define CC1101_BSCFG             0x1A        
#define CC1101_AGCCTRL2          0x1B        
#define CC1101_AGCCTRL1          0x1C        
#define CC1101_AGCCTRL0          0x1D        
#define CC1101_WOREVT1           0x1E        
#define CC1101_WOREVT0           0x1F        
#define CC1101_WORCTRL           0x20        
#define CC1101_FREND1            0x21        
#define CC1101_FREND0            0x22        
#define CC1101_FSCAL3            0x23        
#define CC1101_FSCAL2            0x24        
#define CC1101_FSCAL1            0x25        
#define CC1101_FSCAL0            0x26        
#define CC1101_RCCTRL1           0x27        
#define CC1101_RCCTRL0           0x28        
#define CC1101_FSTEST            0x29        
#define CC1101_PTEST             0x2A        
#define CC1101_AGCTEST           0x2B        
#define CC1101_TEST2             0x2C        
#define CC1101_TEST1             0x2D        
#define CC1101_TEST0             0x2E        

#define CC1101_PARTNUM           0x30        
#define CC1101_VERSION           0x31        
#define CC1101_FREQEST           0x32        
#define CC1101_LQI               0x33        
#define CC1101_RSSI              0x34        
#define CC1101_MARCSTATE         0x35        
#define CC1101_WORTIME1          0x36        
#define CC1101_WORTIME0          0x37        
#define CC1101_PKTSTATUS         0x38        
#define CC1101_VCO_VC_DAC        0x39        
#define CC1101_TXBYTES           0x3A        
#define CC1101_RXBYTES           0x3B        
#define CC1101_RCCTRL1_STATUS    0x3C        
#define CC1101_RCCTRL0_STATUS    0x3D        

#define CC1101_DEFVAL_IOCFG2     0x06        
#define CC1101_DEFVAL_IOCFG1     0x2E        
#define CC1101_DEFVAL_IOCFG0     0x2E        
#define CC1101_DEFVAL_FIFOTHR    0x07        
#define CC1101_DEFVAL_SYNC1      0xB5        
#define CC1101_DEFVAL_SYNC0      0x47        
#define CC1101_DEFVAL_PKTLEN     0x3D        
#define CC1101_DEFVAL_PKTCTRL1   0x06        
#define CC1101_DEFVAL_PKTCTRL0   0x05        
#define CC1101_DEFVAL_ADDR       0x00        
#define CC1101_DEFVAL_CHANNR     0x00        
#define CC1101_DEFVAL_FSCTRL1    0x08        
#define CC1101_DEFVAL_FSCTRL0    0x00        
#define CC1101_DEFVAL_FREQ2_868  0x21        
#define CC1101_DEFVAL_FREQ1_868  0x62        
#define CC1101_DEFVAL_FREQ0_868  0x76        
#define CC1101_DEFVAL_MDMCFG4    0xCA        
#define CC1101_DEFVAL_MDMCFG3    0x83        
#define CC1101_DEFVAL_MDMCFG2    0x93        
#define CC1101_DEFVAL_MDMCFG1    0x22        
#define CC1101_DEFVAL_MDMCFG0    0xF8        
#define CC1101_DEFVAL_DEVIATN    0x35        
#define CC1101_DEFVAL_MCSM2      0x07        
#define CC1101_DEFVAL_MCSM1      0x20        
#define CC1101_DEFVAL_MCSM0      0x18        
#define CC1101_DEFVAL_FOCCFG     0x16        
#define CC1101_DEFVAL_BSCFG      0x6C        
#define CC1101_DEFVAL_AGCCTRL2   0x43        
#define CC1101_DEFVAL_AGCCTRL1   0x40        
#define CC1101_DEFVAL_AGCCTRL0   0x91        
#define CC1101_DEFVAL_WOREVT1    0x87        
#define CC1101_DEFVAL_WOREVT0    0x6B        
#define CC1101_DEFVAL_WORCTRL    0xFB        
#define CC1101_DEFVAL_FREND1     0x56        
#define CC1101_DEFVAL_FREND0     0x10        
#define CC1101_DEFVAL_FSCAL3     0xE9        
#define CC1101_DEFVAL_FSCAL2     0x2A        
#define CC1101_DEFVAL_FSCAL1     0x00        
#define CC1101_DEFVAL_FSCAL0     0x1F        
#define CC1101_DEFVAL_RCCTRL1    0x41        
#define CC1101_DEFVAL_RCCTRL0    0x00        
#define CC1101_DEFVAL_FSTEST     0x59        
#define CC1101_DEFVAL_PTEST      0x7F        
#define CC1101_DEFVAL_AGCTEST    0x3F        
#define CC1101_DEFVAL_TEST2      0x81        
#define CC1101_DEFVAL_TEST1      0x35        
#define CC1101_DEFVAL_TEST0      0x09        

#define CCDEF_CHANNR  CC1101_DEFVAL_CHANNR
#define CCDEF_SYNC0  CC1101_DEFVAL_SYNC0
#define CCDEF_SYNC1  CC1101_DEFVAL_SYNC1
#define CCDEF_ADDR  CC1101_DEFVAL_ADDR

#endif

#ifndef BUTTONCONTROLLER_H
#define BUTTONCONTROLLER_H



#ifndef BUTTONCONFIG_H
#define BUTTONCONFIG_H

#define BTN_COUNT 1
#define BTN_DEBOUNCE_PERIOD 40

#endif

struct btnState {
    uint8_t pin;
    boolean lastValue;
    uint32_t timestamp;
    boolean clicked;
    boolean stateChanged;
};

#define BTN_TEST 0

class ButtonController
{
    private :
        boolean clickedStates[BTN_COUNT];
        boolean currentState;
        btnState btnStates[BTN_COUNT];          
        int16_t i;
    public :
        ButtonController();                             
        void setup();                                   
        void tick(uint32_t currentMillis);         
        boolean hasAnyStateChanged();                      
        boolean hasStateChanged(uint8_t button);     
        boolean* getAllClickedStates();                    
        boolean getClickedState(uint8_t button);     
};

#endif

#ifndef BUZZERCONTROLLER_H
#define BUZZERCONTROLLER_H



class BuzzerController
{
    private :
        boolean state, running;
        uint32_t lastChangeMillis;
    public :
        BuzzerController();                     
        void setup();                           
        void tick(uint32_t currentMillis);      
        void start();                           
        void stop();                            
};

#endif

#ifndef CC1101Controller_H
#define CC1101Controller_H


#include <SPI.h>

#ifndef CCPACKET_H
#define CCPACKET_H

struct CCPACKET
{
  public:
    uint8_t length;                         
    uint8_t data[CC1101_MAX_BUFFER_SIZE];   
    boolean crc_ok;                         
    int8_t rssi;                            
    uint8_t lqi;                            
};

#endif

#ifndef EEPROMCONTROLLER_H
#define EEPROMCONTROLLER_H


#include <extEEPROM.h>

#ifndef TYPESCONFIG_H
#define TYPESCONFIG_H

union ConvertBytesToLong {
    uint8_t byte[4];
    int32_t longVal;
};

union ConvertBytesToULong {
    uint8_t byte[4];
    uint32_t longVal;
};

union ConvertBytesToInt {
    uint8_t byte[2];
    int16_t intVal;
};

union ConvertBytesToUInt {
    uint8_t byte[2];
    uint16_t intVal;
};

#endif

class EEPROMController {
    private :
        extEEPROM eepr;
        uint8_t i2cState;
        ConvertBytesToULong convertBytesToULong;
        ConvertBytesToInt convertBytesToInt;
        ConvertBytesToUInt convertBytesToUInt;

        int16_t i, j, aux;
        boolean checkerAux;
    public :
        EEPROMController();
        void setup();                                   
        boolean getI2CStatus();                         

        void wipeEEPROM();                              

        void setSerialNumber(uint32_t serialNumber);    
        uint32_t getSerialNumber();                     

        void setStartupFlag(boolean state);             
        boolean getStartupFlag();                       

        void setByteValue(uint32_t addr, uint8_t val);  
        uint8_t getByteValue(uint32_t addr);            
        void setUIntValue(uint32_t addr, uint16_t val); 
        uint16_t getUIntValue(uint32_t addr);           
};

#endif

enum RFSTATE
{
  RFSTATE_IDLE = 0,
  RFSTATE_RX,
  RFSTATE_TX
};

#define readConfigReg(regAddr)    readReg(regAddr, CC1101_CONFIG_REGISTER)

#define readStatusReg(regAddr)    readReg(regAddr, CC1101_STATUS_REGISTER)

#define setIdleState()            cmdStrobe(CC1101_SIDLE)

#define flushRxFifo()             cmdStrobe(CC1101_SFRX)

#define flushTxFifo()             cmdStrobe(CC1101_SFTX)

#define disableAddressCheck()     writeReg(CC1101_PKTCTRL1, 0x04)

#define enableAddressCheck()      writeReg(CC1101_PKTCTRL1, 0x06)

#define disableCCA()              writeReg(CC1101_MCSM1, 0)

#define enableCCA()               writeReg(CC1101_MCSM1, CC1101_DEFVAL_MCSM1)

#define PA_LowPower               0x60
#define PA_LongDistance           0xC0

class CC1101Controller
{
  private:
    EEPROMController *eepromController;
    SPISettings *spiSettings;

    uint8_t addr, i, val, marcState, rxBytes, rssi_dec, rssi_offset;
    bool res;
    int tries;

    void writeBurstReg(uint8_t regAddr, uint8_t* buffer, uint8_t len);

    void readBurstReg(uint8_t * buffer, uint8_t regAddr, uint8_t len);

    void setValuesFromEeprom(void);

  public:

    uint8_t rfState;

    uint8_t channel;

    uint8_t syncWord[2];

    uint8_t devAddress;

    CC1101Controller(EEPROMController *eepromController);

    void cmdStrobe(uint8_t cmd);

    void wakeUp(void);

    uint8_t readReg(uint8_t regAddr, uint8_t regType);

    void writeReg(uint8_t regAddr, uint8_t value);

    void setCCregs(void);

    void reset(void);

    void init(void);

    void setSyncWord(uint8_t syncH, uint8_t syncL);

    void setSyncWord(uint8_t *sync);

    void setDevAddress(uint8_t addr);

    void setChannel(uint8_t chnl);

    void setPowerDownState();

    bool sendData(CCPACKET packet);

    uint8_t receiveData(CCPACKET *packet);

    void setRxState(void);

    void setTxState(void);

    void setTxPowerAmp(uint8_t paLevel);

    int computeRssi(uint8_t rssi_dec);

    int computeLqi(uint8_t raw);

    void gotoIdleState(void);
};

#endif

#ifndef LEDCONTROLLER_H
#define LEDCONTROLLER_H



struct ledColor {
    uint8_t valueRed;
    uint8_t animateRed;
};

#define LED_ANIM_ON 0
#define LED_ANIM_SLOW_FADE 1
#define LED_ANIM_NORMAL_FADE 2
#define LED_ANIM_FAST_FADE 3
#define LED_ANIM_PULSE 4

#define LED_COLOR_OFF 0
#define LED_COLOR_RED 1

#define LED_SEMAPHORE_OFF 0
#define LED_SEMAPHORE_RED 1
#define LED_SEMAPHORE_YELLOW 2
#define LED_SEMAPHORE_GREEN 3

class LEDController
{
    private :
        uint8_t currentAnimation, lastSemaphoreColor, lastSemaphoreState;
        int16_t incrementor, pulseTimer;
        void assignColor(ledColor *copyTo, ledColor *copyFrom);   
        boolean isDifferentColor(ledColor cmp1, ledColor cmp2);   
        boolean _canWriteRed;
        ledColor currentColor, currentSetColor, predefinedColors[2];
        uint32_t lastSemaphoreChange;
    public :
        LEDController();                        
        void setup();                           
        void tick(uint32_t currentMillis);      
        void setColor(uint8_t predefinedColor); 
        void setAnimation(uint8_t animation);   
        void set(uint8_t predefinedColor, uint8_t animation);   
        void resetCurrentColor();               
        void setSemaphore(uint8_t color);       
};

#endif

#ifndef TGSCONTROLLER_H
#define TGSCONTROLLER_H



class TGSController
{
    private :
        uint16_t currentValue, lastValue;
        boolean valueChanged;
    public :
        TGSController();            
        void setup();               
        void tick();                
        boolean hasValueChanged();  
        uint16_t getValue();         
};

#endif

#ifndef OUTPUTCONTROLLER_H
#define OUTPUTCONTROLLER_H



#ifndef OUTPUTSCONFIG_H
#define OUTPUTSCONFIG_H

#define OUTPUTS_DEFAULT_ONESHOT_DELAY 3000

#define OUTPUTS_DEFAULT_PULSE_DELAY 1000

#define OUTPUTS_PULSE_COUNT 3

#define OUTPUTS_COUNT 1

#endif

#define RELAY_AC 0

class OutputController {
    private:
        boolean outputState[OUTPUTS_COUNT];             
        uint32_t outputTimer[OUTPUTS_COUNT];            
        uint8_t outputPulseCount[OUTPUTS_COUNT];        
        uint8_t outputPins[OUTPUTS_COUNT];              

        int16_t i;
    public:
        OutputController();                             
        void setup();                                   
        void tick(uint32_t currentMillis);              

        boolean get(int16_t outputId);                                      
        void set(int16_t outputId, boolean state);                          
        void set(int16_t outputId, boolean state, uint32_t timeout);        
        void setPulsed(int16_t outputId, boolean state, uint8_t pulses);    
};

#endif

#ifndef ALARMSERVICE_H
#define ALARMSERVICE_H



#ifndef SERIALDEBUG_H
#define SERIALDEBUG_H



#define DEC 10

class SerialDebug
{
    private :
        boolean debugEnabled;
    public :
        SerialDebug();                                  
        SerialDebug(boolean enableDebug);               
        void setup();                                   
        void setDebugEnabled(boolean state);            
        boolean getDebugEnabled();                      
        void printDebugStart();                         

        size_t print(const __FlashStringHelper *);
        size_t print(const String &);
        size_t print(const char[]);
        size_t print(int8_t);
        size_t print(uint8_t, int16_t = DEC);
        size_t print(int16_t, int16_t = DEC);
        size_t print(uint16_t, int16_t = DEC);
        size_t print(int32_t, int16_t = DEC);
        size_t print(uint32_t, int16_t = DEC);
        size_t print(float, int16_t = 2);
        size_t print(const Printable&);

        size_t println(const __FlashStringHelper *);
        size_t println(const String &s);
        size_t println(const char[]);
        size_t println(int8_t);
        size_t println(uint8_t, int16_t = DEC);
        size_t println(int16_t, int16_t = DEC);
        size_t println(uint16_t, int16_t = DEC);
        size_t println(int32_t, int16_t = DEC);
        size_t println(uint32_t, int16_t = DEC);
        size_t println(float, int16_t = 2);
        size_t println(const Printable&);
        size_t println(void);
};

#endif

#ifndef COMMUNICATIONSERVICE_H
#define COMMUNICATIONSERVICE_H



#define RSSI_LQI_SAMPLES 3

#define BUFF_INDEX_RECEIVER_LO 0
#define BUFF_INDEX_RECEIVER_HI 1
#define BUFF_INDEX_ADDRESS_LO 2
#define BUFF_INDEX_ADDRESS_HI 3
#define BUFF_INDEX_ID_LO 4
#define BUFF_INDEX_ID_HI 5
#define BUFF_INDEX_COMMAND 6
#define BUFF_INDEX_IS_FORWARD 7

#define COMMAND_ID_TEST_RSSI 1
#define COMMAND_ID_ALARM_START 10
#define COMMAND_ID_ALARM_STOP 11
class CommunicationService {
    private :
        CC1101Controller *radio;
        CCPACKET packet;
        SerialDebug *debug;
        EEPROMController *eepromController;
        LEDController *ledController;

        uint8_t radioCommunicationStatus;
        uint16_t deviceAddress, receiverAddress, deviceId;
        ConvertBytesToUInt convertBytesToUInt;

        uint8_t syncWord[2] = {199, 10};
        int16_t lastRssi[RSSI_LQI_SAMPLES], lastLqi[RSSI_LQI_SAMPLES], avarageRssi, avarageLqi;
        uint8_t indexRssiLqi, previousIndexRssiLqi, lqiPercent, rssiPercent;

        int16_t i;
    public :
        CommunicationService(CC1101Controller *radio, SerialDebug *debug, EEPROMController *eepromController, LEDController *ledController);
        void setup();                               
        void reloadEepromValues();                  

        void send(const uint8_t *buff, uint8_t *len);         
        void receive(uint8_t *buff, uint8_t *len);     

        void testConnection();                      
        int16_t getComputedLinkQuality(int16_t lqi, int16_t rssi);  
        int16_t getAvarageLinkQuality();                    
        int16_t getLastMessageComputedLinkQuality();        

        void sendAlarmStartPackage(uint8_t *buff, uint8_t *len);     
        void sendAlarmStopPackage(uint8_t *buff, uint8_t *len);     
};

#endif

class AlarmService {
    private :
        SerialDebug *debug;
        ButtonController *buttonController;
        BuzzerController *buzzerController;
        EEPROMController *eepromController;
        LEDController *ledController;
        TGSController *tgsController;
        CommunicationService *communicationService;
        OutputController *outputController;

        uint16_t currentAlarmValue, alarmTriggerValue, alarmCount;
        uint8_t lastLedBlinkMode, activatedAlarmSendCount, deactivatedAlarmSendCount;
        bool isOtherAlert, testModeActive, alarmStarted, skipStartup;

        uint32_t lastSendStatusMillis, nextSendStatusMillis, alarmRunningSinceMillis;

        uint8_t *sendBuffer;
        uint8_t *sendBufferLen;

        void startAlarm();
        void stopAlarm();
        void checkOtherAlert();
        void generateNextSendStatusMillis();
    public :
        AlarmService(SerialDebug *debug, ButtonController *buttonController, BuzzerController *buzzerController, 
            EEPROMController *eepromController, LEDController *ledController, TGSController *tgsController, 
            CommunicationService *communicationService, OutputController *outputController);
        void setup();                                           
        void tick(uint32_t currentMillis);                      

        void setSendBuffer(uint8_t *sendBufferPtr, uint8_t *sendBufferLenPtr);   
        void sendPeriodicStatus(uint32_t currentMillis);        

        void setAlarmTriggerValue(uint16_t alarmTrigger);       
        uint16_t getAlarmTriggerValue();                        

        void setIsOtherAlert(bool isOtherAlertParam);                

        void setSkipStartup(bool state);                        
};

#endif

#ifndef PROGRAMMINGSERVICE_H
#define PROGRAMMINGSERVICE_H



#ifndef SERVICEMODE_H
#define SERVICEMODE_H



class ServiceMode
{
    private :
        BuzzerController *buzzerController;
        LEDController *ledController;
        OutputController *outputController;
        AlarmService *alarmService;
        CommunicationService *communicationService;

        bool testRunning;

        uint8_t *sendBuffer;
        uint8_t *sendBufferLen;
    public :
        ServiceMode(BuzzerController *buzzerController, LEDController *ledController, OutputController *outputController, 
            AlarmService *alarmService, CommunicationService *communicationService);

        void test1Start();      
        void test1Stop();       
        void test2Start();      
        void test3Start();      

        bool isTestRunning();   
        void setSendBuffer(uint8_t *sendBuffer, uint8_t *sendBufferLen);   
};

#endif

#define SERIAL_RECEIVE_BUFF_SIZE 50
#define SERIAL_WORKING_BUFF_SIZE 25
#define SERIAL_SEND_BUFF_SIZE 25
#define RECV_MSG_MIN_LEN 2
#define RECV_MSG_MAX_LEN 21

struct CompleteData {
    uint16_t address;
    uint8_t channel;
    uint16_t receiverAddress;
    uint16_t deviceId;
    int16_t alarmTrigger;
    boolean debugEnabled;
};

class ProgrammingService {
    private :
        AlarmService *alarmService;
        SerialDebug *serialDebug;
        CC1101Controller *cc1101Controller;
        EEPROMController *eepromController;
        CommunicationService *communicationService;
        ServiceMode *serviceMode;

        uint8_t receiveBuffer[SERIAL_RECEIVE_BUFF_SIZE], workingBuffer[SERIAL_WORKING_BUFF_SIZE], responseBuffer[SERIAL_SEND_BUFF_SIZE];
        uint8_t availableBytesToRead;
        int16_t i, j, k, crc;
        CompleteData dataTemp;

        ConvertBytesToInt convertBytesToInt;
        ConvertBytesToLong convertBytesToLong;

        void serialProgramming();                           
        uint8_t calculateCRC(const uint8_t *buff, uint8_t len);   
        void sendResponse();                                
        void sendResponseOK(uint8_t commandId);             
        void sendResponseFail(uint8_t commandId);           
    public :
        ProgrammingService(AlarmService *alarmService, SerialDebug *serialDebug, CC1101Controller *cc1101Controller, 
            EEPROMController *eepromController, CommunicationService *communicationService, ServiceMode *serviceMode);
        void setup();
        void tick();
};

#endif

#ifndef STARTUP_H
#define STARTUP_H



class Startup
{
    private :
        EEPROMController *eepr;
    public :
        Startup(EEPROMController *eepr);
        void doStartup();                   
};

#endif

boolean packetWaiting, startupFlag;
uint8_t sendBuffer[CC1101_MAX_TX_BUFFER_SIZE];
uint8_t sendBufferLen;

#ifndef DISABLE_CC1101_RECEIVE
    char recvBuffer[CC1101_MAX_RX_BUFFER_SIZE];
    uint8_t recvBufferLen;
#endif

SerialDebug debug;

ButtonController buttonController;
BuzzerController buzzerController;
EEPROMController eepromController;
LEDController ledController;
OutputController outputController;
TGSController tgsController;
CC1101Controller cc1101Controller(&eepromController);

Startup startup(&eepromController);

CommunicationService communicationService(&cc1101Controller, &debug, &eepromController, &ledController);
AlarmService alarmService(&debug, &buttonController, &buzzerController, &eepromController, &ledController, 
    &tgsController, &communicationService, &outputController);
ServiceMode serviceMode(&buzzerController, &ledController, &outputController, &alarmService, &communicationService);
ProgrammingService programmingService(&alarmService, &debug, &cc1101Controller, &eepromController, &communicationService, &serviceMode);

uint32_t currentMillis, lastMillis1Hz = 0, lastMillis2Hz = 0;
uint32_t lastMillis10Hz = 0, lastMillis100Hz = 0, lastMillis200Hz = 0, lastMillis1Min = 0;

void ihrMessageReceived() {
    packetWaiting = true;
}

#ifndef DISABLE_CC1101_RECEIVE
    void cc1101Receive(uint32_t currentMillis) {
        if (packetWaiting) {
            detachInterrupt(CC1101_Interrupt);
            packetWaiting = false;
            communicationService.receive(recvBuffer, &recvBufferLen);
            attachInterrupt(CC1101_Interrupt, ihrMessageReceived, FALLING);
        }
    }
#endif

void setup() {

    MCUSR = 0;
    wdt_disable();

    debug.setup();
    buttonController.setup();
    buzzerController.setup();
    eepromController.setup();
    ledController.setup();
    outputController.setup();
    tgsController.setup();

    startup.doStartup();

    startupFlag = eepromController.getStartupFlag();
    if(startupFlag && WATCHDOG_ENABLED) {
        wdt_reset();
        wdt_enable(WDTO_8S); 
    }

    communicationService.setup();
    alarmService.setup();
    programmingService.setup();

    sendBufferLen = 0;

    #ifndef DISABLE_CC1101_RECEIVE

        attachInterrupt(CC1101_Interrupt, ihrMessageReceived, FALLING);

        recvBufferLen = 0;
        packetWaiting = false;
    #endif

    alarmService.setSendBuffer(sendBuffer, &sendBufferLen);
    serviceMode.setSendBuffer(sendBuffer, &sendBufferLen);
}

void loop() {
    currentMillis = millis();

    if(currentMillis - lastMillis1Min >= 60000) {
        if(!startupFlag) {
            startupFlag = eepromController.getStartupFlag();
        }

        lastMillis1Min = currentMillis;
    }

    if(currentMillis - lastMillis1Hz >= 1000) {
        if(!serviceMode.isTestRunning()) {
            alarmService.sendPeriodicStatus(currentMillis);
        }

        lastMillis1Hz = currentMillis;
    }

    if(currentMillis - lastMillis2Hz >= 500) {
        if(startupFlag && WATCHDOG_ENABLED) {
            wdt_reset();    
        }
        tgsController.tick();
        if(!serviceMode.isTestRunning()) {
            alarmService.tick(currentMillis);
        }

        lastMillis2Hz = currentMillis;
    }

    if(currentMillis - lastMillis10Hz >= 100) {
        buzzerController.tick(currentMillis);
        outputController.tick(currentMillis);
        programmingService.tick();

        lastMillis10Hz = currentMillis;
    }

    if(currentMillis - lastMillis100Hz >= 10) {
        buttonController.tick(currentMillis);

        lastMillis100Hz = currentMillis;
    }

    if(currentMillis - lastMillis200Hz >= 5) {
        #ifndef DISABLE_CC1101_RECEIVE
            cc1101Receive(currentMillis);
        #endif
        ledController.tick(currentMillis);

        lastMillis200Hz = currentMillis;
    }
}

#ifndef ALERTCONFIG_H
#define ALERTCONFIG_H

#define ALARM_STATUS_MESSAGE_RESEND_MS_MIN 30000
#define ALARM_STATUS_MESSAGE_RESEND_MS_MAX 120000

#define ALARM_MESSAGE_MAX_RESEND_COUNT 5

#define ALARM_MIN_ACTIVE_PERIOD_MS 180000

#endif

#ifndef BUILDCONFIG_H
#define BUILDCONFIG_H

#define SERIAL_TX_BUFFER_SIZE 80
#define SERIAL_RX_BUFFER_SIZE 120

#endif

#ifndef BUZZERCONFIG_H
#define BUZZERCONFIG_H

#define BEEPING_INTERVAL_MS 400

#endif

#ifndef EEPROMCONFIG_H
#define EEPROMCONFIG_H

#define EEPROM_PAGE_SIZE 16

#define EEPROM_WIPE 0
#define EEPROM_WIPE_START_INDEX 0
#define EEPROM_WIPE_END_INDEX 150

#define EEPROM_SERIAL_NUMBER_ADDRESS_START 0
#define EEPROM_SERIAL_NUMBER_ADDRESS_END 3

#define EEPROM_STARTUP_FLAG_ADDRESS_INT 10

#define EEPROM_CONFIG_ALARM_TRIGGER_VALUE_INT 50

#define EEPROM_CONFIG_DEVICE_ID_INT 52

#define EEPROM_CC1101_DEVICE_ADDRESS_INT 100
#define EEPROM_CC1101_DEVICE_CHANNEL_BYTE 102
#define EEPROM_CC1101_RECEIVER_ADDRESS_INT 103

#endif

#ifndef HARDWARECONFIG_H
#define HARDWARECONFIG_H

#define HW_STATUS_LED_PIN 6

#define HW_OUTPUT_LED_RED_PIN 5
#define HW_OUTPUT_LED_YELLOW_PIN 4
#define HW_OUTPUT_LED_GREEN_PIN 9
#define HW_OUTPUT_BUZZER_PIN 7
#define HW_OUTPUT_RELAY_PIN 2

#define HW_INPUT_BTN_TEST_PIN 16

#define HW_TGS_SENSOR_PIN 17

#define HW_RXD0_PIN 0

#endif

#ifndef TGSCONFIG_H
#define TGSCONFIG_H

#define TGS_STARTUP_DELAY_MS 60000   

#define TGS_DEFAULT_ALERT_VALUE 406     

#define TGS_NUMBER_OF_MEASUREMENTS_BEFORE_ALARM 6   

#endif



ButtonController::ButtonController() {
    btnStates[0].pin = HW_INPUT_BTN_TEST_PIN;
    btnStates[0].lastValue = false;
    btnStates[0].timestamp = 0;
    btnStates[0].clicked = false;
    btnStates[0].stateChanged = false;
}

void ButtonController::setup() {
    for(i=0; i<BTN_COUNT; i++) {
        pinMode(btnStates[i].pin, INPUT_PULLUP);
    }
}

void ButtonController::tick(uint32_t currentMillis) {
    for(i=0; i<BTN_COUNT; i++) {
        currentState = !digitalRead(btnStates[i].pin); 
        if(btnStates[i].lastValue != currentState) {

            if(currentState == true) {

                btnStates[i].lastValue = currentState;
                btnStates[i].timestamp = currentMillis;
            } else {

                if(currentMillis - btnStates[i].timestamp >= BTN_DEBOUNCE_PERIOD) {

                    btnStates[i].clicked = true;
                    btnStates[i].stateChanged = true;
                }
                btnStates[i].lastValue = currentState;
                btnStates[i].timestamp = 0;
            }
        }
    }
}

bool ButtonController::hasAnyStateChanged() {
    for(i=0; i<BTN_COUNT; i++) {
        if(btnStates[i].stateChanged) return true;
    }
    return false;
}

bool ButtonController::hasStateChanged(uint8_t button) {
    return btnStates[button].stateChanged;
}

bool* ButtonController::getAllClickedStates() {
    for(i=0; i<BTN_COUNT; i++) {
        clickedStates[i] = btnStates[i].clicked;
        btnStates[i].clicked = false;
        btnStates[i].stateChanged = false;
    }
    return clickedStates;
}

bool ButtonController::getClickedState(uint8_t button) {
    currentState = btnStates[button].clicked;
    btnStates[button].clicked = false;
    btnStates[button].stateChanged = false;
    return currentState;
}



BuzzerController::BuzzerController() {
    lastChangeMillis = 0;
    running = false;
    state = false;
}

void BuzzerController::setup() {
    pinMode(HW_OUTPUT_BUZZER_PIN, OUTPUT);

    digitalWrite(HW_OUTPUT_BUZZER_PIN, LOW);
}

void BuzzerController::tick(uint32_t currentMillis) {
    if(running && currentMillis - lastChangeMillis >= BEEPING_INTERVAL_MS) {
        state = !state;
        digitalWrite(HW_OUTPUT_BUZZER_PIN, state);
    }
}

void BuzzerController::start() {
    if(!running) {
        lastChangeMillis = 0;
        running = true;
    }
}

void BuzzerController::stop() {
    if(running) {
        running = false;
        digitalWrite(HW_OUTPUT_BUZZER_PIN, LOW);
    }
}



#define cc1101_Select() digitalWrite(SS, LOW)

#define cc1101_Deselect() digitalWrite(SS, HIGH)

#define wait_Miso() while (digitalRead(MISO) > 0)

#define getGDO2state() digitalRead(CC1101_GDO2)

#define wait_GDO2_high() while (!getGDO2state())

#define wait_GDO2_low() while (getGDO2state())

CC1101Controller::CC1101Controller(EEPROMController *eepromController)
{
  this->eepromController = eepromController;
  this->spiSettings = new SPISettings(26000000, MSBFIRST, SPI_MODE0);

  channel = CC1101_DEFVAL_CHANNR;
  syncWord[0] = CC1101_DEFVAL_SYNC1;
  syncWord[1] = CC1101_DEFVAL_SYNC0;
  devAddress = CC1101_DEFVAL_ADDR;
}

void CC1101Controller::wakeUp(void)
{
  SPI.beginTransaction(*spiSettings);
  cc1101_Select();   
  wait_Miso();       
  cc1101_Deselect(); 
  SPI.endTransaction();
}

void CC1101Controller::writeReg(uint8_t regAddr, uint8_t value)
{
  SPI.beginTransaction(*spiSettings);
  cc1101_Select();       
  wait_Miso();           
  SPI.transfer(regAddr); 
  SPI.transfer(value);   
  cc1101_Deselect();     
  SPI.endTransaction();
}

void CC1101Controller::writeBurstReg(uint8_t regAddr, uint8_t *buffer, uint8_t len)
{
  addr = regAddr | WRITE_BURST; 
  SPI.beginTransaction(*spiSettings);
  cc1101_Select();              
  wait_Miso();                  
  SPI.transfer(addr);           
  for (i = 0; i < len; i++) {
    SPI.transfer(buffer[i]); 
  }
  cc1101_Deselect(); 
  SPI.endTransaction();
}

void CC1101Controller::cmdStrobe(uint8_t cmd)
{
  SPI.beginTransaction(*spiSettings);
  cc1101_Select();   
  wait_Miso();       
  SPI.transfer(cmd); 
  cc1101_Deselect(); 
  SPI.endTransaction();
}

uint8_t CC1101Controller::readReg(uint8_t regAddr, uint8_t regType)
{
  addr = regAddr | regType;
  SPI.beginTransaction(*spiSettings);
  cc1101_Select();          
  wait_Miso();              
  SPI.transfer(addr);       
  val = SPI.transfer(0x00); 
  cc1101_Deselect();        
  SPI.endTransaction();

  return val;
}

void CC1101Controller::readBurstReg(uint8_t *buffer, uint8_t regAddr, uint8_t len)
{
  addr = regAddr | READ_BURST;
  SPI.beginTransaction(*spiSettings);
  cc1101_Select();    
  wait_Miso();        
  SPI.transfer(addr); 
  for (i = 0; i < len; i++) {
    buffer[i] = SPI.transfer(0x00); 
  }
  cc1101_Deselect();                
  SPI.endTransaction();
}

void CC1101Controller::reset(void)
{
  SPI.beginTransaction(*spiSettings);
  cc1101_Deselect(); 
  delayMicroseconds(5);
  cc1101_Select(); 
  delayMicroseconds(10);
  cc1101_Deselect(); 
  delayMicroseconds(41);
  cc1101_Select(); 

  wait_Miso();               
  SPI.transfer(CC1101_SRES); 
  wait_Miso();               

  cc1101_Deselect(); 
  SPI.endTransaction();

  setCCregs(); 
}

void CC1101Controller::setCCregs(void)
{
  writeReg(CC1101_IOCFG2, CC1101_DEFVAL_IOCFG2);
  writeReg(CC1101_IOCFG1, CC1101_DEFVAL_IOCFG1);
  writeReg(CC1101_IOCFG0, CC1101_DEFVAL_IOCFG0);
  writeReg(CC1101_FIFOTHR, CC1101_DEFVAL_FIFOTHR);
  writeReg(CC1101_PKTLEN, CC1101_DEFVAL_PKTLEN);
  writeReg(CC1101_PKTCTRL1, CC1101_DEFVAL_PKTCTRL1);
  writeReg(CC1101_PKTCTRL0, CC1101_DEFVAL_PKTCTRL0);

  setSyncWord(syncWord);

  setDevAddress(devAddress);

  setChannel(channel);

  writeReg(CC1101_FSCTRL1, CC1101_DEFVAL_FSCTRL1);
  writeReg(CC1101_FSCTRL0, CC1101_DEFVAL_FSCTRL0);
  writeReg(CC1101_FREQ2, CC1101_DEFVAL_FREQ2_868);
  writeReg(CC1101_FREQ1, CC1101_DEFVAL_FREQ1_868);
  writeReg(CC1101_FREQ0, CC1101_DEFVAL_FREQ0_868);

  writeReg(CC1101_MDMCFG4, CC1101_DEFVAL_MDMCFG4);
  writeReg(CC1101_MDMCFG3, CC1101_DEFVAL_MDMCFG3);
  writeReg(CC1101_MDMCFG2, CC1101_DEFVAL_MDMCFG2);
  writeReg(CC1101_MDMCFG1, CC1101_DEFVAL_MDMCFG1);
  writeReg(CC1101_MDMCFG0, CC1101_DEFVAL_MDMCFG0);
  writeReg(CC1101_DEVIATN, CC1101_DEFVAL_DEVIATN);
  writeReg(CC1101_MCSM2, CC1101_DEFVAL_MCSM2);
  writeReg(CC1101_MCSM1, CC1101_DEFVAL_MCSM1);
  writeReg(CC1101_MCSM0, CC1101_DEFVAL_MCSM0);
  writeReg(CC1101_FOCCFG, CC1101_DEFVAL_FOCCFG);
  writeReg(CC1101_BSCFG, CC1101_DEFVAL_BSCFG);
  writeReg(CC1101_AGCCTRL2, CC1101_DEFVAL_AGCCTRL2);
  writeReg(CC1101_AGCCTRL1, CC1101_DEFVAL_AGCCTRL1);
  writeReg(CC1101_AGCCTRL0, CC1101_DEFVAL_AGCCTRL0);
  writeReg(CC1101_WOREVT1, CC1101_DEFVAL_WOREVT1);
  writeReg(CC1101_WOREVT0, CC1101_DEFVAL_WOREVT0);
  writeReg(CC1101_WORCTRL, CC1101_DEFVAL_WORCTRL);
  writeReg(CC1101_FREND1, CC1101_DEFVAL_FREND1);
  writeReg(CC1101_FREND0, CC1101_DEFVAL_FREND0);
  writeReg(CC1101_FSCAL3, CC1101_DEFVAL_FSCAL3);
  writeReg(CC1101_FSCAL2, CC1101_DEFVAL_FSCAL2);
  writeReg(CC1101_FSCAL1, CC1101_DEFVAL_FSCAL1);
  writeReg(CC1101_FSCAL0, CC1101_DEFVAL_FSCAL0);
  writeReg(CC1101_RCCTRL1, CC1101_DEFVAL_RCCTRL1);
  writeReg(CC1101_RCCTRL0, CC1101_DEFVAL_RCCTRL0);
  writeReg(CC1101_FSTEST, CC1101_DEFVAL_FSTEST);
  writeReg(CC1101_PTEST, CC1101_DEFVAL_PTEST);
  writeReg(CC1101_AGCTEST, CC1101_DEFVAL_AGCTEST);
  writeReg(CC1101_TEST2, CC1101_DEFVAL_TEST2);
  writeReg(CC1101_TEST1, CC1101_DEFVAL_TEST1);
  writeReg(CC1101_TEST0, CC1101_DEFVAL_TEST0);

  CCPACKET packet;
  packet.length = 0;
  sendData(packet);
}

void CC1101Controller::init()
{
  SPI.begin();                 
  pinMode(CC1101_GDO2, INPUT); 

  setValuesFromEeprom();

  reset(); 

  setTxPowerAmp(PA_LongDistance);

  setRxState();
}

void CC1101Controller::setSyncWord(uint8_t syncH, uint8_t syncL)
{
  writeReg(CC1101_SYNC1, syncH);
  writeReg(CC1101_SYNC0, syncL);
  syncWord[0] = syncH;
  syncWord[1] = syncL;
}

void CC1101Controller::setSyncWord(uint8_t *sync)
{
  CC1101Controller::setSyncWord(sync[0], sync[1]);
}

void CC1101Controller::setDevAddress(uint8_t addr)
{
  writeReg(CC1101_ADDR, addr);
  devAddress = addr;
}

void CC1101Controller::setChannel(uint8_t chnl)
{
  writeReg(CC1101_CHANNR, chnl);
  channel = chnl;
}

void CC1101Controller::setPowerDownState()
{

  cmdStrobe(CC1101_SIDLE);

  cmdStrobe(CC1101_SPWD);
}

bool CC1101Controller::sendData(CCPACKET packet)
{
  res = false;

  setRxState();

  tries = 0;

  while (tries++ < 1000 && ((marcState = readStatusReg(CC1101_MARCSTATE)) & 0x1F) != 0x0D)
  {
    if (marcState == 0x11) 
      flushRxFifo();       
  }
  if (tries >= 1000)
  {

    return false;
  }

  delayMicroseconds(500);

  if (packet.length > 0)
  {

    writeReg(CC1101_TXFIFO, packet.length);

    writeBurstReg(CC1101_TXFIFO, packet.data, packet.length);

    setTxState();
  }

  marcState = readStatusReg(CC1101_MARCSTATE) & 0x1F;
  if ((marcState != 0x13) && (marcState != 0x14) && (marcState != 0x15))
  {
    setIdleState(); 
    flushTxFifo();  
    setRxState();   

    return false;
  }

  wait_GDO2_high();

  wait_GDO2_low();

  if ((readStatusReg(CC1101_TXBYTES) & 0x7F) == 0)
    res = true;

  setIdleState(); 
  flushTxFifo();  

  setRxState();

  return res;
}

uint8_t CC1101Controller::receiveData(CCPACKET *packet)
{
  rxBytes = readStatusReg(CC1101_RXBYTES);

  if (rxBytes & 0x7F && !(rxBytes & 0x80))
  {

    packet->length = readConfigReg(CC1101_RXFIFO);

    if (packet->length > CC1101_MAX_BUFFER_SIZE)
      packet->length = 0; 
    else
    {

      readBurstReg(packet->data, CC1101_RXFIFO, packet->length);

      packet->rssi = readConfigReg(CC1101_RXFIFO);

      val = readConfigReg(CC1101_RXFIFO);
      packet->lqi = val & 0x7F;
      packet->crc_ok = bitRead(val, 7);
    }
  }
  else
    packet->length = 0;

  setIdleState(); 
  flushRxFifo();  

  setRxState();

  return packet->length;
}

void CC1101Controller::setRxState(void)
{
  cmdStrobe(CC1101_SRX);
  rfState = RFSTATE_RX;
}

void CC1101Controller::setTxState(void)
{
  cmdStrobe(CC1101_STX);
  rfState = RFSTATE_TX;
}

void CC1101Controller::setValuesFromEeprom(void) {
  setChannel(eepromController->getByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE));
}

void CC1101Controller::setTxPowerAmp(uint8_t paLevel) {
  writeReg(CC1101_PATABLE, paLevel);
}

int CC1101Controller::computeRssi(uint8_t rssi_dec) {
    rssi_offset = 74;
    if (rssi_dec >= 128) {
        return ((int)(rssi_dec - 256) / 2) - rssi_offset;
    } else {
        return (rssi_dec / 2) - rssi_offset;
    }
}

int CC1101Controller::computeLqi(uint8_t raw) {
    return 0x3F - raw;
}

void CC1101Controller::gotoIdleState() {
    setIdleState();
}



EEPROMController::EEPROMController() {
    eepr.setup(kbits_4, 1, EEPROM_PAGE_SIZE, 80);
}

void EEPROMController::setup() {
    i2cState = eepr.begin();
}

bool EEPROMController::getI2CStatus() {
    return i2cState == 0;
}

void EEPROMController::wipeEEPROM() {
    for(i=EEPROM_WIPE_START_INDEX;i<EEPROM_WIPE_END_INDEX;i++) {
        eepr.write(i,0);
    }
}

void EEPROMController::setSerialNumber(uint32_t serialNumber) {
    convertBytesToULong.longVal = serialNumber;

    j = 0;
    for(i=EEPROM_SERIAL_NUMBER_ADDRESS_START; i<=EEPROM_SERIAL_NUMBER_ADDRESS_END; i++) {
        eepr.write(i, convertBytesToULong.byte[j]);
        j++;
    }
}

uint32_t EEPROMController::getSerialNumber() {
    j = 0;
    for(i=EEPROM_SERIAL_NUMBER_ADDRESS_START; i<=EEPROM_SERIAL_NUMBER_ADDRESS_END; i++) {
        convertBytesToULong.byte[j] = eepr.read(i);
        j++;
    }

    return convertBytesToULong.longVal;
}

void EEPROMController::setStartupFlag(boolean state) {
    if(state) {
        eepr.write(EEPROM_STARTUP_FLAG_ADDRESS_INT, 3);
        eepr.write(EEPROM_STARTUP_FLAG_ADDRESS_INT+1, 8);
    } else {
        eepr.write(EEPROM_STARTUP_FLAG_ADDRESS_INT, 0);
        eepr.write(EEPROM_STARTUP_FLAG_ADDRESS_INT+1, 0);
    }
}

boolean EEPROMController::getStartupFlag() {
    if(eepr.read(EEPROM_STARTUP_FLAG_ADDRESS_INT) == 3 && eepr.read(EEPROM_STARTUP_FLAG_ADDRESS_INT+1) == 8) {
        return true;
    } else {
        return false;
    }
}

void EEPROMController::setByteValue(uint32_t addr, uint8_t val) {
    eepr.write(addr, val);
}

uint8_t EEPROMController::getByteValue(uint32_t addr) {
    return eepr.read(addr);
}

void EEPROMController::setUIntValue(uint32_t addr, uint16_t val) {
    convertBytesToUInt.intVal = val;
    eepr.write(addr, convertBytesToUInt.byte[0]);
    eepr.write(addr+1, convertBytesToUInt.byte[1]);
}

uint16_t EEPROMController::getUIntValue(uint32_t addr) {
    convertBytesToUInt.byte[0] = eepr.read(addr);
    convertBytesToUInt.byte[1] = eepr.read(addr+1);
    return convertBytesToUInt.intVal;
}



LEDController::LEDController() {

    currentColor.valueRed = 0;
    currentColor.animateRed = 0;    

    predefinedColors[LED_COLOR_NONE].valueRed = 0;
    predefinedColors[LED_COLOR_NONE].animateRed = 0;

    predefinedColors[LED_COLOR_RED].valueRed = 255;
    predefinedColors[LED_COLOR_RED].animateRed = 1;

    pulseTimer = 0;
    _canWriteRed = false;
    lastSemaphoreColor = 0;
    lastSemaphoreState = 0;
    lastSemaphoreChange = 0;
}

void LEDController::setup() {
    pinMode(HW_STATUS_LED_PIN, OUTPUT);
    pinMode(HW_OUTPUT_LED_RED_PIN, OUTPUT);
    pinMode(HW_OUTPUT_LED_YELLOW_PIN, OUTPUT);
    pinMode(HW_OUTPUT_LED_GREEN_PIN, OUTPUT);

    set(LED_COLOR_RED, LED_STATUS_OK);
    setSemaphore(LED_SEMAPHORE_OFF);
}

void LEDController::assignColor(ledColor *copyTo, ledColor *copyFrom) {
    copyTo->valueRed = copyFrom->valueRed;
    copyTo->animateRed = copyFrom->animateRed;
}

boolean LEDController::isDifferentColor(ledColor cmp1, ledColor cmp2) {
    if(cmp1.valueRed != cmp2.valueRed) return true;
    return false;
}

void LEDController::tick(uint32_t currentMillis) {
    if(lastSemaphoreColor == LED_SEMAPHORE_RED && (lastSemaphoreChange + BLINK_INTERVAL_MS) < currentMillis) {
        lastSemaphoreState = !lastSemaphoreState;
        digitalWrite(HW_OUTPUT_LED_RED_PIN, lastSemaphoreState);
        lastSemaphoreChange = currentMillis;
    }

    if(currentAnimation == 0) {
        if(isDifferentColor(currentColor, currentSetColor)) {
            assignColor(&currentColor, &currentSetColor);
            _canWriteRed = true;
        } else {
            _canWriteRed = false;
        }
    } else {
        if(currentAnimation == 1) {
            incrementor = 1;
        } else if(currentAnimation == 2) {
            incrementor = 2;
        } else if(currentAnimation == 3) {
            incrementor = 4;
        } else if(currentAnimation == 4) {
            if(pulseTimer == 235) {
                incrementor = 255;
            }
            if(pulseTimer == 250) {
                incrementor = 0;
                pulseTimer = 0;
            }
            pulseTimer++;
        }

        if(incrementor >= 1 && incrementor <= 4) {

            if(currentSetColor.animateRed == 1) {
                if(currentColor.animateRed == 0) {
                    currentColor.animateRed = incrementor;
                    currentColor.valueRed = 0;
                }
                currentColor.valueRed += currentColor.animateRed;
                if(currentColor.valueRed >= currentSetColor.valueRed || currentColor.valueRed <= 0) {
                    currentColor.animateRed *= (-1);
                    currentColor.valueRed += currentColor.animateRed;
                }
                _canWriteRed = true;
            } else {
                if(currentColor.valueRed != 0) {
                    currentColor.valueRed = 0;
                    currentColor.animateRed = 0;
                    _canWriteRed = true;
                } else {
                    _canWriteRed = false;
                }
            }
        } else {

            if(currentSetColor.animateRed == 1) {
                if(currentColor.valueRed != incrementor) {
                    currentColor.valueRed = incrementor;
                    _canWriteRed = true;
                } else {
                    _canWriteRed = false;
                }
            } else {
                if(currentColor.valueRed != 0) {
                    currentColor.valueRed = 0;
                    currentColor.animateRed = 0;
                    _canWriteRed = true;
                } else {
                    _canWriteRed = false;
                }
            }
        }

    }

    if(_canWriteRed) analogWrite(HW_STATUS_LED_PIN, currentColor.valueRed);
}

void LEDController::setColor(uint8_t predefinedColor) {
    if(isDifferentColor(currentSetColor, predefinedColors[predefinedColor])) {
        resetCurrentColor();
        assignColor(&currentSetColor, &predefinedColors[predefinedColor]);
        pulseTimer = 0;
    }
}

void LEDController::setAnimation(uint8_t animation) {
    if(currentAnimation != animation) {
        resetCurrentColor();
        currentAnimation = animation;
        pulseTimer = 0;
    }
}

void LEDController::set(uint8_t predefinedColor, uint8_t animation) {
    if(isDifferentColor(currentSetColor, predefinedColors[predefinedColor]) || currentAnimation != animation) {
        resetCurrentColor();
        assignColor(&currentSetColor, &predefinedColors[predefinedColor]);
        currentAnimation = animation;
        pulseTimer = 0;
    }
}

void LEDController::resetCurrentColor() {
    currentColor.animateRed = 0;
}

void LEDController::setSemaphore(uint8_t color) {
    if(lastSemaphoreColor != color) {
        switch(color) {
            case LED_SEMAPHORE_RED:
                digitalWrite(HW_OUTPUT_LED_RED_PIN, HIGH);
                digitalWrite(HW_OUTPUT_LED_YELLOW_PIN, LOW);
                digitalWrite(HW_OUTPUT_LED_GREEN_PIN, LOW);
            break;
            case LED_SEMAPHORE_YELLOW:
                digitalWrite(HW_OUTPUT_LED_RED_PIN, LOW);
                digitalWrite(HW_OUTPUT_LED_YELLOW_PIN, HIGH);
                digitalWrite(HW_OUTPUT_LED_GREEN_PIN, LOW);
            break;
            case LED_SEMAPHORE_GREEN:
                digitalWrite(HW_OUTPUT_LED_RED_PIN, LOW);
                digitalWrite(HW_OUTPUT_LED_YELLOW_PIN, LOW);
                digitalWrite(HW_OUTPUT_LED_GREEN_PIN, HIGH);
            break;
            case 10:
                digitalWrite(HW_OUTPUT_LED_RED_PIN, HIGH);
                digitalWrite(HW_OUTPUT_LED_YELLOW_PIN, HIGH);
                digitalWrite(HW_OUTPUT_LED_GREEN_PIN, HIGH);
            break;
            default:
                digitalWrite(HW_OUTPUT_LED_RED_PIN, LOW);
                digitalWrite(HW_OUTPUT_LED_YELLOW_PIN, LOW);
                digitalWrite(HW_OUTPUT_LED_GREEN_PIN, LOW);
            break;
        }
        lastSemaphoreColor = color;
        lastSemaphoreState = 0;
        lastSemaphoreChange = 0;
    }
}



OutputController::OutputController() {
    for(i=0;i<OUTPUTS_COUNT;i++) {
        outputState[i] = 0;
        outputTimer[i] = 0;
    }
    outputPins[RELAY_AC] = HW_OUTPUT_RELAY_PIN;
}

void OutputController::setup() {
    for(i=0;i<OUTPUTS_COUNT;i++) {
        pinMode(outputPins[i], OUTPUT);
        digitalWrite(outputPins[i], 0);
    }
}

void OutputController::tick(uint32_t currentMillis) {
    for(i=0;i<OUTPUTS_COUNT;i++) {

        if(outputPulseCount[i] > 0) {

            if(outputTimer[i] <= currentMillis) {
                outputPulseCount[i]--;
                outputState[i] = !outputState[i];
                digitalWrite(outputPins[i], outputState[i]);
                if(outputPulseCount[i] <= 0) {
                    outputTimer[i] = 0;
                } else {
                    outputTimer[i] = millis() + OUTPUTS_DEFAULT_PULSE_DELAY;
                }
            }
        } else {

            if(outputTimer[i] > 0) {

                if(outputTimer[i] <= currentMillis) {

                    outputState[i] = !outputState[i];
                    digitalWrite(outputPins[i], outputState[i]);
                    outputTimer[i] = 0;
                }
            }
        }
    }
}

boolean OutputController::get(int16_t outputId) {
    return outputState[outputId];
}

void OutputController::set(int16_t outputId, boolean state) {

    outputState[outputId] = state;
    digitalWrite(outputPins[outputId], state);
    outputTimer[outputId] = 0;
}

void OutputController::set(int16_t outputId, boolean state, uint32_t timeout) {

    outputState[outputId] = state;
    digitalWrite(outputPins[outputId], state);
    if(timeout > 0) {
      outputTimer[outputId] = millis() + timeout;
    } else {
      outputTimer[outputId] = millis() + OUTPUTS_DEFAULT_ONESHOT_DELAY;
    }
}

void OutputController::setPulsed(int16_t outputId, boolean state, uint8_t pulses) {

    outputState[outputId] = state;
    digitalWrite(outputPins[outputId], state);
    outputTimer[outputId] = millis() + OUTPUTS_DEFAULT_PULSE_DELAY;
    outputPulseCount[outputId] = pulses*2-1;
}



TGSController::TGSController() {
    currentValue = 0;
    lastValue = 0;
    valueChanged = false;
}

void TGSController::setup() {
    pinMode(HW_TGS_SENSOR_PIN, INPUT);
}

void TGSController::tick() {
    currentValue = map(analogRead(HW_TGS_SENSOR_PIN), 0, 1023, 0, 500);
    if(currentValue != lastValue) {
        valueChanged = true;
    }
}

boolean TGSController::hasValueChanged() {
    return valueChanged;
}

uint16_t TGSController::getValue() {
    valueChanged = false;
    lastValue = currentValue;
    return currentValue;
}



AlarmService::AlarmService(SerialDebug *debug, ButtonController *buttonController, BuzzerController *buzzerController, 
                EEPROMController *eepromController, LEDController *ledController, TGSController *tgsController, 
                CommunicationService *communicationService, OutputController *outputController) {
    this->debug = debug;
    this->buttonController = buttonController;
    this->buzzerController = buzzerController;
    this->eepromController = eepromController;
    this->ledController = ledController;
    this->tgsController = tgsController;
    this->communicationService = communicationService;
    this->outputController = outputController;
}

void AlarmService::setup() {
    alarmTriggerValue = eepromController->getUIntValue(EEPROM_CONFIG_ALARM_TRIGGER_VALUE_INT);

    testModeActive = false;
    currentAlarmValue = 0;
    lastLedBlinkMode = 0;
    alarmStarted = false;
    isOtherAlert = false;
    skipStartup = false;

    activatedAlarmSendCount = 0;
    deactivatedAlarmSendCount = 0;
    lastSendStatusMillis = 0;
    alarmRunningSinceMillis = 0;

    generateNextSendStatusMillis();
}

void AlarmService::tick(uint32_t currentMillis) {
    if(buttonController->hasStateChanged(BTN_TEST)) {
        if(buttonController->getClickedState(BTN_TEST)) {

            testModeActive = !testModeActive;
            if(testModeActive) {
                startAlarm();
            } else {
                stopAlarm();
            }
        }
    }
    if(!testModeActive) {

        if(!skipStartup && currentMillis <= TGS_STARTUP_DELAY_MS) {

            if(lastLedBlinkMode == LED_SEMAPHORE_OFF) {
                lastLedBlinkMode = LED_SEMAPHORE_YELLOW;
            } else {
                lastLedBlinkMode = LED_SEMAPHORE_OFF;
            }
            ledController->setSemaphore(lastLedBlinkMode);
        } else {

            currentAlarmValue = tgsController->getValue();

            if(currentAlarmValue > 30) {
                if(currentAlarmValue >= alarmTriggerValue) {
                    alarmCount++;
                    if(alarmCount >= TGS_NUMBER_OF_MEASUREMENTS_BEFORE_ALARM) {

                        if(alarmRunningSinceMillis == 0) {
                            alarmRunningSinceMillis = currentMillis;
                        }
                        startAlarm();
                    }
                } else if(alarmRunningSinceMillis != 0 && (currentMillis - alarmRunningSinceMillis >= ALARM_MIN_ACTIVE_PERIOD_MS)) {
                    alarmRunningSinceMillis = 0;
                    alarmCount = 0;

                    stopAlarm();
                } else {

                    checkOtherAlert();
                }
            } else {

                setIsOtherAlert(true);
            }
        }
    }
}

void AlarmService::setSendBuffer(uint8_t*sendBufferPtr, uint8_t *sendBufferLenPtr) {
    this->sendBuffer = sendBufferPtr;
    this->sendBufferLen = sendBufferLenPtr;
}

void AlarmService::sendPeriodicStatus(uint32_t currentMillis) {
    if(alarmStarted && activatedAlarmSendCount < ALARM_MESSAGE_MAX_RESEND_COUNT) {
        communicationService->sendAlarmStartPackage(sendBuffer, sendBufferLen);
        activatedAlarmSendCount++;
    } else if(!alarmStarted && deactivatedAlarmSendCount < ALARM_MESSAGE_MAX_RESEND_COUNT) {
        communicationService->sendAlarmStopPackage(sendBuffer, sendBufferLen);
        deactivatedAlarmSendCount++;
    }
    if(currentMillis - lastSendStatusMillis > nextSendStatusMillis) {
        if(alarmStarted) {
            communicationService->sendAlarmStartPackage(sendBuffer, sendBufferLen);
        } else {
            communicationService->sendAlarmStopPackage(sendBuffer, sendBufferLen);
        }
        generateNextSendStatusMillis();
        lastSendStatusMillis = currentMillis;
    }
}

void AlarmService::generateNextSendStatusMillis() {
    nextSendStatusMillis = random(ALARM_STATUS_MESSAGE_RESEND_MS_MIN, ALARM_STATUS_MESSAGE_RESEND_MS_MAX);
}

void AlarmService::startAlarm() {
    ledController->setSemaphore(LED_SEMAPHORE_RED);
    buzzerController->start();
    if(!alarmStarted) {

        outputController->setPulsed(RELAY_AC, true, OUTPUTS_PULSE_COUNT);
        communicationService->sendAlarmStartPackage(sendBuffer, sendBufferLen);
        activatedAlarmSendCount = 1;
        alarmStarted = true;
    }
}

void AlarmService::stopAlarm() {
    checkOtherAlert();

    buzzerController->stop();

    if(alarmStarted) {

        communicationService->sendAlarmStopPackage(sendBuffer, sendBufferLen);
        deactivatedAlarmSendCount = 1;
        alarmStarted = false;
    }
}

void AlarmService::checkOtherAlert() {
    if(isOtherAlert) {

        ledController->setSemaphore(LED_SEMAPHORE_YELLOW);
    } else {

        ledController->setSemaphore(LED_SEMAPHORE_GREEN);
    }
}

void AlarmService::setAlarmTriggerValue(uint16_t alarmTrigger) {
    alarmTriggerValue = alarmTrigger;
    eepromController->setUIntValue(EEPROM_CONFIG_ALARM_TRIGGER_VALUE_INT, alarmTrigger);
}

uint16_t AlarmService::getAlarmTriggerValue() {
    return alarmTriggerValue;
}

void AlarmService::setIsOtherAlert(bool isOtherAlertParam) {
    this->isOtherAlert = isOtherAlertParam;
    if(!alarmStarted) {
        if(isOtherAlert) {
            ledController->setSemaphore(LED_SEMAPHORE_YELLOW);
        } else {
            ledController->setSemaphore(LED_SEMAPHORE_GREEN);
        }
    }
}

void AlarmService::setSkipStartup(bool state) {
    this->skipStartup = state;
}



CommunicationService::CommunicationService(CC1101Controller *radio, SerialDebug *debug, EEPROMController *eepromController,
        LEDController *ledController) {
    this->radio = radio;
    this->debug = debug;
    this->eepromController = eepromController;
    this->ledController = ledController;
    this->radioCommunicationStatus = 0;
}

void CommunicationService::setup() {
    reloadEepromValues();

    delay(500); 

    testConnection();
}

void CommunicationService::reloadEepromValues() {
    deviceAddress = eepromController->getUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT);
    receiverAddress = eepromController->getUIntValue(EEPROM_CC1101_RECEIVER_ADDRESS_INT);
    deviceId = eepromController->getUIntValue(EEPROM_CONFIG_DEVICE_ID_INT);
}

void CommunicationService::send(const uint8_t *buff, uint8_t *len) {
    testConnection();

    packet.length = *len;
    for(i=0; i<packet.length; i++) {
        packet.data[i] = buff[i];
    }
    bool resp = radio->sendData(packet);
    if(debug->getDebugEnabled()) {
        debug->print(F("# Radio send ["));
        debug->print(resp);
        debug->print(F("]: "));
        for(i=0; i<packet.length; i++) {
            debug->print(packet.data[i]);
            debug->print(F(" "));
        }
        debug->println();
    }
    *len = 0;
}

void CommunicationService::receive(uint8_t *buff, uint8_t *len) {
    if (radio->receiveData(&packet) > 0) {
        debug->print(F("# Radio recv "));

        lastLqi[indexRssiLqi] = radio->computeLqi(packet.lqi);
        lastRssi[indexRssiLqi] = radio->computeRssi(packet.rssi);
        if(debug->getDebugEnabled()) {
            debug->print(F("[LQI: "));
            debug->print(lastLqi[indexRssiLqi]);
            debug->print(F(" | RSSI: "));
            debug->print(lastRssi[indexRssiLqi]);
            debug->print(F("dBm]: "));
        }
        previousIndexRssiLqi = indexRssiLqi;
        if(indexRssiLqi < RSSI_LQI_SAMPLES-1) indexRssiLqi++;
        else indexRssiLqi = 0;

        if (!packet.crc_ok) {
            debug->println(F("CRC FAIL"));
        }

        if (packet.crc_ok && packet.length > 0) {
            if(debug->getDebugEnabled()) {
                debug->print(F("("));
                debug->print(packet.length);
                debug->print(F("): "));
            }

            for(i=0; i<packet.length; i++) {
                if(debug->getDebugEnabled()) {
                    debug->print(packet.data[i]);
                    debug->print(" ");
                }
                buff[*len+i] = packet.data[i];
            }
            *len += packet.length;

            debug->println();
        }
    }
}

void CommunicationService::testConnection() {
    if(radioCommunicationStatus == 0) {
        radio->init();
        radio->setSyncWord(syncWord);
        radio->disableAddressCheck();

        if (radio->readReg(CC1101_PARTNUM, CC1101_STATUS_REGISTER) == 0 &&
            radio->readReg(CC1101_VERSION, CC1101_STATUS_REGISTER) > 0) {
            debug->println(F("# Radio communication initialized!"));
            radioCommunicationStatus = 1;
        } else {
            debug->println(F("# Could not initilize radio communication!"));
            ledController->setAnimation(LED_STATUS_COMM_ERROR);
            radioCommunicationStatus = 0;
        }
    }
}

int16_t CommunicationService::getComputedLinkQuality(int16_t lqi, int16_t rssi) {
    if(lqi <= 0) {
        lqiPercent = 0;
    } else {
        lqiPercent = constrain(map(lqi, 120, 20, 0, 100), 0, 100);
    }
    rssiPercent = constrain(map(rssi, -110, -20, 0, 100), 0, 100);
    return ((lqiPercent * 4) + (rssiPercent * 6)) / 10;
}

int16_t CommunicationService::getLastMessageComputedLinkQuality() {
    return getComputedLinkQuality(lastLqi[previousIndexRssiLqi], lastRssi[previousIndexRssiLqi]);
}

int16_t CommunicationService::getAvarageLinkQuality() {
    avarageLqi = 0;
    avarageRssi = 0;
    for(i=0; i<RSSI_LQI_SAMPLES; i++) {
        avarageLqi += lastLqi[i];
        avarageRssi += lastRssi[i];
    }
    avarageLqi /= RSSI_LQI_SAMPLES;
    avarageRssi /= RSSI_LQI_SAMPLES;
    return getComputedLinkQuality(avarageLqi, avarageRssi);
}

void CommunicationService::sendAlarmStartPackage(uint8_t *buff, uint8_t *len) {
    convertBytesToUInt.intVal = receiverAddress;
    buff[BUFF_INDEX_RECEIVER_LO] = convertBytesToUInt.byte[0];
    buff[BUFF_INDEX_RECEIVER_HI] = convertBytesToUInt.byte[1];
    convertBytesToUInt.intVal = deviceAddress;
    buff[BUFF_INDEX_ADDRESS_LO] = convertBytesToUInt.byte[0];
    buff[BUFF_INDEX_ADDRESS_HI] = convertBytesToUInt.byte[1];
    convertBytesToUInt.intVal = deviceId;
    buff[BUFF_INDEX_ID_LO] = convertBytesToUInt.byte[0];
    buff[BUFF_INDEX_ID_HI] = convertBytesToUInt.byte[1];
    buff[BUFF_INDEX_COMMAND] = COMMAND_ID_ALARM_START;
    buff[BUFF_INDEX_IS_FORWARD] = 0;
    *len = 8;

    send(buff, len);
}

void CommunicationService::sendAlarmStopPackage(uint8_t *buff, uint8_t *len) {
    convertBytesToUInt.intVal = receiverAddress;
    buff[BUFF_INDEX_RECEIVER_LO] = convertBytesToUInt.byte[0];
    buff[BUFF_INDEX_RECEIVER_HI] = convertBytesToUInt.byte[1];
    convertBytesToUInt.intVal = deviceAddress;
    buff[BUFF_INDEX_ADDRESS_LO] = convertBytesToUInt.byte[0];
    buff[BUFF_INDEX_ADDRESS_HI] = convertBytesToUInt.byte[1];
    convertBytesToUInt.intVal = deviceId;
    buff[BUFF_INDEX_ID_LO] = convertBytesToUInt.byte[0];
    buff[BUFF_INDEX_ID_HI] = convertBytesToUInt.byte[1];
    buff[BUFF_INDEX_COMMAND] = COMMAND_ID_ALARM_STOP;
    buff[BUFF_INDEX_IS_FORWARD] = 0;
    *len = 8;

    send(buff, len);
}



ProgrammingService::ProgrammingService(AlarmService *alarmService, SerialDebug *serialDebug, CC1101Controller *cc1101Controller, 
            EEPROMController *eepromController, CommunicationService *communicationService, ServiceMode *serviceMode) {
    this->alarmService = alarmService;
    this->serialDebug = serialDebug;
    this->cc1101Controller = cc1101Controller;
    this->eepromController = eepromController;
    this->communicationService = communicationService;
    this->serviceMode = serviceMode;
}

void ProgrammingService::setup() {
    if(!Serial) {
        Serial.begin(SERIAL_BAUDRATE);
        pinMode(HW_RXD0_PIN, INPUT_PULLUP);
    }
}

void ProgrammingService::tick() {
    serialProgramming();
}

void ProgrammingService::serialProgramming() {
    availableBytesToRead = Serial.available();
    if(availableBytesToRead > 0) {

        if(availableBytesToRead > SERIAL_RECEIVE_BUFF_SIZE) availableBytesToRead = SERIAL_RECEIVE_BUFF_SIZE;
        Serial.readBytes(receiveBuffer, availableBytesToRead);
        i=0;
        while(i < availableBytesToRead) {

            if(receiveBuffer[i] >= RECV_MSG_MIN_LEN && receiveBuffer[i] <= RECV_MSG_MAX_LEN) {

                for(j=0; j<receiveBuffer[i]; j++) {
                    workingBuffer[j] = receiveBuffer[i+j];
                }

                if(receiveBuffer[i+j] == calculateCRC(workingBuffer, j)) {

                    switch(workingBuffer[1]) {
                        case 1:

                            convertBytesToLong.byte[0] = workingBuffer[2];
                            convertBytesToLong.byte[1] = workingBuffer[3];
                            convertBytesToLong.byte[2] = workingBuffer[4];
                            convertBytesToLong.byte[3] = workingBuffer[5];

                            if(convertBytesToLong.longVal >= 10 && convertBytesToLong.longVal <= 65000) {
                                eepromController->setUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT, (uint16_t)convertBytesToLong.longVal);
                                communicationService->reloadEepromValues();
                                sendResponseOK(1);
                            } else {
                                sendResponseFail(1);
                            }
                        break;
                        case 2:

                            convertBytesToInt.byte[0] = workingBuffer[2];
                            convertBytesToInt.byte[1] = workingBuffer[3];

                            if(convertBytesToInt.intVal >= 0 && convertBytesToInt.intVal <= 10) {
                                eepromController->setByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE, (uint8_t)convertBytesToInt.intVal);
                                cc1101Controller->setChannel((uint8_t)convertBytesToInt.intVal);
                                communicationService->reloadEepromValues();
                                sendResponseOK(2);
                            } else {
                                sendResponseFail(2);
                            }
                        break;
                        case 3:

                            convertBytesToLong.byte[0] = workingBuffer[2];
                            convertBytesToLong.byte[1] = workingBuffer[3];
                            convertBytesToLong.byte[2] = workingBuffer[4];
                            convertBytesToLong.byte[3] = workingBuffer[5];

                            if(convertBytesToLong.longVal >= 1 && convertBytesToLong.longVal <= 65000) {
                                eepromController->setUIntValue(EEPROM_CC1101_RECEIVER_ADDRESS_INT, (uint16_t)convertBytesToLong.longVal);
                                communicationService->reloadEepromValues();
                                sendResponseOK(3);
                            } else {
                                sendResponseFail(3);
                            }
                        break;
                        case 4:

                            convertBytesToLong.byte[0] = workingBuffer[2];
                            convertBytesToLong.byte[1] = workingBuffer[3];
                            convertBytesToLong.byte[2] = workingBuffer[4];
                            convertBytesToLong.byte[3] = workingBuffer[5];

                            if(convertBytesToLong.longVal >= 0 && convertBytesToLong.longVal <= 65000) {
                                eepromController->setUIntValue(EEPROM_CONFIG_DEVICE_ID_INT, (uint16_t)convertBytesToLong.longVal);
                                communicationService->reloadEepromValues();
                                sendResponseOK(4);
                            } else {
                                sendResponseFail(4);
                            }
                        break;
                        case 5:

                            convertBytesToInt.byte[0] = workingBuffer[2];
                            convertBytesToInt.byte[1] = workingBuffer[3];

                            if(convertBytesToInt.intVal >= 0 && convertBytesToInt.intVal <= 500) {
                                alarmService->setAlarmTriggerValue(convertBytesToInt.intVal);
                                sendResponseOK(5);
                            } else {
                                sendResponseFail(5);
                            }
                        break;
                        case 6:

                            if(workingBuffer[2] == 0 || workingBuffer[2] == 1) {

                                serialDebug->setDebugEnabled((bool) workingBuffer[2]);
                                sendResponseOK(6);
                            } else {
                                sendResponseFail(6);
                            }
                        break;
                        case 10:

                            responseBuffer[0] = 4;
                            responseBuffer[1] = 10;

                            convertBytesToInt.intVal = DEVICE_TYPE;
                            responseBuffer[2] = convertBytesToInt.byte[0];
                            responseBuffer[3] = convertBytesToInt.byte[1];

                            sendResponse();
                        break;
                        case 20:

                            responseBuffer[0] = 23;
                            responseBuffer[1] = 20;

                            convertBytesToInt.intVal = DEVICE_TYPE;
                            responseBuffer[2] = convertBytesToInt.byte[0];
                            responseBuffer[3] = convertBytesToInt.byte[1];

                            convertBytesToLong.longVal = eepromController->getUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT);
                            responseBuffer[4] = convertBytesToLong.byte[0];
                            responseBuffer[5] = convertBytesToLong.byte[1];
                            responseBuffer[6] = convertBytesToLong.byte[2];
                            responseBuffer[7] = convertBytesToLong.byte[3];

                            convertBytesToInt.intVal = eepromController->getByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE);
                            responseBuffer[8] = convertBytesToInt.byte[0];
                            responseBuffer[9] = convertBytesToInt.byte[1];

                            convertBytesToLong.longVal = eepromController->getUIntValue(EEPROM_CC1101_RECEIVER_ADDRESS_INT);
                            responseBuffer[10] = convertBytesToLong.byte[0];
                            responseBuffer[11] = convertBytesToLong.byte[1];
                            responseBuffer[12] = convertBytesToLong.byte[2];
                            responseBuffer[13] = convertBytesToLong.byte[3];

                            convertBytesToLong.longVal = eepromController->getUIntValue(EEPROM_CONFIG_DEVICE_ID_INT);
                            responseBuffer[14] = convertBytesToLong.byte[0];
                            responseBuffer[15] = convertBytesToLong.byte[1];
                            responseBuffer[16] = convertBytesToLong.byte[2];
                            responseBuffer[17] = convertBytesToLong.byte[3];

                            convertBytesToInt.intVal = alarmService->getAlarmTriggerValue();
                            responseBuffer[18] = convertBytesToInt.byte[0];
                            responseBuffer[19] = convertBytesToInt.byte[1];

                            responseBuffer[20] = serialDebug->getDebugEnabled();

                            convertBytesToInt.intVal = communicationService->getAvarageLinkQuality();
                            responseBuffer[21] = convertBytesToInt.byte[0];
                            responseBuffer[22] = convertBytesToInt.byte[1];

                            sendResponse();
                        break;
                        case 21:

                            convertBytesToInt.byte[0] = workingBuffer[2];
                            convertBytesToInt.byte[1] = workingBuffer[3];

                            if(convertBytesToInt.intVal == DEVICE_TYPE) {

                                convertBytesToLong.byte[0] = workingBuffer[4];
                                convertBytesToLong.byte[1] = workingBuffer[5];
                                convertBytesToLong.byte[2] = workingBuffer[6];
                                convertBytesToLong.byte[3] = workingBuffer[7];
                                if(convertBytesToLong.longVal >= 10 && convertBytesToLong.longVal <= 65000) {
                                    dataTemp.address = convertBytesToLong.longVal;
                                } else {
                                    sendResponseFail(21);
                                }

                                convertBytesToInt.byte[0] = workingBuffer[8];
                                convertBytesToInt.byte[1] = workingBuffer[9];
                                if(convertBytesToInt.intVal >= 0 && convertBytesToInt.intVal <= 10) {
                                    dataTemp.channel = convertBytesToInt.intVal;
                                } else {
                                    sendResponseFail(21);
                                }

                                convertBytesToLong.byte[0] = workingBuffer[10];
                                convertBytesToLong.byte[1] = workingBuffer[11];
                                convertBytesToLong.byte[2] = workingBuffer[12];
                                convertBytesToLong.byte[3] = workingBuffer[13];
                                if(convertBytesToLong.longVal >= 1 && convertBytesToLong.longVal <= 65000) {
                                    dataTemp.receiverAddress = convertBytesToLong.longVal;
                                } else {
                                    sendResponseFail(21);
                                }

                                convertBytesToLong.byte[0] = workingBuffer[14];
                                convertBytesToLong.byte[1] = workingBuffer[15];
                                convertBytesToLong.byte[2] = workingBuffer[16];
                                convertBytesToLong.byte[3] = workingBuffer[17];
                                if(convertBytesToLong.longVal >= 0 && convertBytesToLong.longVal <= 65000) {
                                    dataTemp.deviceId = convertBytesToLong.longVal;
                                } else {
                                    sendResponseFail(21);
                                }

                                convertBytesToInt.byte[0] = workingBuffer[18];
                                convertBytesToInt.byte[1] = workingBuffer[19];
                                if(convertBytesToInt.intVal >= 0 && convertBytesToInt.intVal <= 500) {
                                    dataTemp.alarmTrigger = convertBytesToInt.intVal;
                                } else {
                                    sendResponseFail(21);
                                }

                                if(workingBuffer[20] == 0 || workingBuffer[20] == 1) {
                                    dataTemp.debugEnabled = (bool) workingBuffer[20];
                                } else {
                                    sendResponseFail(21);
                                }

                                eepromController->setUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT, dataTemp.address);
                                eepromController->setByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE, dataTemp.channel);
                                cc1101Controller->setChannel(dataTemp.channel);
                                eepromController->setUIntValue(EEPROM_CC1101_RECEIVER_ADDRESS_INT, dataTemp.receiverAddress);
                                eepromController->setUIntValue(EEPROM_CONFIG_DEVICE_ID_INT, dataTemp.deviceId);
                                alarmService->setAlarmTriggerValue(dataTemp.alarmTrigger);
                                serialDebug->setDebugEnabled(dataTemp.debugEnabled);
                                communicationService->reloadEepromValues();

                                sendResponseOK(21);
                            } else {
                                sendResponseFail(21);
                            }
                        break;
                        case 101:

                            if(workingBuffer[2] == 1) {
                                serviceMode->test1Start();
                                sendResponseOK(101);
                            } else if(workingBuffer[2] == 0) {
                                serviceMode->test1Stop();
                                sendResponseOK(101);
                            } else {
                                sendResponseFail(101);
                            }
                        break;
                        case 102:

                            serviceMode->test2Start();
                            sendResponseOK(102);
                        break;
                        case 103:

                            serviceMode->test3Start();
                            sendResponseOK(103);
                        break;
                        default:

                            responseBuffer[0] = 3;
                            responseBuffer[1] = workingBuffer[1];
                            responseBuffer[2] = -1;

                            sendResponse();
                        break;
                    }

                    i+=j;
                } else {

                    i++;
                }
            } else {

                i++;
            }
        }
    }
}

uint8_t ProgrammingService::calculateCRC(const uint8_t *buff, uint8_t len) {
    crc = 0;
    for(k=0; k<len; k++) {
        crc += buff[k];
    }
    return (uint8_t)(crc % 256);
}

void ProgrammingService::sendResponseOK(uint8_t commandId) {
    responseBuffer[0] = 3;
    responseBuffer[1] = commandId;
    responseBuffer[2] = 1;

    sendResponse();
}

void ProgrammingService::sendResponseFail(uint8_t commandId) {
    responseBuffer[0] = 3;
    responseBuffer[1] = commandId;
    responseBuffer[2] = 0;

    sendResponse();
}

void ProgrammingService::sendResponse() {
    responseBuffer[(int)responseBuffer[0]] = calculateCRC(responseBuffer, responseBuffer[0]);

    Serial.write(responseBuffer, responseBuffer[0]+1);
    Serial.flush();
}



SerialDebug::SerialDebug() {
    debugEnabled = DEBUG_ENABLED;
}

SerialDebug::SerialDebug(boolean enableDebug) {
    this->debugEnabled = enableDebug;
}

void SerialDebug::setup() {
    Serial.begin(SERIAL_BAUDRATE);
    pinMode(HW_RXD0_PIN, INPUT_PULLUP);
    if(debugEnabled) Serial.println(F("# Debug enabled"));
}

void SerialDebug::setDebugEnabled(boolean state) {
    debugEnabled = state;
}

boolean SerialDebug::getDebugEnabled() {
    return debugEnabled;
}

void SerialDebug::printDebugStart() {
    if(debugEnabled) Serial.print(F("# "));
}

size_t SerialDebug::print(const __FlashStringHelper *m) {
    if(debugEnabled) return Serial.print(m);
    else return 0;
}

size_t SerialDebug::print(const String &m) {
    if(debugEnabled) return Serial.print(m);
    else return 0;
}

size_t SerialDebug::print(const char m[]) {
    if(debugEnabled) return Serial.print(m);
    else return 0;
}

size_t SerialDebug::print(int8_t m) {
    if(debugEnabled) return Serial.print(m);
    else return 0;
}

size_t SerialDebug::print(uint8_t m, int16_t d) {
    if(debugEnabled) return Serial.print(m,d);
    else return 0;
}

size_t SerialDebug::print(int16_t m, int16_t d) {
    if(debugEnabled) return Serial.print(m,d);
    else return 0;
}

size_t SerialDebug::print(uint16_t m, int16_t d) {
    if(debugEnabled) return Serial.print(m,d);
    else return 0;
}

size_t SerialDebug::print(int32_t  m, int16_t d) {
    if(debugEnabled) return Serial.print(m,d);
    else return 0;
}

size_t SerialDebug::print(uint32_t m, int16_t d) {
    if(debugEnabled) return Serial.print(m,d);
    else return 0;
}

size_t SerialDebug::print(float m, int16_t d) {
    if(debugEnabled) return Serial.print(m,d);
    else return 0;
}

size_t SerialDebug::print(const Printable &m) {
    if(debugEnabled) return Serial.print(m);
    else return 0;
}

size_t SerialDebug::println(const __FlashStringHelper *m) {
    if(debugEnabled) return Serial.println(m);
    else return 0;
}

size_t SerialDebug::println(const String &m) {
    if(debugEnabled) return Serial.println(m);
    else return 0;
}

size_t SerialDebug::println(const char m[]) {
    if(debugEnabled) return Serial.println(m);
    else return 0;
}

size_t SerialDebug::println(int8_t m) {
    if(debugEnabled) return Serial.println(m);
    else return 0;
}

size_t SerialDebug::println(uint8_t m, int16_t d) {
    if(debugEnabled) return Serial.println(m,d);
    else return 0;
}

size_t SerialDebug::println(int16_t m, int16_t d) {
    if(debugEnabled) return Serial.println(m,d);
    else return 0;
}

size_t SerialDebug::println(uint16_t m, int16_t d) {
    if(debugEnabled) return Serial.println(m,d);
    else return 0;
}

size_t SerialDebug::println(int32_t  m, int16_t d) {
    if(debugEnabled) return Serial.println(m,d);
    else return 0;
}

size_t SerialDebug::println(uint32_t m, int16_t d) {
    if(debugEnabled) return Serial.println(m,d);
    else return 0;
}

size_t SerialDebug::println(float m, int16_t d) {
    if(debugEnabled) return Serial.println(m,d);
    else return 0;
}

size_t SerialDebug::println(const Printable &m) {
    if(debugEnabled) return Serial.println(m);
    else return 0;
}

size_t SerialDebug::println(void) {
    if(debugEnabled) return Serial.println();
    else return 0;
}

ServiceMode::ServiceMode(BuzzerController *buzzerController, LEDController *ledController, OutputController *outputController, 
    AlarmService *alarmService, CommunicationService *communicationService) {
    this->buzzerController = buzzerController;
    this->ledController = ledController;
    this->outputController = outputController;
    this->alarmService = alarmService;
    this->communicationService = communicationService;

    testRunning = false;
}

void ServiceMode::test1Start() {
    testRunning = true;

    ledController->setSemaphore(10);
    buzzerController->start();
    outputController->set(RELAY_AC, true);
}

void ServiceMode::test1Stop() {
    testRunning = false;

    ledController->setSemaphore(3);
    buzzerController->stop();
    outputController->set(RELAY_AC, false);
}

void ServiceMode::test2Start() {
    communicationService->sendAlarmStartPackage(sendBuffer, sendBufferLen);
}

void ServiceMode::test3Start() {
    alarmService->setSkipStartup(true);
}

bool ServiceMode::isTestRunning() {
    return testRunning;
}

void ServiceMode::setSendBuffer(uint8_t*sendBuffer, uint8_t *sendBufferLen) {
    this->sendBuffer = sendBuffer;
    this->sendBufferLen = sendBufferLen;
}



Startup::Startup(EEPROMController *eepr) {
    this->eepr = eepr;
}

void Startup::doStartup() {

    pinMode(HW_TGS_SENSOR_PIN, INPUT);
    if(!eepr->getStartupFlag()) {
        randomSeed(analogRead(HW_TGS_SENSOR_PIN));
    } else {
        randomSeed(analogRead(HW_TGS_SENSOR_PIN) + eepr->getSerialNumber() + eepr->getUIntValue(EEPROM_CONFIG_DEVICE_ID_INT) +
            eepr->getUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT) + eepr->getByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE));
    }

    if(!eepr->getStartupFlag() || EEPROM_WIPE) {

        eepr->wipeEEPROM();

        eepr->setSerialNumber(random(100000,2147483640));

        eepr->setUIntValue(EEPROM_CONFIG_ALARM_TRIGGER_VALUE_INT, TGS_DEFAULT_ALERT_VALUE);

        eepr->setUIntValue(EEPROM_CONFIG_DEVICE_ID_INT, DEFAULT_DEVICE_ID);

        eepr->setUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT, DEFAULT_DEVICE_ID);
        eepr->setByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE, CC1101_DEFVAL_CHANNR);
        eepr->setUIntValue(EEPROM_CC1101_RECEIVER_ADDRESS_INT, DEFAULT_RECEIVER_ADDRESS);

        eepr->setStartupFlag(true);
    }
}