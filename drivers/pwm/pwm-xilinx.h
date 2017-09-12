/*
* blink.h - the header file with the ioctl definitions.
*
* The declarations here have to be in a header file, because
* they need to be known both to the kernel module
* (in chardev.c) and the process calling ioctl (ioctl.c)
*/
#ifndef PWMGEN_H
#define PWMGEN_H
#include <linux/ioctl.h>
/*
* The major device number. We can't rely on dynamic
* registration any more, because ioctls need to know
* it.
*/
#define MAGIC_NUM 100
/*
* TURN ON LED ioctl
*/
#define IOCTL_SET_FREQ_0 _IOR(MAGIC_NUM, 0, char *)
#define IOCTL_SET_DUTY_0 _IOR(MAGIC_NUM, 1, char *)
#define IOCTL_SET_FREQ_1 _IOR(MAGIC_NUM, 2, char *)
#define IOCTL_SET_DUTY_1 _IOR(MAGIC_NUM, 3, char *)
#define IOCTL_SET_FREQ_2 _IOR(MAGIC_NUM, 4, char *)
#define IOCTL_SET_DUTY_2 _IOR(MAGIC_NUM, 5, char *)

#define DEBUG

#define DEVICE_FILE_NAME_0 "/dev/pwmgen0.0"
#define DEVICE_FILE_NAME_1 "/dev/pwmgen1.0"
#define DEVICE_FILE_NAME_2 "/dev/pwmgen2.0"
#endif
