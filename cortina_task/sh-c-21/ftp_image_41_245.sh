#!/bin/bash

TimeTag=$(date "+%Y%m%d")
CurDIR=$(pwd)

ftp -i -n <<EOF
open 192.168.41.245
user r r
bin
passive
lcd $1/build/tmp/deploy/images/g3-eng/
hash
put Image
put Image-ca7774-engboard.dtb
put major-image-g3-eng.cpio.xz
put u-boot.bin
quit
EOF

