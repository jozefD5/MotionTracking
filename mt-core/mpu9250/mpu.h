#ifndef _MPU_9250_
#define _MPU_9250_


#ifndef MPU_THREAD_STACK_SIZE
#define MPU_THREAD_STACK_SIZE   1024
#endif

#ifndef MPU_THREAD_PRIORITY
#define MPU_THREAD_PRIORITY     (HIGHPRIO - 3)
#endif

extern THD_WORKING_AREA(ThMPU, MPU_THREAD_STACK_SIZE);


/**
 * @brief Enum for setting range from, available range:
 *        2G, 4G, 8G, 16G 
 */
typedef enum {
  ACCEL_RANGE_2G = 0x00,
  ACCEL_RANGE_4G = 0x08,
  ACCEL_RANGE_8G = 0x10,
  ACCEL_RANGE_16G = 0x18

}AccelRange;


/**
 * @brief Enum foe sellecting axis (x, y or z) for reading function
 */
typedef enum{
  ACC_AXIS_X = 0x3B,
  ACC_AXIS_Y = 0x3D,
  ACC_AXIS_Z = 0x3F,
}AccAxis;




//I2C time limit
#define  MPU_TIME_LIM          100

//MPU I2C Address
#define MPU_ADDR               0x68



//Registers address
#define  PWR_MGMT_1            0x6B      //setting for power managment register 1
#define  PWR_MGMT_2            0x6C      //sensors enable/disable settings
#define  ACCEL_CONFIG          0x1C      //accelerometer settngs, range sellection and self-test
#define  ACCEL_CONFIG_2        0x1D      //accelerometer settings, frequency and low-pass filter




//Config settings
//Power managment 1
#define  PWR_MGMT_RESET        (1<<7)                              //Reset
#define  PWR_MGMT_CYCLE        (1<<5)                              //Cycle mode
#define  CONFIG_PWR_MGMT_1     (PWR_MGMT_RESET | PWR_MGMT_CYCLE)

//Power managment 2
#define  CONFIG_PWR_MGMT_2     0x00      //Enable all, accelerometer and gyro

//Acc config 2, 184 Hz bandwidth
#define  ACC_FCHOICE_B         (1<<3)    //frequencie choice
#define  ACC_DLPF              (0x02)    //low pass filter
#define  CONFIG_ACCEL_2        (ACC_FCHOICE_B | ACC_DLPF)


//brief Registers' for sensor accelerometer data
#define ACCEL_XOUT_H           0x3B     //x high byte
#define ACCEL_XOUT_L           0x3C     //x low byte



//Function Prototypes
void mpu_init(void);
void mpum_thread(void *p);

void mpu_who_am_i(void);
msg_t mpu_read_acc_axis(AccAxis axis_select, uint16_t *axis_val);








#endif