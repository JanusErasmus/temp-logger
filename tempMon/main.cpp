#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "debug.h"
#include "sht.h"

double getCPUtemp();
void logSample(time_t tm, double temp, double humid, double cpu);
int parse_param(char * string,int argc, char * argv[], char seperator);
void replaceChar(char * string, char what, char with);

int main(int argc, char ** argv)
{
	SHT21 * mon = new SHT21();

	double temp = mon->getTemp();
	printf("Temp:  %f C\n", temp);

	double humid = mon->getHumid();

	printf("Humid: %f %%\n", humid);

	double cpu = getCPUtemp();
	printf("CPU:   %f C\n", cpu);

	delete mon;

	double prevTemp = 0, prevHumid = 0, prevCpu = 0;

	while(1)
	{
		time_t now = time(0);
		diag_printf((asctime(localtime(&now))));

		mon = new SHT21();
		temp = mon->getTemp();
		humid = mon->getHumid();
		delete mon;

		if(isnan(temp) || isnan(humid))
				continue;

		cpu = getCPUtemp();

		if(		(prevTemp - 1 > temp 	|| temp > prevTemp + 1) ||
				(prevHumid - 1 > humid	|| humid > prevHumid + 1)
		)
		{
			prevTemp = temp;
			prevHumid = humid;
			prevCpu = cpu;

			logSample(now, temp, humid, cpu);
		}
		sleep(5);

	}

	return 0;
}

void logSample(time_t tm, double temp, double humid, double cpu)
{
	char fileName[64];
	struct tm *now = localtime(&tm);
	//between 0 and two is our fileName
	strftime(fileName, 64, "%a_%d_%b_%Y", now);
//	strcpy(fileName, timeString);
//	char * argv[5];
//	int argc = 5;
//	argc = parse_param(fileName, argc, argv, ' ');
////	for (int k = 0; k < argc; ++k)
////	{
////		printf(argv[k]);
////	}
//
//	*argv[4] = 0;
//	replaceChar(fileName, ' ', '_');
	strcat(fileName, ".csv");

	FILE* fd = fopen((const char*)fileName, "r");
	if(!fd)
	{
		printf("Could not open %s, Try creating it\n", fileName);

		fd  = fopen((const char*)fileName, "a");
		fprintf(fd, "Time, Time stamp, Temp, Humid, CPU\n");
	}
	else
		fd = freopen((const char*)fileName, "a", fd);

	if(!fd)
	{
		printf("Could not reopen %s\n", fileName);

		return;
	}


	strftime(fileName, 64, "%d %B %Y %H:%M", now);

	fprintf(fd, "%s, %f, %f, %f, %f\n", fileName, (double)(tm+(3600*2))/86400+25569, temp, humid, cpu);

	fclose(fd);


}

int parse_param(char * string, int argc, char *argv[], char seperator)
{
	int count = 0;
	for (unsigned int k = 0; k < strlen(string); ++k)
	{
		if(string[k] == seperator)
		{
			argv[count] = (char*)&string[k];
			count++;

			if(count > argc)
				return count;
		}
	}

	return count;
}

void replaceChar(char * string, char what, char with)
{
	for (unsigned int k = 0; k < strlen(string); ++k)
	{
		if(string[k] == what)
			string[k] = with;
	}
}



double getCPUtemp()
{
	char cmd[128];
	double cpu = -1;

	FILE* stream = popen("cat /sys/class/thermal/thermal_zone0/temp", "r");
	if(stream)
	{
		if(fgets(cmd, 128, stream))
		{
			//printf(cmd);
			cpu = (double)atoi(cmd) / 1000;
		}


		fclose(stream);
	}

	return cpu;
}
