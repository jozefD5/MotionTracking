/**
 * @brief mpu.c: MPU sensor control thread and functions  
 */
#include "ch.h"
#include "hal.h"
#include "mpu.h"
#include <math.h>

#include "../mtprint/mtprint.h"
#include "../mti2c/mti2c1.h"
#include "../../ChibiOS_21.11.1/os/hal/lib/streams/chprintf.h"



THD_WORKING_AREA(ThMPU, MPU_THREAD_STACK_SIZE);


//mutex 
static mutex_t  qmtx;

//Accelerometer range setting variable
static AccelRange accRange = ACCEL_RANGE_16G;


//tx/rx buffers
uint8_t txbuf[5];
uint8_t rxbuf[5];

//Reading enable variable
static bool read_enable = TRUE;

//Accelerometer scale reference value
static float acc_scale_ref  = 0.0f;

//Axis value
//Row axis value
static uint16_t axis_row_x = 0;
static uint16_t axis_row_y = 0;
static uint16_t axis_row_z = 0;

//Axis in g's
static float axis_x = 0.0f;
static float axis_y = 0.0f;
static float axis_z = 0.0f;

//Axis in degrees
static float axis_ang_x = 0.0f;
static float axis_ang_y = 0.0f;

 





//Reset MPU to defaault settings
static void mpu_reset(void){

  txbuf[0] = PWR_MGMT_1;
  txbuf[1] = CONFIG_PWR_MGMT_1;

  i2c1_transmit(MPU_ADDR, txbuf, 2, rxbuf, 0, MPU_TIME_LIM);

}


//Check communication to MPU by requesting read value from check register
static void mpu_who_am_i(void) {

  msg_t res = MSG_OK;
  txbuf[0] = 0x75;

  res = i2c1_transmit(MPU_ADDR, txbuf, 1, rxbuf, 1, MPU_TIME_LIM);

  if(res == MSG_OK){
    chprintf((BaseSequentialStream*)&SD2, "WAI: %d\n\r", rxbuf[0]);

  }else{
    serial_print("WAI: Error: I2C\n\r");
  }

}


//Update accelerometer axis values
static bool mpu_acc_update(void){

  msg_t res[3] = {MSG_OK, MSG_OK, MSG_OK};

  //Temp calculation values
  uint32_t x2, y2, z2;
  float results = 0.0f;
  
  
  //Read row axis data
  res[0] = mpu_read_acc_axis(ACC_AXIS_X, &axis_row_x);
  res[1] = mpu_read_acc_axis(ACC_AXIS_Y, &axis_row_y);
  res[2] = mpu_read_acc_axis(ACC_AXIS_Z, &axis_row_z);


  //If all row axis were reacived
  if( (res[0] & res[1] & res[2]) != MSG_OK){
    return false;
  }

  //Convert row to g's
  axis_x = (float)axis_row_x * acc_scale_ref;
  axis_y = (float)axis_row_y * acc_scale_ref;
  axis_z = (float)axis_row_z * acc_scale_ref;




  //Calculate angel in degree C

  //Not required as center is zero
  //axis_x -= ACC_XY_CENTER;
  //axis_y-= ACC_XY_CENTER;
  axis_z -= ACC_Z_CENTER;

  //calculate square
  x2 =   (uint32_t)(axis_x * axis_x);
  y2 =   (uint32_t)(axis_y * axis_y);
  z2 =   (uint32_t)(axis_z * axis_z);


  //x-axis
  results = sqrt(y2+z2);
  results = axis_x / results;
  axis_ang_x = atan(results);

  //Y-axis
  results = sqrt(x2+z2);
  results = axis_y / results;
  axis_ang_y = atan(results);

  return true;
}




/**
 * @brief Initiate MPU
 */
void mpu_init(void){

  msg_t res = MSG_OK;

  chMtxObjectInit(&qmtx);


  //reset sensor
  mpu_reset();

  //activate accelerometer and gyro
  txbuf[0] = PWR_MGMT_2;
  txbuf[1] = CONFIG_PWR_MGMT_2;

  res = i2c1_transmit(MPU_ADDR, txbuf, 2, rxbuf, 0, MPU_TIME_LIM);
  if(res == MSG_OK){
    serial_print("Activate acc and gyro: OK\n\r");
  }else{
    serial_print("Activate acc and gyro: Failed\n\r");
  }



  //set accelerometer bandwidth
  txbuf[0] = ACCEL_CONFIG_2;
  txbuf[1] = CONFIG_ACCEL_2;

  res = i2c1_transmit(MPU_ADDR, txbuf, 2, rxbuf, 0, MPU_TIME_LIM);
  if(res == MSG_OK){
    serial_print("Acc frequency set: OK\n\r");
  }else{
    serial_print("Acc frequency set: Failed\n\r");
  }



  //Set accelerometr range
  if(mpu_acc_range(accRange)){
    serial_print("Acc renge set: OK\n\r");
  }else{
    serial_print("Acc renge set: FailedS\n\r");
  }


  mpu_who_am_i();

}


/**
 * @brief         Read accelerometer axis value
 * @param[in]     axis_select   select required axis to be read: ACC_AXIS_X, ACC_AXIS_Y, ACC_AXIS_Z
 * @param[in/out] *axis_val     pointer to variable to store 16-bit axis value
 */
msg_t mpu_read_acc_axis(AccAxis axis_select, uint16_t *axis_val){

  msg_t res = MSG_OK;
  uint8_t val_h, val_l;


  //Read top byte
  txbuf[0] = axis_select;
  res = i2c1_transmit(MPU_ADDR, txbuf, 1, rxbuf, 1, MPU_TIME_LIM);

  if(res != MSG_OK) { return res; }
  val_h = rxbuf[0];
  //chprintf((BaseSequentialStream*)&SD2, "x: %x\r\n", rxbuf[0]);



  //Read lower byte
  ++txbuf[0];
  res = i2c1_transmit(MPU_ADDR, txbuf, 1, rxbuf, 2, MPU_TIME_LIM);

  if(res != MSG_OK) { return res; }
  val_l = rxbuf[1];
  //chprintf((BaseSequentialStream*)&SD2, "x: %x\r\n", rxbuf[1]);


  //Combine upper and lower bytes
  *axis_val  =   (val_h << 8) | val_l;
  return MSG_OK;
}


/**
 * @brief Set accelerometter range
 */
bool mpu_acc_range(AccelRange range){

  msg_t res = MSG_OK;

  txbuf[0] = ACCEL_CONFIG;
  txbuf[1] = range;

  chMtxLock(&qmtx);

  switch (range){

    case ACCEL_RANGE_4G:{
      acc_scale_ref = 4.0f / 32767.5f;
      break;
    }

    case ACCEL_RANGE_8G:{
      acc_scale_ref = 8.0f / 32767.5f;
      break;
    }

    case ACCEL_RANGE_16G:{
      acc_scale_ref = 16.0f / 32767.5f;
      break;
    }

    case ACCEL_RANGE_2G:
    default: {
      acc_scale_ref = 2.0f / 32767.5f;
      break;
    }
  }


  res = i2c1_transmit(MPU_ADDR, txbuf, 2, rxbuf, 0, MPU_TIME_LIM);
  if(res != MSG_OK){return FALSE; }


  chMtxUnlock(&qmtx);
  return TRUE;
}



/**
 * @brief Mpu reading thread
 */
void mpum_thread(void *p){

  (void)p;
  chRegSetThreadName("MPU Thread");
  chThdSetPriority(MPU_THREAD_PRIORITY);

  


  while (true)
  {

    //If red_enabled is ser via serial thread, read accelerometer data
    chMtxLock(&qmtx);

    if(read_enable){
      mpu_acc_update();
    }

    chMtxUnlock(&qmtx);

    chThdSleepMilliseconds(500); 
  }
}




//Serial Commands
/**
 * @brief Enable/Disable read operation
 */
void serial_set_control(bool set){

  chMtxLock(&qmtx);

  if(set){
    read_enable = TRUE;
  }else{
    read_enable = FALSE;
  }

  chMtxUnlock(&qmtx);
}


/**
 * @brief Request lates axis values in degrees
 */
void serial_read_accaxis_deg(float *x, float *y){
  chMtxLock(&qmtx);

    *x = axis_ang_x;
    *y = axis_ang_y;

  chMtxUnlock(&qmtx);
}


/**
 * @brief Request lates axis values in degrees
 */
void serial_read_accaxis_d(float *x, float *y){
  chMtxLock(&qmtx);

    *x = axis_x;
    *y = axis_y;

  chMtxUnlock(&qmtx);
}


/**
 * @brief Request lates axis value in g's
 */
void serial_read_accaxis_g(float *x, float *y, float *z){
  *x = axis_x;
  *y = axis_y;
  *z = axis_z;
}




