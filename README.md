# Chip 8 Emulator

## Screenshots

### Test file

![Test](/screenshots/Screen1.png)

### Tetris

![Tetris](/screenshots/Screen2.png)

## Building PowerShell
```
$ mkdir out/build
$ cd out/build 
$ cmake ../../
$ cmake --build . --config Release
```
## Running

Collection of Chip 8 games — https://github.com/dmatlack/chip8/tree/master/roms/games

```<Scale>``` — The scaling factor for the emulator window. Specifies how much the display should be enlarged. For example, a value of 10 will scale each pixel 10 times its original size.

```<Delay>``` — The delay between each CPU cycle in milliseconds. This parameter controls the execution speed of the program in the emulator, allowing for more accurate emulation of the original Chip 8 timing. A higher value will result in slower execution.

```<ROM>``` — The path to the ROM file to be loaded and run by the emulator. Provide a full or relative path to the Chip 8 game or program file.

```
$ ./chip8-emulator/Release/chip8.exe <Scale> <Delay> <ROM>
```

## Keypad
| Chip 8 | Keyboard|
|:------:|:-------:|
|   1    |   1     |
|   2    |   2     |
|   3    |   3     |
|   4    |   Q     |
|   5    |   W     |
|   6    |   E     |
|   7    |   A     |
|   8    |   S     |
|   9    |   D     |
|   0    |   X     |
|   A    |   Z     |
|   B    |   C     |
|   C    |   4     |
|   D    |   R     |
|   E    |   F     |
|   F    |   V     |