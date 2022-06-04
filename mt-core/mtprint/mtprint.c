/**
 * @brief mtprint: serial (uart) printing functions and settings
 */
#include "ch.h"
#include "hal.h"
#include "string.h"
#include "mtprint.h"

//mutex 
static mutex_t  qmtx;

//Serial (UART) config
static const SerialConfig myserialcfg = {
  115200,
  0,
  USART_CR2_STOP1_BITS,
  0
};




/**
 * @brief Initilise serial (UART) for communication with desktop and initilises mutex
 */ 
void serial_init(void){
  chMtxObjectInit(&qmtx);
  sdStart(&SD2, &myserialcfg);
}


/**
 * @brief Custom print function to print string to serial terminal
 * @param[in] stringVal   row string of type char[]
 */
void serial_print(char* stringVal){
  chMtxLock(&qmtx);
    sdWrite(&SD2, (uint8_t*)stringVal, strlen(stringVal));
  chMtxUnlock(&qmtx);
}


/**
 * @brief Read data from serial (input), this is maninly used to retrive data from desktop app
 * @param [in]  stringVal   pointer to buffer with lingthe of 'SERIAL_BUFFER_SIZE'
 */
msg_t serial_read(uint8_t* stringVal){
  
  chMtxLock(&qmtx);
    msg_t t = sdReadTimeout(&SD2, stringVal, SERIAL_BUFFER_SIZE, TIME_MS2I(50));
  chMtxUnlock(&qmtx);
  return t;
}
