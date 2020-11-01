#include <stdio.h>
#include <dos.h>

DTA *getdta();
char *strrchr();
char *strchr();

int main(argc,argv)
int argc; char **argv;
{
	char *dir;

	if(argc==1)
		dir="*.*";
	else if(argc==2)
		dir=argv[1];
	else
	{
		fprintf(stderr,"USAGE: %s [PATTERN]\n",argv[0]);
		return -1;
	}
	xdel(dir);
	return 0;
}


int xdel(path)
char *path;
{
	int error=0;
	DTA *old_dta;
	DTA dta;
	char buff[PATH_MAX], *p;

	strcpy(buff,path);
	p=strrchr(buff,'\\');
	if(!p)
		p=strchr(buff,':');
	if(p)
		p++;
	else
		p=buff;

	old_dta=getdta();
	setdta(&dta);
	if(!readfirst(path))
	{
	/*
	*/
		do
		{
			if(!strcmp(dta.name,".") || !strcmp(dta.name,".."))
				continue;
			/*
			*/
			*p=0;
			strcat(p,dta.name);
			if(dta.attrib & 0x10)
			{
				strcat(p,"\\*.*");
				xdel(buff);
			}
			else
			/*
				printf("del %s\n",buff);
			*/
				delfile(buff);
		} while(!readnext());
	}
	else
		error=-1;

	if(p!=buff)
	{
		p--;
		if(*p!=':' && p!=buff && *(p-1)!=':')
		{
			*p=0;
			/*
			printf("rmdir %s\n",buff);
			*/
			deldir(buff);
		}
	}
	setdta(old_dta);
	return error;
}

