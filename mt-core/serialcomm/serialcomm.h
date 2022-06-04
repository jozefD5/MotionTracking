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



void serialcomm_thread(void *p);
void commad_check(char* commandStrl);


#endif
