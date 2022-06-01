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


//Serial buffer (string) max size
#define SERIAL_BUFFER_SIZE       20



//Serial commands
#define  ser_start              "cmdstart/n"
#define  ser_stop               "cmdstop/n"



void serialcomm_thread(void *p);
void commad_check(char* commandStrl);


#endif
