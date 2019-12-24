#!/bin/bash

ftp -i -n <<EOF
open 192.168.48.84
user pchen 123456
bin
passive
lcd ./kt_saturn_sfu_yocto-20180703/build/tmp/deploy/images/saturn-sfu-eng-kt/
hash
put uImage
put uImage-saturn_sfu_c20.dtb
put major-image-saturn-sfu-eng-kt.rootfs.squashfs.ubi
put major-image-saturn-sfu-eng-kt_user.ubi
quit
EOF

