#pragma once

#include <iostream>
#include <SPI.h>
#include <Arduino.h>
#include "./MPU9250/src/MPU9250.h"
#include "./MAX525/MAX525.h"
#include "./ADS8688/ADS8688.h"

#include "TaskInterface.h"

using namespace std;
#define CS_IMU1 15
#define CS_IMU2 33
#define CS_DAC 32
#define CS_ADC 14

float SetVoltage = 0.5;

MPU9250FIFO _IMU1(SPI, CS_IMU1);
MPU9250FIFO _IMU2(SPI, CS_IMU2);

struct MySensorbandTask : TaskInterface
{

    bool is_left_sensor_band;
    // MPU9250 _IMU1, _IMU2;
    MAX525 _DAC;
    ADS8688 _ADC;

    MySensorbandTask(bool is_left_sensor_band)
        // :_IMU1(SPI, CS_IMU1), _IMU2(SPI, CS_IMU2)   // Constructors and member initializer lists
        :_DAC(SPI, CS_DAC)
        , _ADC(CS_ADC)
    {
        this->is_left_sensor_band = is_left_sensor_band;
    }

    bool initialize()
    {
        if (is_left_sensor_band)
        {
            description = "Left sensor band";
        }
        else
        {
            description = "Right sensor band";
        }

        low_frequency_sample_names = {
            "FSR1", "FSR2", "FSR3", "FSR4", "FSR5", "FSR6", "FSR7", "FSR8",
            "AccX1", "AccY1", "AccZ1", "GyroX1", "GyroY1", "GyroZ1",
            "AccX2", "AccY2", "AccZ2", "GyroX2", "GyroY2", "GyroZ2"};
        high_frequency_sample_names = {"EMG1", "EMG2", "EMG3", "EMG4"};

        Serial.println("Initialize ADC");
        _ADC.Begin();

        Serial.println("Initialize IMU");
        Serial.println(_IMU1.begin()); // Initiate IMU 1
        Serial.println(_IMU2.begin()); // Initiate IMU 2

        Serial.println("Initialize DAC");
        Serial.println(_DAC.begin_Daisy());

        return true;
    }

    void SetDACVoltaget(uint8_t Channel, float Voltage)
    {
        _DAC.SetVoltage_Daisy(Channel, Voltage);
    }

    bool process_message(const Message &incomming_message, function<bool(const Message &)> send_message_fun)
    {
        return false;
    }

    bool get_low_frequency_samples(vector<float> &low_frequency_samples, bool sending_signals)
    {

        SetDACVoltaget(0, SetVoltage);
        SetDACVoltaget(1, SetVoltage);
        SetDACVoltaget(2, SetVoltage);
        SetDACVoltaget(3, SetVoltage);
        SetDACVoltaget(4, SetVoltage);
        SetDACVoltaget(5, SetVoltage);
        SetDACVoltaget(6, SetVoltage);
        SetDACVoltaget(7, SetVoltage);

        /*FSR ADC is read in high frequency section */
        std::vector<float> FSR = _ADC.ReturnADC_FSR(); // Get FSR data
        for (size_t i = 0; i < FSR.size(); i++)
        {
            low_frequency_samples.push_back(FSR[i]);
        }

        _IMU1.readSensor(); // Read IMU 1 data
        _IMU2.readSensor(); // Read IMU 2 data

        /*IMU*/
        low_frequency_samples.push_back(_IMU1.getAccelX_mss());
        low_frequency_samples.push_back(_IMU1.getAccelY_mss());
        low_frequency_samples.push_back(_IMU1.getAccelZ_mss());
        low_frequency_samples.push_back(_IMU1.getGyroX_rads());
        low_frequency_samples.push_back(_IMU1.getGyroY_rads());
        low_frequency_samples.push_back(_IMU1.getGyroZ_rads());
        low_frequency_samples.push_back(_IMU2.getAccelX_mss());
        low_frequency_samples.push_back(_IMU2.getAccelY_mss());
        low_frequency_samples.push_back(_IMU2.getAccelZ_mss());
        low_frequency_samples.push_back(_IMU2.getGyroX_rads());
        low_frequency_samples.push_back(_IMU2.getGyroY_rads());
        low_frequency_samples.push_back(_IMU2.getGyroZ_rads());

        // What goes here:
        // * Closed loop motor controller
        // * Low frequency sampling of motor controller signal sampling
        // * Gyro and IMU sampling
        return true; // Return false if a critical error occured; otherwise return true.
    }
    bool get_high_frequency_samples(vector<float> &high_frequency_samples, bool sending_signals)
    {
        _ADC.noOpDaisy();
        /*EMG*/
        std::vector<float> EMG = _ADC.ReturnADC_EMG(); // Get EMG data
        for (size_t i = 0; i < EMG.size(); i++)
        {
            high_frequency_samples.push_back(EMG[i]);
        }
        // What goes here:
        // * EMG signal sampling
        return true;
    }
};