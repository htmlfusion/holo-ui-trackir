# Holo-UI TrackIR

Provides absolute position tracking via Linux Track and TrackIR

To compile:

    gcc -o linuxtrack_abs -g -I. linuxtrack.c linuxtrack_abs.c -ldl

Currently requires this custom build of Linux track

http://linuxtrack.eu/test/linuxtrack-0.99.12abs.dmg
