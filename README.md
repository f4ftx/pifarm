# piFarm
piFarm is a personal indoor farm with GUI running on raspberry pi, written in C.

![piFarmscreenshot](https://raw.github.com/f4ftx/pifarm/master/doc/img/piFarmScr.png?raw=true)

### Features
- very simple GUI interface
- manual mode or auto program mode
- temperature / humidity monitoring
- simply configurable by configuration file

### Hardware requirements
- 1 Raspberry pi computer
- 1 cheap 16 relay module
- 1 bme280 i2c module
- at least 3 DC voltage regulator modules (3.3v, 5v, 12v)
- a suitable power supply
- desired actuators (led strip, water pump, fan, heater ... )
- [optional] touchscreen

__Important note :__
Raspberry pi GPIO logic output is 3.3V. My relay board has 5V logic and relays
are powered by a 12V input. Relay board logic is indirectly powered by a 5V
regulator connected to the 12V input. In order to make it work with the
raspberry pi, i had to unsolder the 5V regulator onbord, and power logic via an
extern 3.3 DC source.

### Software requirements
- Raspbian distro (with desktop)

### License
Copyright © 2020, Erwan STIPON. Released under the GNU General Public License v3.0.

