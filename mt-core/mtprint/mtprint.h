/**
 * @ brief mtprint: serial (uart) printing functions and settings
 */
#ifndef _MT_PRINT_H_
#define _MT_PRINT_H_

//Serial buffer (string) max size
#define SERIAL_BUFFER_SIZE       7



void serial_init(void);
void print_serial(char* stringVal);


#endif /* _MT_PRINT_H_ */
