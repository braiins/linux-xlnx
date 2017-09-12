/*
* blink.h - the header file with the ioctl definitions.
*
* The declarations here have to be in a header file, because
* they need to be known both to the kernel module
* (in chardev.c) and the process calling ioctl (ioctl.c)
*/
#ifndef VIDGEN_H
#define VIDGEN_H
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
#define IOCTL_SET_VALUE_0 _IOR(MAGIC_NUM, 0, char *)

#define DEBUG
#define DEVICE_FILE_NAME_0 "/dev/vidgen0.0"
#endif
