#Project monitors device's mosition using accelerometer sensor (mpu9250)
#Development enviroment was setup to use Docker container (volume). To acces/extract build (.hex, .bin etc..) files need to be copyed from container tolocal machine using docker cp cpommand

#Hardware
Project 'mt-core' was setup for STM32 NUCLEO-F02EB development board
mcu: STM32F072RB
sensor: accelerometer MPU-9250


#Setps to setup Dev Enviroment
1. Instal required tools VSCode, Docker and (if working on Windows) Windows Subsistem for Linux
2. Download/clone repository
3. Open VSCode and select    File->OpenFolder->{motiont-tracking-firmware}/arm-dev-container
4. VSCode will prompt you to reopen folder as container, select YES. If you missed or declined the popup option, from command prompt (F1) select 'Remote-Containers: Open Folder in Container..'
5. Open terminal  Terminal-> New Terminal  or  Ctrl+Shift + 
   then clone repository inside the container, this allows us to work inside volume, from terminal execute
   git clone command <repository>



#Extract/Copy build from volume to local machine
1. Open terminal on local machine
2. Run command  'docker ps'  this will list running containers. We run this command to find container's ID
3. run command  'docker cp'  this will copy selected file/directory from container to local machine. Format is 
   docker cp    <source path>     <destination>     
   docker cp    CONTAINER-ID:/workspaces/motion-tracking-firmware/mt-nucleo/build       <C:\Users\User1\Documents\>


#Datasheets
[sensor datasheets](https://3cfeqx1hf82y3xcoull08ihx-wpengine.netdna-ssl.com/wp-content/uploads/2015/02/PS-MPU-9250A-01-v1.1.pdf)
[sensor reg map](https://invensense.tdk.com/wp-content/uploads/2015/02/RM-MPU-9250A-00-v1.6.pdf)


