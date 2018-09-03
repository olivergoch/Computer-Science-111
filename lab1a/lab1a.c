/* Oliver Goch
   123456789
   the.oliver.goch@gmail.com
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <poll.h>
#include <signal.h>
#include <errno.h>

/* Use this variable to remember original terminal attributes. */
struct termios saved_attributes;
/*Global vars for entire program*/
int forkFlag;
pid_t forkNum;
int err;
char *errstr;

void reset_input_mode (void)
{
  tcsetattr(STDIN_FILENO, TCSANOW, &saved_attributes);
  if(forkFlag)
    {
      int status;
      int wc = waitpid(forkNum, &status, 0);
      if(wc == -1)
	{
	  err = errno;
	  errstr = strerror(err);
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

}

void set_input_mode (void){
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
  // newSet.c_lflag &= ~(ICANON|ECHO);
  newSet.c_iflag = ISTRIP;/* only lower 7 bits*/
  newSet.c_oflag = 0;/* no processing*/
  newSet.c_lflag = 0;/* no processing*/
  tcsetattr(STDIN_FILENO, TCSANOW, &newSet);
}

void sigHandler(int sigNum)
{
  if(sigNum == SIGPIPE)
      exit(0);
}

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

int main(int argc, char **argv)
{
  /*Set the new input mode*/
  set_input_mode();
  /*Opt long args*/
  static struct option longopts[] = {
    {"shell", no_argument, NULL, 's'},
    {0, 0, 0, 0}
  
  };
  
  forkFlag = 0;
  int gotten = 0;
  /*Getoptlong function*/
  while((gotten = getopt_long(argc, argv, "s", longopts, NULL)) != -1)
    {
      switch(gotten)
	{
	case 's':
	  signal(SIGPIPE, sigHandler);
    	  forkFlag = 1;
	  break;
	default:
	  exit(1);
	  break;
	}
    }

  if(forkFlag == 1)
    {
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
	  char forkBuffer[256];
	  int current;
	  /*poll stuff*/
	  struct pollfd polls[2];
	  int ret;

	  polls[0].fd = 0; /*this is stdin*/
	  polls[1].fd = fromCTP[0]; /*this is output from shell*/
	  polls[0].events = POLLIN | POLLHUP | POLLERR; /*these are only events we care about*/
	  polls[1].events = POLLIN | POLLHUP | POLLERR;
	  while(1)
	    {
      	      ret = poll(polls, 2, 0);
	      if(ret < 0)
            {
                errstr = strerror(err);
                fprintf(stderr, "Error on poll:\n");
                fprintf(stderr, errstr);
                fprintf(stderr, "\n");
                exit(1);
            }
	      if(polls[0].revents & POLLIN)
		{
		  current = read(0, forkBuffer, 256);
		  if(current == -1)
            readError(strerror(errno));
		  int iter;
		  for(iter = 0; iter < current; iter++)
		    {  
		      if(forkBuffer[iter] == 0x0D || forkBuffer[iter] == 0x0A)
			{
			  if(write(1, "\r\n", 2) == -1)
			    writeError(strerror(errno));
			  if(write(toCFP[1], "\n", 1) == -1)
			    writeError(strerror(errno));
			}
		      else if(forkBuffer[iter] == 0x03)
			kill(forkNum, SIGINT);
		      else if(forkBuffer[iter] == 0x04)
			{
			  if(close(toCFP[1]) == -1)
			    closeError(strerror(errno));
			  exit(0);
			}
		      else
			{
			  char c[1];
			  c[0] = forkBuffer[iter];
			  if(write(1, c, 1) == -1)
			    writeError(strerror(errno));
			  if(write(toCFP[1], c, 1) == -1)
			    writeError(strerror(errno));
			}
		      
		    }
		  
		  if(polls[0].revents & (POLLHUP | POLLERR))
		     exit(0);
		}
	      if(polls[1].revents & POLLIN)
		{
		  current = read(fromCTP[0], forkBuffer, 256);
            if(current == -1)
            readError(strerror(errno));
		  int iter;
		  for(iter = 0; iter < current; iter++)
		    {
		      if(forkBuffer[iter] == 0x0A)
                {
                    if(write(1, "\r\n", 2) == -1)
                    writeError(strerror(errno));
                }
		      else
			{
			  char c[1];
			  c[0] = forkBuffer[iter];
			  if(write(1, c, 1) == -1)
			    writeError(strerror(errno));
			}
		    }
		  if(polls[0].revents & (POLLHUP | POLLERR))
		     exit(0);
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
    }
  char buff[100];
  int curr = read(0, buff, 100);
    if(curr == -1)
    readError(strerror(errno));
  while(curr > 0)
    {
      int i;
      for(i = 0; i < curr; i++)
	{
      /*If read gets ^D*/
      if(buff[i] == 0x04)
        exit(0);
      /*If read gets CF or LF*/
      else if(buff[i] == 0x0D || buff[i] == 0x0A)
        {
            if(write(1, "\r\n", 2) == -1)
            writeError(strerror(errno));
        }
      else
	{
	  char c[1];
	  c[0] = buff[i];
        if(write(1, c, 1) == -1)
        writeError(strerror(errno));
	}
	}
      curr = read(0, buff, 100);
      if(curr == -1)
        readError(strerror(errno));
    }
  return 0;
}
