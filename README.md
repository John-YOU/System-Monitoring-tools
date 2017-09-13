# System-Monitoring-tools
This package is designated to monitoring resource occupation of a specific process,
including Disk I/O, Network usage, and CPU/Memory usage, thus the interacting UI part 
of Jvmtop and Nethogs are disabled. 

It's developped basing on Jvmtop, Iotop and Nethogs, but fixing some bugs in iotop in terms of monitoring specified process.

## Getting Started
### Requirements
  There should available jdk, if not, just use the command below
  ```  
  sudo apt-get install default-jdk
  ```
### Set up
	cd System-Monitoring-tools-master/nethogs
	make clean
	sudo make install
	cd ../work-place
	chmod a+x ./work.sh

## Running the test
You can choose to invoke the script to run Jvmtop, Nethogs and Iotop at the same time and in the same frequency.
You can also invoke their command respectively, but you should following the instructions below, since they are modified in many aspects,
if you want to use the original UI windows versions, please use apt-get to install nethogs and iotop; 
as for jvmtop, please use source install(requires setting up java_home)

### Using shell script to monitor all(recommended)
Run the 'work.sh' in the following format:
```
./work.sh pid frequency(optional)
```
example:
```
./work.sh 20339 2
```
The the output files will show up:
```
llb0189@node-0:~/workspace$ ls
20339_cpu.txt  20339_diskIO.txt  20339_networkIO.txt work.sh
```
### Jvmtop
To use jvmtop, run the command below in jvmtop folder, pid is compulsory while the other two arguments are optional, 
the frequency can be double, but recommend to use integers. It's **important to add &** at the end of command!
```
java -jar jvmtop.jar 'pid'* 'delay' 'path' &
```
example:
```
llb0189@node-0:~/jvmtop$ java -jar jvmtop.jar 21408 0.25 t.txt &
[2] 21443
```
### Iotop
Since I use cron to output the monitoring data to file, the UI window of iotop is preserved here.
The command to store data of specified process in output file is shown below
```
sudo python iotop.py -p 'pid' -f 'filepath' -botqqqk >> 'filepath' &
```
You can also add other parameters in this command, to print it, use:
```
llb0189@node-0:~/iotop-modi$ sudo python iotop.py -h

Options:
  --version             show program's version number and exit
  -h, --help            show this help message and exit
  -f FPATH, --file=FPATH
                        the path of output file
  -o, --only            only show processes or threads actually doing I/O
  -b, --batch           non-interactive mode
  -n NUM, --iter=NUM    number of iterations before ending [infinite]
  -d SEC, --delay=SEC   delay between iterations [1 second]
  -p PID, --pid=PID     processes/threads to monitor [all]
  -u USER, --user=USER  users to monitor [all]
  -P, --processes       only show processes, not all threads
  -a, --accumulated     show accumulated I/O instead of bandwidth
  -k, --kilobytes       use kilobytes instead of a human friendly unit
  -t, --time            add a timestamp on each line (implies --batch)
  -q, --quiet           suppress some lines of header (implies --batch)
```

### Nethogs
Nethogs here must run in background, you can specify the output file path, pid, and frequency:
```
sudo src/nethogs -d 'frequency' -f 'output file' -p 'pid' & 
```
You can also use -h to print other options.
