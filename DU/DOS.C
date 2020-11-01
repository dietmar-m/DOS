#include <dos.h>

DTA *getdta()
{
	union REGS regs;

	regs.h.ah=0x2f;
	sysint(0x21,&regs,&regs);
	return (DTA *)regs.x.bx;
}

void setdta(dta)
DTA *dta;
{
	struct SREGS sregs;
	union REGS regs;

	segread(&sregs);
	regs.x.ds=sregs.ds;
	regs.h.ah=0x1a;
	regs.x.dx=(int)dta;
	sysint(0x21,&regs,&regs);
}

int readfirst(path)
char *path;
{
	struct SREGS sregs;
	union REGS regs;

	segread(&sregs);
	regs.x.ds=sregs.ds;
	regs.h.ah=0x4e;
	regs.x.cx=0xffff;
	regs.x.dx=(int)path;
	if(sysint(0x21,&regs,&regs) & 0x01)
		return -1;
	return 0;
}

int readnext()
{
	union REGS regs;

	regs.h.ah=0x4f;
	if(sysint(0x21,&regs,&regs) & 0x01)
		return -1;
	return 0;
}

static int delentry(code,path)
BYTE code; char *path;
{
	struct SREGS sregs;
	union REGS regs;

	segread(&sregs);
	regs.x.ds=sregs.ds;
	regs.h.ah=code;
	regs.x.dx=(int)path;
	if(sysint(0x21,&regs,&regs) & 0x01)
		return -1;
	return 0;
}

int delfile(path)
char *path;
{
	return delentry(0x41,path);
}

int deldir(path)
char *path;
{
	return delentry(0x3a,path);
}

