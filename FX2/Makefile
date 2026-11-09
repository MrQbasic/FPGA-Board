CC = sdcc
CFLAGS = -mmcs51
TARGET = main
VID = 04b4
PID = 8613

all: $(TARGET).ihx

$(TARGET).ihx: $(TARGET).c
	$(CC) $(CFLAGS) $(TARGET).c

flash: $(TARGET).ihx
	sudo fxload -t fx2lp -d $(VID):$(PID) -i $(TARGET).ihx

clean:
	rm -f *.asm *.cdb *.ihx *.lnk *.lst *.map *.mem *.omf *.rel *.rst *.sym

.PHONY: all flash clean