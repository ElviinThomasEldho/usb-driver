sudo rmmod driver
sudo make clean
sudo make
sudo insmod driver.ko
sudo rmmod uas usb_storage
