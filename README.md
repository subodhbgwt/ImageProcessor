# Image processing on the DE10-Lite (RISC-V)

Convolution image filters written in C, running on a RISC-V soft processor on a DE10-Lite FPGA board. This was our final project in IS1500 Computer Organization and Components at KTH.

![Input and output](project/input.png)

## What it does

- Five filters: identity, sharpen, 5x5 Gaussian blur, edge detection and emboss
- Two filters can be chained, for example blur and then edge detection
- The filters are picked with the switches on the board
- The result shows on a VGA monitor as it's being processed
- 320x240 grayscale RAW images are uploaded to and downloaded from the board
- Performance counters measure how many cycles each convolution takes

## How it fits together

```
 Host PC  --upload-->  DE10-Lite (RISC-V)  --VGA-->  Monitor
          <-download-    load image
                         filter 1
                         filter 2 (optional)
                         write to VGA and RAM
                              ^
                              | switches pick the filters
```

## Files

```
project/          the image processing project
  imageproc.c     main loop and filter control
  conv.c/.h       convolution
  kernels.c/.h    filter kernels
  vga.c/.h        VGA framebuffer driver
  perf.c/.h       performance counters
  labmain.c       entry point
  boot.S          RISC-V boot code
  timetemplate.S  timer and interrupt template
  dtekv-lib.*     board support library
  input.png       sample input
  output.png      sample output
labs/             course labs (pointers and primes in C, I/O in assembly, timers, interrupts)
riscv32tests/     RISC-V instruction tests
```

## Building and running

You need the RISC-V GCC cross-compiler and a DE10-Lite with a USB-Blaster. A VGA monitor and ImageMagick are optional.

```bash
cd project
make clean && make

dtekv-upload input.raw 0x01000000              # 320x240 grayscale RAW image
dtekv-run main.bin
# set the switches, then press KEY1 to process
dtekv-download output.raw 0x01030000 76800     # get the result back
```

### Switches

| Switch | Function |
|--------|----------|
| SW1:0 | Filter 1 (00 identity, 01 sharpen, 10 Gaussian, 11 edge) |
| SW3:2 | Filter 2 (same mapping) |
| SW4 | 0 = filter 1 only, 1 = filter 1 then filter 2 |
| SW8 | Use emboss as filter 1 |

LED9 lights up when processing is done.

## License and credits

BSD-style license, see [COPYING](project/COPYING).

The board framework is by Artur Podobas, Wiktor Szczerek and Pedro Antunes at KTH. The project itself is by Subodh Bhagwat and Tingyuan Hu.
