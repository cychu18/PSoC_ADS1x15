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
#include "ADS1115.h"

void Adafruit_ADS1115(uint8_t i2cAddress) {
  m_i2cAddress = i2cAddress;
  m_conversionDelay = ADS1115_CONVERSIONDELAY;
  m_bitShift = 0;
  m_rate = RATE_EIGHT;   // (default: 128SPS for 1115, and 1600 for 1105
  m_gain = GAIN_TWOTHIRDS; /* +/- 6.144V range (limited to VDD +0.3V max!) */
}
void Adafruit_ADS1105(uint8_t i2cAddress) {
  m_i2cAddress = i2cAddress;
  m_conversionDelay = ADS1015_CONVERSIONDELAY;
  m_bitShift = 4;
  m_rate = RATE_EIGHT;   // (default: 128SPS for 1115, and 1600 for 1105
  m_gain = GAIN_TWOTHIRDS; /* +/- 6.144V range (limited to VDD +0.3V max!) */
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
      ADS1015_REG_CONFIG_CMODE_TRAD |   // Traditional comparator (default val)
   //   ADS1115_REG_CONFIG_DR_128SPS |   // 128 samples per second (default)
      ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

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

  // Set 'start single-conversion' bit
  config |= ADS1015_REG_CONFIG_OS_SINGLE;

  // Write config register to the ADC
  // note that PSoC I2C only send Byte to the register, so config has to be brokwn down into two command
 // writeRegister(m_i2cAddress, ADS1015_REG_POINTER_CONFIG, config);
  uint8_t config_H = config_H>>8;
  uint8_t config_L = config_H & 0xFF;
  // Write config register to the ADC
    I2C_1_MasterSendStart(m_i2cAddress, 00);
    I2C_1_MasterWriteByte(ADS1015_REG_POINTER_CONFIG);
    I2C_1_MasterWriteByte(config_H);
    I2C_1_MasterWriteByte(config_L);
    I2C_1_MasterSendStop();
  // Read the conversion results
  // ADS1115 has 16 bits output shift 8 to reconstrcut
    I2C_1_MasterSendStart(m_i2cAddress, 00);
    I2C_1_MasterWriteByte(ADS1015_REG_POINTER_CONVERT);
    I2C_1_MasterSendRestart(m_i2cAddress, 01); 
    uint16_t conversion_H = I2C_1_MasterReadByte(I2C_1_ACK_DATA);
    uint16_t conversion_L = I2C_1_MasterReadByte(I2C_1_NAK_DATA);
    
    I2C_1_MasterSendStop();
    
  return conversion_H<<8 | conversion_L;
    //readRegister(m_i2cAddress, ADS1015_REG_POINTER_CONVERT) >> m_bitShift;
    
}


  int16_t readADC_Differential_0_1(){
    // Start with default values
  uint16_t config =
      ADS1015_REG_CONFIG_CQUE_NONE |    // Disable the comparator (default val)
      ADS1015_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
      ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
      ADS1015_REG_CONFIG_CMODE_TRAD |   // Traditional comparator (default val)
//      ADS1115_REG_CONFIG_DR_128SPS |   // 128 samples per second (default)
      ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)
    
  // Set Data rate 
  config |= m_rate;
  // Set PGA/voltage range
  config |= m_gain;

  // Set channels
  config |= ADS1015_REG_CONFIG_MUX_DIFF_0_1; // AIN0 = P, AIN1 = N

  // Set 'start single-conversion' bit
  config |= ADS1015_REG_CONFIG_OS_SINGLE;
  uint8_t config_H = config_H>>8;
  uint8_t config_L = config_H & 0xFF;
  // Write config register to the ADC
  //writeRegister(m_i2cAddress, ADS1015_REG_POINTER_CONFIG, config);
    I2C_1_MasterSendStart(m_i2cAddress, 00);
    I2C_1_MasterWriteByte(ADS1015_REG_POINTER_CONFIG);
    I2C_1_MasterWriteByte(config_H);
    I2C_1_MasterWriteByte(config_L);
    I2C_1_MasterSendStop();
    
  // Wait for the conversion to complete
  CyDelay(m_conversionDelay);

  // Read the conversion results
    I2C_1_MasterSendStart(m_i2cAddress, 00);
    I2C_1_MasterWriteByte(ADS1015_REG_POINTER_CONVERT);
    I2C_1_MasterSendRestart(m_i2cAddress, 01); 
    uint16_t res_H = I2C_1_MasterReadByte(I2C_1_ACK_DATA);
    uint16_t res_L = I2C_1_MasterReadByte(I2C_1_NAK_DATA);
    
  uint16_t res = res_H<<8 |res_L;

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
      ADS1015_REG_CONFIG_CMODE_TRAD |   // Traditional comparator (default val)
 //     ADS1115_REG_CONFIG_DR_128SPS |   // 128 samples per second (default)
      ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)
    
  // Set Data rate 
  config |= m_rate;
  // Set PGA/voltage range
  config |= m_gain;

  // Set channels
  config |= ADS1015_REG_CONFIG_MUX_DIFF_2_3; // AIN2 = P, AIN3 = N

  // Set 'start single-conversion' bit
  config |= ADS1015_REG_CONFIG_OS_SINGLE;
  uint8_t config_H = config_H>>8;
  uint8_t config_L = config_H & 0xFF;
  // Write config register to the ADC
 // writeRegister(m_i2cAddress, ADS1015_REG_POINTER_CONFIG, config);
    I2C_1_MasterSendStart(m_i2cAddress, 00);
    I2C_1_MasterWriteByte(ADS1015_REG_POINTER_CONFIG);
    I2C_1_MasterWriteByte(config_H);
    I2C_1_MasterWriteByte(config_L);
    I2C_1_MasterSendStop();
  // Wait for the conversion to complete
  CyDelay(m_conversionDelay);

  // Read the conversion results
    I2C_1_MasterSendStart(m_i2cAddress, 00);
    I2C_1_MasterWriteByte(ADS1015_REG_POINTER_CONVERT);
    I2C_1_MasterSendRestart(m_i2cAddress, 01); 
    uint16_t res_H = I2C_1_MasterReadByte(I2C_1_ACK_DATA);
    uint16_t res_L = I2C_1_MasterReadByte(I2C_1_NAK_DATA);
    
  uint16_t res = res_H<<8 |res_L;
    
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
      ADS1015_REG_CONFIG_CMODE_TRAD |   // Traditional comparator (default val)
 //     ADS1115_REG_CONFIG_DR_128SPS |   // 128 samples per second (default)
      ADS1015_REG_CONFIG_MODE_CONTIN;   // Continuous conversion mode
    
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
    I2C_1_MasterWriteByte(threshold);
    I2C_1_MasterSendStop();
    
    
  // Write config register to the ADC
  uint8_t config_H = config_H>>8;
  uint8_t config_L = config_H & 0xFF;
    I2C_1_MasterSendStart(m_i2cAddress, 00);
    I2C_1_MasterWriteByte(ADS1015_REG_POINTER_CONFIG);
    I2C_1_MasterWriteByte(config_H);
    I2C_1_MasterWriteByte(config_L);
    I2C_1_MasterSendStop();
}

  int16_t getLastConversionResults();


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
  // Set PGA/voltage range
  config |= m_gain;
    
  uint8_t config_H = config_H>>8;
  uint8_t config_L = config_H & 0xFF;
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

/* [] END OF FILE */
