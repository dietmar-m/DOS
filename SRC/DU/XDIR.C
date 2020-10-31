#include <stdio.h>
#include <dos.h>

DTA *getdta();
char *strrchr();
char *strchr();

int main(argc,argv)
int argc; char **argv;
{
	if(argc==1)
		xdir("","*.*");
	else if(argc==2)
		xdir("",argv[1]);
	else
	{
		fprintf(stderr,"USAGE: %s [PATTERN]\n",argv[0]);
		return -1;
	}
	return 0;
}


int xdir(prefix,path)
char *prefix; char *path;
{
	static char *attributes="ADVSHR";
	int error=0;
	DTA *old_dta;
	DTA dta;

/*	printf("path=%s\n",path);
*/
	old_dta=getdta();
	setdta(&dta);
	if(!readfirst(path))
	{
		char buff[PATH_MAX], *p;
		int i;
		BYTE mask;
		char attrib[7];

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
			printf("%s",prefix);
			for(i=0, mask=0x20; mask; i++, mask>>=1)
				if(dta.attrib & mask)
					attrib[i]=attributes[i];
				else
					attrib[i]='-';
			attrib[i]=0;
			printf("%s ",attrib);
			printf("%02d:%02d:%02d ",
					HOUR(dta.time),MINUTE(dta.time),SECOND(dta.time));
			printf("%4d-%02d-%02d ",
					YEAR(dta.date),MONTH(dta.date),DAY(dta.date));
			printf("%8ld ",dta.size);
			printf("%-12.12s  ",dta.name);
			printf("\n");
			if(dta.attrib & 0x10)
			{
				char buff2[PATH_MAX];

				strcpy(buff2,prefix);
				strcat(buff2,"\t");
				*p=0;
				strcat(p,dta.name);
				strcat(p,"\\*.*");
				xdir(buff2,buff);
			}
		} while(!readnext());
	}
	else
		error=-1;

	setdta(old_dta);
	return error;
}

