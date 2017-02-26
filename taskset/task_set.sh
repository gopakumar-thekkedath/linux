#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Provide the core number to bind infinite"
	exit
fi
taskset -c $1 ./measure

