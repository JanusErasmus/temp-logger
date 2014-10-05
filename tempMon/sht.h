#ifndef _SHT_H
#define _SHT_H
#include <math.h>

class SHT21
{
	int fd;

public:
	SHT21(int  address = 0x40);

	double getTemp();
	double getHumid();

	~SHT21();
};

#endif //_SHT_H
