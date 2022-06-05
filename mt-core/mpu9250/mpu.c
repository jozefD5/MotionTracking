/**
 * @brief mpu.c: MPU sensor control thread and functions  
 */
#include "ch.h"
#include "hal.h"
#include "mpu.h"

#include "../mtprint/mtprint.h"
#include "../mti2c/mti2c1.h"
#include "../../ChibiOS_21.11.1/os/hal/lib/streams/chprintf.h"



THD_WORKING_AREA(ThMPU, MPU_THREAD_STACK_SIZE);


//mutex 
static mutex_t  qmtx;

//Accelerometer range setting variable
AccelRange accRange = ACCEL_RANGE_2G;


//tx/rx buffers
uint8_t txbuf[5];
uint8_t rxbuf[5];

//Reading enable variable
static bool read_enable = FALSE;

//Accelerometer scale reference value
float acc_scale_ref  = 0.0f;

//Row axis value
uint16_t xr_axis = 0;


//Acc in g's
float xg_axis = 0.0f;






//Reset MPU to defaault settings
static void mpu_reset(void){

  txbuf[0] = PWR_MGMT_1;
  txbuf[1] = CONFIG_PWR_MGMT_1;

  i2c1_transmit(MPU_ADDR, txbuf, 2, rxbuf, 0, MPU_TIME_LIM);

}


//Set accelerometter range
static bool mpu_acc_range(AccelRange range){

  msg_t res = MSG_OK;

  txbuf[0] = ACCEL_CONFIG;
  txbuf[1] = range;

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


  return TRUE;
}


//Initiate MPU
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


//Check communication to MPU by requesting read value from check register
void mpu_who_am_i(void) {

  msg_t res = MSG_OK;
  txbuf[0] = 0x75;

  res = i2c1_transmit(MPU_ADDR, txbuf, 1, rxbuf, 1, MPU_TIME_LIM);

  if(res == MSG_OK){
    chprintf((BaseSequentialStream*)&SD2, "WAI: %d\n\r", rxbuf[0]);

  }else{
    serial_print("WAI: Error: I2C\n\r");
  }

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
 * Update accelerometer axis values
 */
void mpu_acc_update(void){

  mpu_read_acc_axis(ACC_AXIS_X, &xr_axis);


  xg_axis = (float)xr_axis * acc_scale_ref;
  
  chprintf((BaseSequentialStream*)&SD2, "x: %f\r\n", acc_scale_ref);
  chprintf((BaseSequentialStream*)&SD2, "x: %d\r\n", xr_axis);
  chprintf((BaseSequentialStream*)&SD2, "x: %.6f\r\n\n", xg_axis);

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
 * @brief Request lates axis values
 */
void serial_read_acc_axis(float *x_g){
  chMtxLock(&qmtx);
    *x_g = xg_axis;
  chMtxUnlock(&qmtx);
}




