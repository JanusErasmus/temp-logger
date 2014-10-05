#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "debug.h"
#include "sht.h"

SHT21::SHT21(int  address)
{
	if ((fd = open("/dev/i2c-1", O_RDWR)) < 0) // Open port for reading and writing
	{
		printf("Failed to open i2c port\n");
		fd = 0;
		return;
	}

	if (ioctl(fd, I2C_SLAVE, address) < 0)// Set the port options and set the address of the device we wish to speak to
	{
		printf("Unable to get bus access to talk to slave\n");
		fd = 0;
	}
}

double SHT21::getTemp()
{
	if(!fd)
		return NAN;

	unsigned char buf[10];
	double temp = 0;

	buf[0] = 0xF3;													// Commands for performing a ranging

	if ((write(fd, buf, 1)) != 1)								// Write commands to the i2c port
	{
		printf("Error writing to i2c slave\n");
		return NAN;
	}

	usleep(100000);												// This sleep waits for the ping to come back



	if (read(fd, buf, 3) != 3)
	{								// Read back data into buf[]
		printf("Unable to read slave\n");
		return NAN;
	}
	else
	{
		uint32_t St = buf[0] << 8 | buf[1] ;
		temp = (((double)St / 65536) * 175.72) - 46.85;
	}

	diag_printf((" T %f\n", temp));

	return temp;
}

double SHT21::getHumid()
{
	if(!fd)
		return NAN;

	double humid = 0;
	unsigned char buf[10];

	buf[0] = 0xF5;

	if(write(fd, buf, 1) != 1)
	{
		printf("Error writing to sensor\n");
		return NAN;
	}

	usleep(100000);

	if(read(fd, buf, 3) != 3)
	{
		printf("Unable to read sensor\n");
		return NAN;
	}
	else
	{
		uint32_t Srh = buf[0] << 8 | buf[1];
		humid = (((double)Srh / 65536) * 125) - 6;
	}

	diag_printf((" H %f\n", humid));

	return humid;
}

SHT21::~SHT21()
{
	if(fd)
		close(fd);
}
