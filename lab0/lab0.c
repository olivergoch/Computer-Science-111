/* Oliver Goch
   123456789
   the.oliver.goch@gmail.com
*/
#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

void segFaultFunc()
{
  char *sacrifice = NULL;
  *sacrifice = 0;
}

void sighandler(int signum)
{
  fprintf(stderr, "Caught segmentation fault\n");
  fprintf(stderr, strerror(signum));
  fprintf(stderr, "\n");
  exit(4);
}

int main(int argc, char **argv)
{
  /*flags for catch and segfault along with default values for fd0 and fd1*/
  int catchFlag, segFaultFlag, fd0 = 0;
  int fd1 = 1; 
  /*This is the optional command line args*/
  static struct option longopts[] = {
    { "input=", required_argument, NULL, 'i'},
    { "output=", required_argument, NULL, 'o'},
    { "segfault", no_argument, NULL, 's'},
    { "catch", no_argument, NULL, 'c'},
    {0, 0, 0, 0}
  };
  /*This is all the file and getopt vars*/
  char *filein, *fileout = "";
  int c;
  int fdz;
  int fdz1;
  /*This is the getopt statement*/
  while((c = getopt_long(argc, argv, "i:o:sc", longopts, NULL)) != -1)
    {
      switch(c)
	{
	case 'i':
	  filein = optarg;
	  fdz = open(filein, O_RDONLY);
	  /*file error*/
	  if(fdz == -1)
	    {
	      int fileinerr = errno;
	      char *errstr = strerror(fileinerr);
	      fprintf(stderr, "Error on --input, could not open specified file:\n");
	      fprintf(stderr, errstr);
	      fprintf(stderr, "\n");
	      exit(2);
	    }
	  close(0);
	  fd0 = dup(fdz);
	  break;
	case 'o':
	  fileout = optarg;
	  fdz1 = creat(fileout, S_IRUSR | S_IWUSR);
	  if(fdz1 == -1)
	    {
	      int fileouterr = errno;
	      char *errstr = strerror(fileouterr);
	      fprintf(stderr, "Error on --output, could not create specified file:\n");
	      fprintf(stderr, errstr);
	      fprintf(stderr, "\n");
	      exit(3);
	    }
	  close(1);
	  fd1 = dup(fdz1);
	  break;
	case 'c':
	  catchFlag = 1;
	  break;
	case 's':
	  segFaultFlag = 1;
	  break;
	default:
	  exit(1);
	  break;
	}
    }
  
  /*Set up the handler*/
  if(catchFlag)
    signal(SIGSEGV, sighandler);

  /*Cause a segfault*/
  if(segFaultFlag)
    segFaultFunc();

  /*Read bytes in one at a time and writes them*/
  char buff[1];
  int curr = read(fd0, buff, 1);
  while(curr > 0)
    {
      write(fd1, buff, 1);
      curr = read(fd0, buff, 1);
    }
  exit(0);    
}

