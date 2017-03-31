#!/bin/bash

echo "=======Sections======"
readelf --sections hello.ko
echo "=======modinfo========"
modinfo hello.ko
