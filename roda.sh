#!/bin/bash

echo "performance" > /sys/devices/system/cpu/cpufreq/policy3/scaling_governor
TOTAL_ELEMENTOS=2048

for n in {1..8}
do
    for k in {1..10}
    do
        echo "Running 10 times with $1 elements to find the $k least, using $n threads: " 
        ./findKLeast $1 $TOTAL_ELEMENTOS $n | grep -oP '(?<=Total time in seconds: )[^ ]*'
        
    done
        echo "******************************************************************************"
done

echo "powersave" > /sys/devices/system/cpu/cpufreq/policy3/scaling_governor 