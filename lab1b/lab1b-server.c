/* Oliver Goch
   123456789
   the.oliver.goch@gmail.com
*/
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <poll.h>
#include <sys/wait.h>
#include "zlib.h"

/*For handling errors*/
char *errstr;

/*global var*/
int forkNum;
int newsockfd;
z_stream sShellToStdout;
z_stream sStdinToShell;

/*Errors*/
void pipeError(char *errstring)
{
  fprintf(stderr, "Error on pipe:\n");
  fprintf(stderr, errstring);
  fprintf(stderr, "\n");
  exit(1);
}

void readError(char *errstring)
{
  fprintf(stderr, "Error on read:\n");
  fprintf(stderr, errstring);
  fprintf(stderr, "\n");
  exit(1);
}

void writeError(char *errstring)
{
  fprintf(stderr, "Error on write:\n");
  fprintf(stderr, errstring);
  fprintf(stderr, "\n");
  exit(1);
}

void closeError(char *errstring)
{
    fprintf(stderr, "Error on close:\n");
    fprintf(stderr, errstring);
    fprintf(stderr, "\n");
    exit(1);
}

void dup2Error(char *errstring)
{
    fprintf(stderr, "Error on dup2:\n");
    fprintf(stderr, errstring);
    fprintf(stderr, "\n");
    exit(1);
}

void checkExit(void)
{
  int status;
  int wc = waitpid(forkNum, &status, 0);
  if(wc == -1)
  {
    errstr = strerror(errno);
    fprintf(stderr, "Error on waitpid:\n");
    fprintf(stderr, errstr);
    fprintf(stderr, "\n");
    exit(1);
  }

  if(WIFEXITED(status))
  {
    fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(status), WEXITSTATUS(status));
    exit(0);
  }
}

void sigHandler(int sigNum)
{
  if(sigNum == SIGPIPE)
      exit(0);
}



/*main func*/
int main(int argc, char **argv)
{
  atexit(checkExit);
  signal(SIGPIPE, sigHandler);

	static struct option longopts[] = {
    {"port=", required_argument, NULL, 'p'},
    {"compress", no_argument, NULL, 'c'},
    {0, 0, 0, 0}
  };

  /*Struct for addresses*/
  struct sockaddr_in serv_addr, cli_addr;

  int sockfd, portno;
  socklen_t clilen;
  int passFlag = 0, compFlag = 0, gotten = 0;
  /*Getoptlong function*/
  while((gotten = getopt_long(argc, argv, "p:c", longopts, NULL)) != -1)
  {
  	switch(gotten)
  	{
  		case 'p':
    		passFlag = 1;
    		portno = atoi(optarg);
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
    sShellToStdout.zalloc = Z_NULL;
    sShellToStdout.zfree = Z_NULL;
    sShellToStdout.opaque = Z_NULL;
    if(deflateInit(&sShellToStdout, Z_DEFAULT_COMPRESSION) < 0)
    {
      errstr = strerror(errno);
      fprintf(stderr, "Error on deflateInit:\r\n");
      fprintf(stderr, errstr);
      fprintf(stderr, "\r\n");
      exit(1);
    }

    sStdinToShell.zalloc = Z_NULL;
    sStdinToShell.zfree = Z_NULL;
    sStdinToShell.opaque = Z_NULL;
    if(inflateInit(&sStdinToShell) < 0)
    {
      errstr = strerror(errno);
      fprintf(stderr, "Error on inflateInit:\r\n");
      fprintf(stderr, errstr);
      fprintf(stderr, "\r\n");
      exit(1);
    }
  }

  /*open socket*/
  sockfd = socket(AF_INET, SOCK_STREAM,  0);
  if(sockfd < 0)
  {
  	errstr = strerror(errno);
  	fprintf(stderr, "Error on socket:\n");
  	fprintf(stderr, errstr);
  	fprintf(stderr, "\n");
  	exit(1);
  }

  memset((char *) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  /*Bind socket and error*/
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
  {
  	errstr = strerror(errno);
    fprintf(stderr, "Error on bind:\n");
    fprintf(stderr, errstr);
    fprintf(stderr, "\n");
    exit(1);
  }

  /*listen and error*/
  if(listen(sockfd,5) == -1)
  {
  	errstr = strerror(errno);
  	fprintf(stderr, "Error on socket:\n");
  	fprintf(stderr, errstr);
  	fprintf(stderr, "\n");
  	exit(1);
  }

	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
	{
  	errstr = strerror(errno);
  	fprintf(stderr, "Error on accept:\n");
  	fprintf(stderr, errstr);
  	fprintf(stderr, "\n");
  	exit(1);
	}

  int toCFP[2]; /*To child from parent, 0 is stdin for child and 1 is stdout for parent */
  int fromCTP[2]; /*From child to parent, 0 is stdin for parent and 1 is stdout for child*/
  /*Add pipe Errors*/
  if(pipe(toCFP) == -1)
    pipeError(strerror(errno));
  if(pipe(fromCTP) == -1)
    pipeError(strerror(errno));
      
  forkNum = fork();

  if(forkNum > 0) /*This is parent*/
  {
    if(close(toCFP[0]) == -1)
        closeError(strerror(errno));
    if(close(fromCTP[1]) == -1)
        closeError(strerror(errno));
    /*read stuff*/
    unsigned char forkBuffer[4096];
    unsigned char zbuff[4096];
    int current;
    /*poll stuff*/
    struct pollfd polls[2];
    int ret;

    polls[0].fd = newsockfd; /*this is from the client*/
    polls[1].fd = fromCTP[0]; /*this is output from shell*/
    polls[0].events = POLLIN | POLLHUP | POLLERR; /*these are only events we care about*/
    polls[1].events = POLLIN | POLLHUP | POLLERR;
    while(1)
    {
      ret = poll(polls, 2, 0);
      if(ret < 0)
      {
        errstr = strerror(errno);
        fprintf(stderr, "Error on poll:\n");
        fprintf(stderr, errstr);
        fprintf(stderr, "\n");
        exit(1);
      }
      if(polls[1].revents & (POLLHUP | POLLERR))
         exit(0);
      /*Client is talking, write to shell*/
      if(polls[0].revents & POLLIN)
      {
        current = read(newsockfd, forkBuffer, 4096);
        if(current == -1)
          readError(strerror(errno));
        /*inflate input because it was compressed*/
        if(compFlag)
        {
          sStdinToShell.avail_in = current;
          sStdinToShell.next_in = forkBuffer;
          sStdinToShell.avail_out = sizeof(zbuff);
          sStdinToShell.next_out = zbuff;
          do{
            inflate(&sStdinToShell, Z_SYNC_FLUSH);
          }while(sStdinToShell.avail_in > 0);

          unsigned int iter;

          for(iter = 0; iter < sizeof(zbuff)-sStdinToShell.avail_out; iter++)
          {  
            if(zbuff[iter] == 0x0D || zbuff[iter] == 0x0A)
            {
              if(write(toCFP[1], "\n", 1) == -1)
                writeError(strerror(errno));
            }
            else if(zbuff[iter] == 0x03)
              kill(forkNum, SIGINT);
            else if(zbuff[iter] == 0x04)
            {
              if(close(toCFP[1]) == -1)
                closeError(strerror(errno));
              if(close(newsockfd) == -1)
                closeError(strerror(errno));
              exit(0);
            }
            else
            {
              char c[1];
              c[0] = zbuff[iter];
              if(write(toCFP[1], c, 1) == -1)
                writeError(strerror(errno));
            }
          }
        }
        else
        {
          int iter;
          for(iter = 0; iter < current; iter++)
          {  
            if(forkBuffer[iter] == 0x0D || forkBuffer[iter] == 0x0A)
            {
              if(write(toCFP[1], "\n", 1) == -1)
                writeError(strerror(errno));
            }
            else if(forkBuffer[iter] == 0x03)
              kill(forkNum, SIGINT);
            else if(forkBuffer[iter] == 0x04)
            {
              if(close(toCFP[1]) == -1)
                closeError(strerror(errno));
              if(close(newsockfd) == -1)
                closeError(strerror(errno));
              exit(0);
            }
            else
            {
              char c[1];
              c[0] = forkBuffer[iter];
              if(write(toCFP[1], c, 1) == -1)
                writeError(strerror(errno));
            }
            
          }
        }
      }
        
      /*Shell is talking, write to client*/
      if(polls[1].revents & POLLIN)
      {
        current = read(fromCTP[0], forkBuffer, 4096);
        if(current == -1)
          readError(strerror(errno));
        if(compFlag)
        {
          sShellToStdout.avail_in = current;
          sShellToStdout.next_in = forkBuffer;
          sShellToStdout.avail_out = sizeof(zbuff);
          sShellToStdout.next_out = zbuff;
          do
          {
            deflate(&sShellToStdout, Z_SYNC_FLUSH);
          }while(sShellToStdout.avail_in > 0);

          if(write(newsockfd, zbuff, sizeof(zbuff) - sShellToStdout.avail_out) == -1)
            writeError(strerror(errno));
        }
        else
        {
          int iter;
          for(iter = 0; iter < current; iter++)
          {
            if(forkBuffer[iter] == 0x0D || forkBuffer[iter] == 0x0A)
            {
              if(write(newsockfd, "\n", 1) == -1)
                writeError(strerror(errno));
            }
            else
            {
              char c[1];
              c[0] = forkBuffer[iter];
              if(write(newsockfd, c, 1) == -1)
                writeError(strerror(errno));
            }
          }
        }
      }     
    }
  }
  else if(forkNum == 0) /*This is child*/
  {
    if(close(toCFP[1]) == -1)
        closeError(strerror(errno));
    if(close(fromCTP[0]) == -1)
        closeError(strerror(errno));
    if(dup2(toCFP[0], 0) == -1)
        dup2Error(strerror(errno));
    if(dup2(fromCTP[1], 1) == -1)
        dup2Error(strerror(errno));
    if(close(toCFP[0]) == -1)
        closeError(strerror(errno));
    if(close(fromCTP[1]) == -1)
        closeError(strerror(errno));

    char *args[2];
    args[0] = "/bin/bash";
    args[1] = NULL;
    if(execvp(args[0], args) == -1)
    {
     errstr = strerror(errno);
     fprintf(stderr, "Error on exec of shell:\n");
     fprintf(stderr, errstr);
     fprintf(stderr, "\n");
     exit(1);
    }
  }
  else
  {
    errstr = strerror(errno);
    fprintf(stderr, "Error on fork:\n");
    fprintf(stderr, errstr);
    fprintf(stderr, "\n");
    exit(1);
  }
  inflateEnd(&sStdinToShell);
  deflateEnd(&sShellToStdout);
	return 0;
}
