/**
 * @brief Serial communication thread, handles commnication between MCU and descktop app via UART
 */
#ifndef _SERIAL_COMM_H_
#define _SERIAL_COMM_H_


#ifndef SERIAL_THREAD_STACK_SIZE
#define SERIAL_THREAD_STACK_SIZE   1024
#endif

#ifndef SERIALCOMM_THREAD_PRIORITY
#define SERIALCOMM_THREAD_PRIORITY     (HIGHPRIO - 3)
#endif


extern THD_WORKING_AREA(ThdSerialComm, SERIAL_THREAD_STACK_SIZE);



//Sets counter max limit, controls frequency of printing accelerometer data
//with delay of 100ms and counter set to 5, a frequency=2Hz
#define  SERIAL_COUNTER_LIMIT     5



//Serial commands
#define  SER_START              "cmdstr"         //start accelerometer reading
#define  SER_STOP               "cmdstp"         //stop accelerometer reading

#define  SER_REDG               "cmdred"         //read degrees
#define  SER_REGS               "cmdreg"         //read g's

#define  SER_R02                "cmdr02"         //set range to 2g
#define  SER_R04                "cmdr04"         //set range to 4g
#define  SER_R08                "cmdr08"         //set range to 8g
#define  SER_R16                "cmdr16"         //set range to 16g



void serialcomm_thread(void *p);
void commad_check(char* commandStrl);


#endif
