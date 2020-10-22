# Exoaider
Main repository for the Exoaider armband with sEMG and FSR sensors running on an ESP32 feather with Bluetooth interface to Matlab

## Usage

**Important: Remember to turn on the battery!**


### ESP32

Upload: `ESP32/src/SppBluetooth.cpp`

When uploaded you will see the name of the ESP (e.g. `0877A9E350CC@Exo-Aider`) in the serial monitor which you will need in the Matlab code later.

### Matlab

Open `Matlab/Tests/SppBluetooth_simsig.m`

**Important: Include the folders `Matlab/Tests` and `Matlab/cobss-matlab` for the Matlab code to work**

Move to the section saying `%% Read sensors, move "function" into section` around line 150. 
Here you will see the function `SppBluetooth(exoaiderName, taskname, bufferSize)`
* `exoaiderName`: The name from the serial monitor earlier.
* `taskname`: Name of a task specified in the ESP code.
* `bufferSize`: The amount of data received for every request. 

Run this section and you will be able to request data from the armband. 

By running the next section `%% Real time plotting` you will get a real-time plot of the sensor data depending on what you specify in the plot function in the bottom of the section. 

Note: Depending on the bufferSize you might have to wait until the buffer is filled before the plot will show.
Note: If error occour, try restarting the ESP32 by turning the battery on and off and running the Matlab code again.
