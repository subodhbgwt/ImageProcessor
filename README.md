# 🖼️ Image Processing on DE10-Lite (RISC-V)

Real-time image processing using convolution kernels, running on a **DE10-Lite FPGA** via a custom RISC-V soft processor. Built as the final project for **IS1500 Computer Organization and Components** at KTH Royal Institute of Technology.

![Input → Output](project/input.png)

---

## ✨ Features

- **5 convolution filters** — Identity, Sharpen, Gaussian Blur (5×5), Edge Detection, Emboss
- **Filter chaining** — Apply two filters sequentially (e.g., Blur → Edge Detection)
- **Hardware switch control** — Select filters and toggle chaining via physical DIP switches
- **VGA output** — View processed images in real-time on a connected monitor
- **UART upload/download** — Transfer 320×240 grayscale RAW images to/from the board
- **Performance counters** — Measure cycle counts for convolution operations

## 🏗️ Architecture

```
┌─────────────┐     UART      ┌──────────────────────┐     VGA
│  Host PC    │ ──────────▶   │  DE10-Lite (RISC-V)  │ ──────────▶  Monitor
│  (upload/   │               │                      │
│   download) │   ◀──────────  │  • Load image        │
└─────────────┘     UART      │  • Apply Filter 1    │
                              │  • Apply Filter 2    │
                              │  • Output to VGA/RAM │
                              └──────────────────────┘
                                     ▲
                                     │ DIP Switches
                                     │ (filter select)
```

## 🗂️ Repository Structure

```
├── project/              # Main image processing project
│   ├── imageproc.c       # Core processing loop & filter control
│   ├── conv.c / conv.h   # Convolution engine
│   ├── kernels.c / .h    # Filter kernel definitions
│   ├── vga.c / vga.h     # VGA framebuffer driver
│   ├── perf.c / perf.h   # Performance counter utilities
│   ├── labmain.c         # Entry point
│   ├── boot.S            # RISC-V boot assembly
│   ├── timetemplate.S    # Timer/interrupt template
│   ├── dtekv-lib.*       # Board support library
│   ├── input.png         # Sample input image
│   └── output.png        # Sample processed output
├── labs/                  # Course lab exercises
│   ├── lab2/             # Pointers, primes, sieves (C)
│   ├── lab3/             # I/O, hex display (Assembly + C)
│   ├── time4int/         # Interrupt-driven timer
│   └── time4timer/       # Hardware timer lab
├── riscv32tests/         # RISC-V instruction test suite
└── .gitignore
```

## 🔧 Build & Run

### Requirements
- RISC-V GCC cross-compilation toolchain
- DE10-Lite board with USB-Blaster
- VGA monitor (optional, for real-time output)
- ImageMagick (optional, for image conversion)

### Build
```bash
cd project
make clean && make
```

### Run on DE10-Lite
```bash
# Upload a 320×240 grayscale RAW image
dtekv-upload input.raw 0x01000000

# Flash and run the program
dtekv-run main.bin

# Configure switches, press KEY1 to process

# Download the result
dtekv-download output.raw 0x01030000 76800
```

### Switch Configuration

| Switch | Function |
|--------|----------|
| SW1:0 | Filter 1 select (00=Identity, 01=Sharpen, 10=Gaussian, 11=Edge) |
| SW3:2 | Filter 2 select (same mapping) |
| SW4 | Chain enable (0=Filter 1 only, 1=Filter 1 → Filter 2) |
| SW8 | Override Filter 1 to Emboss |

LED9 lights up when processing is complete.

## 📜 License

BSD-style license — see [COPYING](project/COPYING) for details.

**Original framework:** Artur Podobas, Wiktor Szczerek, Pedro Antunes (KTH)
**Project work:** Subodh Bhagwat, Tingyuan Hu
