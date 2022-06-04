/**
 * @ brief mtprint: serial (uart) printing functions and settings
 */
#ifndef _MT_PRINT_H_
#define _MT_PRINT_H_

//Serial buffer (string) max size
#define SERIAL_BUFFER_SIZE       6



void serial_init(void);
void serial_print(char* stringVal);
msg_t serial_read(uint8_t* stringVal);

#endif /* _MT_PRINT_H_ */
