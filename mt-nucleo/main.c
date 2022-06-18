/**
 * Motion Tracking Firmware for STM32F072.Firmware requires MPU9250 sensor to be connected to Nucleo 
 * board via I2C buss-1. Firmware supports number of commands via serial port.'
*/ 

#include "ch.h"
#include "hal.h"
#include "rt_test_root.h"
#include "oslib_test_root.h"

#include "../mt-core/mtprint/mtprint.h"
#include "../mt-core/mti2c/mti2c1.h"
#include "../mt-core/mpu9250/mpu.h"
#include "../mt-core/serialcomm/serialcomm.h"





/*
 * Application entry point.
 */
int main(void) {
  
  //ChibiOS init
  halInit();
  chSysInit();

  //init serial/UART 
  serial_init();

  serial_print("\n--------------------------------------\n\r");
  serial_print("\n--- Starting MotionTracking: V:0.1 ---\n\r");
  serial_print("\n--------------------------------------\n\r");


  //init custom components
  mti2c_init();
  mpu_init();



  

  //Creates threads
  //Serial 
  (void)chThdCreateStatic(ThdSerialComm, sizeof(ThdSerialComm), NORMALPRIO +1, (tfunc_t)serialcomm_thread, NULL);
  serial_print("Serial thread: running\n\r");

  //MPU
  (void)chThdCreateStatic(ThMPU, sizeof(ThMPU), NORMALPRIO +1, (tfunc_t)mpum_thread, NULL);
  serial_print("MPU thread: running\n\r");




  //Normal main() thread
  while (true) {
    
    chThdSleepMilliseconds(1000);
  } 

  
}
