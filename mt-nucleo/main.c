/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/ 

#include "ch.h"
#include "hal.h"
#include "rt_test_root.h"
#include "oslib_test_root.h"

#include "../mt-core/mtprint/mtprint.h"
#include "../mt-core/mti2c/mti2c1.h"
#include "../mt-core/mpu9250/mpu.h"
#include "../mt-core/serialcomm/serialcomm.h"



/**
 * @brief Serial communication thread
 */ 


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
