#MT (Motion Tracking)
#Project monitors device's mosition using accelerometer sensor (mpu9250)
Project was setup to work with VSCode on Linux machine or in Docker container (volume).

#Hardware
Project 'mt-core' was setup for STM32 NUCLEO-F02EB development board
mcu: STM32F072RB
sensor: accelerometer MPU-9250


#Project Info
main function (entery point) is located in mt-nucleo/main.c
mt-core folder acts as libary and component folder which contains all other source and header files. This includes
threads, preprocessors, serial drivers, i2c driver etc.



#Docker Container-Volume
If project development takes place inside a Docker container, a build needs to be extracted to local machine
#Extract/Copy build from volume to local machine
1. Open terminal on local machine
2. Run command  'docker ps'  this will list running containers. We run this command to find container's ID
3. run command  'docker cp'  this will copy selected file/directory from container to local machine. Format is 
   docker cp    <source path>     <destination>     
   docker cp    CONTAINER-ID:/workspaces/mt-firmware/mt-nucleo/build       <C:\Users\User1\Documents\>


#Datasheets
[sensor datasheets](https://3cfeqx1hf82y3xcoull08ihx-wpengine.netdna-ssl.com/wp-content/uploads/2015/02/PS-MPU-9250A-01-v1.1.pdf)
[sensor reg map](https://invensense.tdk.com/wp-content/uploads/2015/02/RM-MPU-9250A-00-v1.6.pdf)


