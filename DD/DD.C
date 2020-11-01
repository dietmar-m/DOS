#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "bios.h"

#define BLOCK_SIZE 512
#define BLOCKS_PER_TRACK 9
#define DEFAULT_NO_TRACKS 80
#define BUFF_SIZE (BLOCKS_PER_TRACK*BLOCK_SIZE)

#define OP_NONE 0
#define OP_READ 1
#define OP_WRITE 2
#define MAX_TRIES 3

static char buffer[BUFF_SIZE];

static int usage(char *argv0)
{
	fprintf(stderr,"USAGE: %s if=<infile> of=<outfile> [tc=<trackcount>]\n",
			argv0);
	return -1;
}

int main(int argc, char **argv)
{
	int error=0;
	char *ifname=NULL;
	char *ofname=NULL;
	int tc=DEFAULT_NO_TRACKS;
	int op=OP_NONE;
	int drive;
	int fd;
	int n,t,s;

	if(argc<3)
		return usage(argv[0]);

	for(n=1; n<argc; n++)
	{
		if(!strncmp(argv[n],"if=",3))
			ifname=argv[n]+3;
		else if(!strncmp(argv[n],"of=",3))
			ofname=argv[n]+3;
		else if(!strncmp(argv[n],"tc=",3))
			tc=atoi(argv[n]+3);
		else
			return usage(argv[0]);
	}

	if(!ifname || !ofname || tc<1 || tc>80)
		return usage(argv[0]);

	if(!strcmp(ifname,"a:"))
	{
		op=OP_READ;
		drive=0;
		fd=open(ofname,O_TRUNC| O_APPEND);
	}
	else if(!strcmp(ifname,"b:"))
	{
		op=OP_READ;
		drive=1;
		fd=open(ofname,O_TRUNC| O_APPEND);
	}
	else if(!strcmp(ofname,"a:"))
	{
		op=OP_WRITE;
		drive=0;
		fd=open(ifname,O_RDONLY);
	}
	else if(!strcmp(ofname,"b:"))
	{
		op=OP_WRITE;
		drive=1;
		fd=open(ifname,O_RDONLY);
	}
	else
		return usage(argv[0]);

/*
	printf("if=%s\n",ifname);
	printf("of=%s\n",ofname);
	printf("op=%d\n",op);
	printf("drive=%d\n",drive);
	printf("tc=%d\n",tc);
*/

	if(fd<0)
	{
		fprintf(stderr,"failed to open %s\n", op==OP_READ ? ofname: ifname);
		return -1;
	}

/*
	n=reset_disk();
	if(n)
	{
		fprintf(stderr,"reset_disk returned 0x%04x\n",n);
		close(fd);
		return -1;
	}
*/

	if(op==OP_READ)
		for(t=0; !error && t<tc; t++)
			for(s=0; !error && s<2; s++)
			{
				int i;
				printf("reading track %2d, surface %1d\r",t,s);
				fflush(stdout);
				for(i=0; i<MAX_TRIES; i++)
				{
					n=read_block(drive,s,t,1,buffer,BLOCKS_PER_TRACK);
					if(n<0 && i<MAX_TRIES-1)
						reset_disk();
					else
						break;
				}
				if(n<0)
					error=n;
				else
					if(write(fd,buffer,BUFF_SIZE)<BUFF_SIZE)
						error=-1;
			}
	else
		for(t=0; !error && t<tc; t++)
			for(s=0; !error && s<2; s++)
			{
				int i,format;
				printf("writing track %2d, surface %1d\r",t,s);
				fflush(stdout);
				if(read(fd,buffer,BUFF_SIZE)==BUFF_SIZE)
				{
					for(i=0, format=1; i<MAX_TRIES; i++)
					{
						if(format)
						{
							n=format_track(drive,s,t,BLOCKS_PER_TRACK);
							if(!n)
								format=0;
						}
						if(!format)
						{
							n=write_block(drive,s,t,1,buffer,BLOCKS_PER_TRACK);
						}
						if(n<0 && i<MAX_TRIES-1)
						{
							reset_disk();
						}
						else
							break;
					}
					if(n<0)
						error=n;
				}
				else
					error=-1;
			}

	if(error && error!=-1)
		fprintf(stderr,"%s disk returned 0x%04x\n", 
				op==OP_READ ? "reading" : "writing", error);
	close(fd);

	return error;
}

