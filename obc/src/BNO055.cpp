/***************************************************************************
  This is a library for the BNO055 orientation sensor

  Designed specifically to work with the Adafruit BNO055 Breakout.

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products

  These sensors use I2C to communicate, 2 pins are required to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by KTOWN for Adafruit Industries.

  MIT license, all text above must be included in any redistribution
 ***************************************************************************/

#include <math.h>
#include <limits.h>
#include <iostream>
#include <unistd.h> // for usleep 
#include <sys/ioctl.h>

#include "BNO055.h"
#include "I2C.h"

using namespace exploringBB;

/***************************************************************************
 CONSTRUCTOR
 ***************************************************************************/

/**************************************************************************/
/*!
    @brief  Instantiates a new BNO055 class
*/
/**************************************************************************/
BNO055::BNO055(unsigned int I2CBus,uint8_t address)
  : I2CDevice(I2CBus, address)
{
  _sensorID = 0;
  _address = address;
  _i2cChannel = I2CBus;
}

/***************************************************************************
 PUBLIC FUNCTIONS
 ***************************************************************************/

/**************************************************************************/
/*!
    @brief  Sets up the HW
*/
/**************************************************************************/
bool BNO055::begin(adafruit_bno055_opmode_t mode)
{

  std::cout << "BNO begin" << std::endl; 

  /* Make sure we have the right device */
  uint8_t id = read8(BNO055_CHIP_ID_ADDR);

  if (id != BNO055_ID)
  {
    usleep(1000 * 1000);
    id = read8(BNO055_CHIP_ID_ADDR);
    if (id != BNO055_ID)
    {
      std::cout << "WRONG ID" << std::endl;
      return false; // still not? ok bail
    }
  }

  /* Switch to config mode (just in case since this is the default) */
  setMode(OPERATION_MODE_CONFIG);

  write8(BNO055_PAGE_ID_ADDR, 0);

  /* Reset */
  write8(BNO055_SYS_TRIGGER_ADDR, 0x20);
  std::cout << "Write to page sys trigger addr" << BNO055_SYS_TRIGGER_ADDR <<  std::endl; 

  while (read8(BNO055_CHIP_ID_ADDR) != BNO055_ID)
  {
    usleep(100000*5);
      std::cout << "Read Chip addr id : " << BNO055_CHIP_ID_ADDR <<  std::endl; 
  }
  usleep(1000*50); 


  /* Set to normal power mode */
  write8(BNO055_PWR_MODE_ADDR, POWER_MODE_NORMAL);
   std::cout << "Write power mode " << POWER_MODE_NORMAL << "  address:" << BNO055_PWR_MODE_ADDR <<  std::endl; 

  usleep(1000*10);

  write8(BNO055_SYS_TRIGGER_ADDR, 0x0);
  std::cout << "Write Trigger : " << BNO055_SYS_TRIGGER_ADDR <<  std::endl; 

  usleep(1000*10);
  /* Set the requested operating mode (see section 3.3) */
  setMode(OPERATION_MODE_NDOF);
  std::cout << "Set mode : " << OPERATION_MODE_NDOF <<  std::endl; 

  usleep(1000*20);

  return true;
}

/**************************************************************************/
/*!
    @brief  Puts the chip in the specified operating mode
*/
/**************************************************************************/
void BNO055::setMode(adafruit_bno055_opmode_t mode)
{
  _mode = mode;
  write8(BNO055_OPR_MODE_ADDR, _mode);
  std::cout << "Write mode : " << BNO055_OPR_MODE_ADDR <<  std::endl; 

  usleep(1000*30);
}

/**************************************************************************/
/*!
    @brief  Use the external 32.768KHz crystal
*/
/**************************************************************************/
void BNO055::setExtCrystalUse(bool usextal)
{
  adafruit_bno055_opmode_t modeback = _mode;

  /* Switch to config mode (just in case since this is the default) */
  setMode(OPERATION_MODE_CONFIG);
  usleep(1000*25);
  write8(BNO055_PAGE_ID_ADDR, 0);
  if (usextal)
  {
    write8(BNO055_SYS_TRIGGER_ADDR, 0x80);
  }
  else
  {
    write8(BNO055_SYS_TRIGGER_ADDR, 0x00);
  }
  usleep(1000*10);
  /* Set the requested operating mode (see section 3.3) */
  setMode(modeback);
  usleep(1000*20);
}

/**************************************************************************/
/*!
    @brief  Gets the latest system status info
*/
/**************************************************************************/
int8_t BNO055::getSystemStatus(void)
{
  write8(BNO055_PAGE_ID_ADDR, 0);

  /* System Status (see section 4.3.58)
     ---------------------------------
     0 = Idle
     1 = System Error
     2 = Initializing Peripherals
     3 = System Iniitalization
     4 = Executing Self-Test
     5 = Sensor fusio algorithm running
     6 = System running without fusion algorithms */

  int8_t status = read8(BNO055_SYS_STAT_ADDR);

  /* Self Test Results (see section )
     --------------------------------
     1 = test passed, 0 = test failed

     Bit 0 = Accelerometer self test
     Bit 1 = Magnetometer self test
     Bit 2 = Gyroscope self test
     Bit 3 = MCU self test

     0x0F = all good! */

  // int8_t self_test_result = read8(BNO055_SELFTEST_RESULT_ADDR);

  /* System Error (see section 4.3.59)
     ---------------------------------
     0 = No error
     1 = Peripheral initialization error
     2 = System initialization error
     3 = Self test result failed
     4 = Register map value out of range
     5 = Register map address out of range
     6 = Register map write error
     7 = BNO low power mode not available for selected operat ion mode
     8 = Accelerometer power mode not available
     9 = Fusion algorithm configuration error
     A = Sensor configuration error */

  // int8_t system_error = read8(BNO055_SYS_ERR_ADDR);

  usleep(1000*200);
  return status;
}

/**************************************************************************/
/*!
    @brief  Gets the chip revision numbers
*/
/**************************************************************************/
void BNO055::getRevInfo(adafruit_bno055_rev_info_t *info)
{
  uint8_t a, b;

  memset(info, 0, sizeof(adafruit_bno055_rev_info_t));

  /* Check the accelerometer revision */
  info->accel_rev = read8(BNO055_ACCEL_REV_ID_ADDR);

  /* Check the magnetometer revision */
  info->mag_rev = read8(BNO055_MAG_REV_ID_ADDR);

  /* Check the gyroscope revision */
  info->gyro_rev = read8(BNO055_GYRO_REV_ID_ADDR);

  /* Check the SW revision */
  info->bl_rev = read8(BNO055_BL_REV_ID_ADDR);

  a = read8(BNO055_SW_REV_ID_LSB_ADDR);
  b = read8(BNO055_SW_REV_ID_MSB_ADDR);
  info->sw_rev = (((uint16_t)b) << 8) | ((uint16_t)a);
}

/**************************************************************************/
/*!
    @brief  Gets current calibration state.  Each value should be a uint8_t
            pointer and it will be set to 0 if not calibrated and 3 if
            fully calibrated.
*/
/**************************************************************************/
void BNO055::getCalibration(uint8_t *sys, uint8_t *gyro, uint8_t *accel, uint8_t *mag)
{
  uint8_t calData = read8(BNO055_CALIB_STAT_ADDR);
  if (sys != NULL)
  {
    *sys = (calData >> 6) & 0x03;
  }
  if (gyro != NULL)
  {
    *gyro = (calData >> 4) & 0x03;
  }
  if (accel != NULL)
  {
    *accel = (calData >> 2) & 0x03;
  }
  if (mag != NULL)
  {
    *mag = calData & 0x03;
  }
}

/**************************************************************************/
/*!
    @brief  Gets the temperature in degrees celsius
*/
/**************************************************************************/
int8_t BNO055::getTemp(void)
{
  int8_t temp = (int8_t)(read8(BNO055_TEMP_ADDR));
  return temp;
}

/**************************************************************************/
/*!
    @brief  Gets a vector reading from the specified source
*/
/**************************************************************************/
imu::Vector<3> BNO055::getVector(adafruit_vector_type_t vector_type)
{
  imu::Vector<3> xyz;
  uint8_t buffer[6];
  memset(buffer, 0, 6);

  int16_t x, y, z;
  x = y = z = 0;

  /* Read vector data (6 bytes) */
  readLen((adafruit_bno055_reg_t)vector_type, buffer, 6);

  x = ((int16_t)buffer[0]) | (((int16_t)buffer[1]) << 8);
  y = ((int16_t)buffer[2]) | (((int16_t)buffer[3]) << 8);
  z = ((int16_t)buffer[4]) | (((int16_t)buffer[5]) << 8);

  /* Convert the value to an appropriate range (section 3.6.4) */
  /* and assign the value to the Vector type */
  switch (vector_type)
  {
  case VECTOR_MAGNETOMETER:
    /* 1uT = 16 LSB */
    xyz[0] = ((double)x) / 16.0;
    xyz[1] = ((double)y) / 16.0;
    xyz[2] = ((double)z) / 16.0;
    break;
  case VECTOR_GYROSCOPE:
    /* 1rps = 900 LSB */
    xyz[0] = ((double)x) / 900.0;
    xyz[1] = ((double)y) / 900.0;
    xyz[2] = ((double)z) / 900.0;
    break;
  case VECTOR_EULER:
    /* 1 degree = 16 LSB */
    xyz[0] = ((double)x) / 16.0;
    xyz[1] = ((double)y) / 16.0;
    xyz[2] = ((double)z) / 16.0;
    break;
  case VECTOR_ACCELEROMETER:
  case VECTOR_LINEARACCEL:
  case VECTOR_GRAVITY:
    /* 1m/s^2 = 100 LSB */
    xyz[0] = ((double)x) / 100.0;
    xyz[1] = ((double)y) / 100.0;
    xyz[2] = ((double)z) / 100.0;
    break;
  }

  return xyz;
}

/**************************************************************************/
/*!
    @brief  Gets a quaternion reading from the specified source
*/
/**************************************************************************/
imu::Quaternion BNO055::getQuat(void)
{
  uint8_t buffer[8];
  memset(buffer, 0, 8);

  int16_t x, y, z, w;
  x = y = z = w = 0;

  /* Read quat data (8 bytes) */
  readLen(BNO055_QUATERNION_DATA_W_LSB_ADDR, buffer, 8);
  w = (((uint16_t)buffer[1]) << 8) | ((uint16_t)buffer[0]);
  x = (((uint16_t)buffer[3]) << 8) | ((uint16_t)buffer[2]);
  y = (((uint16_t)buffer[5]) << 8) | ((uint16_t)buffer[4]);
  z = (((uint16_t)buffer[7]) << 8) | ((uint16_t)buffer[6]);

  /* Assign to Quaternion */
  /* See http://ae-bst.resource.bosch.com/media/products/dokumente/bno055/BST_BNO055_DS000_12~1.pdf
     3.6.5.5 Orientation (Quaternion)  */
  const double scale = (1.0 / (1 << 14));
  imu::Quaternion quat(scale * w, scale * x, scale * y, scale * z);
  return quat;
}

/**************************************************************************/
/*!
    @brief  Provides the sensor_t data for this sensor
*/
/**************************************************************************/
void BNO055::getSensor(sensor_t *sensor)
{
  /* Clear the sensor_t object */
  memset(sensor, 0, sizeof(sensor_t));

  /* Insert the sensor name in the fixed length char array */
  strncpy(sensor->name, "BNO055", sizeof(sensor->name) - 1);
  sensor->name[sizeof(sensor->name) - 1] = 0;
  sensor->version = 1;
  sensor->sensor_id = _sensorID;
  sensor->type = SENSOR_TYPE_ORIENTATION;
  sensor->min_delay = 0;
  sensor->max_value = 0.0F;
  sensor->min_value = 0.0F;
  sensor->resolution = 0.01F;
}

/**************************************************************************/
/*!
@brief  Reads the sensor's offset registers into a byte array
*/
/**************************************************************************/
bool BNO055::getSensorOffsets(uint8_t *calibData)
{
  if (isFullyCalibrated())
  {
    adafruit_bno055_opmode_t lastMode = _mode;
    setMode(OPERATION_MODE_CONFIG);

    readLen(ACCEL_OFFSET_X_LSB_ADDR, calibData, NUM_BNO055_OFFSET_REGISTERS);

    setMode(lastMode);
    return true;
  }
  return false;
}

/**************************************************************************/
/*!
@brief  Reads the sensor's offset registers into an offset struct
*/
/**************************************************************************/
bool BNO055::getSensorOffsets(adafruit_bno055_offsets_t &offsets_type)
{
  if (isFullyCalibrated())
  {
    adafruit_bno055_opmode_t lastMode = _mode;
    setMode(OPERATION_MODE_CONFIG);
    usleep(1000*25);

    offsets_type.accel_offset_x = (read8(ACCEL_OFFSET_X_MSB_ADDR) << 8) | (read8(ACCEL_OFFSET_X_LSB_ADDR));
    offsets_type.accel_offset_y = (read8(ACCEL_OFFSET_Y_MSB_ADDR) << 8) | (read8(ACCEL_OFFSET_Y_LSB_ADDR));
    offsets_type.accel_offset_z = (read8(ACCEL_OFFSET_Z_MSB_ADDR) << 8) | (read8(ACCEL_OFFSET_Z_LSB_ADDR));

    offsets_type.gyro_offset_x = (read8(GYRO_OFFSET_X_MSB_ADDR) << 8) | (read8(GYRO_OFFSET_X_LSB_ADDR));
    offsets_type.gyro_offset_y = (read8(GYRO_OFFSET_Y_MSB_ADDR) << 8) | (read8(GYRO_OFFSET_Y_LSB_ADDR));
    offsets_type.gyro_offset_z = (read8(GYRO_OFFSET_Z_MSB_ADDR) << 8) | (read8(GYRO_OFFSET_Z_LSB_ADDR));

    offsets_type.mag_offset_x = (read8(MAG_OFFSET_X_MSB_ADDR) << 8) | (read8(MAG_OFFSET_X_LSB_ADDR));
    offsets_type.mag_offset_y = (read8(MAG_OFFSET_Y_MSB_ADDR) << 8) | (read8(MAG_OFFSET_Y_LSB_ADDR));
    offsets_type.mag_offset_z = (read8(MAG_OFFSET_Z_MSB_ADDR) << 8) | (read8(MAG_OFFSET_Z_LSB_ADDR));

    offsets_type.accel_radius = (read8(ACCEL_RADIUS_MSB_ADDR) << 8) | (read8(ACCEL_RADIUS_LSB_ADDR));
    offsets_type.mag_radius = (read8(MAG_RADIUS_MSB_ADDR) << 8) | (read8(MAG_RADIUS_LSB_ADDR));

    setMode(lastMode);
    return true;
  }
  return false;
}

/**************************************************************************/
/*!
@brief  Writes an array of calibration values to the sensor's offset registers
*/
/**************************************************************************/
void BNO055::setSensorOffsets(const uint8_t *calibData)
{
  adafruit_bno055_opmode_t lastMode = _mode;
  setMode(OPERATION_MODE_CONFIG);
  usleep(1000*25);

  /* A writeLen() would make this much cleaner */
  write8(ACCEL_OFFSET_X_LSB_ADDR, calibData[0]);
  write8(ACCEL_OFFSET_X_MSB_ADDR, calibData[1]);
  write8(ACCEL_OFFSET_Y_LSB_ADDR, calibData[2]);
  write8(ACCEL_OFFSET_Y_MSB_ADDR, calibData[3]);
  write8(ACCEL_OFFSET_Z_LSB_ADDR, calibData[4]);
  write8(ACCEL_OFFSET_Z_MSB_ADDR, calibData[5]);

  write8(GYRO_OFFSET_X_LSB_ADDR, calibData[6]);
  write8(GYRO_OFFSET_X_MSB_ADDR, calibData[7]);
  write8(GYRO_OFFSET_Y_LSB_ADDR, calibData[8]);
  write8(GYRO_OFFSET_Y_MSB_ADDR, calibData[9]);
  write8(GYRO_OFFSET_Z_LSB_ADDR, calibData[10]);
  write8(GYRO_OFFSET_Z_MSB_ADDR, calibData[11]);

  write8(MAG_OFFSET_X_LSB_ADDR, calibData[12]);
  write8(MAG_OFFSET_X_MSB_ADDR, calibData[13]);
  write8(MAG_OFFSET_Y_LSB_ADDR, calibData[14]);
  write8(MAG_OFFSET_Y_MSB_ADDR, calibData[15]);
  write8(MAG_OFFSET_Z_LSB_ADDR, calibData[16]);
  write8(MAG_OFFSET_Z_MSB_ADDR, calibData[17]);

  write8(ACCEL_RADIUS_LSB_ADDR, calibData[18]);
  write8(ACCEL_RADIUS_MSB_ADDR, calibData[19]);

  write8(MAG_RADIUS_LSB_ADDR, calibData[20]);
  write8(MAG_RADIUS_MSB_ADDR, calibData[21]);

  setMode(lastMode);
}

/**************************************************************************/
/*!
@brief  Writes to the sensor's offset registers from an offset struct
*/
/**************************************************************************/
void BNO055::setSensorOffsets(const adafruit_bno055_offsets_t &offsets_type)
{
  adafruit_bno055_opmode_t lastMode = _mode;
  setMode(OPERATION_MODE_CONFIG);
  usleep(1000*25);

  write8(ACCEL_OFFSET_X_LSB_ADDR, (offsets_type.accel_offset_x) & 0x0FF);
  write8(ACCEL_OFFSET_X_MSB_ADDR, (offsets_type.accel_offset_x >> 8) & 0x0FF);
  write8(ACCEL_OFFSET_Y_LSB_ADDR, (offsets_type.accel_offset_y) & 0x0FF);
  write8(ACCEL_OFFSET_Y_MSB_ADDR, (offsets_type.accel_offset_y >> 8) & 0x0FF);
  write8(ACCEL_OFFSET_Z_LSB_ADDR, (offsets_type.accel_offset_z) & 0x0FF);
  write8(ACCEL_OFFSET_Z_MSB_ADDR, (offsets_type.accel_offset_z >> 8) & 0x0FF);

  write8(GYRO_OFFSET_X_LSB_ADDR, (offsets_type.gyro_offset_x) & 0x0FF);
  write8(GYRO_OFFSET_X_MSB_ADDR, (offsets_type.gyro_offset_x >> 8) & 0x0FF);
  write8(GYRO_OFFSET_Y_LSB_ADDR, (offsets_type.gyro_offset_y) & 0x0FF);
  write8(GYRO_OFFSET_Y_MSB_ADDR, (offsets_type.gyro_offset_y >> 8) & 0x0FF);
  write8(GYRO_OFFSET_Z_LSB_ADDR, (offsets_type.gyro_offset_z) & 0x0FF);
  write8(GYRO_OFFSET_Z_MSB_ADDR, (offsets_type.gyro_offset_z >> 8) & 0x0FF);

  write8(MAG_OFFSET_X_LSB_ADDR, (offsets_type.mag_offset_x) & 0x0FF);
  write8(MAG_OFFSET_X_MSB_ADDR, (offsets_type.mag_offset_x >> 8) & 0x0FF);
  write8(MAG_OFFSET_Y_LSB_ADDR, (offsets_type.mag_offset_y) & 0x0FF);
  write8(MAG_OFFSET_Y_MSB_ADDR, (offsets_type.mag_offset_y >> 8) & 0x0FF);
  write8(MAG_OFFSET_Z_LSB_ADDR, (offsets_type.mag_offset_z) & 0x0FF);
  write8(MAG_OFFSET_Z_MSB_ADDR, (offsets_type.mag_offset_z >> 8) & 0x0FF);

  write8(ACCEL_RADIUS_LSB_ADDR, (offsets_type.accel_radius) & 0x0FF);
  write8(ACCEL_RADIUS_MSB_ADDR, (offsets_type.accel_radius >> 8) & 0x0FF);

  write8(MAG_RADIUS_LSB_ADDR, (offsets_type.mag_radius) & 0x0FF);
  write8(MAG_RADIUS_MSB_ADDR, (offsets_type.mag_radius >> 8) & 0x0FF);

  setMode(lastMode);
}

bool BNO055::isFullyCalibrated(void)
{
  uint8_t system, gyro, accel, mag;
  getCalibration(&system, &gyro, &accel, &mag);
  if (system < 3 || gyro < 3 || accel < 3 || mag < 3)
    return false;
  return true;
}

/***************************************************************************
 PRIVATE FUNCTIONS
 ***************************************************************************/

/**************************************************************************/
/*!
    @brief  Writes an 8 bit value over I2C
*/
/**************************************************************************/
bool BNO055::write8(adafruit_bno055_reg_t reg, uint8_t value)
{
  writeRegister(reg, value);

  /* ToDo: Check for error! */
  return true;
}

/**************************************************************************/
/*!
    @brief  Reads an 8 bit value over I2C
*/
/**************************************************************************/
uint8_t BNO055::read8(adafruit_bno055_reg_t reg)
{
  uint8_t value = readRegister(reg);

  return value;
}

/**************************************************************************/
/*!
    @brief  Reads the specified number of bytes over I2C
*/
/**************************************************************************/
bool BNO055::readLen(adafruit_bno055_reg_t reg, uint8_t *buffer, uint8_t len)
{

  int BRead = i2cReadI2CBlockData(reg, (char *)buffer, len);

  if (BRead != (int)len)
    return -1;

  /* ToDo: Check for errors! */
  return true;
}

int BNO055::i2cReadI2CBlockData(unsigned reg, char *buf, unsigned count)
{
  uint32_t size; 
  union my_smbus_data data;

  if (count == 32){
    size = 6;
  }
  else{
    size = 8; 
  }
  
  data.block[0] = count;

  int status;

  status = (my_smbus_access(
      file, 1, reg, size, &data));

  if (status < 0) {
    std::cout << "Could not read block" << std::endl;
  }

  if (data.block[0] <= 32)
  {
    for (int i=0; i<data.block[0]; i++){
      buf[i] = data.block[i+1];
    }
  } 
  else {
    std::cout << "I2C read block data failed" << std::endl;
  }
  
  return data.block[0];
}

int BNO055::my_smbus_access(
   int fd, char rw, uint8_t cmd, int size, union BNO055::my_smbus_data *data)
{
   struct my_smbus_ioctl_data args;

  //  DBG(DBG_INTERNAL, "rw=%d reg=%d cmd=%d data=%s",
  //     rw, cmd, size, myBuf2Str(data->byte+1, (char*)data));

   args.read_write = rw;
   args.command    = cmd;
   args.size       = size;
   args.data       = data;

   return ioctl(fd, 0x0720, &args);
}
