Image Processing via Convolution Kernels on DE10-Lite (for dtek)

Requirements
- RISC-V GCC toolchain.
- DE10-Lite board with USB-Blaster and VGA output (VGA Optional).
- ImageMagick (Optional but useful).

Filter selection
- SW1:0 -> Filter 1 (00 Identity, 01 Sharpen, 10 Gaussian 5×5, 11 Edge)
- SW3:2 -> Filter 2 ( -||-)
- SW4   -> Chain enable (0 = only F1, 1 = F1→F2)
- SW8   -> Override Filter 1 to Emboss
- LEDs mirror switches; LED9 enables when processing completes.

Build
- make clean && make

Run on the board
- dtekv-upload input.raw 0x01000000
- dtekv-run main.bin
- adjust switch orientation based on desired output, and press KEY1 once to process. Then end the program (to download via JTAG, keep running for VGA)
- View results via:
    - dtekv-download output.raw 0x01030000 76800 and convert to png/jpg via ImageMagick.
    - View processing in realtime through a VGA output connected to a monitor.

