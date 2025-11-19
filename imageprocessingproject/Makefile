CC      = riscv32-unknown-elf-gcc
OBJCOPY = riscv32-unknown-elf-objcopy

CFLAGS  = -Os -Wall -Wextra -march=rv32im -mabi=ilp32
LDFLAGS = -nostartfiles -T dtekv-script.lds

# C sources in this folder
SRCS = main.c conv.c kernels.c dtekv-lib.c
OBJS = $(SRCS:.c=.o)

# Default: build main.bin
all: main.bin

# Link ELF – boot.o is provided by the linker script (STARTUP(boot.o)),
# but we must have it compiled in this directory.
main.elf: $(OBJS) boot.o
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

# Build boot.o from boot.S
boot.o: boot.S
	$(CC) -c -o $@ $<

# Convert ELF -> BIN
main.bin: main.elf
	$(OBJCOPY) -O binary $< $@

run: main.bin
	dtekv-run main.bin

clean:
	rm -f *.o main.elf main.bin
