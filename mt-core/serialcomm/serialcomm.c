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
static bool read_enable = TRUE;

//Reading option 0=g's and 1=degrees, default option is g's
static uint32_t readOption = 0;


//Serial read character and charecter buffer
msg_t strChar;                                  
static uint8_t  strbuf[SERIAL_BUFFER_SIZE];           

//Read iteration counter
static int32_t rd_count = 0;

//Axis value
static float axis_x = 0.0f;
static float axis_y = 0.0f;
static float axis_z = 0.0f;



/**
 * @brief Serial communication thread main function
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
            
          
            //Read accelerometer axis in required format, degrees or g's
            if(readOption == 1){

                //serial_read_accaxis_deg(&axis_x, &axis_y);
                serial_read_accaxis_d(&axis_x, &axis_y);

                //output data to serial
                chprintf((BaseSequentialStream*)&SD2, "x: %.3f\r\n", axis_x);
                chprintf((BaseSequentialStream*)&SD2, "y: %.3f\r\n\n", axis_y);

            }else{

                //serial_read_accaxis_deg(&axis_x, &axis_y);
                serial_read_accaxis_g(&axis_x, &axis_y, &axis_z);

                //output data to serial
                chprintf((BaseSequentialStream*)&SD2, "x: %.3f\r\n", axis_x);
                chprintf((BaseSequentialStream*)&SD2, "y: %.3f\r\n", axis_y);
                chprintf((BaseSequentialStream*)&SD2, "z: %.3f\r\n\n", axis_z);
            }
            rd_count = 0;
        }
        chMtxUnlock(&qmtx);
        

        rd_count++;
        chThdSleepMilliseconds(100); 
    }
    
}


/**
 * @brief Serial command response, function checks if serial input
 *        matches any of the available commands
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


    //Set accelerometer reading format to degrees
    }else if(strcmp(commandStrl, SER_REDG) == 0){
        chMtxLock(&qmtx);
            readOption = 1;
        chMtxUnlock(&qmtx);

    //Set accelerometer reading format to g's
    }else if(strcmp(commandStrl, SER_REGS) == 0){
        chMtxLock(&qmtx);
            readOption = 0;
        chMtxUnlock(&qmtx);


    //Set accelerometer range to 2g
    }else if(strcmp(commandStrl, SER_R02) == 0){
        chMtxLock(&qmtx);
            mpu_acc_range(ACCEL_RANGE_2G);
        chMtxUnlock(&qmtx);

    //Set accelerometer range to 4g
    }else if(strcmp(commandStrl, SER_R04) == 0){
        chMtxLock(&qmtx);
            mpu_acc_range(ACCEL_RANGE_4G);
        chMtxUnlock(&qmtx);

    //Set accelerometer range to 8g
    }else if(strcmp(commandStrl, SER_R08) == 0){
        chMtxLock(&qmtx);
            mpu_acc_range(ACCEL_RANGE_8G);
        chMtxUnlock(&qmtx);
    
    //Set accelerometer range to 16g
    }else if(strcmp(commandStrl, SER_R16) == 0){
        chMtxLock(&qmtx);
            mpu_acc_range(ACCEL_RANGE_16G);
        chMtxUnlock(&qmtx);
    
    
    //Default, unknown command
    }else{
        serial_print("cmd: unknown command: ");
        serial_print(commandStrl);
        serial_print("\n\r");
    }


}

