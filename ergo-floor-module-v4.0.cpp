#include <avr/wdt.h>

#include <Arduino.h>

#define APP_VERSION 40

#define DEVICE_TYPE 2

#define WATCHDOG_ENABLED true

#define WATCHDOG_RECEIVE_RESET_MS 300000 

#define SERIAL_BAUDRATE 19200

#define DETECTOR_OFFLINE_MS 420000  

#define DEBUG_ENABLED false

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



#define BTN_COUNT 5
#define BTN_DEBOUNCE_PERIOD 40

struct btnState {
    uint8_t pin;
    boolean lastValue;
    uint32_t timestamp;
    boolean clicked;
    boolean stateChanged;
};

#define BTN_BACK 0
#define BTN_LEFT 1
#define BTN_RIGHT 2
#define BTN_OK 3

class ButtonController
{
    private :
        boolean clickedStates[BTN_COUNT];
        boolean currentState;
        btnState btnStates[BTN_COUNT];          
        int16_t i;

        void configureButton(uint8_t pin, uint8_t id);  
    public :
        ButtonController();                             
        void setup();                                   
        void tick(uint32_t currentMillis);         
        boolean hasAnyStateChanged();                      
        boolean hasStateChanged(uint8_t button);     
        boolean* getAllClickedStates();                    
        boolean getClickedState(uint8_t button);     
};


#include <SPI.h>

struct CCPACKET
{
  public:
    uint8_t length;                         
    uint8_t data[CC1101_MAX_BUFFER_SIZE];   
    boolean crc_ok;                         
    int8_t rssi;                            
    uint8_t lqi;                            
};


#include <extEEPROM.h>

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

struct AlarmEntry {
    uint16_t address;
    uint16_t id;
    boolean state;
};

struct DetectorEntry {
    uint16_t address;
    uint16_t id;
    uint8_t linkQuality;
    uint8_t status;
    uint32_t lastUpdateMs;
};

struct OfflineDetectorEntry {
    uint16_t detectorEntryIndex;
};

struct AddressVerificationInterval {
    uint16_t min;
    uint16_t max;
};

class EEPROMController {
    private :
        extEEPROM eepr;
        int8_t i2cState;
        ConvertBytesToLong convertBytesToLong;
        ConvertBytesToInt convertBytesToInt;
        ConvertBytesToUInt convertBytesToUInt;

        int16_t i, j, aux, index;

        uint8_t numberOfAlarms, numberOfAddressVerificationIntervals;
        uint8_t alarmAuxBuffer[5];
        uint8_t comparatorAuxBuffer[2];

        void get(uint32_t addr, uint8_t *val, uint8_t len);
        void put(uint32_t addr, uint8_t *val, uint8_t len);
        int16_t calculateIndex(uint8_t count);
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

        void saveAlarmDetails();                                    
        void loadAlarmDetails();                                    
        uint8_t getNumberOfAlarms();                                
        void addAlarm(AlarmEntry* alarm);                           
        boolean getAlarm(AlarmEntry* alarm, uint8_t elemIndex);     
        boolean deleteAlarm(uint16_t address);                      
        boolean clearAlarms();                                      

        uint8_t getNumberOfAddressVerificationIntervals();                              
        boolean addAddressVerificationInterval(AddressVerificationInterval *interval);  
        boolean getAddressVerificationInterval(uint8_t index, AddressVerificationInterval *interval) ;      
        boolean clearAddressVerificationIntervals();                                       
};

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




#include <SPI.h>

const uint8_t SRAM_WRITE_MODE_REG = 0x01; 
const uint8_t SRAM_READ_MODE_REG = 0x05;  
const uint8_t SRAM_BYTE_MODE = B00000000; 
const uint8_t SRAM_PAGE_MODE = B10000000; 
const uint8_t SRAM_SEQ_MODE = B01000000;  
const uint32_t SRAM_1024 = 131072;        
const uint32_t SRAM_512 = 65536;          
const uint32_t SRAM_256 = 32768;          
const uint32_t SRAM_128 = 16384;          
const uint32_t SRAM_64 = 8192;            
const uint8_t SRAM_WRITE_CODE = 0x02;     
const uint8_t SRAM_READ_CODE = 0x03;      

class MicrochipSRAM {
private:
  uint8_t _SSPin = 0; 
  SPISettings *spiSettings;

public:
  MicrochipSRAM(const uint8_t SSPin, const uint32_t sramType);
  ~MicrochipSRAM();

  void clearMemory(const uint8_t clearValue = 0);

  template <typename T> uint32_t &get(const uint32_t addr, T &value) {
    static uint32_t returnAddress;
    uint8_t *bytePtr = (uint8_t *)&value;             
    returnAddress = (addr + sizeof(T)) % SRAMBytes;   
    SPI.beginTransaction(*spiSettings);
    digitalWrite(_SSPin, LOW);                        
    SPI.transfer(SRAM_READ_CODE);                     
    if (SRAMBytes == SRAM_1024) {
      SPI.transfer((uint8_t)(addr >> 16) & 0xFF);     
    }
    SPI.transfer((uint8_t)(addr >> 8) & 0xFF);        
    SPI.transfer((uint8_t)addr);                      
    for (uint32_t i = 0; i < sizeof(T); i++) {
      *bytePtr++ = SPI.transfer(0x00);                
    }
    digitalWrite(_SSPin, HIGH);                       
    SPI.endTransaction();
    return (returnAddress);                           
  }

  template <typename T> uint32_t &put(const uint32_t addr, const T &value) {
    static uint32_t returnAddress;
    const uint8_t *bytePtr = (const uint8_t *)&value; 
    returnAddress = (addr + sizeof(T)) % SRAMBytes;   
    SPI.beginTransaction(*spiSettings);
    digitalWrite(_SSPin, LOW);                        
    SPI.transfer(SRAM_WRITE_CODE);                    
    if (SRAMBytes == SRAM_1024) {
      SPI.transfer((uint8_t)(addr >> 16) & 0xFF);     
    }
    SPI.transfer((uint8_t)(addr >> 8) & 0xFF);        
    SPI.transfer((uint8_t)addr);                      
    for (uint32_t i = 0; i < sizeof(T); i++) {
      SPI.transfer(*bytePtr++);                       
    }
    digitalWrite(_SSPin, HIGH);                       
    SPI.endTransaction();
    return (returnAddress);                           
  }

  template <typename T> uint32_t &fillMemory(uint32_t addr, T &value) {
    while (addr < (SRAMBytes - sizeof(T)))
      addr = put(addr, value);                        
    return (addr);
  }

  uint32_t SRAMBytes = 0;   
};

class SRAMController {
    private :
        MicrochipSRAM *sram;
        EEPROMController *eepr;

        ConvertBytesToULong convertBytesToULong;
        ConvertBytesToInt convertBytesToInt;
        ConvertBytesToUInt convertBytesToUInt;

        boolean anyErrorsExist, foundElement, isStateChange, lastState;
        uint16_t i, j, index;
        uint16_t numberOfAlarms, numberOfDetectors, numberOfOfflineDetectors;

        uint8_t alarmAuxBuffer[5];
        uint8_t detectorAuxBuffer[10];
        uint8_t offlineDetectorAuxBuffer[2];
        uint8_t comparatorAuxBuffer[2];
    public :
        SRAMController(EEPROMController *eepr);

        void setup();                           
        uint32_t getSRAMType();                 

        uint16_t getNumberOfAlarms();                
        uint16_t getNumberOfDetectors();             
        uint16_t getNumberOfOfflineDetectors();      

        int16_t addAlarm(AlarmEntry *alarm, boolean isUpdateOnly);
        int16_t addAlarm(AlarmEntry *alarm, boolean isUpdateOnly, boolean isStartup);       
        boolean getAlarm(AlarmEntry *alarm, uint16_t elemIndex);        
        int16_t findAlarm(uint16_t address);                            
        boolean deleteAlarm(uint16_t elemIndex);                        
        boolean clearAlarms();                                          

        int16_t addDetector(DetectorEntry *detector);                        
        boolean getDetector(DetectorEntry *detector, uint16_t elemIndex);   
        int16_t findDetector(uint16_t address);                             
        boolean deleteDetector(uint16_t elemIndex);                         
        boolean clearDetectors();                                           

        int16_t addOfflineDetector(OfflineDetectorEntry *offlineDetector);                          
        boolean getOfflineDetector(OfflineDetectorEntry *offlineDetector, uint16_t elemIndex);      
        int16_t findOfflineDetector(uint16_t detectorEntryIndex);                                   
        boolean deleteOfflineDetector(uint16_t detectorEntryIndex);                                 
        boolean clearOfflineDetectors();                                                            
};



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

class LEDController
{
    private :
        uint8_t currentAnimation;
        int16_t incrementor, pulseTimer;
        void assignColor(ledColor *copyTo, ledColor *copyFrom);   
        boolean isDifferentColor(ledColor cmp1, ledColor cmp2);   
        boolean _canWriteRed;
        ledColor currentColor, currentSetColor, predefinedColors[2];
    public :
        LEDController();  
        void setup();     
        void tick();      
        void setColor(uint8_t predefinedColor); 
        void setAnimation(uint8_t animation);   
        void set(uint8_t predefinedColor, uint8_t animation); 
        void resetCurrentColor();   
};



#define OUTPUTS_DEFAULT_ONESHOT_DELAY 3000

#define OUTPUTS_DEFAULT_PULSE_DELAY 1000

#define OUTPUTS_PULSE_COUNT 3

#define OUTPUTS_COUNT 2

#define RELAY_AC 0
#define RELAY_DC 1

class OutputController {
    private:
        boolean outputState[OUTPUTS_COUNT];             
        uint32_t  outputTimer[OUTPUTS_COUNT];           
        uint8_t outputPulseCount[OUTPUTS_COUNT];        
        uint8_t outputPins[OUTPUTS_COUNT];              

        int16_t i;
    public:
        OutputController();                             
        void setup();                                   
        void tick(uint32_t  currentMillis);             

        boolean get(int16_t outputId);                                      
        void set(int16_t outputId, boolean state);                          
        void set(int16_t outputId, boolean state, uint32_t  timeout);       
        void setPulsed(int16_t outputId, boolean state, uint8_t pulses);    
};





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

#define RSSI_LQI_SAMPLES 20

class CommunicationService {
    private :
        CC1101Controller *radio;
        CCPACKET packet;
        SerialDebug *debug;
        EEPROMController *eepromController;
        LEDController *ledController;

        uint8_t radioCommunicationStatus;

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
};


#include <LiquidCrystal_I2C.h>

#define LCD_ADDRESS 0x27
#define LCD_NUMBER_OF_CHARS 16
#define LCD_NUMBER_OF_ROWS 2

#define MENU_NUMBER_OF_PAGES 10          
#define MENU_TIMEOUT_MS 120000          
#define MENU_BACKLIGHT_OFF_MS 60000     
#define MENU_VALUE_REFRESH_MS 3000      

#define MENU_TEXT_ENERGO_INSTAL F("ERGO")
#define MENU_TEXT_EVERYTHING_OK F("Everything OK")
#define MENU_TEXT_DETECTOR_ALARMS F("Detector alarms:")
#define MENU_TEXT_NO_ALARMS F("No alarms")
#define MENU_TEXT_CONNECTED F("Connected")
#define MENU_TEXT_ALERT_OR_ALERTS F(" alert(s)")
#define MENU_TEXT_ALARM_OR_ALARMS F(" alarm(s)")
#define MENU_TEXT_ALARM F("Alarm ")
#define MENU_TEXT_DETECTOR F("Detector ")
#define MENU_TEXT_DETECTORS F("detectors: ")
#define MENU_TEXT_ONLINE F("Online")
#define MENU_TEXT_OFFLINE F("Offline")
#define MENU_TEXT_CLEAR_ALARMS F("Clear alarms?")
#define MENU_TEXT_NO_YES F("No           Yes")
#define MENU_TEXT_CLEAR_DETECTORS F("Clear detectors?")
#define MENU_TEXT_OPERATION_MODE F("Operation mode")
#define MENU_TEXT_MASTER F("Master")
#define MENU_TEXT_SLAVE F("Slave")
#define MENU_TEXT_SHIFTER F("Shifter")
#define MENU_TEXT_DEVICE_ADDRESS F("Device address:")
#define MENU_TEXT_DEVICE_CHANNEL F("Device channel:")
#define MENU_TEXT_FORWARD_ADDRESS F("Forward address:")
#define MENU_TEXT_FORWARD_CHANNEL F("Forward channel:")
#define MENU_TEXT_SEPARATOR_DASH F("/")
#define MENU_TEXT_SEPARATOR_TWO_POINTS F(":")
#define MENU_TEXT_SEPARATOR_SPACE F(" ")
#define MENU_TEXT_LQI_OPEN F(" (")
#define MENU_TEXT_LQI_CLOSED F("%)")
#define MENU_TEXT_HRS F(" hrs")
#define MENU_TEXT_MIN F(" min")
#define MENU_TEXT_SEC F(" sec")
#define MENU_TEXT_NA F("N/A")
#define MENU_TEXT_SEPARATOR_DASH_SPACED F(" | ")
#define MENU_TEXT_ON F("ON")
#define MENU_TEXT_OFF F("OFF")
#define MENU_TEXT_NOT_IMPLEMENTED F("Not implemented!")
#define MENU_TEXT_FIRMWARE F("Firmware")
#define MENU_TEXT_VERSION F(" v")
#define MENU_TEXT_SEPARATOR_DOT F(".")

struct MenuElement {
    const __FlashStringHelper *textLine1;   
    const __FlashStringHelper *textLine2;   
    boolean isLine1Dynamic;                 
    boolean isLine2Dynamic;                 
    uint8_t back, left, right, ok;          
    boolean valueRefreshEnabled;            
};

#define PAGE_INDEX_HOME 0
#define PAGE_INDEX_ALARMS 1
#define PAGE_INDEX_DETECTORS 2
#define PAGE_INDEX_DETECTORS_OFFLINE 3
#define PAGE_INDEX_LIST_ALARMS 4
#define PAGE_INDEX_LIST_DETECTORS 5
#define PAGE_INDEX_LIST_DETECTORS_OFFLINE 6
#define PAGE_INDEX_CLEAR_ALARMS 7
#define PAGE_INDEX_CLEAR_DETECTORS 8
#define PAGE_INDEX_FIRMWARE 9

class MenuService {
    private :
        LiquidCrystal_I2C *lcd;
        ButtonController *buttonController;
        EEPROMController *eepromController;
        SRAMController *sramController;

        MenuElement menu[MENU_NUMBER_OF_PAGES];
        uint8_t currentPageId, lastPageId, charsPrinted;
        uint32_t lastButtonClick, lastValueRefresh;

        AlarmEntry alarmEntry;
        DetectorEntry detectorEntry;
        OfflineDetectorEntry offlineDetectorEntry;

        int8_t line1Text[LCD_NUMBER_OF_CHARS], line2Text[LCD_NUMBER_OF_CHARS];

        uint16_t i, homeAlarmIndex, alarmIndex, detectorIndex, offlineDetectorIndex;

        void onButtonAction(uint8_t button);
        void loadPage(uint8_t id);
        void refreshDisplayedValues();
        void setLine1DynamicText(uint8_t id);
        void setLine2DynamicText(uint8_t id);
        void setupMenuSystem();
        void printTimeFromMs(uint32_t timeSinceLastMessageMs);
        void printClearPadding();

    public :
        MenuService(ButtonController *buttonController, EEPROMController *eepromController, SRAMController *sramController);
        void setup();
        void tick(uint32_t currentTimeMs);
};





#define OPERATION_MODE_MASTER 1
#define OPERATION_MODE_SLAVE 2

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

#define MESSAGE_IS_FORWARDED 100

class CoreService {
    private :
        OutputController *outputController;
        EEPROMController *eepromController;
        SRAMController *sramController;
        SerialDebug *debug;
        CommunicationService *communicationService;

        uint8_t *receiveBuffer, *sendBuffer;
        uint8_t *receiveBufferLen, *sendBufferLen;

        uint16_t deviceAddress, forwardAddress;
        uint8_t operationMode, deviceChannel, forwardChannel;
        int16_t aux;

        AlarmEntry alarmEntry;
        DetectorEntry detectorEntry;
        OfflineDetectorEntry offlineDetectorEntry;

        uint8_t numberOfAddressVerificationIntervals;
        AddressVerificationInterval addressVerificationIntervals[2];

        ConvertBytesToUInt convertBytesToUInt;

        uint16_t i, j, index;

        void addReceiveToDetectorsList(uint32_t currentMillis, uint16_t index);
        int16_t addReceiveToAlarmsList(uint16_t index, boolean state);
        void removeReceiveFromAlarmsList(uint16_t index);
        void sendRssiMessageFromReceive(uint16_t index);
        void forwardReceiveMessage(uint16_t index);
        void operationModeMaster(uint32_t currentMillis);
        void operationModeSlave(uint32_t currentMillis);
        void doSend();
    public :
        CoreService(OutputController *outputController, EEPROMController *eepromController, SRAMController *sramController, 
            SerialDebug *debug, CommunicationService *communicationService);
        void setup();
        void tick(uint32_t currentMillis);
        void checkOfflineDetectors(uint32_t currentMillis);

        void setReceiveBuffer(uint8_t *receiveBuffer, uint8_t *receiveBufferLen);
        void setSendBuffer(uint8_t *sendBuffer, uint8_t *sendBufferLen);
        void reloadEepromValues();

        void loadAddressVerificationIntervals();
        boolean addAddressVerificationInterval(uint16_t min, uint16_t max);
        boolean clearAddressVerificationIntervals();
};



class ServiceMode
{
    private :
        OutputController *outputController;
        CommunicationService *communicationService;

        bool testRunning;

        uint8_t *sendBuffer;
        uint8_t *sendBufferLen;
    public :
        ServiceMode(OutputController *outputController, CommunicationService *communicationService);

        void test1Start();      
        void test1Stop();       

        bool isTestRunning();   
        void setSendBuffer(uint8_t *sendBuffer, uint8_t *sendBufferLen);   
};

#define SERIAL_RECEIVE_BUFF_SIZE 50
#define SERIAL_WORKING_BUFF_SIZE 25
#define SERIAL_SEND_BUFF_SIZE 25
#define RECV_MSG_MIN_LEN 2
#define RECV_MSG_MAX_LEN 19

#define COMMAND_ID_SET_DEVICE_ADDRESS 1
#define COMMAND_ID_SET_DEVICE_CHANNEL 2
#define COMMAND_ID_SET_FORWARD_ADDRESS 3
#define COMMAND_ID_SET_FORWARD_CHANNEL 4
#define COMMAND_ID_SET_OPERATION_MODE 5
#define COMMAND_ID_SET_DEBUG_MODE 6
#define COMMAND_ID_GET_DEVICE_TYPE 10
#define COMMAND_ID_GET_ALL_VALUES 20
#define COMMAND_ID_SET_ALL_VALUES 21
#define COMMAND_ID_STREAM_ALL_MEMORY 30
#define COMMAND_ID_CLEAR_ALARMS 31
#define COMMAND_ID_CLEAR_DETECTORS 32
#define COMMAND_ID_ADD_ADDRESS_VERIFICATION_INTERVAL 40
#define COMMAND_ID_CLEAR_ADDRESS_VERIFICATION_INTERVALS 41
#define COMMAND_ID_TEST_MODE 101

struct CompleteData {
    uint16_t address;
    uint8_t channel;
    uint16_t forwardAddress;
    uint8_t forwardChannel;
    uint8_t operationMode;
    boolean debugEnabled;
};

class ProgrammingService {
    private :
        SerialDebug *serialDebug;
        CC1101Controller *cc1101Controller;
        EEPROMController *eepromController;
        CommunicationService *communicationService;
        CoreService *coreService;
        ServiceMode *serviceMode;
        SRAMController *sramController;

        uint8_t receiveBuffer[SERIAL_RECEIVE_BUFF_SIZE], workingBuffer[SERIAL_WORKING_BUFF_SIZE], responseBuffer[SERIAL_SEND_BUFF_SIZE];
        uint8_t availableBytesToRead;
        int16_t i, j, k, crc;
        uint16_t l;
        CompleteData dataTemp;

        AlarmEntry alarmEntry;
        DetectorEntry detectorEntry;
        OfflineDetectorEntry offlineDetectorEntry;

        ConvertBytesToInt convertBytesToInt;
        ConvertBytesToLong convertBytesToLong;
        ConvertBytesToULong convertBytesToULong;

        void serialProgramming();                           
        uint8_t calculateCRC(uint8_t *buff, uint8_t len);   
        void sendResponse();                                
        void sendResponseStartStream(uint8_t commandId);    
        void sendResponseOK(uint8_t commandId);             
        void sendResponseFail(uint8_t commandId);           
    public :
        ProgrammingService(SerialDebug *serialDebug, CC1101Controller *cc1101Controller, EEPROMController *eepromController, 
            CommunicationService *communicationService, CoreService *coreService, ServiceMode *serviceMode, SRAMController *sramController);
        void setup();
        void tick();
};



class Startup
{
    private :
        EEPROMController *eepr;
        SRAMController *sram;
    public :
        Startup(EEPROMController *eepr, SRAMController *sram);
        void doStartup();                   
};

boolean startupFlag;
volatile boolean packetWaiting;
uint8_t sendBuffer[CC1101_MAX_TX_BUFFER_SIZE], recvBuffer[CC1101_MAX_RX_BUFFER_SIZE];
uint8_t sendBufferLen, recvBufferLen;
int32_t communicationStartupDelayMs;

SerialDebug debug;

ButtonController buttonController;
EEPROMController eepromController;
SRAMController sramController(&eepromController);
LEDController ledController;
OutputController outputController;
CC1101Controller cc1101Controller(&eepromController);

Startup startup(&eepromController, &sramController);

CommunicationService communicationService(&cc1101Controller, &debug, &eepromController, &ledController);
MenuService menuService(&buttonController, &eepromController, &sramController);
CoreService coreService(&outputController, &eepromController, &sramController, &debug, &communicationService);
ServiceMode serviceMode(&outputController, &communicationService);
ProgrammingService programmingService(&debug, &cc1101Controller, &eepromController, &communicationService, &coreService, &serviceMode, &sramController);

uint32_t currentMillis = 0, lastMillis1Min = 0, lastMillis20Hz = 0, lastMillis10Hz = 0, lastMillis100Hz = 0, lastMillis200Hz = 0, lastMillis2Hz = 0;
uint32_t lastCommunicationRX = 0;

void ihrMessageReceived() {
    packetWaiting = true;
}

void cc1101Receive() {
    if (packetWaiting) {
        detachInterrupt(CC1101_Interrupt);
        communicationService.receive(recvBuffer, &recvBufferLen);
        lastCommunicationRX = millis();
        packetWaiting = false;
        attachInterrupt(CC1101_Interrupt, ihrMessageReceived, FALLING);
    }
}

void setup() {

    MCUSR = 0;
    wdt_disable();

    debug.setup();
    buttonController.setup();
    eepromController.setup();
    ledController.setup();
    outputController.setup();
    sramController.setup();

    startup.doStartup();

    startupFlag = eepromController.getStartupFlag();
    if(startupFlag && WATCHDOG_ENABLED) {
        wdt_reset();
        wdt_enable(WDTO_8S); 
    }

    menuService.setup();
    coreService.setup();
    programmingService.setup();

    attachInterrupt(CC1101_Interrupt, ihrMessageReceived, FALLING);

    communicationStartupDelayMs = random(0, 3000);   
    sendBufferLen = 0;
    recvBufferLen = 0;
    packetWaiting = false;
    coreService.setReceiveBuffer(recvBuffer, &recvBufferLen);
    coreService.setSendBuffer(sendBuffer, &sendBufferLen);
    serviceMode.setSendBuffer(sendBuffer, &sendBufferLen);
}

void loop() {
    currentMillis = millis();

    if(currentMillis - lastMillis1Min >= 60000) {
        if(!startupFlag) {
            startupFlag = eepromController.getStartupFlag();
            if(startupFlag && WATCHDOG_ENABLED) {
                wdt_reset();
                wdt_enable(WDTO_8S); 
            }
        }
        coreService.checkOfflineDetectors(currentMillis);

        lastMillis1Min = currentMillis;
    }

    if(currentMillis - lastMillis2Hz >= 500) {
        if(startupFlag && WATCHDOG_ENABLED) {

            if(currentMillis - lastCommunicationRX < WATCHDOG_RECEIVE_RESET_MS) {
                wdt_reset();    
            }
        }

        lastMillis2Hz = currentMillis;
    }

    if(currentMillis - lastMillis10Hz >= 100) {
        if(!serviceMode.isTestRunning()) {
            outputController.tick(currentMillis);
        }
        menuService.tick(currentMillis);

        lastMillis10Hz = currentMillis;
    }

    if(currentMillis - lastMillis20Hz >= 50) {
        programmingService.tick();

        lastMillis20Hz = currentMillis;
    }

    if(currentMillis - lastMillis100Hz >= 10) {

        if(communicationStartupDelayMs != -1 && currentMillis > (unsigned long)communicationStartupDelayMs) {
            communicationService.setup();
            communicationStartupDelayMs = -1;
        }

        coreService.tick(currentMillis);

        buttonController.tick(currentMillis);

        lastMillis100Hz = currentMillis;
    }

    if(currentMillis - lastMillis200Hz >= 5) {
        cc1101Receive();
        ledController.tick();

        lastMillis200Hz = currentMillis;
    }
}

#define SERIAL_TX_BUFFER_SIZE 80
#define SERIAL_RX_BUFFER_SIZE 120

#define EEPROM_PAGE_SIZE 16

#define EEPROM_WIPE 0
#define EEPROM_WIPE_START_INDEX 0
#define EEPROM_WIPE_END_INDEX 210

#define EEPROM_SERIAL_NUMBER_ADDRESS_START 0
#define EEPROM_SERIAL_NUMBER_ADDRESS_END 3

#define EEPROM_STARTUP_FLAG_ADDRESS_INT 10

#define EEPROM_CONFIG_OPERATION_MODE_BYTE 50

#define EEPROM_CC1101_DEVICE_ADDRESS_INT 100
#define EEPROM_CC1101_DEVICE_CHANNEL_BYTE 102
#define EEPROM_CC1101_FORWARD_ADDRESS_INT 103
#define EEPROM_CC1101_FORWARD_CHANNEL_BYTE 105

#define EEPROM_ALARMS_MEMORY_INDEX_BYTE 109
#define EEPROM_ALARMS_MEMORY_COUNT_BYTE 110
#define EEPROM_ALARMS_MEMORY_START 111
#define EEPROM_ALARMS_MEMORY_END 210
#define EEPROM_ALARMS_MEMORY_SEGMENT_SIZE 5

#define EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_COUNT_BYTE 250
#define EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_START 251
#define EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_END 291
#define EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_SEGMENT_SIZE 4

#define HW_STATUS_LED_PIN 6

#define HW_OUTPUT_RELAY_AC_PIN 9
#define HW_OUTPUT_RELAY_DC_PIN 7

#define HW_INPUT_BTN_BACK_PIN 5
#define HW_INPUT_BTN_LEFT_PIN 4
#define HW_INPUT_BTN_RIGHT_PIN 17
#define HW_INPUT_BTN_OK_PIN 16

#define HW_SPI_CS_SRAM_PIN 15

#define HW_IO_NC_PIN 14

#define HW_RXD0_PIN 0

#define LED_COLOR_NONE 0
#define LED_COLOR_RED 1

#define LED_STATUS_COUNT 5

#define LED_STATUS_NONE 0
#define LED_STATUS_OK 1
#define LED_STATUS_WARNING 2
#define LED_STATUS_ERROR 3
#define LED_STATUS_COMM_ERROR 4

#define SRAM_ALARMS_MEMORY_START 0
#define SRAM_ALARMS_MEMORY_END 6050
#define SRAM_ALARMS_MEMORY_SEGMENT_SIZE 5

#define SRAM_DETECTORS_MEMORY_START 6050
#define SRAM_DETECTORS_MEMORY_END 54050
#define SRAM_DETECTORS_MEMORY_SEGMENT_SIZE 10

#define SRAM_OFFLINE_DETECTORS_MEMORY_START 54050
#define SRAM_OFFLINE_DETECTORS_MEMORY_END 63650
#define SRAM_OFFLINE_DETECTORS_MEMORY_SEGMENT_SIZE 2



ButtonController::ButtonController() {
    configureButton(HW_INPUT_BTN_BACK_PIN, 0);
    configureButton(HW_INPUT_BTN_LEFT_PIN, 1);
    configureButton(HW_INPUT_BTN_RIGHT_PIN, 2);
    configureButton(HW_INPUT_BTN_OK_PIN, 3);
}

void ButtonController::configureButton(uint8_t pin, uint8_t id) {
    btnStates[id].pin = pin;
    btnStates[id].lastValue = false;
    btnStates[id].timestamp = 0;
    btnStates[id].clicked = false;
    btnStates[id].stateChanged = false;
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

            if(currentState) {

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
    numberOfAddressVerificationIntervals = 254;
}

void EEPROMController::setup() {
    i2cState = eepr.begin();
}

boolean EEPROMController::getI2CStatus() {
    return i2cState == 0;
}

void EEPROMController::wipeEEPROM() {
    for(i=EEPROM_WIPE_START_INDEX;i<EEPROM_WIPE_END_INDEX;i++) {
        eepr.write(i,0);
    }
}

void EEPROMController::setSerialNumber(uint32_t serialNumber) {
    convertBytesToLong.longVal = serialNumber;

    j = 0;
    for(i=EEPROM_SERIAL_NUMBER_ADDRESS_START; i<=EEPROM_SERIAL_NUMBER_ADDRESS_END; i++) {
        eepr.write(i, convertBytesToLong.byte[j]);
        j++;
    }
}

uint32_t EEPROMController::getSerialNumber() {
    j = 0;
    for(i=EEPROM_SERIAL_NUMBER_ADDRESS_START; i<=EEPROM_SERIAL_NUMBER_ADDRESS_END; i++) {
        convertBytesToLong.byte[j] = eepr.read(i);
        j++;
    }

    return convertBytesToLong.longVal;
}

void EEPROMController::setStartupFlag(boolean state) {
    if(state) {
        eepr.write(EEPROM_STARTUP_FLAG_ADDRESS_INT, 4);
        eepr.write(EEPROM_STARTUP_FLAG_ADDRESS_INT+1, 2);
    } else {
        eepr.write(EEPROM_STARTUP_FLAG_ADDRESS_INT, 0);
        eepr.write(EEPROM_STARTUP_FLAG_ADDRESS_INT+1, 0);
    }
}

boolean EEPROMController::getStartupFlag() {
    if(eepr.read(EEPROM_STARTUP_FLAG_ADDRESS_INT) == 4 && eepr.read(EEPROM_STARTUP_FLAG_ADDRESS_INT+1) == 2) {
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

void EEPROMController::get(uint32_t addr, uint8_t *val, uint8_t len) {
    for(aux=0; aux<len; aux++) {
        val[aux] = eepr.read(addr+aux);
    }
}

void EEPROMController::put(uint32_t addr, uint8_t *val, uint8_t len) {
    for(aux=0; aux<len; aux++) {
        eepr.write(addr+aux, val[aux]);
    }
}

int16_t EEPROMController::calculateIndex(uint8_t elemIndex) {
    aux = index + elemIndex * EEPROM_ALARMS_MEMORY_SEGMENT_SIZE;
    if(aux > EEPROM_ALARMS_MEMORY_END - EEPROM_ALARMS_MEMORY_SEGMENT_SIZE + 1) {
        aux = EEPROM_ALARMS_MEMORY_START + (aux - EEPROM_ALARMS_MEMORY_END) - 1;
    }
    return aux;
}

void EEPROMController::saveAlarmDetails() {
    eepr.write(EEPROM_ALARMS_MEMORY_INDEX_BYTE, index);
    eepr.write(EEPROM_ALARMS_MEMORY_COUNT_BYTE, numberOfAlarms);
}

void EEPROMController::loadAlarmDetails() {
    index = eepr.read(EEPROM_ALARMS_MEMORY_INDEX_BYTE);
    numberOfAlarms = eepr.read(EEPROM_ALARMS_MEMORY_COUNT_BYTE);
}

uint8_t EEPROMController::getNumberOfAlarms() {
    return numberOfAlarms;
}

void EEPROMController::addAlarm(AlarmEntry* alarm) {
    convertBytesToUInt.intVal = alarm->address;
    alarmAuxBuffer[0] = convertBytesToUInt.byte[0];
    alarmAuxBuffer[1] = convertBytesToUInt.byte[1];
    convertBytesToUInt.intVal = alarm->id;
    alarmAuxBuffer[2] = convertBytesToUInt.byte[0];
    alarmAuxBuffer[3] = convertBytesToUInt.byte[1];
    alarmAuxBuffer[4] = alarm->state;

    put(calculateIndex(numberOfAlarms), alarmAuxBuffer, 5);

    if(numberOfAlarms < 20) numberOfAlarms++;
    else {
        index += EEPROM_ALARMS_MEMORY_SEGMENT_SIZE;
        if(index >= EEPROM_ALARMS_MEMORY_END) {
            index = EEPROM_ALARMS_MEMORY_START;
        }
    }
    saveAlarmDetails();
}

boolean EEPROMController::getAlarm(AlarmEntry* alarm, uint8_t elemIndex) {
    if(elemIndex >= 0 && elemIndex < numberOfAlarms) {
        get(calculateIndex(elemIndex), alarmAuxBuffer, 5);
        convertBytesToUInt.byte[0] = alarmAuxBuffer[0];
        convertBytesToUInt.byte[1] = alarmAuxBuffer[1];
        alarm->address = convertBytesToUInt.intVal;
        convertBytesToUInt.byte[0] = alarmAuxBuffer[2];
        convertBytesToUInt.byte[1] = alarmAuxBuffer[3];
        alarm->id = convertBytesToUInt.intVal;
        alarm->state = alarmAuxBuffer[4];
        return true;
    }
    return false;
}

boolean EEPROMController::deleteAlarm(uint16_t address) {
    for(j=0;j<numberOfAlarms;j++) {
        if(getUIntValue(calculateIndex(j)) == address) {
            if(j+1 != numberOfAlarms) {

                get(calculateIndex(numberOfAlarms-1), alarmAuxBuffer, 5);
                put(calculateIndex(j), alarmAuxBuffer, 5);
            }
            numberOfAlarms--;
            saveAlarmDetails();
            return true;
        }
    }
    return false;
}

boolean EEPROMController::clearAlarms() {
    numberOfAlarms = 0;
    index = EEPROM_ALARMS_MEMORY_START;
    saveAlarmDetails();
    return true;
}

uint8_t EEPROMController::getNumberOfAddressVerificationIntervals() {
    if(numberOfAddressVerificationIntervals == 254) {
        numberOfAddressVerificationIntervals = (uint8_t)eepr.read(EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_COUNT_BYTE);
    }
    return numberOfAddressVerificationIntervals;
}

boolean EEPROMController::addAddressVerificationInterval(AddressVerificationInterval *interval) {
    if(numberOfAddressVerificationIntervals == 254) {
        numberOfAddressVerificationIntervals = (uint8_t)eepr.read(EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_COUNT_BYTE);
    }
    aux = (numberOfAddressVerificationIntervals * EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_SEGMENT_SIZE) + EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_START;
    if(aux < EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_END) {
        convertBytesToUInt.intVal = interval->min;
        eepr.write(aux, convertBytesToUInt.byte[0]);
        eepr.write(aux+1, convertBytesToUInt.byte[1]);
        convertBytesToUInt.intVal = interval->max;
        eepr.write(aux+2, convertBytesToUInt.byte[0]);
        eepr.write(aux+3, convertBytesToUInt.byte[1]);

        numberOfAddressVerificationIntervals++;
        eepr.write(EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_COUNT_BYTE, numberOfAddressVerificationIntervals);

        return true;
    }
    return false;
}

boolean EEPROMController::getAddressVerificationInterval(uint8_t index, AddressVerificationInterval *interval) {
    if(numberOfAddressVerificationIntervals == 254) {
        numberOfAddressVerificationIntervals = (uint8_t)eepr.read(EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_COUNT_BYTE);
    }
    if(index < numberOfAddressVerificationIntervals) {
        aux = (index * EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_SEGMENT_SIZE) + EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_START;
        convertBytesToUInt.byte[0] = eepr.read(aux);
        convertBytesToUInt.byte[1] = eepr.read(aux+1);
        interval->min = convertBytesToUInt.intVal;
        convertBytesToUInt.byte[0] = eepr.read(aux+2);
        convertBytesToUInt.byte[1] = eepr.read(aux+3);
        interval->max = convertBytesToUInt.intVal;
        return true;
    }
    return false;
}

boolean EEPROMController::clearAddressVerificationIntervals() {
    if(eepr.write(EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_COUNT_BYTE, 0) == 0) {
        numberOfAddressVerificationIntervals = 254;
        return true;
    }
    return false;
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
}

void LEDController::setup() {
    pinMode(HW_STATUS_LED_PIN, OUTPUT);

    set(LED_COLOR_RED, LED_STATUS_OK);
}

void LEDController::assignColor(ledColor *copyTo, ledColor *copyFrom) {
    copyTo->valueRed = copyFrom->valueRed;
    copyTo->animateRed = copyFrom->animateRed;
}

boolean LEDController::isDifferentColor(ledColor cmp1, ledColor cmp2) {
    if(cmp1.valueRed != cmp2.valueRed) return true;
    return false;
}

void LEDController::tick() {
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

void LEDController::setColor(uint8_t  predefinedColor) {
    if(isDifferentColor(currentSetColor, predefinedColors[predefinedColor])) {
        resetCurrentColor();
        assignColor(&currentSetColor, &predefinedColors[predefinedColor]);
        pulseTimer = 0;
    }
}

void LEDController::setAnimation(uint8_t  animation) {
    if(currentAnimation != animation) {
        resetCurrentColor();
        currentAnimation = animation;
        pulseTimer = 0;
    }
}

void LEDController::set(uint8_t  predefinedColor, uint8_t  animation) {
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

MicrochipSRAM::MicrochipSRAM(const uint8_t SSPin, const uint32_t sramType) {
  _SSPin = SSPin;
  SRAMBytes = sramType;
  spiSettings = new SPISettings(26000000, MSBFIRST, SPI_MODE0);

  pinMode(_SSPin, OUTPUT);            
  digitalWrite(_SSPin, HIGH);         
  SPI.begin();                        
  SPI.beginTransaction(*spiSettings); 
  digitalWrite(_SSPin, LOW);          
  SPI.transfer(SRAM_WRITE_MODE_REG);  
  SPI.transfer(SRAM_SEQ_MODE);        
  digitalWrite(_SSPin, HIGH);         
}

MicrochipSRAM::~MicrochipSRAM() {}

void MicrochipSRAM::clearMemory(const uint8_t clearValue)
{
  SPI.beginTransaction(*spiSettings);
  digitalWrite(_SSPin, LOW);     
  SPI.transfer(SRAM_WRITE_CODE); 
  if (SRAMBytes == SRAM_1024)
    SPI.transfer(0x00); 
  SPI.transfer(0x00);   
  SPI.transfer(0x00);   
  for (uint32_t i = 0; i < SRAMBytes; i++)
    SPI.transfer(clearValue); 
  digitalWrite(_SSPin, HIGH); 
  SPI.endTransaction();
}



OutputController::OutputController() {
    for(i=0;i<OUTPUTS_COUNT;i++) {
        outputState[i] = 0;
        outputTimer[i] = 0;
    }
    outputPins[RELAY_AC] = HW_OUTPUT_RELAY_AC_PIN;
    outputPins[RELAY_DC] = HW_OUTPUT_RELAY_DC_PIN;
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



SRAMController::SRAMController(EEPROMController *eepr) {
    this->eepr = eepr;

    numberOfAlarms = 0;            
    numberOfDetectors = 0;
    numberOfOfflineDetectors = 0;
}

uint32_t SRAMController::getSRAMType() {
    return sram->SRAMBytes;
}

void SRAMController::setup() {
    delay(200);  
    sram = new MicrochipSRAM(HW_SPI_CS_SRAM_PIN, SRAM_512);
}

uint16_t SRAMController::getNumberOfAlarms() {
    return numberOfAlarms;
}

uint16_t SRAMController::getNumberOfDetectors() {
    return numberOfDetectors;
}

uint16_t SRAMController::getNumberOfOfflineDetectors() {
    return numberOfOfflineDetectors;
}

int16_t SRAMController::addAlarm(AlarmEntry *alarm, boolean isUpdateOnly) {
    return addAlarm(alarm, isUpdateOnly, false);
}

int16_t SRAMController::addAlarm(AlarmEntry *alarm, boolean isUpdateOnly, boolean isStartup) {
    index = SRAM_ALARMS_MEMORY_START + numberOfAlarms * SRAM_ALARMS_MEMORY_SEGMENT_SIZE;
    if(index + SRAM_ALARMS_MEMORY_SEGMENT_SIZE <= SRAM_ALARMS_MEMORY_END) {
        foundElement = false;
        isStateChange = false;
        for(i=SRAM_ALARMS_MEMORY_START; i<index; i+=SRAM_ALARMS_MEMORY_SEGMENT_SIZE) {
            sram->get(i, comparatorAuxBuffer);
            convertBytesToUInt.intVal = alarm->address;
            if(convertBytesToUInt.byte[0] == comparatorAuxBuffer[0] && convertBytesToUInt.byte[1] == comparatorAuxBuffer[1]) {
                foundElement = true;
                sram->get(i+4, lastState);
                if(alarm->state != lastState) isStateChange = true;
                break;
            }
        }

        convertBytesToUInt.intVal = alarm->address;
        alarmAuxBuffer[0] = convertBytesToUInt.byte[0];
        alarmAuxBuffer[1] = convertBytesToUInt.byte[1];
        convertBytesToUInt.intVal = alarm->id;
        alarmAuxBuffer[2] = convertBytesToUInt.byte[0];
        alarmAuxBuffer[3] = convertBytesToUInt.byte[1];
        alarmAuxBuffer[4] = alarm->state;

        if(!foundElement && !isUpdateOnly) {

            if(!isStartup) eepr->addAlarm(alarm);
            sram->put(index, alarmAuxBuffer);
            numberOfAlarms++;
            return -1;
        } else {

            sram->put(i, alarmAuxBuffer);
            if(isStateChange && alarm->state) {
                return -3;
            } else {
                return (i-SRAM_ALARMS_MEMORY_START)/SRAM_ALARMS_MEMORY_SEGMENT_SIZE;
            }
        }
    }
    return -2;
}

boolean SRAMController::getAlarm(AlarmEntry *alarm, uint16_t elemIndex) {
    if(elemIndex >= 0 && elemIndex < numberOfAlarms) {
        index = SRAM_ALARMS_MEMORY_START + elemIndex * SRAM_ALARMS_MEMORY_SEGMENT_SIZE;
        sram->get(index, alarmAuxBuffer);
        convertBytesToUInt.byte[0] = alarmAuxBuffer[0];
        convertBytesToUInt.byte[1] = alarmAuxBuffer[1];
        alarm->address = convertBytesToUInt.intVal;
        convertBytesToUInt.byte[0] = alarmAuxBuffer[2];
        convertBytesToUInt.byte[1] = alarmAuxBuffer[3];
        alarm->id = convertBytesToUInt.intVal;
        alarm->state = alarmAuxBuffer[4];
        return true;
    }
    return false;
}

int16_t SRAMController::findAlarm(uint16_t address) {
    index = SRAM_ALARMS_MEMORY_START + numberOfAlarms * SRAM_ALARMS_MEMORY_SEGMENT_SIZE;
    if(index + SRAM_ALARMS_MEMORY_SEGMENT_SIZE <= SRAM_ALARMS_MEMORY_END) {
        for(i=SRAM_ALARMS_MEMORY_START; i<index; i+=SRAM_ALARMS_MEMORY_SEGMENT_SIZE) {
            sram->get(i, comparatorAuxBuffer);
            convertBytesToUInt.intVal = address;
            if(convertBytesToUInt.byte[0] == comparatorAuxBuffer[0] && convertBytesToUInt.byte[1] == comparatorAuxBuffer[1]) {
                return (i - SRAM_ALARMS_MEMORY_START) / SRAM_ALARMS_MEMORY_SEGMENT_SIZE;
            }
        }
    }
    return -1;
}

boolean SRAMController::deleteAlarm(uint16_t elemIndex) {
    if(elemIndex >= 0 && numberOfAlarms > 0 && elemIndex < numberOfAlarms) {
        index = SRAM_ALARMS_MEMORY_START + elemIndex * SRAM_ALARMS_MEMORY_SEGMENT_SIZE;
        if(elemIndex+1 != numberOfAlarms) {

            i = SRAM_ALARMS_MEMORY_START + (numberOfAlarms-1) * SRAM_ALARMS_MEMORY_SEGMENT_SIZE;
            sram->get(i, alarmAuxBuffer);
            sram->put(index, alarmAuxBuffer);
        }
        numberOfAlarms--;
        return true;
    }
    return false;
}

boolean SRAMController::clearAlarms() {
    numberOfAlarms = 0;
    return true;
}

int16_t SRAMController::addDetector(DetectorEntry *detector) {
    index = SRAM_DETECTORS_MEMORY_START + numberOfDetectors * SRAM_DETECTORS_MEMORY_SEGMENT_SIZE;
    if(index + SRAM_DETECTORS_MEMORY_SEGMENT_SIZE <= SRAM_DETECTORS_MEMORY_END) {
        foundElement = false;
        for(i=SRAM_DETECTORS_MEMORY_START; i<index; i+=SRAM_DETECTORS_MEMORY_SEGMENT_SIZE) {
            sram->get(i, comparatorAuxBuffer);
            convertBytesToUInt.intVal = detector->address;
            if(convertBytesToUInt.byte[0] == comparatorAuxBuffer[0] && convertBytesToUInt.byte[1] == comparatorAuxBuffer[1]) {
                foundElement = true;
                break;
            }
        }

        convertBytesToUInt.intVal = detector->address;
        detectorAuxBuffer[0] = convertBytesToUInt.byte[0];
        detectorAuxBuffer[1] = convertBytesToUInt.byte[1];
        convertBytesToUInt.intVal = detector->id;
        detectorAuxBuffer[2] = convertBytesToUInt.byte[0];
        detectorAuxBuffer[3] = convertBytesToUInt.byte[1];
        detectorAuxBuffer[4] = detector->linkQuality;
        detectorAuxBuffer[5] = detector->status;
        convertBytesToULong.longVal = detector->lastUpdateMs;
        detectorAuxBuffer[6] = convertBytesToULong.byte[0];
        detectorAuxBuffer[7] = convertBytesToULong.byte[1];
        detectorAuxBuffer[8] = convertBytesToULong.byte[2];
        detectorAuxBuffer[9] = convertBytesToULong.byte[3];

        if(!foundElement) {

            sram->put(index, detectorAuxBuffer);
            numberOfDetectors++;
            return -1;
        } else {

            sram->put(i, detectorAuxBuffer);
            return (i-SRAM_DETECTORS_MEMORY_START)/SRAM_DETECTORS_MEMORY_SEGMENT_SIZE;
        }
    }
    return -2;
}

boolean SRAMController::getDetector(DetectorEntry* detector, uint16_t elemIndex) {
    if(elemIndex >= 0 && elemIndex < numberOfDetectors) {
        index = SRAM_DETECTORS_MEMORY_START + elemIndex * SRAM_DETECTORS_MEMORY_SEGMENT_SIZE;
        sram->get(index, detectorAuxBuffer);
        convertBytesToUInt.byte[0] = detectorAuxBuffer[0];
        convertBytesToUInt.byte[1] = detectorAuxBuffer[1];
        detector->address = convertBytesToUInt.intVal;
        convertBytesToUInt.byte[0] = detectorAuxBuffer[2];
        convertBytesToUInt.byte[1] = detectorAuxBuffer[3];
        detector->id = convertBytesToUInt.intVal;
        detector->linkQuality = detectorAuxBuffer[4];
        detector->status = detectorAuxBuffer[5];
        convertBytesToULong.byte[0] = detectorAuxBuffer[6];
        convertBytesToULong.byte[1] = detectorAuxBuffer[7];
        convertBytesToULong.byte[2] = detectorAuxBuffer[8];
        convertBytesToULong.byte[3] = detectorAuxBuffer[9];
        detector->lastUpdateMs = convertBytesToULong.longVal;
        return true;
    }
    return false;
}

int16_t SRAMController::findDetector(uint16_t address) {
    index = SRAM_DETECTORS_MEMORY_START + numberOfDetectors * SRAM_DETECTORS_MEMORY_SEGMENT_SIZE;
    if(index + SRAM_DETECTORS_MEMORY_SEGMENT_SIZE <= SRAM_DETECTORS_MEMORY_END) {
        for(i=SRAM_DETECTORS_MEMORY_START; i<index; i+=SRAM_DETECTORS_MEMORY_SEGMENT_SIZE) {
            sram->get(i, comparatorAuxBuffer);
            convertBytesToUInt.intVal = address;
            if(convertBytesToUInt.byte[0] == comparatorAuxBuffer[0] && convertBytesToUInt.byte[1] == comparatorAuxBuffer[1]) {
                return (i - SRAM_DETECTORS_MEMORY_START) / SRAM_DETECTORS_MEMORY_SEGMENT_SIZE;
            }
        }
    }
    return -1;
}

boolean SRAMController::deleteDetector(uint16_t elemIndex) {
    if(elemIndex >= 0 && numberOfDetectors > 0 && elemIndex < numberOfDetectors) {
        index = SRAM_DETECTORS_MEMORY_START + elemIndex * SRAM_DETECTORS_MEMORY_SEGMENT_SIZE;
        if(elemIndex+1 != numberOfDetectors) {

            i = SRAM_DETECTORS_MEMORY_START + (numberOfDetectors-1) * SRAM_DETECTORS_MEMORY_SEGMENT_SIZE;
            sram->get(i, detectorAuxBuffer);
            sram->put(index, detectorAuxBuffer);
        }
        numberOfDetectors--;
        return true;
    }
    return false;
}

boolean SRAMController::clearDetectors() {
    numberOfDetectors = 0;
    return true;
}

int16_t SRAMController::addOfflineDetector(OfflineDetectorEntry *offlineDetector) {
    index = SRAM_OFFLINE_DETECTORS_MEMORY_START + numberOfOfflineDetectors * SRAM_OFFLINE_DETECTORS_MEMORY_SEGMENT_SIZE;
    if(index + SRAM_OFFLINE_DETECTORS_MEMORY_SEGMENT_SIZE <= SRAM_OFFLINE_DETECTORS_MEMORY_END) {
        foundElement = false;
        for(i=SRAM_OFFLINE_DETECTORS_MEMORY_START; i<index; i+=SRAM_OFFLINE_DETECTORS_MEMORY_SEGMENT_SIZE) {
            sram->get(i, offlineDetectorAuxBuffer);
            convertBytesToUInt.byte[0] = offlineDetectorAuxBuffer[0];
            convertBytesToUInt.byte[1] = offlineDetectorAuxBuffer[1];
            if(convertBytesToUInt.intVal == offlineDetector->detectorEntryIndex) {
                foundElement = true;
                break;
            }
        }
        if(!foundElement) {

            convertBytesToUInt.intVal = offlineDetector->detectorEntryIndex;
            offlineDetectorAuxBuffer[0] = convertBytesToUInt.byte[0];
            offlineDetectorAuxBuffer[1] = convertBytesToUInt.byte[1];
            sram->put(index, offlineDetectorAuxBuffer);
            numberOfOfflineDetectors++;
            return -1;
        } else {
            return (i-SRAM_OFFLINE_DETECTORS_MEMORY_START)/SRAM_OFFLINE_DETECTORS_MEMORY_SEGMENT_SIZE;
        }
    }
    return -2;
}

boolean SRAMController::getOfflineDetector(OfflineDetectorEntry* offlineDetector, uint16_t elemIndex) {
    if(elemIndex >= 0 && elemIndex < numberOfOfflineDetectors) {
        index = SRAM_OFFLINE_DETECTORS_MEMORY_START + elemIndex * SRAM_OFFLINE_DETECTORS_MEMORY_SEGMENT_SIZE;
        sram->get(index, offlineDetectorAuxBuffer);
        convertBytesToUInt.byte[0] = offlineDetectorAuxBuffer[0];
        convertBytesToUInt.byte[1] = offlineDetectorAuxBuffer[1];
        offlineDetector->detectorEntryIndex = convertBytesToUInt.intVal;
        return true;
    }
    return false;
}

int16_t SRAMController::findOfflineDetector(uint16_t detectorEntryIndex) {
    index = SRAM_OFFLINE_DETECTORS_MEMORY_START + numberOfOfflineDetectors * SRAM_OFFLINE_DETECTORS_MEMORY_SEGMENT_SIZE;
    if(index + SRAM_OFFLINE_DETECTORS_MEMORY_SEGMENT_SIZE <= SRAM_OFFLINE_DETECTORS_MEMORY_END) {
        for(i=SRAM_OFFLINE_DETECTORS_MEMORY_START; i<index; i+=SRAM_OFFLINE_DETECTORS_MEMORY_SEGMENT_SIZE) {
            sram->get(i, offlineDetectorAuxBuffer);
            convertBytesToUInt.byte[0] = offlineDetectorAuxBuffer[0];
            convertBytesToUInt.byte[1] = offlineDetectorAuxBuffer[1];
            if(convertBytesToUInt.intVal == detectorEntryIndex) {
                return (i - SRAM_OFFLINE_DETECTORS_MEMORY_START) / SRAM_OFFLINE_DETECTORS_MEMORY_SEGMENT_SIZE;
            }
        }
    }
    return -1;
}

boolean SRAMController::deleteOfflineDetector(uint16_t elemIndex) {
    if(elemIndex >= 0 && numberOfOfflineDetectors > 0 && elemIndex < numberOfOfflineDetectors) {
        index = SRAM_OFFLINE_DETECTORS_MEMORY_START + elemIndex * SRAM_OFFLINE_DETECTORS_MEMORY_SEGMENT_SIZE;
        if(elemIndex+1 != numberOfOfflineDetectors) {

            i = SRAM_OFFLINE_DETECTORS_MEMORY_START + (numberOfOfflineDetectors-1) * SRAM_OFFLINE_DETECTORS_MEMORY_SEGMENT_SIZE;
            sram->get(i, offlineDetectorAuxBuffer);
            sram->put(index, offlineDetectorAuxBuffer);
        }
        numberOfOfflineDetectors--;
        return true;
    }
    return false;
}

boolean SRAMController::clearOfflineDetectors() {
    numberOfOfflineDetectors = 0;
    return true;
}



CommunicationService::CommunicationService(CC1101Controller *radio, SerialDebug *debug, EEPROMController *eepromController, LEDController *ledController) {
    this->radio = radio;
    this->debug = debug;
    this->eepromController = eepromController;
    this->ledController = ledController;
    this->radioCommunicationStatus = 0;

    indexRssiLqi = 0;
}

void CommunicationService::setup() {
    reloadEepromValues();

    delay(500); 

    testConnection();
}

void CommunicationService::reloadEepromValues() {

}

void CommunicationService::send(const uint8_t *buff, uint8_t *len) {

    packet.length = *len;
    for(i=0; i<packet.length; i++) {
        packet.data[i] = buff[i];
    }
    bool resp = radio->sendData(packet);
    if(debug->getDebugEnabled()) {
        debug->print(F("# RS["));
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
        debug->print(F("# RR "));

        lastLqi[indexRssiLqi] = radio->computeLqi(packet.lqi);
        lastRssi[indexRssiLqi] = radio->computeRssi(packet.rssi);
        previousIndexRssiLqi = indexRssiLqi;
        if(indexRssiLqi < RSSI_LQI_SAMPLES-1) indexRssiLqi++;
        else indexRssiLqi = 0;

        if (!packet.crc_ok && debug->getDebugEnabled()) {
            debug->print(F("CRC FAIL: "));
            for(i=0; i<packet.length; i++) {
                debug->print(packet.data[i]);
                debug->print(" ");
            }
            debug->println();
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
            debug->println(F("# RCI!"));
            radioCommunicationStatus = 1;
        } else {
            debug->println(F("# RCNI!"));
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



CoreService::CoreService(OutputController *outputController, EEPROMController *eepromController, SRAMController *sramController, 
            SerialDebug *debug, CommunicationService *communicationService) {
    this->outputController = outputController;
    this->eepromController = eepromController;
    this->sramController = sramController;
    this->debug = debug;
    this->communicationService = communicationService;

    numberOfAddressVerificationIntervals = 0;
}

void CoreService::setup() {
    reloadEepromValues();
    loadAddressVerificationIntervals();
}

void CoreService::tick(uint32_t currentMillis) {
    switch(operationMode) {
        case OPERATION_MODE_MASTER:
            operationModeMaster(currentMillis);
        break;
        case OPERATION_MODE_SLAVE:
            operationModeSlave(currentMillis);
        break;
    }
}

void CoreService::checkOfflineDetectors(uint32_t currentMillis) {
    for(i=0; i<sramController->getNumberOfDetectors(); i++) {
        if(sramController->getDetector(&detectorEntry, i)) {
            if(currentMillis - detectorEntry.lastUpdateMs >= DETECTOR_OFFLINE_MS) {
                detectorEntry.status = 0;
                sramController->addDetector(&detectorEntry);
                offlineDetectorEntry.detectorEntryIndex = i;
                sramController->addOfflineDetector(&offlineDetectorEntry);
            }
        }
    }
}

void CoreService::operationModeMaster(uint32_t currentMillis) {
    if(*receiveBufferLen > 0) {
        index = 0;

        while(index < *receiveBufferLen && *receiveBufferLen > 7) {

            convertBytesToUInt.intVal = deviceAddress;
            if(receiveBuffer[index + BUFF_INDEX_RECEIVER_LO] == convertBytesToUInt.byte[0] && receiveBuffer[index + BUFF_INDEX_RECEIVER_HI] == convertBytesToUInt.byte[1]) {

                if(numberOfAddressVerificationIntervals > 0) {
                    aux = 0;
                    convertBytesToUInt.byte[0] = receiveBuffer[index + BUFF_INDEX_ADDRESS_LO];
                    convertBytesToUInt.byte[1] = receiveBuffer[index + BUFF_INDEX_ADDRESS_HI];
                    for(j=0;j<numberOfAddressVerificationIntervals;j++) {
                        if(addressVerificationIntervals[j].min <= convertBytesToUInt.intVal && convertBytesToUInt.intVal <= addressVerificationIntervals[j].max) {
                            aux = 1;
                            break;
                        }
                    }
                    if(aux == 0) {
                        goto SKIP_PROCESSING;
                    }
                }

                switch(receiveBuffer[index + BUFF_INDEX_COMMAND]) {
                    case(COMMAND_ID_TEST_RSSI):
                        sendRssiMessageFromReceive(index);
                    break;
                    case(COMMAND_ID_ALARM_START):
                        addReceiveToDetectorsList(currentMillis, index);
                        aux = addReceiveToAlarmsList(index, true);
                        if(aux == -1 || aux == -3) {

                            outputController->setPulsed(RELAY_AC, true, OUTPUTS_PULSE_COUNT);
                            outputController->setPulsed(RELAY_DC, true, OUTPUTS_PULSE_COUNT);
                        }
                    break;
                    case(COMMAND_ID_ALARM_STOP):
                        addReceiveToDetectorsList(currentMillis, index);
                        addReceiveToAlarmsList(index, false);

                    break;
                    default:
                        debug->print(F("# UCI: "));
                        debug->println(receiveBuffer[index + BUFF_INDEX_COMMAND]);
                    break;
                }

                doSend();
            }
            SKIP_PROCESSING:
            index += 8;
        }
        *receiveBufferLen = 0;
    }
}

void CoreService::operationModeSlave(uint32_t currentMillis) {
    if(*receiveBufferLen > 0) {
        index = 0;

        while(index < *receiveBufferLen && *receiveBufferLen > 7) {

            convertBytesToUInt.intVal = deviceAddress;
            if(receiveBuffer[index + BUFF_INDEX_RECEIVER_LO] == convertBytesToUInt.byte[0] && receiveBuffer[index + BUFF_INDEX_RECEIVER_HI] == convertBytesToUInt.byte[1]) {

                switch(receiveBuffer[index + BUFF_INDEX_COMMAND]) {
                    case(COMMAND_ID_TEST_RSSI):
                        sendRssiMessageFromReceive(index);
                    break;
                    case(COMMAND_ID_ALARM_START):
                        addReceiveToDetectorsList(currentMillis, index);
                        forwardReceiveMessage(index);
                    break;
                    case(COMMAND_ID_ALARM_STOP):
                        addReceiveToDetectorsList(currentMillis, index);
                        forwardReceiveMessage(index);
                    break;
                    default:
                        debug->print(F("# UCI: "));
                        debug->println(receiveBuffer[index + BUFF_INDEX_COMMAND]);
                    break;
                }

                doSend();
            }
            index += 8;
        }
        *receiveBufferLen = 0;
    }
}

void CoreService::sendRssiMessageFromReceive(uint16_t index) {

    sendBuffer[BUFF_INDEX_RECEIVER_LO] = receiveBuffer[index + BUFF_INDEX_ADDRESS_LO];
    sendBuffer[BUFF_INDEX_RECEIVER_HI] = receiveBuffer[index + BUFF_INDEX_ADDRESS_HI];
    convertBytesToUInt.intVal = deviceAddress;
    sendBuffer[BUFF_INDEX_ADDRESS_LO] = convertBytesToUInt.byte[0];
    sendBuffer[BUFF_INDEX_ADDRESS_HI] = convertBytesToUInt.byte[1];
    sendBuffer[BUFF_INDEX_ID_LO] = 0;
    sendBuffer[BUFF_INDEX_ID_HI] = 0;
    sendBuffer[BUFF_INDEX_COMMAND] = 1;
    *sendBufferLen = 7;
}

void CoreService::addReceiveToDetectorsList(uint32_t currentMillis, uint16_t index) {

    convertBytesToUInt.byte[0] = receiveBuffer[index + BUFF_INDEX_ADDRESS_LO];
    convertBytesToUInt.byte[1] = receiveBuffer[index + BUFF_INDEX_ADDRESS_HI];
    detectorEntry.address = convertBytesToUInt.intVal;
    convertBytesToUInt.byte[0] = receiveBuffer[index + BUFF_INDEX_ID_LO];
    convertBytesToUInt.byte[1] = receiveBuffer[index + BUFF_INDEX_ID_HI];
    detectorEntry.id = convertBytesToUInt.intVal;
    detectorEntry.linkQuality = (uint8_t)communicationService->getLastMessageComputedLinkQuality();
    detectorEntry.status = 1;
    detectorEntry.lastUpdateMs = currentMillis;

    if(operationMode == OPERATION_MODE_MASTER) {
        i = sramController->addDetector(&detectorEntry);
        if(i >= 0) {

            j = sramController->findOfflineDetector(i);
            if(j >= 0) {
                sramController->deleteOfflineDetector(j);
            }
        }
    } else {
        if(receiveBuffer[index + BUFF_INDEX_IS_FORWARD] != MESSAGE_IS_FORWARDED) {
            i = sramController->addDetector(&detectorEntry);
            if(i >= 0) {

                j = sramController->findOfflineDetector(i);
                if(j >= 0) {
                    sramController->deleteOfflineDetector(j);
                }
            }
        }
    }

}

int16_t CoreService::addReceiveToAlarmsList(uint16_t index, boolean state) {

    convertBytesToUInt.byte[0] = receiveBuffer[index + BUFF_INDEX_ADDRESS_LO];
    convertBytesToUInt.byte[1] = receiveBuffer[index + BUFF_INDEX_ADDRESS_HI];
    alarmEntry.address = convertBytesToUInt.intVal;
    convertBytesToUInt.byte[0] = receiveBuffer[index + BUFF_INDEX_ID_LO];
    convertBytesToUInt.byte[1] = receiveBuffer[index + BUFF_INDEX_ID_HI];
    alarmEntry.id = convertBytesToUInt.intVal;
    alarmEntry.state = state;
    if(state) {
        return sramController->addAlarm(&alarmEntry, false);
    } else {
        return sramController->addAlarm(&alarmEntry, true);
    }
}

void CoreService::removeReceiveFromAlarmsList(uint16_t index) {

    convertBytesToUInt.byte[0] = receiveBuffer[index + BUFF_INDEX_ADDRESS_LO];
    convertBytesToUInt.byte[1] = receiveBuffer[index + BUFF_INDEX_ADDRESS_HI];
    eepromController->deleteAlarm(convertBytesToUInt.intVal);
    sramController->deleteAlarm(sramController->findAlarm(convertBytesToUInt.intVal));
}

void CoreService::forwardReceiveMessage(uint16_t index) {
    convertBytesToUInt.intVal = forwardAddress;
    sendBuffer[BUFF_INDEX_RECEIVER_LO] = convertBytesToUInt.byte[0];
    sendBuffer[BUFF_INDEX_RECEIVER_HI] = convertBytesToUInt.byte[1];
    sendBuffer[BUFF_INDEX_ADDRESS_LO] = receiveBuffer[index + BUFF_INDEX_ADDRESS_LO];
    sendBuffer[BUFF_INDEX_ADDRESS_HI] = receiveBuffer[index + BUFF_INDEX_ADDRESS_HI];
    sendBuffer[BUFF_INDEX_ID_LO] = receiveBuffer[index + BUFF_INDEX_ID_LO];
    sendBuffer[BUFF_INDEX_ID_HI] = receiveBuffer[index + BUFF_INDEX_ID_HI];
    sendBuffer[BUFF_INDEX_COMMAND] = receiveBuffer[index + BUFF_INDEX_COMMAND];
    sendBuffer[BUFF_INDEX_IS_FORWARD] = MESSAGE_IS_FORWARDED;

    *sendBufferLen = 8;
}

void CoreService::setReceiveBuffer(uint8_t *receiveBuffer, uint8_t *receiveBufferLen) {
    this->receiveBuffer = receiveBuffer;
    this->receiveBufferLen = receiveBufferLen;
}

void CoreService::setSendBuffer(uint8_t *sendBuffer, uint8_t *sendBufferLen) {
    this->sendBuffer = sendBuffer;
    this->sendBufferLen = sendBufferLen;
}

void CoreService::reloadEepromValues() {
    operationMode = eepromController->getByteValue(EEPROM_CONFIG_OPERATION_MODE_BYTE);
    deviceAddress = eepromController->getUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT);
    deviceChannel = eepromController->getByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE);
    forwardAddress = eepromController->getUIntValue(EEPROM_CC1101_FORWARD_ADDRESS_INT);
}

void CoreService::doSend() {
    if(*sendBufferLen > 0) {
        communicationService->send(sendBuffer, sendBufferLen);
        sendBufferLen = 0;
    }
}

void CoreService::loadAddressVerificationIntervals() {
    numberOfAddressVerificationIntervals = eepromController->getNumberOfAddressVerificationIntervals();
    for(i=0;i<numberOfAddressVerificationIntervals;i++) {
        eepromController->getAddressVerificationInterval(i, &addressVerificationIntervals[i]);
    }
}

boolean CoreService::addAddressVerificationInterval(uint16_t min, uint16_t max) {
    if(numberOfAddressVerificationIntervals < 10) {
        addressVerificationIntervals[numberOfAddressVerificationIntervals].min = min;
        addressVerificationIntervals[numberOfAddressVerificationIntervals].max = max;
        if(eepromController->addAddressVerificationInterval(&addressVerificationIntervals[numberOfAddressVerificationIntervals])) {
            numberOfAddressVerificationIntervals++;
            return true;
        } else {
            return false;
        }
    }
    return false;
}

boolean CoreService::clearAddressVerificationIntervals() {
    if(eepromController->clearAddressVerificationIntervals()) {
        numberOfAddressVerificationIntervals = 0;
        return true;
    }
    return false;
}



#include <Configs/MainConfig.h>

MenuService::MenuService(ButtonController *buttonController, EEPROMController *eepromController, SRAMController *sramController) {
    this->buttonController = buttonController;
    this->eepromController = eepromController;
    this->sramController = sramController;

    this->lcd = new LiquidCrystal_I2C(LCD_ADDRESS, LCD_NUMBER_OF_CHARS, LCD_NUMBER_OF_ROWS);
}

void MenuService::setup() {
    lcd->begin();
    lcd->backlight();
    lcd->noBlink();

    setupMenuSystem();

    lastPageId = PAGE_INDEX_HOME;
    currentPageId = PAGE_INDEX_HOME;
    lastButtonClick = 0;
    lastValueRefresh = 0;

    homeAlarmIndex = 0;
    alarmIndex = 0;
    detectorIndex = 0;
    offlineDetectorIndex = 0;

    loadPage(PAGE_INDEX_HOME);
}

void MenuService::tick(uint32_t currentTimeMs) {

    for(i=BTN_BACK; i<=BTN_OK; i++) {
        if(buttonController->hasStateChanged(i)) {
            if(buttonController->getClickedState(i)) {
                onButtonAction(i);
                lastButtonClick = currentTimeMs;
                lastValueRefresh = currentTimeMs;
            }
        }
    }

    if(sramController->getNumberOfAlarms() > 0 && !lcd->getBacklight()) {
        lcd->backlight();
    }

    if(sramController->getNumberOfAlarms() == 0 && currentTimeMs - lastButtonClick >= MENU_BACKLIGHT_OFF_MS) {
        lcd->noBacklight();
    }

    if(currentPageId != PAGE_INDEX_HOME && currentTimeMs - lastButtonClick >= MENU_TIMEOUT_MS) {
        loadPage(PAGE_INDEX_HOME);
        lastValueRefresh = currentTimeMs;
    }

    if(menu[currentPageId].valueRefreshEnabled && currentTimeMs - lastValueRefresh >= MENU_VALUE_REFRESH_MS) {
        refreshDisplayedValues();
        lastValueRefresh = currentTimeMs;
    }
}

void MenuService::onButtonAction(uint8_t button) {
    if(!lcd->getBacklight()) {

        lcd->backlight();
    } else {
        switch(button) {
            case BTN_BACK:
                if(menu[currentPageId].back != currentPageId) {
                    loadPage(menu[currentPageId].back);
                }
            break;
            case BTN_LEFT:
                if(menu[currentPageId].left == menu[currentPageId].right) {

                    if(currentPageId == PAGE_INDEX_LIST_ALARMS) {
                        if(alarmIndex > 0) {
                            alarmIndex--;
                            loadPage(currentPageId);
                        }
                    } else if(currentPageId == PAGE_INDEX_LIST_DETECTORS) {
                        if(detectorIndex > 0) {
                            detectorIndex--;
                            loadPage(currentPageId);
                        }
                    } else if(currentPageId == PAGE_INDEX_LIST_DETECTORS_OFFLINE) {
                        if(offlineDetectorIndex > 0) {
                            offlineDetectorIndex--;
                            loadPage(currentPageId);
                        }
                    }
                } else if(menu[currentPageId].left != currentPageId) {
                    loadPage(menu[currentPageId].left);
                }
            break;
            case BTN_RIGHT:
                if(menu[currentPageId].left == menu[currentPageId].right) {

                    if(currentPageId == PAGE_INDEX_LIST_ALARMS) {
                        if(sramController->getNumberOfAlarms() > 0 && alarmIndex < sramController->getNumberOfAlarms()-1) {
                            alarmIndex++;
                            loadPage(currentPageId);
                        }
                    } else if(currentPageId == PAGE_INDEX_LIST_DETECTORS) {
                        if(sramController->getNumberOfDetectors() > 0 && detectorIndex < sramController->getNumberOfDetectors()-1) {
                            detectorIndex++;
                            loadPage(currentPageId);
                        }
                    } else if(currentPageId == PAGE_INDEX_LIST_DETECTORS_OFFLINE) {
                        if(sramController->getNumberOfOfflineDetectors() > 0 && offlineDetectorIndex < sramController->getNumberOfOfflineDetectors()-1) {
                            offlineDetectorIndex++;
                            loadPage(currentPageId);
                        }
                    }
                } else if(menu[currentPageId].right != currentPageId) {
                    loadPage(menu[currentPageId].right);
                }
            break;
            case BTN_OK:
                if(currentPageId == PAGE_INDEX_CLEAR_ALARMS) {

                    sramController->clearAlarms();
                    eepromController->clearAlarms();
                    homeAlarmIndex = 0;
                }
                if(currentPageId == PAGE_INDEX_CLEAR_DETECTORS) {

                    sramController->clearDetectors();
                    sramController->clearOfflineDetectors();
                }
                if((sramController->getNumberOfAlarms() == 0 && menu[currentPageId].ok == PAGE_INDEX_LIST_ALARMS) ||
                    (sramController->getNumberOfDetectors() == 0 && menu[currentPageId].ok == PAGE_INDEX_LIST_DETECTORS) ||
                    (sramController->getNumberOfOfflineDetectors() == 0 && menu[currentPageId].ok == PAGE_INDEX_LIST_DETECTORS_OFFLINE)) {

                } else if(currentPageId == PAGE_INDEX_HOME) {

                    i = sramController->getNumberOfAlarms();
                    if(i > 0) {
                        alarmIndex = 0;
                        loadPage(PAGE_INDEX_LIST_ALARMS);
                    }
                } else if(menu[currentPageId].ok != currentPageId) {
                    switch(menu[currentPageId].ok) {
                        case PAGE_INDEX_LIST_ALARMS:
                            alarmIndex = 0;
                        break;
                        case PAGE_INDEX_LIST_DETECTORS:
                            detectorIndex = 0;
                        break;
                        case PAGE_INDEX_LIST_DETECTORS_OFFLINE:
                            offlineDetectorIndex = 0;
                        break;
                    }
                    loadPage(menu[currentPageId].ok);
                }
            break;
        }
    }
}

void MenuService::loadPage(uint8_t id) {
    lcd->clear();
    if(menu[id].isLine1Dynamic) {
        setLine1DynamicText(id);
    } else {
        lcd->print(menu[id].textLine1);
    }
    lcd->setCursor(0,1);
    if(menu[id].isLine2Dynamic) {
        setLine2DynamicText(id);
    } else {
        lcd->print(menu[id].textLine2);
    }
    lastPageId = currentPageId;
    currentPageId = id;
}

void MenuService::refreshDisplayedValues() {
    if(menu[currentPageId].isLine1Dynamic) {
        lcd->setCursor(0,0);
        charsPrinted = 0;
        setLine1DynamicText(currentPageId);
        printClearPadding();
    }
    if(menu[currentPageId].isLine2Dynamic) {
        lcd->setCursor(0,1);
        charsPrinted = 0;
        setLine2DynamicText(currentPageId);
        printClearPadding();
    }
}

void MenuService::setLine1DynamicText(uint8_t id) {
    switch(id) {
        case PAGE_INDEX_HOME:
            charsPrinted += lcd->print(MENU_TEXT_ENERGO_INSTAL);
            charsPrinted += lcd->print(MENU_TEXT_VERSION);
            charsPrinted += lcd->print(APP_VERSION/10);
            charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_DOT);
            charsPrinted += lcd->print(APP_VERSION%10);
        break;
        case PAGE_INDEX_LIST_ALARMS:

            charsPrinted += lcd->print(MENU_TEXT_ALARM);
            charsPrinted += lcd->print(alarmIndex+1);
            charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_TWO_POINTS);
            charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_SPACE);
            if(sramController->getAlarm(&alarmEntry, alarmIndex)) {
                charsPrinted += lcd->print(alarmEntry.id);
            } else {
                charsPrinted += lcd->print(MENU_TEXT_NA);
            }
        break;
        case PAGE_INDEX_LIST_DETECTORS:

            if(sramController->getDetector(&detectorEntry, detectorIndex)) {
                charsPrinted += lcd->print(MENU_TEXT_DETECTOR);
                charsPrinted += lcd->print(detectorEntry.id);
                charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_TWO_POINTS);
            } else {
                charsPrinted += lcd->print(MENU_TEXT_DETECTOR);
                charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_TWO_POINTS);
                charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_SPACE);
                charsPrinted += lcd->print(MENU_TEXT_NA);
            }
        break;
        case PAGE_INDEX_LIST_DETECTORS_OFFLINE:

            if(sramController->getOfflineDetector(&offlineDetectorEntry, offlineDetectorIndex) &&
                sramController->getDetector(&detectorEntry, offlineDetectorEntry.detectorEntryIndex)) {
                charsPrinted += lcd->print(MENU_TEXT_DETECTOR);
                charsPrinted += lcd->print(detectorEntry.id);
                charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_TWO_POINTS);
            } else {
                charsPrinted += lcd->print(MENU_TEXT_DETECTOR);
                charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_TWO_POINTS);
                charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_SPACE);
                charsPrinted += lcd->print(MENU_TEXT_NA);
            }
        break;
        case PAGE_INDEX_FIRMWARE:

            charsPrinted += lcd->print(MENU_TEXT_FIRMWARE);
            charsPrinted += lcd->print(MENU_TEXT_VERSION);
            charsPrinted += lcd->print(APP_VERSION/10);
            charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_DOT);
            charsPrinted += lcd->print(APP_VERSION%10);
        break;
        default:
            charsPrinted += lcd->print(MENU_TEXT_NOT_IMPLEMENTED);
        break;
    }
}

void MenuService::setLine2DynamicText(uint8_t id) {
    switch(id) {
        case PAGE_INDEX_HOME:

            i = sramController->getNumberOfAlarms();
            if(i == 0) {
                charsPrinted += lcd->print(MENU_TEXT_EVERYTHING_OK);
            } else {
                charsPrinted += lcd->print(MENU_TEXT_ALARM);
                charsPrinted += lcd->print(homeAlarmIndex+1);
                charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_TWO_POINTS);
                charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_SPACE);
                if(sramController->getAlarm(&alarmEntry, homeAlarmIndex)) {
                    charsPrinted += lcd->print(alarmEntry.id);
                } else {
                    charsPrinted += lcd->print(MENU_TEXT_NA);
                }
                homeAlarmIndex++;
                if(homeAlarmIndex >= i) homeAlarmIndex = 0;
            }
        break;
        case PAGE_INDEX_ALARMS:

            if(sramController->getNumberOfAlarms() == 0) {
                charsPrinted += lcd->print(MENU_TEXT_NO_ALARMS);
            } else {
                charsPrinted += lcd->print(sramController->getNumberOfAlarms());
                charsPrinted += lcd->print(MENU_TEXT_ALARM_OR_ALARMS);
            }
        break;
        case PAGE_INDEX_DETECTORS:

            charsPrinted += lcd->print(MENU_TEXT_DETECTORS);
            charsPrinted += lcd->print(sramController->getNumberOfDetectors());
        break;
        case PAGE_INDEX_DETECTORS_OFFLINE:

            charsPrinted += lcd->print(MENU_TEXT_DETECTORS);
            charsPrinted += lcd->print(sramController->getNumberOfOfflineDetectors());
        break;
        case PAGE_INDEX_LIST_ALARMS:

            if(sramController->getAlarm(&alarmEntry, alarmIndex)) {
                charsPrinted += lcd->print(alarmEntry.address);
                charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_DASH_SPACED);
                if(alarmEntry.state) {
                    charsPrinted += lcd->print(MENU_TEXT_ON);
                } else {
                    charsPrinted += lcd->print(MENU_TEXT_OFF);
                }
            }
        break;
        case PAGE_INDEX_LIST_DETECTORS:

            if(sramController->getDetector(&detectorEntry, detectorIndex)) {
                if(detectorEntry.status == 1) {
                charsPrinted += lcd->print(MENU_TEXT_ONLINE);
                } else {
                    charsPrinted += lcd->print(MENU_TEXT_OFFLINE);
                }
                charsPrinted += lcd->print(MENU_TEXT_LQI_OPEN);
                charsPrinted += lcd->print(detectorEntry.linkQuality);
                charsPrinted += lcd->print(MENU_TEXT_LQI_CLOSED);
            }
        break;
        case PAGE_INDEX_LIST_DETECTORS_OFFLINE:

            if(sramController->getOfflineDetector(&offlineDetectorEntry, offlineDetectorIndex) &&
                sramController->getDetector(&detectorEntry, offlineDetectorEntry.detectorEntryIndex)) {
                printTimeFromMs(millis()-detectorEntry.lastUpdateMs);
            }
        break;
        default:
            charsPrinted += lcd->print(MENU_TEXT_NOT_IMPLEMENTED);
        break;
    }
}

void MenuService::printClearPadding() {
    for(i=charsPrinted; i<LCD_NUMBER_OF_CHARS; i++) {
        lcd->print(MENU_TEXT_SEPARATOR_SPACE);
    }
}

void MenuService::printTimeFromMs(uint32_t timeSinceLastMessageMs) {
    if(timeSinceLastMessageMs > 3600000) {

        charsPrinted += lcd->print(timeSinceLastMessageMs / 3600000);
        charsPrinted += lcd->print(MENU_TEXT_HRS);
        charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_SPACE);
        charsPrinted += lcd->print((timeSinceLastMessageMs % 3600000) / 60000);
        charsPrinted += lcd->print(MENU_TEXT_MIN);
    } else if(timeSinceLastMessageMs > 60000) {

        charsPrinted += lcd->print(timeSinceLastMessageMs / 60000);
        charsPrinted += lcd->print(MENU_TEXT_MIN);
        charsPrinted += lcd->print(MENU_TEXT_SEPARATOR_SPACE);
        charsPrinted += lcd->print((timeSinceLastMessageMs % 60000) / 1000);
        charsPrinted += lcd->print(MENU_TEXT_SEC);
    } else if(timeSinceLastMessageMs > 1000) {
        charsPrinted += lcd->print(timeSinceLastMessageMs / 1000);
        charsPrinted += lcd->print(MENU_TEXT_SEC);
    } else {
        charsPrinted += lcd->print(MENU_TEXT_NA);
    }
}

void MenuService::setupMenuSystem() {
    menu[PAGE_INDEX_HOME].isLine1Dynamic = true;
    menu[PAGE_INDEX_HOME].isLine2Dynamic = true;
    menu[PAGE_INDEX_HOME].back = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_HOME].left = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_HOME].right = PAGE_INDEX_ALARMS;
    menu[PAGE_INDEX_HOME].ok = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_HOME].valueRefreshEnabled = true;

    menu[PAGE_INDEX_ALARMS].textLine1 = MENU_TEXT_DETECTOR_ALARMS;
    menu[PAGE_INDEX_ALARMS].isLine1Dynamic = false;
    menu[PAGE_INDEX_ALARMS].isLine2Dynamic = true;
    menu[PAGE_INDEX_ALARMS].back = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_ALARMS].left = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_ALARMS].right = PAGE_INDEX_DETECTORS;
    menu[PAGE_INDEX_ALARMS].ok = PAGE_INDEX_LIST_ALARMS;
    menu[PAGE_INDEX_ALARMS].valueRefreshEnabled = true;

    menu[PAGE_INDEX_DETECTORS].textLine1 = MENU_TEXT_CONNECTED;
    menu[PAGE_INDEX_DETECTORS].isLine1Dynamic = false;
    menu[PAGE_INDEX_DETECTORS].isLine2Dynamic = true;
    menu[PAGE_INDEX_DETECTORS].back = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_DETECTORS].left = PAGE_INDEX_ALARMS;
    menu[PAGE_INDEX_DETECTORS].right = PAGE_INDEX_DETECTORS_OFFLINE;
    menu[PAGE_INDEX_DETECTORS].ok = PAGE_INDEX_LIST_DETECTORS;
    menu[PAGE_INDEX_DETECTORS].valueRefreshEnabled = true;

    menu[PAGE_INDEX_DETECTORS_OFFLINE].textLine1 = MENU_TEXT_OFFLINE;
    menu[PAGE_INDEX_DETECTORS_OFFLINE].isLine1Dynamic = false;
    menu[PAGE_INDEX_DETECTORS_OFFLINE].isLine2Dynamic = true;
    menu[PAGE_INDEX_DETECTORS_OFFLINE].back = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_DETECTORS_OFFLINE].left = PAGE_INDEX_DETECTORS;
    menu[PAGE_INDEX_DETECTORS_OFFLINE].right = PAGE_INDEX_FIRMWARE;
    menu[PAGE_INDEX_DETECTORS_OFFLINE].ok = PAGE_INDEX_LIST_DETECTORS_OFFLINE;
    menu[PAGE_INDEX_DETECTORS_OFFLINE].valueRefreshEnabled = true;

    menu[PAGE_INDEX_FIRMWARE].textLine2 = MENU_TEXT_SEPARATOR_SPACE;
    menu[PAGE_INDEX_FIRMWARE].isLine1Dynamic = true;
    menu[PAGE_INDEX_FIRMWARE].isLine2Dynamic = false;
    menu[PAGE_INDEX_FIRMWARE].back = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_FIRMWARE].left = PAGE_INDEX_DETECTORS_OFFLINE;
    menu[PAGE_INDEX_FIRMWARE].right = PAGE_INDEX_FIRMWARE;
    menu[PAGE_INDEX_FIRMWARE].ok = PAGE_INDEX_HOME;
    menu[PAGE_INDEX_FIRMWARE].valueRefreshEnabled = false;

    menu[PAGE_INDEX_LIST_ALARMS].isLine1Dynamic = true;
    menu[PAGE_INDEX_LIST_ALARMS].isLine2Dynamic = true;
    menu[PAGE_INDEX_LIST_ALARMS].back = PAGE_INDEX_ALARMS;
    menu[PAGE_INDEX_LIST_ALARMS].left = PAGE_INDEX_LIST_ALARMS;
    menu[PAGE_INDEX_LIST_ALARMS].right = PAGE_INDEX_LIST_ALARMS;
    menu[PAGE_INDEX_LIST_ALARMS].ok = PAGE_INDEX_CLEAR_ALARMS;
    menu[PAGE_INDEX_LIST_ALARMS].valueRefreshEnabled = true;

    menu[PAGE_INDEX_LIST_DETECTORS].isLine1Dynamic = true;
    menu[PAGE_INDEX_LIST_DETECTORS].isLine2Dynamic = true;
    menu[PAGE_INDEX_LIST_DETECTORS].back = PAGE_INDEX_DETECTORS;
    menu[PAGE_INDEX_LIST_DETECTORS].left = PAGE_INDEX_LIST_DETECTORS;
    menu[PAGE_INDEX_LIST_DETECTORS].right = PAGE_INDEX_LIST_DETECTORS;
    menu[PAGE_INDEX_LIST_DETECTORS].ok = PAGE_INDEX_CLEAR_DETECTORS;
    menu[PAGE_INDEX_LIST_DETECTORS].valueRefreshEnabled = true;

    menu[PAGE_INDEX_LIST_DETECTORS_OFFLINE].isLine1Dynamic = true;
    menu[PAGE_INDEX_LIST_DETECTORS_OFFLINE].isLine2Dynamic = true;
    menu[PAGE_INDEX_LIST_DETECTORS_OFFLINE].back = PAGE_INDEX_DETECTORS_OFFLINE;
    menu[PAGE_INDEX_LIST_DETECTORS_OFFLINE].left = PAGE_INDEX_LIST_DETECTORS_OFFLINE;
    menu[PAGE_INDEX_LIST_DETECTORS_OFFLINE].right = PAGE_INDEX_LIST_DETECTORS_OFFLINE;
    menu[PAGE_INDEX_LIST_DETECTORS_OFFLINE].ok = PAGE_INDEX_LIST_DETECTORS_OFFLINE;
    menu[PAGE_INDEX_LIST_DETECTORS_OFFLINE].valueRefreshEnabled = true;

    menu[PAGE_INDEX_CLEAR_ALARMS].textLine1 = MENU_TEXT_CLEAR_ALARMS;
    menu[PAGE_INDEX_CLEAR_ALARMS].textLine2 = MENU_TEXT_NO_YES;
    menu[PAGE_INDEX_CLEAR_ALARMS].isLine1Dynamic = false;
    menu[PAGE_INDEX_CLEAR_ALARMS].isLine2Dynamic = false;
    menu[PAGE_INDEX_CLEAR_ALARMS].back = PAGE_INDEX_LIST_ALARMS;
    menu[PAGE_INDEX_CLEAR_ALARMS].left = PAGE_INDEX_CLEAR_ALARMS;
    menu[PAGE_INDEX_CLEAR_ALARMS].right = PAGE_INDEX_CLEAR_ALARMS;
    menu[PAGE_INDEX_CLEAR_ALARMS].ok = PAGE_INDEX_ALARMS;
    menu[PAGE_INDEX_CLEAR_ALARMS].valueRefreshEnabled = false;

    menu[PAGE_INDEX_CLEAR_DETECTORS].textLine1 = MENU_TEXT_CLEAR_DETECTORS;
    menu[PAGE_INDEX_CLEAR_DETECTORS].textLine2 = MENU_TEXT_NO_YES;
    menu[PAGE_INDEX_CLEAR_DETECTORS].isLine1Dynamic = false;
    menu[PAGE_INDEX_CLEAR_DETECTORS].isLine2Dynamic = false;
    menu[PAGE_INDEX_CLEAR_DETECTORS].back = PAGE_INDEX_LIST_DETECTORS;
    menu[PAGE_INDEX_CLEAR_DETECTORS].left = PAGE_INDEX_CLEAR_DETECTORS;
    menu[PAGE_INDEX_CLEAR_DETECTORS].right = PAGE_INDEX_CLEAR_DETECTORS;
    menu[PAGE_INDEX_CLEAR_DETECTORS].ok = PAGE_INDEX_DETECTORS;
    menu[PAGE_INDEX_CLEAR_DETECTORS].valueRefreshEnabled = false;
}



ProgrammingService::ProgrammingService(SerialDebug *serialDebug, CC1101Controller *cc1101Controller, EEPROMController *eepromController, 
    CommunicationService *communicationService, CoreService *coreService, ServiceMode *serviceMode, SRAMController *sramController) {
    this->serialDebug = serialDebug;
    this->cc1101Controller = cc1101Controller;
    this->eepromController = eepromController;
    this->communicationService = communicationService;
    this->coreService = coreService;
    this->serviceMode = serviceMode;
    this->sramController = sramController;
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
                        case COMMAND_ID_SET_DEVICE_ADDRESS:

                            convertBytesToLong.byte[0] = workingBuffer[2];
                            convertBytesToLong.byte[1] = workingBuffer[3];
                            convertBytesToLong.byte[2] = workingBuffer[4];
                            convertBytesToLong.byte[3] = workingBuffer[5];

                            if(convertBytesToLong.longVal >= 1 && convertBytesToLong.longVal <= 65000) {
                                eepromController->setUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT, (uint16_t)convertBytesToLong.longVal);
                                communicationService->reloadEepromValues();
                                coreService->reloadEepromValues();
                                sendResponseOK(COMMAND_ID_SET_DEVICE_ADDRESS);
                            } else {
                                sendResponseFail(COMMAND_ID_SET_DEVICE_ADDRESS);
                            }
                        break;
                        case COMMAND_ID_SET_DEVICE_CHANNEL:

                            convertBytesToInt.byte[0] = workingBuffer[2];
                            convertBytesToInt.byte[1] = workingBuffer[3];

                            if(convertBytesToInt.intVal >= 0 && convertBytesToInt.intVal <= 10) {
                                eepromController->setByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE, (uint8_t)convertBytesToInt.intVal);
                                cc1101Controller->setChannel((uint8_t)convertBytesToInt.intVal);
                                communicationService->reloadEepromValues();
                                coreService->reloadEepromValues();
                                sendResponseOK(COMMAND_ID_SET_DEVICE_CHANNEL);
                            } else {
                                sendResponseFail(COMMAND_ID_SET_DEVICE_CHANNEL);
                            }
                        break;
                        case COMMAND_ID_SET_FORWARD_ADDRESS:

                            convertBytesToLong.byte[0] = workingBuffer[2];
                            convertBytesToLong.byte[1] = workingBuffer[3];
                            convertBytesToLong.byte[2] = workingBuffer[4];
                            convertBytesToLong.byte[3] = workingBuffer[5];

                            if(convertBytesToLong.longVal >= 1 && convertBytesToLong.longVal <= 65000) {
                                eepromController->setUIntValue(EEPROM_CC1101_FORWARD_ADDRESS_INT, (uint16_t)convertBytesToLong.longVal);
                                communicationService->reloadEepromValues();
                                coreService->reloadEepromValues();
                                sendResponseOK(COMMAND_ID_SET_FORWARD_ADDRESS);
                            } else {
                                sendResponseFail(COMMAND_ID_SET_FORWARD_ADDRESS);
                            }
                        break;
                        case COMMAND_ID_SET_FORWARD_CHANNEL:

                            convertBytesToInt.byte[0] = workingBuffer[2];
                            convertBytesToInt.byte[1] = workingBuffer[3];

                            if(convertBytesToInt.intVal >= 0 && convertBytesToInt.intVal <= 10) {
                                eepromController->setByteValue(EEPROM_CC1101_FORWARD_CHANNEL_BYTE, (uint8_t)convertBytesToInt.intVal);
                                communicationService->reloadEepromValues();
                                sendResponseOK(COMMAND_ID_SET_FORWARD_CHANNEL);
                            } else {
                                sendResponseFail(COMMAND_ID_SET_FORWARD_CHANNEL);
                            }
                        break;
                        case COMMAND_ID_SET_OPERATION_MODE:

                            convertBytesToInt.byte[0] = workingBuffer[2];
                            convertBytesToInt.byte[1] = workingBuffer[3];

                            if(convertBytesToInt.intVal >= 1 && convertBytesToInt.intVal <= 2) {
                                eepromController->setByteValue(EEPROM_CONFIG_OPERATION_MODE_BYTE, (uint8_t)convertBytesToInt.intVal);
                                communicationService->reloadEepromValues();
                                coreService->reloadEepromValues();
                                sendResponseOK(COMMAND_ID_SET_OPERATION_MODE);
                            } else {
                                sendResponseFail(COMMAND_ID_SET_OPERATION_MODE);
                            }
                        break;
                        case COMMAND_ID_SET_DEBUG_MODE:

                            if(workingBuffer[2] == 0 || workingBuffer[2] == 1) {
                                serialDebug->setDebugEnabled((bool) workingBuffer[2]);
                                sendResponseOK(COMMAND_ID_SET_DEBUG_MODE);
                            } else {
                                sendResponseFail(COMMAND_ID_SET_DEBUG_MODE);
                            }
                        break;
                        case COMMAND_ID_GET_DEVICE_TYPE:

                            responseBuffer[0] = 4;
                            responseBuffer[1] = COMMAND_ID_GET_DEVICE_TYPE;

                            convertBytesToInt.intVal = DEVICE_TYPE;
                            responseBuffer[2] = convertBytesToInt.byte[0];
                            responseBuffer[3] = convertBytesToInt.byte[1];

                            sendResponse();
                        break;
                        case COMMAND_ID_GET_ALL_VALUES:

                            responseBuffer[0] = 21;
                            responseBuffer[1] = COMMAND_ID_GET_ALL_VALUES;

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

                            convertBytesToLong.longVal = eepromController->getUIntValue(EEPROM_CC1101_FORWARD_ADDRESS_INT);
                            responseBuffer[10] = convertBytesToLong.byte[0];
                            responseBuffer[11] = convertBytesToLong.byte[1];
                            responseBuffer[12] = convertBytesToLong.byte[2];
                            responseBuffer[13] = convertBytesToLong.byte[3];

                            convertBytesToInt.intVal = eepromController->getByteValue(EEPROM_CC1101_FORWARD_CHANNEL_BYTE);
                            responseBuffer[14] = convertBytesToInt.byte[0];
                            responseBuffer[15] = convertBytesToInt.byte[1];

                            convertBytesToInt.intVal = eepromController->getByteValue(EEPROM_CONFIG_OPERATION_MODE_BYTE);
                            responseBuffer[16] = convertBytesToInt.byte[0];
                            responseBuffer[17] = convertBytesToInt.byte[1];

                            responseBuffer[18] = serialDebug->getDebugEnabled();

                            convertBytesToInt.intVal = communicationService->getAvarageLinkQuality();
                            responseBuffer[19] = convertBytesToInt.byte[0];
                            responseBuffer[20] = convertBytesToInt.byte[1];

                            sendResponse();
                        break;
                        case COMMAND_ID_SET_ALL_VALUES:

                            convertBytesToInt.byte[0] = workingBuffer[2];
                            convertBytesToInt.byte[1] = workingBuffer[3];

                            if(convertBytesToInt.intVal == DEVICE_TYPE) {
                                l=1;

                                convertBytesToLong.byte[0] = workingBuffer[4];
                                convertBytesToLong.byte[1] = workingBuffer[5];
                                convertBytesToLong.byte[2] = workingBuffer[6];
                                convertBytesToLong.byte[3] = workingBuffer[7];
                                if(convertBytesToLong.longVal >= 1 && convertBytesToLong.longVal <= 65000) {
                                    dataTemp.address = convertBytesToLong.longVal;
                                } else {
                                    l=0;
                                }

                                convertBytesToInt.byte[0] = workingBuffer[8];
                                convertBytesToInt.byte[1] = workingBuffer[9];
                                if(convertBytesToInt.intVal >= 0 && convertBytesToInt.intVal <= 10) {
                                    dataTemp.channel = convertBytesToInt.intVal;
                                } else {
                                    l=0;
                                }

                                convertBytesToLong.byte[0] = workingBuffer[10];
                                convertBytesToLong.byte[1] = workingBuffer[11];
                                convertBytesToLong.byte[2] = workingBuffer[12];
                                convertBytesToLong.byte[3] = workingBuffer[13];
                                if(convertBytesToLong.longVal >= 0 && convertBytesToLong.longVal <= 65000) {
                                    dataTemp.forwardAddress = convertBytesToLong.longVal;
                                } else {
                                    l=0;
                                }

                                convertBytesToInt.byte[0] = workingBuffer[14];
                                convertBytesToInt.byte[1] = workingBuffer[15];
                                if(convertBytesToInt.intVal >= 0 && convertBytesToInt.intVal <= 10) {
                                    dataTemp.forwardChannel = convertBytesToInt.intVal;
                                } else {
                                    l=0;
                                }

                                convertBytesToInt.byte[0] = workingBuffer[16];
                                convertBytesToInt.byte[1] = workingBuffer[17];
                                if(convertBytesToInt.intVal >= 1 && convertBytesToInt.intVal <= 3) {
                                    dataTemp.operationMode = convertBytesToInt.intVal;
                                } else {
                                    l=0;
                                }

                                if(workingBuffer[18] == 0 || workingBuffer[18] == 1) {
                                    dataTemp.debugEnabled = (bool) workingBuffer[18];
                                } else {
                                    l=0;
                                }

                                if(l == 1) {
                                    eepromController->setUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT, dataTemp.address);
                                    eepromController->setByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE, dataTemp.channel);
                                    cc1101Controller->setChannel(dataTemp.channel);
                                    eepromController->setUIntValue(EEPROM_CC1101_FORWARD_ADDRESS_INT, dataTemp.forwardAddress);
                                    eepromController->setByteValue(EEPROM_CC1101_FORWARD_CHANNEL_BYTE, dataTemp.forwardChannel);
                                    eepromController->setByteValue(EEPROM_CONFIG_OPERATION_MODE_BYTE, dataTemp.operationMode);
                                    serialDebug->setDebugEnabled(dataTemp.debugEnabled);

                                    communicationService->reloadEepromValues();
                                    coreService->reloadEepromValues();

                                    sendResponseOK(COMMAND_ID_SET_ALL_VALUES);
                                } else {
                                    sendResponseFail(COMMAND_ID_SET_ALL_VALUES);
                                }
                            } else {
                                sendResponseFail(COMMAND_ID_SET_ALL_VALUES);
                            }
                        break;
                        case COMMAND_ID_STREAM_ALL_MEMORY:

                            sendResponseStartStream(COMMAND_ID_STREAM_ALL_MEMORY);
                            delay(100);

                            for(l=0;l<sramController->getNumberOfAlarms();l++) {
                                sramController->getAlarm(&alarmEntry, l);

                                responseBuffer[0] = 11;
                                responseBuffer[1] = COMMAND_ID_STREAM_ALL_MEMORY;

                                convertBytesToLong.longVal = alarmEntry.address;
                                responseBuffer[2] = convertBytesToLong.byte[0];
                                responseBuffer[3] = convertBytesToLong.byte[1];
                                responseBuffer[4] = convertBytesToLong.byte[2];
                                responseBuffer[5] = convertBytesToLong.byte[3];

                                convertBytesToLong.longVal = alarmEntry.id;
                                responseBuffer[6] = convertBytesToLong.byte[0];
                                responseBuffer[7] = convertBytesToLong.byte[1];
                                responseBuffer[8] = convertBytesToLong.byte[2];
                                responseBuffer[9] = convertBytesToLong.byte[3];

                                responseBuffer[10] = alarmEntry.state;

                                sendResponse();
                                delay(100);
                            }

                            for(l=0;l<sramController->getNumberOfDetectors();l++) {
                                sramController->getDetector(&detectorEntry, l);

                                responseBuffer[0] = 17;
                                responseBuffer[1] = COMMAND_ID_STREAM_ALL_MEMORY;

                                convertBytesToLong.longVal = detectorEntry.address;
                                responseBuffer[2] = convertBytesToLong.byte[0];
                                responseBuffer[3] = convertBytesToLong.byte[1];
                                responseBuffer[4] = convertBytesToLong.byte[2];
                                responseBuffer[5] = convertBytesToLong.byte[3];

                                convertBytesToLong.longVal = detectorEntry.id;
                                responseBuffer[6] = convertBytesToLong.byte[0];
                                responseBuffer[7] = convertBytesToLong.byte[1];
                                responseBuffer[8] = convertBytesToLong.byte[2];
                                responseBuffer[9] = convertBytesToLong.byte[3];

                                convertBytesToInt.intVal = detectorEntry.linkQuality;
                                responseBuffer[10] = convertBytesToInt.byte[0];
                                responseBuffer[11] = convertBytesToInt.byte[1];

                                responseBuffer[12] = detectorEntry.status;

                                convertBytesToULong.longVal = detectorEntry.lastUpdateMs;
                                responseBuffer[13] = convertBytesToULong.byte[0];
                                responseBuffer[14] = convertBytesToULong.byte[1];
                                responseBuffer[15] = convertBytesToULong.byte[2];
                                responseBuffer[16] = convertBytesToULong.byte[3];

                                sendResponse();
                                delay(100);
                            }

                            sendResponseOK(COMMAND_ID_STREAM_ALL_MEMORY);     
                        break;
                        case COMMAND_ID_CLEAR_ALARMS:

                            sramController->clearAlarms();
                            eepromController->clearAlarms();
                            sendResponseOK(COMMAND_ID_CLEAR_ALARMS);
                        break;
                        case COMMAND_ID_CLEAR_DETECTORS:

                            sramController->clearDetectors();
                            sramController->clearOfflineDetectors();
                            sendResponseOK(COMMAND_ID_CLEAR_DETECTORS);
                        break;
                        case COMMAND_ID_ADD_ADDRESS_VERIFICATION_INTERVAL:

                            convertBytesToLong.byte[0] = workingBuffer[2];
                            convertBytesToLong.byte[1] = workingBuffer[3];
                            convertBytesToLong.byte[2] = workingBuffer[4];
                            convertBytesToLong.byte[3] = workingBuffer[5];
                            l = convertBytesToLong.longVal;

                            convertBytesToLong.byte[0] = workingBuffer[6];
                            convertBytesToLong.byte[1] = workingBuffer[7];
                            convertBytesToLong.byte[2] = workingBuffer[8];
                            convertBytesToLong.byte[3] = workingBuffer[9];

                            if(coreService->addAddressVerificationInterval(l, convertBytesToLong.longVal)) {
                                sendResponseOK(COMMAND_ID_ADD_ADDRESS_VERIFICATION_INTERVAL);
                            } else {
                                sendResponseFail(COMMAND_ID_ADD_ADDRESS_VERIFICATION_INTERVAL);
                            }
                        break;
                        case COMMAND_ID_CLEAR_ADDRESS_VERIFICATION_INTERVALS:

                            if(coreService->clearAddressVerificationIntervals()) {
                                sendResponseOK(COMMAND_ID_CLEAR_ADDRESS_VERIFICATION_INTERVALS);
                            } else {
                                sendResponseFail(COMMAND_ID_CLEAR_ADDRESS_VERIFICATION_INTERVALS);
                            }
                        break;
                        case COMMAND_ID_TEST_MODE:

                            if(workingBuffer[2] == 1) {
                                serviceMode->test1Start();
                                sendResponseOK(COMMAND_ID_TEST_MODE);
                            } else if(workingBuffer[2] == 0) {
                                serviceMode->test1Stop();
                                sendResponseOK(COMMAND_ID_TEST_MODE);
                            } else {
                                sendResponseFail(COMMAND_ID_TEST_MODE);
                            }
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

uint8_t ProgrammingService::calculateCRC(uint8_t *buff, uint8_t len) {
    crc = 0;
    for(k=0; k<len; k++) {
        crc += buff[k];
    }
    return (char)(crc % 256);
}

void ProgrammingService::sendResponseStartStream(uint8_t commandId) {
    responseBuffer[0] = 3;
    responseBuffer[1] = commandId;
    responseBuffer[2] = 2;

    sendResponse();
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
    if(debugEnabled) Serial.println(F("# DE"));
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



ServiceMode::ServiceMode(OutputController *outputController, CommunicationService *communicationService) {
    this->outputController = outputController;
    this->communicationService = communicationService;

    testRunning = false;
}

void ServiceMode::test1Start() {
    testRunning = true;

    outputController->set(RELAY_AC, true);
    outputController->set(RELAY_DC, true);
}

void ServiceMode::test1Stop() {
    testRunning = false;

    outputController->set(RELAY_AC, false);
    outputController->set(RELAY_DC, false);
}

bool ServiceMode::isTestRunning() {
    return testRunning;
}

void ServiceMode::setSendBuffer(uint8_t*sendBuffer, uint8_t *sendBufferLen) {
    this->sendBuffer = sendBuffer;
    this->sendBufferLen = sendBufferLen;
}



Startup::Startup(EEPROMController *eepr, SRAMController *sram) {
    this->eepr = eepr;
    this->sram = sram;
}

void Startup::doStartup() {

    pinMode(HW_IO_NC_PIN, INPUT);
    if(!eepr->getStartupFlag()) {
        randomSeed(analogRead(HW_IO_NC_PIN));
    } else {
        randomSeed(analogRead(HW_IO_NC_PIN) + eepr->getSerialNumber() +
            eepr->getUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT) + eepr->getByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE));
    }

    if(!eepr->getStartupFlag() || EEPROM_WIPE) {

        eepr->wipeEEPROM();

        eepr->setSerialNumber(random(100000,2147483640));

        eepr->setByteValue(EEPROM_CONFIG_OPERATION_MODE_BYTE, 1);    

        eepr->setUIntValue(EEPROM_CC1101_DEVICE_ADDRESS_INT, 1);
        eepr->setByteValue(EEPROM_CC1101_DEVICE_CHANNEL_BYTE, CC1101_DEFVAL_CHANNR);
        eepr->setUIntValue(EEPROM_CC1101_FORWARD_ADDRESS_INT, 0);
        eepr->setByteValue(EEPROM_CC1101_FORWARD_CHANNEL_BYTE, 0);

        eepr->setByteValue(EEPROM_ALARMS_MEMORY_INDEX_BYTE, EEPROM_ALARMS_MEMORY_START);
        eepr->setByteValue(EEPROM_ALARMS_MEMORY_COUNT_BYTE, 0);
        eepr->setByteValue(EEPROM_ADDRESS_VERIFICATION_INTERVAL_MEMORY_COUNT_BYTE, 0);

        eepr->setStartupFlag(true);
    }

    eepr->loadAlarmDetails();
    AlarmEntry alarmEntry;
    for(uint8_t i=0;i<eepr->getNumberOfAlarms();i++) {
        eepr->getAlarm(&alarmEntry, i);
        sram->addAlarm(&alarmEntry, false, true);
    }
}

