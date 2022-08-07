# Audio Video player for the Bornhack Badge 2022

This is a crude fork of the [ST7735 TFT-Library for the Raspberry Pi Pico](https://github.com/bablokb/pico-st7735).

The AM audio transmitter is implemented based on the PWM example from the [Pico examples](https://github.com/raspberrypi/pico-examples).


# Building and flashing

```
cd demo

# Download a video
yt-dlp https://www.youtube.com/watch?v=kuNixp-wvWM

# Convert the video and audio to raw data
./doit.sh

# Program the raw data to certain locations on the external flash
picotool load -v -t bin video_data.bin -o 0x10010000
picotool load -v -t bin audio_data.raw -o 0x10800000

# Prepare the cmake build directory and compile
mkdir build
cd build
cmake ..
make -j8

# Now put the badge in boot mode and program the uf2
sudo mount /dev/sda1 /mnt/usb && sudo cp demo/demo.uf2 /mnt/usb/ && sync && sudo umount /mnt/usb

# Done
