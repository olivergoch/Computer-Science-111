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
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <termios.h>

//global vars
//server stuff
int idNum = 0;
char* host = "";
int portno = -1;
int sockfd = -1;
struct sockaddr_in serv_addr;
struct hostent *server;
//temp stuff
int farFlag = 1; //set to fahrenheit by default
int logFlag = 0;
int period = 1;
const int R0 = 100000;
const int B = 4275;
FILE* logF = NULL;
int printFlag = 1;
//error
char* errstr = "";

//command handling
void commands(char* command)
{
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
		dprintf(sockfd, "%s SHUTDOWN\n", cStringTime);
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
	    { "log=", required_argument, NULL, 'l'},
	    { "id=", required_argument, NULL, 'i'},
	    { "host=", required_argument, NULL, 'h'},
	    {0, 0, 0, 0}
	  };

	int c;
	char *filein = "";
	while((c = getopt_long(argc, argv, "p:s:l:i:h:", longopts, NULL)) != -1)
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
			case 'i':
			idNum = atoi(optarg);
            if(idNum <= 0 || strlen(optarg) != 9)
            {
                fprintf(stderr, "Did not give valid argument for id\n");
                exit(1);
            }
            break;
            case 'h':
            host = optarg;
            break;
   			default:
			exit(1);
			break;
		}
	}

	if(logFlag == 0)
	{
		fprintf(stderr, "Did not provide log option\n");
		exit(1);
	}

	//port number will be last argument
	portno = atoi(argv[argc-1]);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
	{
		errstr = strerror(errno);
		fprintf(stderr, "Error on socket:\n");
		fprintf(stderr, errstr);
		fprintf(stderr, "\n");
		exit(2);
	}

	server = gethostbyname(host);
	if (server == NULL)
	{
		fprintf(stderr,"ERROR, no such host\n");
	    exit(2);
	}

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
	{
		errstr = strerror(errno);
		fprintf(stderr, "Error on connect:\n");
		fprintf(stderr, errstr);
		fprintf(stderr, "\n");
		exit(2);
	}

	//after the server is set up we send over id
	dprintf(sockfd, "ID=%d\n", idNum);
	if(logFlag)
	{
		fprintf(logF, "ID=%d\n", idNum);
		fflush(logF);
	}

	//setup time
	time_t currTime, startPer, endPer;
	struct tm* timeStuff;
	char stringTime[10];
	//step up temp
	mraa_aio_context tempSensor;
	tempSensor = mraa_aio_init(1);
	

	//set up poll
	struct pollfd readIn[1];
	int ret;
	readIn[0].fd = sockfd; //stdin
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
	    	dprintf(sockfd, "%s %.1f\n", stringTime, temperature);
			if(logFlag)
			{
				fprintf(logF, "%s %.1f\n", stringTime, temperature);
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
	            exit(2);
	   		}
			//do polling
			if((readIn[0].revents & POLLIN))
			{
				char buff[64];
				int got = read(sockfd, buff, 64);
				if(got == 0)
				{
					fprintf(stderr, "Got EOF\n");
					exit(2);
				}
				int i;
				int lengthString = 1;
				for(i = 0; i < got; i++)
				{
					if(buff[i] == '\n')
					{
						char* command = malloc((lengthString+1)*sizeof(char));
						//copy the strings!
						int j, k;
						for(j = i, k = lengthString-1; k > -1; j--, k--)
						{
							command[k] = buff[j];
							//printf("%c\n", buff[j]);
						}
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
