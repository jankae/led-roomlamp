# led-roomlamp
The led-roomlamp consists of up to six LED spots connected to a central baseplate.
<p align="left">
  <img src="/pictures/WholeLamp.JPG?size=200" width="350"/>
</p>

Each spot features a 10W LED on a heatsink with its own (AVR controller) boost-converter (led-driver):
<p align="left">
  <img src="/pictures/leddriver/LEDspot.JPG?size=200" width="550"/>
</p>
The spot needs a 12V supply and can be controlled via I2C. All the spots are connected to the central led-control board on the baseplate:
<p align="left">
  <img src="/pictures/ledcontrol/ControlWithBaseplate.JPG?size=200" width="550"/>
</p>
The led-control board essentially is just an ATmega328 with a switching controller, a microphone (one of the available MAX9814 breakout-boards) and (optionally) an ESP-01.

The controller continuously samples the microphone, performs an FFT and identifies whistle tones. By whistling different frequencies, the brightness of the lamp can be controlled.

Alternatively, the lamp can be controlled via UART (38400 Baud) which is connected to the ESP. Using the [ESP-link](https://github.com/jeelabs/esp-link) firmware, commands can be send via WiFi. Send 'help' to retrieve a list of available commands.
