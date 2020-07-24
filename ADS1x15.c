/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "ADS1x15.h"

void Adafruit_ADS1115(uint8_t i2cAddress) {
    m_i2cAddress = i2cAddress;
    m_conversionDelay = ADS1115_CONVERSIONDELAY;
    m_bitShift = 0;
    m_rate = RATE_EIGHT;   // (default: 128SPS for 1115, and 1600 for 1105
    m_mode = 0x01;
    m_gain = GAIN_TWOTHIRDS; /* +/- 6.144V range (limited to VDD +0.3V max!) */ 
    m_config = ///< ADC default configuration
        ADS1015_REG_CONFIG_CQUE_NONE |    // Disable the comparator (default val)
        ADS1015_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
        ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
        ADS1015_REG_CONFIG_CMODE_TRAD|   // Traditional comparator (default val)
        ADS1115_REG_CONFIG_DR_128SPS |   // 128 samples per second (default)
        ADS1015_REG_CONFIG_MODE_SINGLE|   // Single-shot mode (default)
        GAIN_TWOTHIRDS;
    setConfig(m_config);
}
void Adafruit_ADS1105(uint8_t i2cAddress) {
    m_i2cAddress = i2cAddress;
    m_conversionDelay = ADS1015_CONVERSIONDELAY;
    m_bitShift = 4;
    m_rate = RATE_EIGHT;   // (default: 128SPS for 1115, and 1600 for 1105
    m_mode = 0x01;
    m_gain = GAIN_TWOTHIRDS; /* +/- 6.144V range (limited to VDD +0.3V max!) */
    m_config = ///< ADC default configuration
        ADS1015_REG_CONFIG_CQUE_NONE |    // Disable the comparator (default val)
        ADS1015_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
        ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
        ADS1015_REG_CONFIG_CMODE_TRAD|   // Traditional comparator (default val)
        ADS1115_REG_CONFIG_DR_128SPS |   // 128 samples per second (default)
        ADS1015_REG_CONFIG_MODE_SINGLE|   // Single-shot mode (default)
        GAIN_TWOTHIRDS;
    setConfig(m_config);
}

// begin is not needed since PSoC I2C_Start() will initial the Communication
void begin(void){
    //PSoC I2C initiasation method, see other method in the datasheet
    I2C_1_Start();
}
uint16_t readADC_SingleEnded( uint8_t channel){
  
      uint16_t config =
      ADS1015_REG_CONFIG_CQUE_NONE |    // Disable the comparator (default val)
      ADS1015_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
      ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
      ADS1015_REG_CONFIG_CMODE_TRAD;   // Traditional comparator (default val)
    //  ADS1115_REG_CONFIG_DR_128SPS |   // 128 samples per second (default)
    //  ADS1015_REG_CONFIG_MODE_CONTIN;   // Single-shot mode (default)

      // Set Data rate 
      config |= (m_mode<<8);
      // Set Data rate 
      config |= m_rate;
      // Set PGA/voltage range
      config |= m_gain;

  // Set single-ended input channel
//  switch (channel) {
//  case (0):
//    config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
//    break;
//  case (1):
//    config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
//    break;
//  case (2):
//    config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
//    break;
//  case (3):
//    config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
//    break;
//  }
    
 //algorithmic approach for branches
    if(channel<=3 && channel >=0){ //check it is actually within input range
        config |= ADS1015_REG_CONFIG_MUX_SINGLE_0 | channel<<12;
    }
    uint8_t update=0;
    if (config!=l_config){
        l_config = config;
        update=1;
    } //else no need to config again except when it is single-shot mode
    
    if (m_mode==1){
    // Set 'start single-conversion' bit
        config |= ADS1015_REG_CONFIG_OS_SINGLE;
        update=1;
    }
    if (update==1){
        // Write config register to the ADC
        setConfig(config);
    }
    // Wait for the conversion to complete
    CyDelay(0);

  // Read the conversion results
  // ADS1115 has 16 bits output shift 8 to reconstrcut
    I2C_1_MasterSendStart(m_i2cAddress, 00);
    I2C_1_MasterWriteByte(ADS1015_REG_POINTER_CONVERT);
    I2C_1_MasterSendRestart(m_i2cAddress, 01); 
    uint16_t conversion_H = I2C_1_MasterReadByte(I2C_1_ACK_DATA);
    uint16_t conversion_L = I2C_1_MasterReadByte(I2C_1_NAK_DATA);
    
    I2C_1_MasterSendStop();
    
    return conversion_H<<(8 - m_bitShift) | conversion_L;
    //readRegister(m_i2cAddress, ADS1015_REG_POINTER_CONVERT) >> m_bitShift;
    
}


int16_t readADC_Differential_0_1(){
    // Start with default values
    uint16_t config =
        ADS1015_REG_CONFIG_CQUE_NONE |    // Disable the comparator (default val)
        ADS1015_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
        ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
        ADS1015_REG_CONFIG_CMODE_TRAD ;   // Traditional comparator (default val)
//      ADS1115_REG_CONFIG_DR_128SPS |   // 128 samples per second (default)
//      ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)
    
    // Set Data rate 
    config |= (m_mode<<8);  
    // Set Data rate 
    config |= m_rate;
    // Set PGA/voltage range
    config |= m_gain;

    // Set channels
    config |= ADS1015_REG_CONFIG_MUX_DIFF_0_1; // AIN0 = P, AIN1 = N

    uint8_t update=0;
    if (config!=l_config){
        l_config = config;
        update=1;
    } //else no need to config again except when it is single-shot mode
    
    if (m_mode==1){
    // Set 'start single-conversion' bit
        config |= ADS1015_REG_CONFIG_OS_SINGLE;
        update=1;
    }
    if (update==1){
        // Write config register to the ADC
        setConfig(config);
    }
    // Wait for the conversion to complete
    CyDelay(m_conversionDelay);

  // Read the conversion results
    I2C_1_MasterSendStart(m_i2cAddress, 00);
    I2C_1_MasterWriteByte(ADS1015_REG_POINTER_CONVERT);
    I2C_1_MasterSendRestart(m_i2cAddress, 01); 
    uint16_t res_H = I2C_1_MasterReadByte(I2C_1_ACK_DATA);
    uint16_t res_L = I2C_1_MasterReadByte(I2C_1_NAK_DATA);
    
    uint16_t res = res_H<<(8 - m_bitShift) |res_L;

    if (m_bitShift != 0) {
        // Shift 12-bit results right 4 bits for the ADS1015,
        // making sure we keep the sign bit intact
        if (res > 0x07FF) {
            // negative number - extend the sign to 16th bit
            res |= 0xF000;
        }
    }
    return (int16_t)res;
}
int16_t readADC_Differential_2_3(){
  // Start with default values
    uint16_t config =
        ADS1015_REG_CONFIG_CQUE_NONE |    // Disable the comparator (default val)
        ADS1015_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
        ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
        ADS1015_REG_CONFIG_CMODE_TRAD ;   // Traditional comparator (default val)
 //     ADS1115_REG_CONFIG_DR_128SPS |   // 128 samples per second (default)
 //     ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)
    
    // Set Data rate 
    config |= (m_mode<<8);
    // Set Data rate 
    config |= m_rate;
    // Set PGA/voltage range
    config |= m_gain;

    // Set channels
    config |= ADS1015_REG_CONFIG_MUX_DIFF_2_3; // AIN2 = P, AIN3 = N

    uint8_t update=0;
    if (config!=l_config){
        l_config = config;
        update=1;
    } //else no need to config again except when it is single-shot mode
    
    if (m_mode==1){
    // Set 'start single-conversion' bit
        config |= ADS1015_REG_CONFIG_OS_SINGLE;
        update=1;
    }
    if (update==1){
        // Write config register to the ADC
        setConfig(config);
    }
    // Wait for the conversion to complete
    CyDelay(m_conversionDelay);

  // Read the conversion results
    I2C_1_MasterSendStart(m_i2cAddress, 00);
    I2C_1_MasterWriteByte(ADS1015_REG_POINTER_CONVERT);
    I2C_1_MasterSendRestart(m_i2cAddress, 01); 
    uint16_t res_H = I2C_1_MasterReadByte(I2C_1_ACK_DATA);
    uint16_t res_L = I2C_1_MasterReadByte(I2C_1_NAK_DATA);
    
    uint16_t res = res_H<<(8 - m_bitShift) |res_L;
    
    if (m_bitShift != 0) {
        // Shift 12-bit results right 4 bits for the ADS1015,
        // making sure we keep the sign bit intact
        if (res > 0x07FF) {
            // negative number - extend the sign to 16th bit
            res |= 0xF000;
        }
    }
    return (int16_t)res;
}


void startComparator_SingleEnded(uint8_t channel, int16_t threshold){
    // Start with default values
    uint16_t config =
        ADS1015_REG_CONFIG_CQUE_1CONV |   // Comparator enabled and asserts on 1
                                        // match
        ADS1015_REG_CONFIG_CLAT_LATCH |   // Latching mode
        ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
        ADS1015_REG_CONFIG_CMODE_TRAD ;   // Traditional comparator (default val)
//        ADS1115_REG_CONFIG_DR_128SPS |   // 128 samples per second (default)
//        ADS1015_REG_CONFIG_MODE_CONTIN;   // Continuous conversion mode
    // Set Data rate 
    config |= (m_mode<<8);
    // Set Data rate 
    config |= m_rate;
    // Set PGA/voltage range
    config |= m_gain;

// Set single-ended input channel
//  switch (channel) {
//  case (0):
//    config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
//    break;
//  case (1):
//    config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
//    break;
//  case (2):
//    config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
//    break;
//  case (3):
//    config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
//    break;
//  }
 //algorithmic approach for branches
    if(channel<=3 && channel >=0){ //check it is actually within input range
        config |= ADS1015_REG_CONFIG_MUX_SINGLE_0 | channel<<12;
    }
    

  // Set the high threshold register
  // Shift 12-bit results left 4 bits for the ADS1015

    I2C_1_MasterSendStart(m_i2cAddress, 00);
    I2C_1_MasterWriteByte(ADS1015_REG_POINTER_HITHRESH);
    I2C_1_MasterWriteByte(threshold>>(8 - m_bitShift));
    I2C_1_MasterWriteByte(threshold & 0xFF);
    I2C_1_MasterSendStop();
    
    // Write config register to the ADC    
    setConfig(config); 
}

int16_t getLastConversionResults(){
    // Wait for the conversion to complete
    CyDelay(m_conversionDelay);

    // Read the conversion results
    I2C_1_MasterSendStart(m_i2cAddress, 00);
    I2C_1_MasterWriteByte(ADS1015_REG_POINTER_CONVERT);
    I2C_1_MasterSendRestart(m_i2cAddress, 01);
    uint16_t res_H = I2C_1_MasterReadByte(I2C_1_ACK_DATA);
    uint16_t res_L = I2C_1_MasterReadByte(I2C_1_NAK_DATA);

    uint16_t res = res_H << (8 - m_bitShift) | res_L;

    if (m_bitShift != 0) {
        // Shift 12-bit results right 4 bits for the ADS1015,
        // making sure we keep the sign bit intact
        if (res > 0x07FF) {
            // negative number - extend the sign to 16th bit
            res |= 0xF000;
        }
    }
    return (int16_t)res;
}


uint16_t getConfig(){
    I2C_1_MasterSendStart(m_i2cAddress, 00);
    I2C_1_MasterWriteByte(ADS1015_REG_POINTER_CONFIG);
    I2C_1_MasterSendRestart(m_i2cAddress, 01); 
    uint16_t config_H = I2C_1_MasterReadByte(I2C_1_ACK_DATA);
    uint16_t config_L = I2C_1_MasterReadByte(I2C_1_NAK_DATA);
    I2C_1_MasterSendStop();
    
    return (uint16_t) config_H<<8|config_L;
}
void setConfig(uint16 config){
    uint8_t config_H = config>>8;
    uint8_t config_L = config & 0xFF;
    // Write config register to the ADC
    // writeRegister(m_i2cAddress, ADS1015_REG_POINTER_CONFIG, config);
    I2C_1_MasterSendStart(m_i2cAddress, 00);
    I2C_1_MasterWriteByte(ADS1015_REG_POINTER_CONFIG);
    I2C_1_MasterWriteByte(config_H);
    I2C_1_MasterWriteByte(config_L);
    I2C_1_MasterSendStop();
}
/*
reset the Config to default values
*/
void resetConfig(){
    uint8_t config_H = m_config>>8;
    uint8_t config_L = m_config & 0xFF;
  // Write config register to the ADC
 // writeRegister(m_i2cAddress, ADS1015_REG_POINTER_CONFIG, config);
    I2C_1_MasterSendStart(m_i2cAddress, 00);
    I2C_1_MasterWriteByte(ADS1015_REG_POINTER_CONFIG);
    I2C_1_MasterWriteByte(config_H);
    I2C_1_MasterWriteByte(config_L);
    I2C_1_MasterSendStop();
}
void setRate(adsRate_t rate){m_rate = rate; }
adsRate_t getRate(void){ return m_rate; };

void setGain(adsGain_t gain){m_gain = gain; }
adsGain_t getGain(void){ return m_gain; };

void setMode(uint8_t mode){m_mode = mode; }
uint8_t getMode(void){ return m_mode; };

void setDelay(uint8_t delay){m_conversionDelay = delay; }
uint8_t getDelay(void){ return m_conversionDelay; };

/* [] END OF FILE */
