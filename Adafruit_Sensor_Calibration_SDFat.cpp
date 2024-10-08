#include "Adafruit_Sensor_Calibration.h"
#include <LittleFS.h>
#if defined(ADAFRUIT_SENSOR_CALIBRATION_USE_SDFAT)

#ifdef ADAFRUIT_SENSOR_CALIBRATION_USE_FLASH
#ifdef ADAFRUIT_SENSOR_CALIBRATION_USE_QSPIFLASH
static Adafruit_FlashTransport_QSPI flashTransport(PIN_QSPI_SCK, PIN_QSPI_CS,
                                                   PIN_QSPI_IO0, PIN_QSPI_IO1,
                                                   PIN_QSPI_IO2, PIN_QSPI_IO3);
#elif defined(ADAFRUIT_SENSOR_CALIBRATION_USE_SPIFLASH)
static Adafruit_FlashTransport_SPI flashTransport(EXTERNAL_FLASH_USE_CS,
                                                  &EXTERNAL_FLASH_USE_SPI);
#endif

static Adafruit_SPIFlash flash(&flashTransport);
static FatFileSystem fatfs;
#endif

Adafruit_Sensor_Calibration_SDFat::Adafruit_Sensor_Calibration_SDFat() {}

/**************************************************************************/
/*!
    @brief Initializes Flash and/or filesystem
    @param filename The filename we expect to find calibration in, defaults
    to "calib.json" if not supplied
    @param filesys The optional, external filesystem we'll use to access. If
    not provided, we look for an internal (Q)SPI Flash memory
    @returns False if any failure to initialize flash or filesys
*/
/**************************************************************************/
bool Adafruit_Sensor_Calibration_SDFat::begin(const char *filename,
                                              FatFileSystem *filesys) {
  // Initialize the filesystem with LittleFS LittleFS.begin();
  LittleFS.begin();


  Serial.println("Mounted filesystem!");

  //File root = LittleFS.open("sensor_calib.json");
  char file[80];
  


  if (filename) {
    _cal_filename = filename;
  } else {
    _cal_filename = "sensor_calib.json";
  }
  return true;
}

/**************************************************************************/
/*!
    @brief Save the calibration file and serialize this object's calibrations
    into JSON format
    @returns false if anything went wrong with opening the file
*/
/**************************************************************************/
bool Adafruit_Sensor_Calibration_SDFat::saveCalibration(void) {
  // if (!theFS)
  //   return false;

  File file = LittleFS.open("sensor_calib.json", "w");
  if (!file) {
    Serial.println(F("Failed to create file"));
    return false;
  }

  JsonObject root = calibJSON.to<JsonObject>();
  JsonArray mag_hard_data = root.createNestedArray("mag_hardiron");
  for (int i = 0; i < 3; i++) {
    mag_hard_data.add(mag_hardiron[i]);
  }
  JsonArray mag_soft_data = root.createNestedArray("mag_softiron");
  for (int i = 0; i < 9; i++) {
    mag_soft_data.add(mag_softiron[i]);
  }
  root["mag_field"] = mag_field;
  JsonArray gyro_zerorate_data = root.createNestedArray("gyro_zerorate");
  for (int i = 0; i < 3; i++) {
    gyro_zerorate_data.add(gyro_zerorate[i]);
  }
  JsonArray accel_zerog_data = root.createNestedArray("accel_zerog");
  for (int i = 0; i < 3; i++) {
    accel_zerog_data.add(accel_zerog[i]);
  }
  // serializeJsonPretty(root, Serial);

  // Serialize JSON to a buffer
  char buffer[1024];
  size_t n = serializeJsonPretty(root, buffer);
  file.write((const uint8_t *)buffer, n);

  
  // Close the file (File's destructor doesn't close the file)
  file.close();

  return true;
}

/**************************************************************************/
/*!
    @brief Print the raw calibration file/EEPROM data
    @returns false if anything went wrong with opening the file
*/
/**************************************************************************/
bool Adafruit_Sensor_Calibration_SDFat::printSavedCalibration(void) {
  
  File file = LittleFS.open("sensor_calib.json", "r");
  if (!file) {
    Serial.println(F("Failed to read file"));
    return false;
  }

  Serial.println("------------");
  while (file.available()) {
    Serial.write(file.read());
  }
  Serial.println("\n------------");
  file.close();
  yield();
  return true;
}

/**************************************************************************/
/*!
    @brief Load the calibration file and parse JSON into this object
    @returns false if anything went wrong with opening the file
*/
/**************************************************************************/
bool Adafruit_Sensor_Calibration_SDFat::loadCalibration(void) {

  File file = LittleFS.open("sensor_calib.json", "r");
  if (!file) {
    Serial.println(F("Failed to read file"));
    return false;
  }

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(calibJSON, file);
  if (error) {
    Serial.println(F("Failed to read file"));
    return false;
  }

  // Close the file (File's destructor doesn't close the file)
  file.close();

  for (int i = 0; i < 3; i++) {
    mag_hardiron[i] = calibJSON["mag_hardiron"][i] | 0.0;
  }
  for (int i = 0; i < 9; i++) {
    float def = 0;
    if (i == 0 || i == 4 || i == 8) {
      def = 1;
    }
    mag_softiron[i] = calibJSON["mag_softiron"][i] | def;
  }
  mag_field = calibJSON["mag_field"] | 0.0;
  for (int i = 0; i < 3; i++) {
    gyro_zerorate[i] = calibJSON["gyro_zerorate"][i] | 0.0;
  }
  for (int i = 0; i < 3; i++) {
    accel_zerog[i] = calibJSON["accel_zerog"][i] | 0.0;
  }

  return true;
}

#endif