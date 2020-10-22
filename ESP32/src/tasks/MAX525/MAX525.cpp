#include <Arduino.h>
#include "MAX525.h"

/*  Constructor */
MAX525::MAX525(SPIClass &bus, uint8_t csPin)
{
    _spi = &bus;    // SPI bus
    _csPin = csPin; // chip select pin
    //_useSPI = true; // set to use SPI
}

/*  Initiate MAX525 DAC in single configuration */
int MAX525::begin()
{
    pinMode(_csPin, OUTPUT);
    digitalWrite(_csPin, HIGH);
    _spi->begin();
    _spi->beginTransaction(SPISettings(_SCLK, MSBFIRST, SPI_MODE0)); // begin the transaction
    digitalWrite(_csPin, LOW);
    _spi->transfer16(Update_All_DAC_Reg);
    digitalWrite(_csPin, HIGH);
    _spi->endTransaction();
    return 1;
}

/*  Initiate MAX525 DAC in two stage daisy-chain configuration */
int MAX525::begin_Daisy()
{
    pinMode(_csPin, OUTPUT);
    digitalWrite(_csPin, HIGH);    
    _spi->begin();
    delay(50);
    for (size_t i = 0; i < 8; i++){SetVoltage_Daisy(i,0); delay(10);} // set all channels to zero 
    return 1;
}

/*  Write two bytes to MAX525 registers. 4 address/control bits and 12 data bits.
    DAC_x can take values between 0 - 3 corrosponding to 4 channels on the DACs'.
    This only works in non-daisy-chain configuration */
int MAX525::SetVoltage(uint8_t DAC_x, float Voltage)
{
    _spi->beginTransaction(SPISettings(_SCLK, MSBFIRST, SPI_MODE0)); // begin the transaction
    digitalWrite(_csPin, LOW);

    switch (DAC_x)
    {
    case 0:
    //Serial.println("DAC A");
        _spi->transfer16(LIR_A_UP | _GetReference(Voltage));
        digitalWrite(_csPin, HIGH);
        _spi->endTransaction();
        break;
    case 1:
        //Serial.println("DAC B");
        _spi->transfer16(LIR_D_UP | _GetReference(Voltage));
        digitalWrite(_csPin, HIGH);
        _spi->endTransaction();
        break;
    case 2:
        //Serial.println("DAC C");
        _spi->transfer16(LIR_B_UP | _GetReference(Voltage));
        digitalWrite(_csPin, HIGH);
        _spi->endTransaction();
        break;
    case 3:
        //Serial.println("DAC D");
        _spi->transfer16(LIR_C_UP | _GetReference(Voltage));
        digitalWrite(_csPin, HIGH);
        _spi->endTransaction();
        break;
    default:
        _spi->endTransaction();
        return -1;
    }
    return 1;
}

/*  Write two bytes to MAX525 registers. 4 address/control bits and 12 data bits.
    DAC_x can take values between 0 - 7 corrosponding to the 8 channels on the DACs'.
    This only works in a two stage daisy-chain configuration. 
    This code is configured to the Exo-Aider PCB V0 layout in a fasion such the pin header is in a cronological 
    order. Henceforth, the actual DAC channel, e.g [1,2,3,4], do not match with the PCB pin header [1,2,3,4]*/
int MAX525::SetVoltage_Daisy(int DAC_x, float Voltage)
{   
    if(5 < Voltage){Voltage = 5;}
    if(Voltage< 0 ){Voltage = 0;}

    _spi->beginTransaction(SPISettings(_SCLK, MSBFIRST, SPI_MODE0)); // begin the transaction
    digitalWrite(_csPin, LOW);

    switch (DAC_x)
    {
    case 0: // pin header 1
        //Serial.println("DAC A0");
        _spi->transfer16(LIR_A_UP | _DAC_OLD[DAC_x + 4]);    // daisy chain - set register for DAC two to present voltage value.
        _spi->transfer16(LIR_A_UP | _GetReference(Voltage)); // daisy chain - set register for DAC one to new voltage value.
        digitalWrite(_csPin, HIGH);
        _spi->endTransaction();
        _DAC_OLD[DAC_x] = _GetReference(Voltage);
        break;
    case 1: // pin header 2
        //Serial.println("DAC B0");
        _spi->transfer16(LIR_D_UP | _DAC_OLD[DAC_x + 4]);    // daisy chain - set register for DAC two to present voltage value.
        _spi->transfer16(LIR_D_UP | _GetReference(Voltage)); // daisy chain - set register for DAC one to new voltage value.
        digitalWrite(_csPin, HIGH);
        _spi->endTransaction();
        _DAC_OLD[DAC_x] = _GetReference(Voltage);
        break;
    case 2: // pin header 3
        //Serial.println("DAC C0");
        _spi->transfer16(LIR_B_UP | _DAC_OLD[DAC_x + 4]);    // daisy chain - set register for DAC two to present voltage value.
        _spi->transfer16(LIR_B_UP | _GetReference(Voltage)); // daisy chain - set register for DAC one to new voltage value.
        digitalWrite(_csPin, HIGH);
        _spi->endTransaction();
        _DAC_OLD[DAC_x] = _GetReference(Voltage);
        break;
    case 3: // pin header 4
        //Serial.println("DAC D0");
        _spi->transfer16(LIR_C_UP | _DAC_OLD[DAC_x + 4]);    // daisy chain - set register for DAC two to present voltage value.
        _spi->transfer16(LIR_C_UP | _GetReference(Voltage)); // daisy chain - set register for DAC one to new voltage value.
        digitalWrite(_csPin, HIGH);
        _spi->endTransaction();
        _DAC_OLD[DAC_x] = _GetReference(Voltage);
        break;
    case 4: // pin header 4
        //Serial.println("DAC A1");
        _spi->transfer16(LIR_A_UP | _GetReference(Voltage)); // daisy chain - set register for DAC two to new voltage value.
        _spi->transfer16(LIR_A_UP | _DAC_OLD[DAC_x - 4]);    // daisy chain - set register for DAC one to present voltage value.
        digitalWrite(_csPin, HIGH);
        _spi->endTransaction();
        _DAC_OLD[DAC_x] = _GetReference(Voltage);
        break;
    case 5: // pin header 5
        //Serial.println("DAC B1");
        _spi->transfer16(LIR_D_UP | _GetReference(Voltage)); // daisy chain - set register for DAC two to new voltage value.
        _spi->transfer16(LIR_D_UP | _DAC_OLD[DAC_x - 4]);    // daisy chain - set register for DAC one to present voltage value.
        digitalWrite(_csPin, HIGH);
        _spi->endTransaction();
        _DAC_OLD[DAC_x] = _GetReference(Voltage);
        break;
    case 6: // pin header 6
        //Serial.println("DAC C1");
        _spi->transfer16(LIR_B_UP | _GetReference(Voltage)); // daisy chain - set register for DAC two to new voltage value.
        _spi->transfer16(LIR_B_UP | _DAC_OLD[DAC_x - 4]);    // daisy chain - set register for DAC one to present voltage value.
        digitalWrite(_csPin, HIGH);
        _spi->endTransaction();
        _DAC_OLD[DAC_x] = _GetReference(Voltage);
        break;
    case 7: // pin header 7
        //Serial.println("DAC D1");
        _spi->transfer16(LIR_C_UP | _GetReference(Voltage)); // daisy chain - set register for DAC two to new voltage value.
        _spi->transfer16(LIR_C_UP | _DAC_OLD[DAC_x - 4]);    // daisy chain - set register for DAC two to present voltage value.
        digitalWrite(_csPin, HIGH);
        _spi->endTransaction();
        _DAC_OLD[DAC_x] = _GetReference(Voltage);
        break;
    default:
        _spi->endTransaction();
        return -1;
    }
    return 1;
}

/*  Calculates the voltage reference for the DAC.*/
uint16_t MAX525::_GetReference(float Voltage){
    uint16_t reference = ( Voltage/(_VoltageReference*_DAC_Gain) ) * 4096; 
    if(reference < 0){
        reference = 0;
    }
    else if (reference > 4096)
    {
        reference = 4096;
    }
    
    return reference;
}

/* Set the noninverting feedback gain of the DAC.*/
void MAX525::SetDACFeedbackGain(float gain){
    _DAC_Gain = gain;
}