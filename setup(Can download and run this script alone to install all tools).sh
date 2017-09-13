wget https://github.com/John-YOU/System-Monitoring-tools/archive/master.zip
unzip master.zip
cd System-Monitoring-tools-master/nethogs
sudo apt-get install default-jdk
make clean
sudo make install
cd ../work-place
chmod a+x ./work.sh
