//NAME:Oliver Goch
//EMAIL:the.oliver.goch@gmail.com
//ID:123456789
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <poll.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <mraa.h>
#include <mraa/aio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>

//global vars
int period = 1;
int farFlag = 1; //set to fahrenheit by default
int logFlag = 0;
int buttPress = 0;
const int R0 = 100000;
const int B = 4275;
FILE* logF = NULL;
int printFlag = 1;

//command handling
void commands(char* command)
{
	/*int i;
	int len = strlen(in);
	for(i = 0; i < len; i++)
	{
		if(in[i] == '\n')
			break;
		if(in[i] == '\0')
			return;
	}
	char* command = malloc(i*sizeof(char));
	strncpy(command, in, i);
	strcpy(command, in);*/
	//deal with log command
	if(strncmp("LOG", command, 3) ==0)
	{
		fprintf(logF, "%s\n", command);
		fflush(logF);
	}
	//deal with scale command
	if(strncmp("SCALE=", command, 5) ==0)
	{
		char comp = command[6];
		if(comp == 'F')
			farFlag = 1;
		else if(comp == 'C')
			farFlag = 0;
		if(logF)
		{
			fprintf(logF, "%s\n", command);
			fflush(logF);
		}
	}
	//deal with period command
	if(strncmp("PERIOD=", command, 6) ==0)
	{
		period = atoi(&command[7]);
        if(period <= 0)
        {
            fprintf(stderr, "Did not give valid argument for period\n");
            exit(1);
        }
		if(logF)
		{
			fprintf(logF, "%s\n", command);
			fflush(logF);
		}
	}

	//deal with start and stop
	if(strncmp("START", command, 5) == 0)
	{
		printFlag = 1;
		if(logF)
		{
			fprintf(logF, "%s\n", command);
			fflush(logF);
		}
	}

	if(strncmp("STOP", command, 4) == 0)
	{
		printFlag = 0;
		if(logF)
		{
			fprintf(logF, "%s\n", command);
			fflush(logF);
		}
	}
	//deal with off
	if(strncmp("OFF", command, 3) == 0)
	{
		time_t commandTime;
		struct tm* cTimeStuff;
		time(&commandTime);
		cTimeStuff = localtime(&commandTime);
		char cStringTime[10];
		strftime(cStringTime, 10, "%H:%M:%S", cTimeStuff);
		printf("%s ", cStringTime);
		printf("SHUTDOWN\n");
		if(logFlag)
		{
			fprintf(logF, "%s\n", command);
			fflush(logF);
			fprintf(logF, "%s ", cStringTime);
			fprintf(logF, "SHUTDOWN\n");
			fflush(logF);
		}
		exit(0);

	}

}


void buttonPresed()
{
	buttPress = 1;
}

float convertTemp(float tempin)
{

	float R = 1023.0/tempin-1.0;
    R = R0*R;

    float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15;
    if(farFlag)
    	return temperature*1.8+32;

    return	temperature;
}

int main(int argc, char **argv)
{
	static struct option longopts[] = {
	    { "period=", required_argument, NULL, 'p'},
	    { "scale=", required_argument, NULL, 's'},
	    { "logF=", required_argument, NULL, 'l'},
	    {0, 0, 0, 0}
	  };

	int c;
	char *filein = "";
	while((c = getopt_long(argc, argv, "i:o:sc", longopts, NULL)) != -1)
	{
		switch(c)
		{
			case 'p':
			period = atoi(optarg);
            if(period <= 0)
            {
                fprintf(stderr, "Did not give valid argument for period\n");
                exit(1);
            }
			break;
			case 's':
			if(strcmp(optarg, "F") == 0)
				farFlag = 1;
			else if(strcmp(optarg, "C") == 0)
				farFlag = 0;
			else
			{
				fprintf(stderr, "Did not give valid argument for scale\n");
				exit(1);
			}
			break;
			case 'l':
			logFlag = 1;
			filein = optarg;
			logF = fopen(filein, "w");
			if(logF == NULL)
			{
				fprintf(stderr, "Error creating or opening file\n");
				exit(1);
			}
			break;
			default:
			exit(1);
			break;
		}
	}
	//setup time
	time_t currTime, startPer, endPer;
	struct tm* timeStuff;
	char stringTime[10];
	//step up temp
	mraa_aio_context tempSensor;
	tempSensor = mraa_aio_init(1);

	//set up button
	mraa_gpio_context button;
	button = mraa_gpio_init(60);
	mraa_gpio_dir(button, MRAA_GPIO_IN);
	

	//set up poll
	struct pollfd readIn[1];
	int ret;
	readIn[0].fd = 0; //stdin
	readIn[0].events = POLLIN;

   	while(1)
   	{
   		//get time
   		time(&currTime);
		timeStuff = localtime(&currTime);
		strftime(stringTime, 10, "%H:%M:%S", timeStuff);
		//get temp
   		float tempin = mraa_aio_read(tempSensor);
    	float temperature = convertTemp(tempin);
    	//print everything
    	if(printFlag)
    	{
	    	printf("%s ", stringTime);
			printf("%.1f\n",temperature);
			if(logFlag)
			{
				fprintf(logF, "%s ", stringTime);
				fprintf(logF, "%.1f\n",temperature);
				fflush(logF);
			}
		}
		//this handles the period
		time(&startPer);
		time(&endPer);
		//this spins until the wait period is over
		while(difftime(endPer, startPer) < period)
		{
			ret = poll(readIn, 1, 0);
	   		if(ret < 0)
	   		{
	   			int err = errno;
	   			char *errstr = strerror(err);
	            fprintf(stderr, "Error on poll:\n");
	           	fprintf(stderr, errstr);
	            fprintf(stderr, "\n");
	            exit(1);
	   		}
			//if the button gets pressed
			mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &buttonPresed, NULL);
			if(buttPress)
			{
				time(&currTime);
				timeStuff = localtime(&currTime);
				strftime(stringTime, 10, "%H:%M:%S", timeStuff);
				printf("%s ", stringTime);
				printf("SHUTDOWN\n");
				if(logFlag)
				{
					fprintf(logF, "%s ", stringTime);
					fprintf(logF, "SHUTDOWN\n");
					fflush(logF);
				}
				exit(0);
			}
			//do polling
			if((readIn[0].revents & POLLIN))
			{
				char buff[64];
				int got = read(0, buff, 64);
				if(got == 0)
				{
					fprintf(stderr, "Got EOF\n");
					exit(1);
				}
				int i;
				int lengthString = 1;
				for(i = 0; i < got; i++)
				{
					if(buff[i] == '\n')
					{
						char* command = malloc((lengthString+1)*sizeof(char));
						//strncpy(command, buff, lengthString);
						//copy the strings!
						int j, k;
						for(j = i, k = lengthString-1; k > -1; j--, k--)
							command[k] = buff[j];
						command[lengthString] = '\0';
						commands(command);
						lengthString = 0;
					}
					lengthString++;
				}
			}
			time(&endPer);
		}
	}
	return 0;
}
