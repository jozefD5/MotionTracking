/**
 * @ brief mtprint: serial (uart) printing functions and settings
 */
#include "ch.h"
#include "hal.h"
#include "string.h"
#include "mtprint.h"

//mutex 
static mutex_t  qmtx;

//config
static const SerialConfig myserialcfg = {
  115200,
  0,
  USART_CR2_STOP1_BITS,
  0
};




//Activates the serial driver
/**
 * @brief Initilise serial (UART) for communication with desktop and initilises mutex
 */ 
void serial_init(void){
  chMtxObjectInit(&qmtx);
  sdStart(&SD2, &myserialcfg);
}



/**
 * @brief   Custom print function to print string to serial terminal
 * @param[stringVal]  row string of type char[]
 */
void print_serial(char* stringVal){
  chMtxLock(&qmtx);
  sdWrite(&SD2, (uint8_t*)stringVal, strlen(stringVal));
  chMtxUnlock(&qmtx);
}


void read_serial(uint8_t* stringVal){
  chMtxLock(&qmtx);

  sdRead(&SD2, stringVal, SERIAL_BUFFER_SIZE);
  //sdReadTimeout(&SD2, stringVal, 6, TIME_MS2I(50));

  chMtxUnlock(&qmtx);
}
