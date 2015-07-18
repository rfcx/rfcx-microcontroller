#RFCx Microcontroller Software
@gibsjose, @Jesse-Millwood, @gaberex, @topherwhite
###8 July 2015

##Description
This is the code for the ATmega328P microcontroller on the RFCx board responsible for taking temperature, humidity, and power measurements and sending them to the Guardian App for display and/or transmission.

##AVRISP mkII
To configure the AVRISP mkII on Ubuntu, the following extra steps need to be taken. The steps are also outlined [here](http://stackoverflow.com/questions/5412727/avrisp-mkii-doesnt-work-with-avrdude-in-linux).

###Create a new `udev` rules file:
```bash
cd /etc/udev/
sudo touch avrisp.rules
```
The file should contain the following:

```bash
SUBSYSTEM!="usb_device", ACTION!="add", GOTO="avrisp_end"

# Atmel Corp. JTAG ICE mkII
ATTR{idVendor}=="03eb", ATTR{idProduct}=="2103", MODE="660", GROUP="dialout"
# Atmel Corp. AVRISP mkII
ATTR{idVendor}=="03eb", ATTR{idProduct}=="2104", MODE="660", GROUP="dialout"
# Atmel Corp. Dragon
ATTR{idVendor}=="03eb", ATTR{idProduct}=="2107", MODE="660", GROUP="dialout"

LABEL="avrisp_end"
```

Next, create a hard link for the file:
```bash
cd /etc/udev/rules.d/
ln ../avrisp.rules 60-avrisp.rules
```

###Restart the `udev` service
```bash
sudo service udev restart
sudo udevadm control --reload-rules; udevadm trigger
```

And finally restart the computer
```bash
sudo reboot
```

###Getting information about the device
There are a number of different ways to gain information about the device:

To see the device vendor ID and device ID after plugging it in:
```bash
dmesg | tail
```

Very detailed information about the device (vendorID:deviceID):
```bash
lsusb -v -d 03eb:2104
```
