/**
 * @brief mti2c: I2C controll function  
 */
#ifndef _MTCORE_I2C1_H_
#define _MTCORE_I2C1_H_

//#include "ch.h"



void mti2c_init(void);
msg_t i2c1_transmit(i2caddr_t addr, const uint8_t *txbuf, size_t txsize, uint8_t *rxbuf, size_t rxsize, systime_t timeout);



#endif /* MTCORE_I2C1_H_ */
