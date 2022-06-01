#include "ch.h"
#include "hal.h"
#include "mpu.h"

#include "../mtprint/mtprint.h"
#include "../mti2c/mti2c1.h"
#include "../../ChibiOS_21.11.1/os/hal/lib/streams/chprintf.h"



THD_WORKING_AREA(ThMPU, MPU_THREAD_STACK_SIZE);




//tx/rx buffers
uint8_t txbuf[5];
uint8_t rxbuf[5];


//Accelerometer scale reference value
float acc_scale_ref  = 0.0f;


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


  //reset sensor
  mpu_reset();



  //activate accelerometer and gyro
  txbuf[0] = PWR_MGMT_2;
  txbuf[1] = CONFIG_PWR_MGMT_2;

  res = i2c1_transmit(MPU_ADDR, txbuf, 2, rxbuf, 0, MPU_TIME_LIM);
  if(res == MSG_OK){
    print_serial("Activate acc and gyro: OK\n\r");
  }else{
    print_serial("Activate acc and gyro: Failed\n\r");
  }



  //set accelerometer bandwidth
  txbuf[0] = ACCEL_CONFIG_2;
  txbuf[1] = CONFIG_ACCEL_2;

  res = i2c1_transmit(MPU_ADDR, txbuf, 2, rxbuf, 0, MPU_TIME_LIM);
  if(res == MSG_OK){
    print_serial("Acc frequency set: OK\n\r");
  }else{
    print_serial("Acc frequency set: Failed\n\r");
  }



  //Set accelerometr range
  if(mpu_acc_range(ACCEL_RANGE_2G)){
    print_serial("Acc renge set: OK\n\r");
  }else{
    print_serial("Acc renge set: FailedS\n\r");
  }



  mpu_who_am_i();

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

    chThdSleepMilliseconds(100); 
  }
  

}













//Check communication to MPU by requesting read value from check register
void mpu_who_am_i(void) {

  msg_t res = MSG_OK;
  txbuf[0] = 0x75;

  res = i2c1_transmit(MPU_ADDR, txbuf, 1, rxbuf, 1, MPU_TIME_LIM);

  if(res == MSG_OK){
    chprintf((BaseSequentialStream*)&SD2, "WAI: %d\n\r", rxbuf[0]);

  }else{
    print_serial("WAI: Error: I2C\n\r");
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



