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
uint8_t  strbuf[SERIAL_BUFFER_SIZE];           

//Read iteration counter
int32_t rd_count = 0;

//Row axis value
static uint16_t xr_axis = 0;








/**
 * @brief     Serial communication thread main function
 */
void serialcomm_thread(void *p) {

    (void)p;
    chRegSetThreadName("Serial Comm Thread");
    chThdSetPriority(SERIALCOMM_THREAD_PRIORITY);

    //Serial transmission checking variable
    msg_t sdt = MSG_OK;

    //Init mutex
    chMtxObjectInit(&qmtx);

    chThdSleepMilliseconds(100); 



    
    while (true)
    {
        //Inpur Section
        //Process input data, only check command if read finished without error
        sdt = serial_read(strbuf);

        if(sdt != MSG_OK){
            commad_check((char*)strbuf);
        }

        //Flush (reset) input queu
        iqResetI(&SD2.iqueue);
        


        //Output section
        chMtxLock(&qmtx);
        if(read_enable && rd_count > SERIAL_COUNTER_LIMIT){

            //Rad axis
            serial_read_acc_axis(&xr_axis);


            //Output data to serial
            chprintf((BaseSequentialStream*)&SD2, "x: %x\r\n", xr_axis);

            rd_count = 0;
        }
        chMtxUnlock(&qmtx);
        

        rd_count++;
        chThdSleepMilliseconds(100); 
    }
    
}


/**
 * @brief      Serial command response, function checks if serial input
 *             matches any of the available commands
 * @param [in] commandStrl   string to be checked
 */
void commad_check(char* commandStrl){

    //Start motion tracking
    if( strcmp(commandStrl, SER_START) == 0 ){
        serial_print("cmd: mt start\n\r");
        serial_set_control(TRUE);

        chMtxLock(&qmtx);
            read_enable = TRUE;
        chMtxUnlock(&qmtx);


    //Stop motion tracking
    }else if( strcmp(commandStrl, SER_STOP) == 0 ){
        serial_print("cmd: mt stop\n\r");
        serial_set_control(FALSE);

        chMtxLock(&qmtx);
            read_enable = FALSE;
        chMtxUnlock(&qmtx);


    }else{
        serial_print("cmd: unknown command: ");
        serial_print(commandStrl);
        serial_print("\n\r");
    }


}

