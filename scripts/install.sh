#!/bin/bash

cd /home/pi

# update system
sudo apt-get update
sudo apt-get upgrade -y
sudo apt-get install -y git

# get program
git clone https://github.com/f4ftx/pifarm

# install dependencies
sudo apt-get install wiringpi -y
sudo apt-get install xorg xorg-dev -y
sudo apt-get install i2c-tools -y

# build
cd pifarm
make release
cd -

# install configurations
cp ./pifarm/cfg/xerverrc .xserverrc
cp ./pifarm/cfg/xinitrc .xinitrc
cp ./pifarm/cfg/pifarmrc .pifarmrc


#TODO : ENABLE I2C
#TODO : CONFIGURE SCREEN
