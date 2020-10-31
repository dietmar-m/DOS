; :ts=8
		include	bios.i

		public	reset_disk_, read_block_, write_block_, format_track_

codeseg		segment
		assume	cs:codeseg, ss:codeseg


sector_max	equ	9
bytes_per_block	equ	2


reset_disk_	proc	near

		push	dx
		xor	dx,dx
		mov	ah,floppy_reset
		int	bios_floppy
		jc	reset_err
		xor	ax,ax
		jmp	reset_end
reset_err:
		mov	ah,80
reset_end:
		pop	dx
		ret

reset_disk_	endp


read_block_	proc	near

		mov	ah,floppy_read
		jmp	do_block

read_block_	endp


write_block_	proc	near

		mov	ah,floppy_write
		jmp	do_block

write_block_	endp


do_block	proc	near

drive_no	=	4
surface_no	=	6
track_no	=	8
sector_no	=	10
buffer		=	12
count		=	14

		push	bp
		mov	bp,sp
		push	bx
		push	cx
		push	dx
		push	es
		push	ax

		mov	dx,[bp+drive_no]
		mov	ax,[bp+surface_no]
		mov	dh,al
		mov	cx,[bp+sector_no]
		mov	ax,[bp+track_no]
		mov	ch,al
		mov	ax,ds
		mov	es,ax
		pop	ax
		mov	bx,[bp+count]
		mov	al,bl
		mov	bx,[bp+buffer]
		int	bios_floppy

		pop	es
		pop	dx
		pop	cx
		pop	bx
		jc	do_err
		mov	ax,[bp+count]
		jmp	do_end
do_err:
		mov	al,ah
		mov	ah,80h
do_end:
		pop	bp
		ret

do_block	endp


format_track_	proc	near

drive_no	=	4
surface_no	=	6
track_no	=	8
count		=	10

		push	bp
		mov	bp,sp
		push	bx
		push	cx
		push	dx
		push	si
		push	es

		mov	ax,[bp+surface_no]
		mov	dh,al
		mov	ax,[bp+track_no]
		mov	dl,al
		mov	ah,1
		mov	al,bytes_per_block
		mov	cx,[bp+count]
		mov	si,offset format_buff
format_loop0:
		mov	[si],dl
		mov	[si+1],dh
		mov	[si+2],ah
		mov	[si+3],al
		add	si,4
		inc	ah
		loop	format_loop0

		mov	ch,dl
		mov	ax,[bp+drive_no]
		mov	dl,al
		mov	bx,offset format_buff
		mov	ax,ds
		mov	es,ax
		mov	ax,[bp+count]
		mov	ah,floppy_format
		int	bios_floppy

		pop	es
		pop	si
		pop	dx
		pop	cx
		pop	bx
		pop	bp
		jc	format_err
		xor	ax,ax
		ret
format_err:
		mov	ah,80
		ret

format_track_	endp

format_buff	db	4*sector_max dup(?)

codeseg		ends

		end

