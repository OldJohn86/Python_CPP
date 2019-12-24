#!/bin/bash

DestMailsList=( \
"pengpeng.chen@cortina-access.com" \
);
BUILD_DIR=Venus_fpgabuild
LOG_FILE="venus_dailybuild.txt"
TAIL_FILE="./tail.txt"
#CurTime=$(date "+%Y%m%d")
DayTimeTag=$(date "+%Y%m%d")
MonTimeTag=$(date "+%Y%m")
YestTag=$(date -d '3 days ago' +%Y%m%d)
WORK_DIR=/space/pchen/Venus_build/

cd $WORK_DIR
#if [ -d $MonTimeTag ]; then
#	echo "Have created dir $MonTimeTag"
#else
#	mkdir $MonTimeTag
#fi
#mkdir $MonTimeTag/$DayTimeTag

#Gitclone Source Codes of Titan.
if [ -d $BUILD_DIR-$YestTag ]; then
    echo "The Source Codes had git cloned before should be removed first."
    #rm $BUILD_DIR-$YestTag -rf
else
    echo "The First time to Git clone the Source Code and build. "
fi

git clone -b ca-krogoth-master gitolite@cs-local-gitmirror:sw/platforms/cs-unified/distro/yocto $BUILD_DIR-$DayTimeTag
cd $BUILD_DIR-$DayTimeTag

git clone -b ca-krogoth-master gitolite@cs-local-gitmirror:sw/platforms/cs-unified/distro/openembedded/meta-openembedded
git clone -b ca-krogoth-master gitolite@cs-local-gitmirror:sw/platforms/cs-unified/distro/meta-oe-yocto-cortina
git clone -b ca-krogoth-master gitolite@cs-local-gitmirror:sw/platforms/cs-unified/distro/meta-ca-bsp
export TEMPLATECONF=meta-ca-bsp/conf/venus-fpga/
#pwd
#ls 
source ./oe-init-build-env

#Make build
bitbake basic-image > $WORK_DIR/$LOG_FILE

#Copy the output image 
#echo $CurTime
#if [ -d $CurTime ]; then
#    echo "remove the first."
#    rm $CurTime -rf
#    mkdir $CurTime
#else
#    echo "12233"
#    mkdir $CurTime
#fi
cd $WORK_DIR
#cp $BUILD_DIR-$DayTimeTag/build/tmp/deploy/images/venus-fpga/Image $MonTimeTag/$DayTimeTag
#cp $BUILD_DIR-$DayTimeTag/build/tmp/deploy/images/venus-fpga/Image-ca7774-engboard.dtb $MonTimeTag/$DayTimeTag
#cp $BUILD_DIR-$DayTimeTag/build/tmp/deploy/images/venus-fpga/major-image-g3-eng.cpio.xz  $MonTimeTag/$DayTimeTag
#cp $BUILD_DIR-$DayTimeTag/build/tmp/deploy/images/venus-fpga/u-boot.bin $MonTimeTag/$DayTimeTag
#cp $LOG_FILE $MonTimeTag/$DayTimeTag/$DayTimeTag-$LOG_FILE

#tail -10 $LOG_FILE > $TAIL_FILE
#grep -I "Error" $TAIL_FILE
#if [ $? -eq 0 ]; then
#    echo "The daily compile building had failed. --- Please refer the attchment log file!" > $TAIL_FILE
#else
#    echo "The daily compile building had PASSED successful. --- OK!" > $TAIL_FILE
#fi
#echo "#######################################################" >> $TAIL_FILE
#echo "" >> $TAIL_FILE
#echo "... ... ..." >> $TAIL_FILE
#echo "" >> $TAIL_FILE
#tail -20 $LOG_FILE >> $TAIL_FILE

#Restart the Mail Services
#service postfix restart
#service sendmail restart

#Send Emails
#for m in ${DestMailsList[@]}; do
#   echo $m
#   cat $TAIL_FILE | mutt -s "G3 ASIC YOCTO Daily Build - "$DayTimeTag $m -a $LOG_FILE
#done
#End
