#!/bin/bash

ftp -i -n <<EOF
open 192.168.45.119
user pchen 123456
bin
passive
lcd ./$1/build/tmp/deploy/images/saturn-sfu-eng
hash
put uImage
put uboot-env.bin
put uImage-saturn_sfu.dtb
put major-image-saturn-sfu-eng.cpio.xz
put major-image-saturn-sfu-eng.ubi
quit
EOF

