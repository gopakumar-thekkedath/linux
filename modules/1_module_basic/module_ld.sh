#!/bin/bash
ln -s /home/gopa/Amrita/linux_programming/modules/1_module_basic/hello.ko /lib/modules/3.5.0-32-generic/hello.ko
depmod
cat /lib/modules/3.5.0-32-generic/modules.dep | grep hello
