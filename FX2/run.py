import sys
import usb.core
import usb.util

# 1. Locate FX2
dev = usb.core.find(idVendor=0x04b4, idProduct=0x8613)

if dev is None:
    print("Error: FX2 device not found!")
    sys.exit(1)

# 2. Detach kernel driver if active
try:
    if dev.is_kernel_driver_active(0):
        dev.detach_kernel_driver(0)
except (NotImplementedError, usb.core.USBError):
    pass

# 3. Claim configuration and Alt Setting 1
try:
    dev.set_configuration()
except usb.core.USBError:
    pass

try:
    dev.set_interface_altsetting(interface=0, alternate_setting=1)
except usb.core.USBError:
    pass

# 4. Target EP2 OUT (0x02) explicitly matching FX2 firmware (EP2CFG / EP2FIFOBUF)
EP2_OUT = 0x02

# 5. Read bitstream binary
with open("top.bin", "rb") as f:
    bitstream = f.read()

total_bytes = len(bitstream)
print(f"Loaded top.bin ({total_bytes} bytes). Streaming to FPGA via EP2 (0x02)...")

# 6. Stream in 512-byte chunks to match EP2 FIFO packet size
CHUNK_SIZE = 512
total_sent = 0

try:
    for i in range(0, total_bytes, CHUNK_SIZE):
        chunk = bitstream[i:i + CHUNK_SIZE]
        dev.write(EP2_OUT, chunk, timeout=10000)
        total_sent += len(chunk)
        print(f"\rProgress: {total_sent}/{total_bytes} bytes ({(total_sent/total_bytes)*100:.1f}%)", end="", flush=True)

    print("\n\nSuccess! Transferred all bytes to FPGA.")

except usb.core.USBError as e:
    print(f"\nTransfer failed at byte {total_sent}: {e}")