/**
 * @brief mti2c: I2C controll function  
 */
#include "ch.h"
#include "hal.h"

#include <stdio.h>


#include "../mtprint/mtprint.h"
#include "mti2c1.h"


//uart buffer
char uart_buf[30];


//I2C driver config
static const I2CConfig i2ccfg = {
  STM32_TIMINGR_PRESC(15U) |
  STM32_TIMINGR_SCLDEL(4U) | STM32_TIMINGR_SDADEL(2U) |
  STM32_TIMINGR_SCLH(15U)  | STM32_TIMINGR_SCLL(21U),
  0,
  0
};



/**
 * @brief Start I2C-1 and setup required pins
 */
void mti2c_init(void){
  i2cStart(&I2CD1, &i2ccfg);

  palSetPadMode(GPIOB, 8, PAL_MODE_ALTERNATE(1));
  palSetPadMode(GPIOB, 9, PAL_MODE_ALTERNATE(1));
}


/**
 * @brief   Transmitt via i2c bus-1, function is alo abe to transmitt and then recieve data
 * @param[in] addr      i2c addres of target device
 * @param[in] *txbuf    pointer to transmission buffer
 * @param[in] txsize    number of bytes to be send
 * @param[in] *rxbuf    pointer to reacieve buffer
 * @param[in] rxsize    number of bytes to reacive, set to zero for transmission only
 * @param[in] timeout   number of ticks to wait for transmission to be available
 */
msg_t i2c1_transmit(
    i2caddr_t      addr,
    const uint8_t  *txbuf,
    size_t         txsize,
    uint8_t        *rxbuf,
    size_t         rxsize,
    systime_t      timeout
    )
{

  msg_t res = MSG_OK;


  i2cAcquireBus(&I2CD1);
  res = i2cMasterTransmitTimeout(&I2CD1, addr, txbuf, txsize, rxbuf, rxsize, timeout);
  i2cReleaseBus(&I2CD1);

  return res;
}



