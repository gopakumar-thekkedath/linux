#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Provide the core number to bind infinite"
	exit
fi
numactl --physcpubind=$1 ./infinite &

ps -aF
