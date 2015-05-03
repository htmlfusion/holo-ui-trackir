all:
	gcc -o linuxtrack_abs -g -I. linuxtrack.c linuxtrack_abs.c -ldl
