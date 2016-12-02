#!/bin/bash

# On the Pi-0, the UART is ttyAMA0
# On the Pi-3, the UART is ttyS0
# A symlink/alias on all Pi's is serial0
# You may have some other serial port you want to use <shrug>
xSER=serial0

echo -----------------------------------------------------------
echo DEBUG INFO BEGINS
echo -----------------------------------------------------------
echo tty devices :
ls /dev/tty*
echo -----------------------------------------------------------
echo serial port mapping :
ls /dev/serial* -l
echo -----------------------------------------------------------
echo /boot/cmdline.txt : 
cat /boot/cmdline.txt
echo -----------------------------------------------------------
echo DEBUG INFO ENDS
echo -----------------------------------------------------------


echo "I am going to try to free up ${xSER}"
echo "if you have some non-standard setup with serial somewhere else"
echo "IE. A USB<->UART (prolifi/ftdi) lead or similar, then now"
echo "is the time to hit ^C and edit the port name a the top of this script"
read -p "[cr] to continue, [^c] to stop:"

echo -----------------------------------------------------------
echo "Kill the serial getty"
echo "This is the thing that runs the login prompt on the serial port"
echo "This is a serial keyboard - not a serial terminal!"
set -x
systemctl stop    serial-getty@${xSER}.service
systemctl disable serial-getty@${xSER}.service
set +x
echo "To undo this run:"
echo "  systemctl enable serial-getty@${xSER}.service"
echo "  systemctl start  serial-getty@${xSER}.service"


xI=`grep console=${xSER} /boot/cmdline.txt | wc -l`
if [ ${xI} -eq 1 ] ; then
echo -----------------------------------------------------------
	echo "You need to stop the kernel throwing debug messages at the serial port"
	echo "  make a backup of /boot/cmdline.txt and then..."
	echo "  edit /boot/cmdline.txt and remove the 'console=S{xSER},<baud>' bit"
	echo "  then reboot"
	echo "I'm not going to risk automating this in case I screw up your boot config"
fi


echo -----------------------------------------------------------
echo "With those two things done, you should be good to run ./chatpad"
echo -----------------------------------------------------------

