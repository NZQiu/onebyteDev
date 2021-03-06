# Build a device module

The first step of driver writing is defining the capabilities (the mechanism) the driver will offer to user programs. Our “device” is part of the computer’s memory. The device we will build is global and persistent one-byte char device. “Global” means that if the device is opened multiple times, the data contained within the device is shared by all the file descriptors that opened it. “Persistent” means that if the device is closed and reopened, data isn’t lost. This device can be fun to work with, because it can be accessed and tested using conventional commands, such as cp, cat, and shell I/O redirection. “One byte” means that this device can only deal with one byte.

To figure out device drivers in your kernel, you can use the command “ls -l /dev” and check the meaning of every column. The first letter of every line can be “c” for char device or “b” for block device. The major and minor numbers are also shown. Traditionally, the major number identifies the driver associated with the device. The minor number is used by the kernel to determine exactly which device is being referred to.

Basically, the device driver needs to be registered before using and unregistered during exit. For simplicity, we will use a classic mode of registering/unregistering device. The structure of file_operation needs to be checked. The read and write function needs to be implemented and the structure of the code is given. Read function should read the context of the one-byte device. Write function should write one byte to the device. However, if there are more than 1 bytes written to the device at the same time, the first byte will be written and there will be an error message. If you are really familiar with fancier way of implementing a char driver, you can use other approaches.

```
make && sudo insmod ./onebyte.ko && sudo mknod /dev/onebyte c 61 0
sudo rmmod onebyte && sudo rm -rf /dev/onebyte && make clean

gcc -o read read.c
gcc -o write write.c
```
