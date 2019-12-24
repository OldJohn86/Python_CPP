#!/bin/bash

DestMailsList=("pengpeng.chen@cortina-access.com");
BUILD_DIR=g3_asic_yocto
LOG_FILE="g3_asic_dailybuild.txt"
TAIL_FILE="./tail.txt"
CurTime=$(date "+%Y%m%d")
TimeTag=$(date "+%Y%m%d")
YestTag=$(date -d '3 days ago' +%Y%m%d)
WORK_DIR=/space/pchen/g3_yocto_build

cd $WORK_DIR
CurDIR=$(pwd)
cd $BUILD_DIR-$TimeTag
#Gitclone Source Codes of Titan.
source ./oe-init-build-env

#Make build
bitbake basic-image > $CurDIR/$LOG_FILE

#Copy the output image 
cd $CurDIR
echo $CurDIR
echo $CurTime
if [ -d $CurTime ]; then
    echo "remove the first."
    rm $CurTime -rf
    mkdir $CurTime
else
    echo "12233"
    mkdir $CurTime
fi

cp $BUILD_DIR-$TimeTag/build/tmp/deploy/images/g3-eng/Image $CurTime
cp $BUILD_DIR-$TimeTag/build/tmp/deploy/images/g3-eng/Image-ca7774-engboard.dtb $CurTime
cp $BUILD_DIR-$TimeTag/build/tmp/deploy/images/g3-eng/basic-image-g3-eng.cpio.xz  $CurTime
cp $BUILD_DIR-$TimeTag/build/tmp/deploy/images/g3-eng/u-boot.bin $CurTime
cp $LOG_FILE $CurTime/$CurTime-$LOG_FILE

tail -10 $LOG_FILE > $TAIL_FILE
grep -I "Error" $TAIL_FILE
if [ $? -eq 0 ]; then
    echo "The daily compile building had failed. --- Please refer the attchment log file!" > $TAIL_FILE
else
    echo "The daily compile building had PASSED successful. --- OK!" > $TAIL_FILE
fi
echo "#######################################################" >> $TAIL_FILE
echo "" >> $TAIL_FILE
echo "... ... ..." >> $TAIL_FILE
echo "" >> $TAIL_FILE
tail -20 $LOG_FILE >> $TAIL_FILE

#Restart the Mail Services
#service postfix restart
#service sendmail restart

#End
