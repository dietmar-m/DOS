#include <stdio.h>
#include <dos.h>

DTA *getdta();
char *strrchr();
char *strchr();
long du();

int main(argc,argv)
int argc; char **argv;
{
	char *dir;
	long size;

	if(argc==1)
		dir="*.*";
	else if(argc==2)
		dir=argv[1];
	else
	{
		fprintf(stderr,"USAGE: %s [PATTERN]\n",argv[0]);
		return -1;
	}
	size=du(dir);
	printf("total of %s is %ld bytes\n",dir,size);
	return 0;
}


long du(path)
char *path;
{
	DTA *old_dta;
	DTA dta;
	long size=0;

	old_dta=getdta();
	setdta(&dta);
	if(!readfirst(path))
	{
		char buff[PATH_MAX], *p;

		strcpy(buff,path);
		p=strrchr(buff,'\\');
		if(!p)
			p=strchr(buff,':');
		if(p)
			p++;
		else
			p=buff;
		do
		{
			if(!strcmp(dta.name,".") || !strcmp(dta.name,".."))
				continue;
			if(dta.attrib & 0x10)
			{
				*p=0;
				strcat(p,dta.name);
				strcat(p,"\\*.*");
				size+=du(buff);
			}
			else
				size+=dta.size;
		} while(!readnext());
	}

	setdta(old_dta);
	return size;
}

