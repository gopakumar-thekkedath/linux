#!/bin/bash
echo "Creating new cpuset called 'mycpuset' and assigning CPU 3 and memory node 0 to it"
mkdir /sys/fs/cgroup/cpuset/mycpuset
echo 3 > /sys/fs/cgroup/cpuset/mycpuset/cpuset.cpus
echo 0 > /sys/fs/cgroup/cpuset/mycpuset/cpuset.mems
echo "Perform echo <pid> > tasks to add a task in this cgoup"
echo "/proc/<pid>/status would give details on CPU and memory used by the task 
