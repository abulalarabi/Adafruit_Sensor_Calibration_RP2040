# Adafruit Unified Sensor Calibration Library for RP2040
Adafruit sensor calibration causes errors with RP2040 MCUs. The problem is with the SDFAT library provided with the original bundle. I have modified the code for SDFat so that it works with RP2040. I used the LittleFS library to read/write files into the builtin flash memory. This code should work with ESP devices as well (not tested yet) as ESP devices are also supported by LittleFS library.

# Important Changes to Remember
* Currently the calibration is saved into sensor_calib.json file (even if any other file name is provided through parameters).
* Make sure LittleFS library is installed.

  # ToDo:
  * Check with ESP devices to make sure they use the SDFat instead of simulated EEPROM.
