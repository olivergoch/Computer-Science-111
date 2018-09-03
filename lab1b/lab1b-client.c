/* Oliver Goch
   123456789
   the.oliver.goch@gmail.com
*/
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <termios.h>
#include <poll.h>
#include <ulimit.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "zlib.h"


/*For handling errors*/
char *errstr;

/*global vars*/
int logFD;
z_stream stdinToShell;
z_stream shellToStdout;

/* Use this variable to remember original terminal attributes. */
struct termios saved_attributes;

/*Reset Input mode*/
void reset_input_mode (void)
{
  tcsetattr(STDIN_FILENO, TCSANOW, &saved_attributes);
}

void set_input_mode (void)
{
  struct termios newSet;

  /* Make sure stdin is a terminal. */
  if (!isatty(STDIN_FILENO))
    {
      fprintf(stderr, "Not a terminal.\n");
      exit(1);
    }

  /* Save the terminal attributes so we can restore them later. */
  tcgetattr(STDIN_FILENO, &saved_attributes);
  atexit(reset_input_mode);

  tcgetattr(STDIN_FILENO, &newSet);
  newSet.c_iflag = ISTRIP;/* only lower 7 bits*/
  newSet.c_oflag = 0;/* no processing*/
  newSet.c_lflag = 0;/* no processing*/
  tcsetattr(STDIN_FILENO, TCSANOW, &newSet);
}

/*Read Error func*/
void readError(char *errstring)
{
  fprintf(stderr, "Error on read:\r\n");
  fprintf(stderr, errstring);
  fprintf(stderr, "\r\n");
  exit(1);
}

void writeError(char *errstring)
{
  fprintf(stderr, "Error on write:\r\n");
  fprintf(stderr, errstring);
  fprintf(stderr, "\r\n");
  exit(1);
}

void closeError(char *errstring)
{
    fprintf(stderr, "Error on close:\n");
    fprintf(stderr, errstring);
    fprintf(stderr, "\n");
    exit(1);
}

int main(int argc, char **argv)
{

	/*struct for getopt*/
	static struct option longopts[] = {
    {"port=", required_argument, NULL, 'p'},
    {"log=", required_argument, NULL, 'l'},
    {"compress", no_argument, NULL, 'c'},
    {0, 0, 0, 0}
  };

  struct sockaddr_in serv_addr;
  struct hostent *server;

  char *logName = "";
  int portno, sockfd;
  int passFlag = 0, logFlag = 0, compFlag = 0, gotten = 0; /*This happens if port argument is not passed*/
  /*Getoptlong function*/
  while((gotten = getopt_long(argc, argv, "p:l:c", longopts, NULL)) != -1)
  {
  	switch(gotten)
  	{
  		case 'p':
  		passFlag = 1;
  		portno = atoi(optarg);
  		break;
      case 'l':
      logName = optarg;
      //logFD = open(logName, O_CREAT | O_WRONLY,  S_IRUSR | S_IWUSR);
      logFD = creat(logName, S_IRUSR | S_IWUSR);
      if(logFD == -1)
      {
        errstr = strerror(errno);
        fprintf(stderr, "Error on --log, could not create specified file:\n");
        fprintf(stderr, errstr);
        fprintf(stderr, "\n");
        exit(1);
      }
      struct stat st;
      int fileStat = stat(logName, &st);
      if(fileStat != 0)
      {
        fprintf(stderr, "Error on --log, could not read/write to file");
        fprintf(stderr, "\n");
        exit(1);
      }
      logFlag = 1;
      ulimit(UL_SETFSIZE, 10000);
      break;
      case 'c':
      compFlag = 1;
      break;
  		default:
  		exit(1);
  		break;
  	}
  }

    if(!passFlag)
    {
    	fprintf(stderr, "Did not specify port number\n");
    	exit(1);
    }

  /*Set up of compression stuff*/
    if(compFlag)
    {
      /*Setup and check deflation*/
      stdinToShell.zalloc = Z_NULL;
      stdinToShell.zfree = Z_NULL;
      stdinToShell.opaque = Z_NULL;
      if(deflateInit(&stdinToShell, Z_DEFAULT_COMPRESSION) < 0)
      {
        errstr = strerror(errno);
        fprintf(stderr, "Error on deflateInit:\r\n");
        fprintf(stderr, errstr);
        fprintf(stderr, "\r\n");
        exit(1);
      }

      shellToStdout.zalloc = Z_NULL;
      shellToStdout.zfree = Z_NULL;
      shellToStdout.opaque = Z_NULL;
      if(inflateInit(&shellToStdout) < 0)
      {
        errstr = strerror(errno);
        fprintf(stderr, "Error on inflateInit:\r\n");
        fprintf(stderr, errstr);
        fprintf(stderr, "\r\n");
        exit(1);
      }
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
	  {
	  	errstr = strerror(errno);
  		fprintf(stderr, "Error on socket:\r\n");
  		fprintf(stderr, errstr);
  		fprintf(stderr, "\r\n");
  		exit(1);
	  }

	server = gethostbyname("localhost");
	if (server == NULL)
  {
        fprintf(stderr,"ERROR, no such host\r\n");
        exit(1);
  }

  memset((char *) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  memcpy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(portno);
  if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
  {
    errstr = strerror(errno);
		fprintf(stderr, "Error on connect:\r\n");
		fprintf(stderr, errstr);
		fprintf(stderr, "\r\n");
		exit(1);
  }

  set_input_mode();

  /*Read and write with poll*/
  struct pollfd polls[2];
  int ret;
  polls[0].fd = 0; /*this is stdin*/
  polls[1].fd = sockfd; /*this is output from server*/
  polls[0].events = POLLIN | POLLHUP | POLLERR; /*these are only events we care about*/
  polls[1].events = POLLIN | POLLHUP | POLLERR;
  while(1)
  {
    ret = poll(polls, 2, 0);
    /*poll error*/
    if(ret < 0)
    {
      errstr = strerror(errno);
      fprintf(stderr, "Error on poll:\r\n");
      fprintf(stderr, errstr);
      fprintf(stderr, "\r\n");
      exit(1);
    }


    int current;
    unsigned char buff[4096];
    unsigned char zbuff[4096];
    /*STDIN is talking, write to server*/
    if(polls[0].revents & POLLIN)
    {
      current = read(0, buff, 4096);
      //fprintf(stderr, "%d\r\n", current);
      if(current == -1)
        readError(strerror(errno));
      if(compFlag)
      {
        int iter;
        for(iter = 0; iter < current; iter++)
        {
          /* CR\r = 0x0D LF\n = 0x0A */
          if(buff[iter] == 0x0D || buff[iter] == 0x0A) 
          {
              if(write(1, "\r\n", 2) == -1)
                writeError(strerror(errno));
          }
          else
          {
            if(buff[iter] == 0x03)
            {
              if(write(1, "^C", 2) == -1)
                writeError(strerror(errno));
            }
            else if(buff[iter] == 0x04)
            {
              if(write(1, "^D", 2) == -1)
                writeError(strerror(errno));
            }
            char c[1];
            c[0] = buff[iter];
            if(write(1, c, 1) == -1)
              writeError(strerror(errno));
          }
        }
        stdinToShell.avail_in = current;
        stdinToShell.next_in = buff;
        stdinToShell.avail_out = sizeof(zbuff);
        stdinToShell.next_out = zbuff;
        do
        {
          deflate(&stdinToShell, Z_SYNC_FLUSH);
        }while(stdinToShell.avail_in > 0);

        if(write(sockfd, (unsigned char*) zbuff, sizeof(zbuff)-stdinToShell.avail_out) == -1)
          writeError(strerror(errno));
        if(logFlag)
        {
          int i, num;
          char result[4096];
          int ans = sizeof(zbuff)-stdinToShell.avail_out;
          num = sprintf(result, "%d", ans);
          if(write(logFD, "SENT ", strlen("SENT ")+1) == -1)
            writeError(strerror(errno));
          for(i = 0; i < num; i++)
          {
            char c[1];
            c[0] = result[i];
            if(write(logFD, c, 1) == -1)
              writeError(strerror(errno));
          }
          if(write(logFD, " bytes: ", strlen(" bytes: ")+1) == -1)
            writeError(strerror(errno));
          if(write(logFD, zbuff, sizeof(zbuff)-stdinToShell.avail_out) == -1)
          writeError(strerror(errno));
          if(write(logFD, "\n", 1) == -1)
            writeError(strerror(errno));
        }
      }
      else
      {
        if(logFlag)
        {
          int i, num;
          char result[4096];
          num = sprintf(result, "%d", current);
          if(write(logFD, "SENT ", strlen("SENT ")) == -1)
            writeError(strerror(errno));
          for(i = 0; i < num; i++)
          {
            char c[1];
            c[0] = result[i];
            if(write(logFD, c, 1) == -1)
              writeError(strerror(errno));
          }
          if(write(logFD, " bytes: ", strlen(" bytes: ")) == -1)
            writeError(strerror(errno));
          for(i = 0; i < current; i++)
          {
            char c[1];
            c[0] = buff[i];
            if(write(logFD, c, 1) == -1)
              writeError(strerror(errno));
          }
          if(write(logFD, "\n", 1) == -1)
            writeError(strerror(errno));
        }
      int iter;
      for(iter = 0; iter < current; iter++)
      {
        /* CR\r = 0x0D LF\n = 0x0A */
        if(buff[iter] == 0x0D || buff[iter] == 0x0A) 
        {
            if(write(1, "\r\n", 2) == -1)
              writeError(strerror(errno));
            if(write(sockfd, "\n", 1) == -1)
              writeError(strerror(errno));
        }
        else
        {
          if(buff[iter] == 0x03)
          {
            if(write(1, "^C", 2) == -1)
              writeError(strerror(errno)); 
          }
          else if(buff[iter] == 0x04)
          {
            if(write(1, "^D", 2) == -1)
              writeError(strerror(errno)); 
          }
          char c[1];
          c[0] = buff[iter];
          if(write(1, c, 1) == -1)
            writeError(strerror(errno));
          if(write(sockfd, c, 1) == -1)
            writeError(strerror(errno));
        }
      }
    }  
      if(polls[0].revents & (POLLHUP | POLLERR))
         exit(0);
   }

    /*server is talking, write to STDOUT*/
    if(polls[1].revents & POLLIN)
    {
      current = read(sockfd, buff, 4096);
      if(current == -1)
        readError(strerror(errno));
      if(current == 0)
        exit(0);
      if(logFlag)
      {
        int i, num;
        char result[4096];
        num = sprintf(result, "%d", current);
        if(write(logFD, "RECEIVED ", strlen("RECEIVED ")) == -1)
          writeError(strerror(errno));
        for(i = 0; i < num; i++)
        {
          char c[1];
          c[0] = result[i];
          if(write(logFD, c, 1) == -1)
            writeError(strerror(errno));
        }
        if(write(logFD, " bytes: ", strlen(" bytes: ")) == -1)
          writeError(strerror(errno));
        for(i = 0; i < current; i++)
        {
          char c[1];
          c[0] = buff[i];
          if(write(logFD, c, 1) == -1)
            writeError(strerror(errno));
        }
        if(write(logFD, "\n", 1) == -1)
          writeError(strerror(errno));
      }
      if(compFlag)
      {
        shellToStdout.avail_in = current;
        shellToStdout.next_in = (unsigned char *) buff;
        shellToStdout.avail_out = sizeof(zbuff);
        shellToStdout.next_out = (unsigned char *) zbuff;
        do
        {
          inflate(&shellToStdout, Z_SYNC_FLUSH);
        }while(shellToStdout.avail_in > 0);

        unsigned int iter;
        for(iter = 0; iter < sizeof(zbuff)-shellToStdout.avail_out; iter++)
        {
            if(zbuff[iter] == 0x0D || zbuff[iter] == 0x0A)
            {
              if(write(1, "\r\n", 2) == -1)
                writeError(strerror(errno));
            }
            else
            {
              char c[1];
              c[0] = zbuff[iter];
              if(write(1, c, 1) == -1)
                writeError(strerror(errno));
            }
        }
      }
      else
      {
        int iter;
        for(iter = 0; iter < current; iter++)
        {
            if(buff[iter] == 0x0D || buff[iter] == 0x0A)
            {
              if(write(1, "\r\n", 2) == -1)
                writeError(strerror(errno));
            }
            else
            {
              char c[1];
              c[0] = buff[iter];
              if(write(1, c, 1) == -1)
                writeError(strerror(errno));
            }
        }
      }
      if(polls[1].revents & (POLLHUP | POLLERR))
         exit(0);
    }
  }
  deflateEnd(&stdinToShell);
  inflateEnd(&shellToStdout);
  return 0;
}
