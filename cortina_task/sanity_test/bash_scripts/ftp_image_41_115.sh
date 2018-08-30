#!/bin/bash

ftp -i -n <<EOF
open 192.168.41.115
user pchen 123456
bin
passive
lcd ./$1/build/tmp/deploy/images/saturn-sfu-eng
hash
put uImage
put u-boot.bin
put uImage-saturn_sfu.dtb
put major-image-saturn-sfu-eng.cpio.xz
quit
EOF

