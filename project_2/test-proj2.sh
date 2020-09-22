echo "Testing project 2"
echo "Clear kernel message"
sudo dmesg -c
echo "kernel debug message cleared"

sudo insmod proj2.ko int_str="1,2,3,4,5"
echo "cat /proc/proj2"
cat /proc/proj2
echo "dmesg"
dmesg
sudo rmmod proj2




