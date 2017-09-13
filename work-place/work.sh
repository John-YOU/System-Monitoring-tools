### Ensure that you are in workplace
### arguments: 1. pid 2. frequency
if [ -z "$1" ];then
        echo "please specify the pid(int)"
        exit
fi
if [ ! -z "$2" ];then
        java -jar ../jvmtop/jvmtop.jar "$1" "$2" "$1_cpu.txt" &
        sudo python ../iotop/iotop.py -d "$2" -p "$1" -f "$1_diskIO.txt" -botqqqk >> "$1_diskIO.txt" &
        sudo ../nethogs/src/nethogs -d "$2" -p "$1" -f "$1_networkIO.txt" &
else
		java -jar ../jvmtop/jvmtop.jar "$1" "$1_cpu.txt" &
    	sudo python ../iotop/iotop.py --p "$1" -f "$1_diskIO.txt" -botqqqk >> "$1_diskIO.txt" &
        sudo ../nethogs/src/nethogs -p "$1" -f "$1_networkIO.txt" &
fi