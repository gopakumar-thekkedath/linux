#!/bin/bash

echo "Memory consumption, initially"
free -m
echo "echo 3 > /proc/sys/vm/drop_caches"
echo 3 > /proc/sys/vm/drop_caches
echo "Memory consumption, now"
free -m

