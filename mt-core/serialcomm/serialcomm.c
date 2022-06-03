/**
 * @brief Serial communication thread, handles commnication between MCU and descktop app via UART
 */
#include "ch.h"
#include "hal.h"
#include "rt_test_root.h"
#include "oslib_test_root.h"
#include "serialcomm.h"

#include <string.h>
#include "chprintf.h"
#include "../mtprint/mtprint.h"
#include "../mti2c/mti2c1.h"
#include "../mpu9250/mpu.h"


THD_WORKING_AREA(ThdSerialComm, SERIAL_THREAD_STACK_SIZE);

//mutex 
static mutex_t  qmtx;

//Enable/disable reading of acc
static bool read_enable = FALSE;


//Serial read character and charecter buffer
msg_t strChar;                                  
char  strbuf[SERIAL_BUFFER_SIZE];           


//Row axis value
static uint16_t xr_axis = 0;








/**
 * @brief     Serial communication thread main function
 */
void serialcomm_thread(void *p) {

    (void)p;
    chRegSetThreadName("Serial Comm Thread");
    chThdSetPriority(SERIALCOMM_THREAD_PRIORITY);


    chMtxObjectInit(&qmtx);





    //Setup UART read functionality 
    event_listener_t UartData;      //register event listner
    eventflags_t flags;             //event flag

    //Registers event listner on event source
    chEvtRegisterMask((event_source_t *)chnGetEventSource(&SD2), &UartData, EVENT_MASK(1));
    

    chThdSleepMilliseconds(100); 


    while (true)
    {
        //Inpur Section
        //Wait for event then read flag
        chEvtWaitOneTimeout(EVENT_MASK(1), TIME_MS2I(10));
        chSysLock();
        flags = chEvtGetAndClearFlags(&UartData);
        chSysUnlock();


        //Read serial data
        //heck flag and if data are available
        if(flags & CHN_INPUT_AVAILABLE){
           int32_t i = 0;

            //Fill serial buffer until enter is pressed or buffer is full
            do{
               strChar = chnGetTimeout(&SD2, TIME_IMMEDIATE);

                if(strChar != Q_TIMEOUT){
                    strbuf[i] = (char)strChar;
                    i++;
                }


            }while ( strChar != 0x0d && i != (SERIAL_BUFFER_SIZE -1)); 


            //Decoding serial commands
            commad_check(strbuf);
        }




        //Output section
        chMtxLock(&qmtx);

        if(read_enable){

            //Rad axis
            serial_read_acc_axis(&xr_axis);


            //Output data to serial
            //chprintf((BaseSequentialStream*)&SD2, "x: %x\r\n", xr_axis);

        }

        chMtxUnlock(&qmtx);







        chThdSleepMilliseconds(2); 
    }
    
}


/**
 * @brief                   Serial command response, function checks if serial input
 *                          matches any of the available commands
 * @param [in] commandStrl  string to be checked
 */
void commad_check(char* commandStrl){

    //Start motion tracking
    if( strcmp(commandStrl, ser_start) == 0 ){
        print_serial("cmd: mt start\n\r");

        chMtxLock(&qmtx);
            read_enable = TRUE;
        chMtxUnlock(&qmtx);

        serial_set_control(TRUE);


    //Stop motion tracking
    }else if( strcmp(commandStrl, ser_start) == 0 ){
        print_serial("cmd: mt stop\n\r");

        chMtxLock(&qmtx);
            read_enable = FALSE;
        chMtxUnlock(&qmtx);

        serial_set_control(FALSE);


    }else{
        print_serial("cmd: unknown command\n\r");
    }


}

