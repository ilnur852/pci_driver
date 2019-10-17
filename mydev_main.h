#ifndef MYDEV_H
#define MYDEV_H

#include <linux/types.h>

struct my_driver_priv {
	u8 __iomem *hwmem;
};

#endif