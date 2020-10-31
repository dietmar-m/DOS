#ifndef BOIS_H
#define BIOS_H

int reset_disk(void);
int read_block(size_t drive,size_t surface,size_t track,size_t sector,
				void *buffer,size_t count);
int write_block(size_t drive,size_t surface,size_t track,size_t sector,
				void *buffer,size_t count);
int format_track(size_t drive,size_t surface,size_t track,size_t count);

#endif

