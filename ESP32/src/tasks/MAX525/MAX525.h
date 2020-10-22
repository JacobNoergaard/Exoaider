#ifndef MAX525_h
#define MAX525_h

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

class MAX525
{
public:
    MAX525(SPIClass &bus, uint8_t csPin);
    int begin();
    int begin_Daisy();
    int SetVoltage(uint8_t DAC_x, float Voltage);
    int SetVoltage_Daisy(int DAC_x, float Voltage);
    void SetDACFeedbackGain(float gain);


    // Serial-Interface Programming Commands
    const uint16_t LIR_A_UNC = 0x1000; // Load input register A; DAC registers unchanged.
    const uint16_t LIR_B_UNC = 0x5000; // Load input register B; DAC registers unchanged.
    const uint16_t LIR_C_UNC = 0x9000; // Load input register C; DAC registers unchanged.
    const uint16_t LIR_D_UNC = 0xD000; // Load input register D; DAC registers unchanged.

    const uint16_t LIR_A_UP = 0x3000; // Load input register A; all DAC registers updated.
    const uint16_t LIR_B_UP = 0x7000; // Load input register B; all DAC registers updated.
    const uint16_t LIR_C_UP = 0xB000; // Load input register C; all DAC registers updated.
    const uint16_t LIR_D_UP = 0xF000; // Load input register D; all DAC registers updated.

    const uint16_t Update_All_DAC_Reg = 0x4000; // Update all DAC registers from their respective input registers (start-up)
    const uint16_t Load_All_DAC_Reg = 0x8000;   // Load all DAC registers from shift register (start-up).
    const uint16_t Shutdown = 0xC000;           // Shutdown (provided PDL = 1)
    const uint16_t UPO_LOW = 0x2000;            // UPO goes low (default)
    const uint16_t UPO_HIGH = 0x6000;           // UPO goes HIGH
    const uint16_t NOP = 0x0000;                // No operation (NOP) to DAC registers
    const uint16_t MODE_1 = 0xE000;             // Mode 1, DOUT clocked out on SCLK’s rising edge. All DAC registers updated.
    const uint16_t MODE_0 = 0xA000;             // Mode 0, DOUT clocked out on SCLK’s falling edge. All DAC registers updated (default)
    
protected:
    // SPI
    SPIClass *_spi;
    uint8_t _csPin;
    bool _useSPI;
    const uint32_t _SCLK = 5000000;      // 5 MHz
    

private:
    uint16_t _DAC_OLD[8] = {0x00};        // Storing old voltage values for DAC (only needed for daisy-chain configuration)
    float _VoltageReference = 3.3;        // 3.3 Volt
    //float _DAC_Gain = 1.5;              // Feedback gain non-inverting (1 + 5k/10k)
    float _DAC_Gain = 3;                  // Feedback gain non-inverting (1 + 10k/5k)
    uint16_t _GetReference(float Voltage);// Calculate the voltage reference for the DACs
};

#endif