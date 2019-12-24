#!/bin/bash

DestMailsList=("pengpeng.chen@cortina-access.com");
BUILD_DIR=cig_saturn_sfu_yocto
LOG_FILE="saturn_sfu_dailybuild.txt"
TAIL_FILE="./tail.txt"
CurTime=$(date "+%Y%m%d")
TimeTag=$(date "+%Y%m%d")
YestTag=$(date -d '2 days ago' +%Y%m%d)
WORK_DIR=/space/pchen/Saturn_SFU

cd $WORK_DIR
#Gitclone Source Codes of Titan.

#if [ -d $BUILD_DIR-$YestTag ]; then
    #echo "The Source Codes had git cloned before should be removed first."
    #rm $BUILD_DIR-$YestTag -rf
#else
    #echo "The First time to Git clone the Source Code and build. "
#fi

git clone -b ca-krogoth-master gitolite@cs-local-gitmirror:sw/platforms/cs-unified/distro/yocto $BUILD_DIR-$TimeTag
cd $BUILD_DIR-$TimeTag

git clone -b ca-krogoth-master gitolite@cs-local-gitmirror:sw/platforms/cs-unified/distro/openembedded/meta-openembedded
git clone -b ca-krogoth-master gitolite@cs-local-gitmirror:sw/platforms/cs-unified/distro/meta-oe-yocto-cortina
git clone -b ca-krogoth-master gitolite@cs-local-gitmirror:sw/platforms/cs-unified/distro/meta-ca-bsp
#git clone -b ca-krogoth-20181211 gitolite@cs-local-gitmirror:sw/platforms/cs-unified/distro/meta-oe-ca
cp $WORK_DIR/meta-oe-ca $WORK_DIR/$BUILD_DIR-$TimeTag -r

cd $WORK_DIR/$BUILD_DIR-$TimeTag
export TEMPLATECONF=meta-oe-ca/conf/saturn-sfu-c0
source ./oe-init-build-env

#Make build
bitbake major-image > $WORK_DIR/$LOG_FILE

cd $WORK_DIR
#Copy the output image 
# cd $WORK_DIR
# cp $BUILD_DIR-$TimeTag/build/tmp/deploy/images/g3-eng/Image $CurTime
# cp $BUILD_DIR-$TimeTag/build/tmp/deploy/images/g3-eng/Image-ca7774-engboard.dtb $CurTime
# cp $BUILD_DIR-$TimeTag/build/tmp/deploy/images/g3-eng/basic-image-g3-eng.cpio.xz  $CurTime
# cp $BUILD_DIR-$TimeTag/build/tmp/deploy/images/g3-eng/u-boot.bin $CurTime
# cp $LOG_FILE $CurTime/$CurTime-$LOG_FILE

tail -20 $LOG_FILE > $TAIL_FILE
grep -I "Error" $TAIL_FILE
if [ $? -eq 0 ]; then
    echo "The daily compile building had FAILED, Please review the error logs!" > $TAIL_FILE
else
    echo "The daily compile building had PASSED successful. --- OK!" > $TAIL_FILE
fi
echo "#######################################################" >> $TAIL_FILE
echo "Email List Including: " >> $TAIL_FILE
#echo "" >> $TAIL_FILE
for m in ${DestMailsList[@]}; do
    echo $m >> $TAIL_FILE
done
echo "#######################################################" >> $TAIL_FILE
echo "" >> $TAIL_FILE
#Restart the Mail Services
#service postfix restart
#service sendmail restart

#Send Emails
grep -I "PASSED" $TAIL_FILE
if [ $? -eq 0 ]; then
    tail -20 $LOG_FILE >> $TAIL_FILE
    for m in ${DestMailsList[@]}; do
        echo $m
    done
else
    tail -20 $LOG_FILE >> $TAIL_FILE
    for m in ${DestMailsList[@]}; do
        echo $m
        cat $TAIL_FILE | mutt -s "Saturn SFU YOCTO Daily Building Result - "$DayTimeTag $m -a $MonTimeTag/$DayTimeTag/$ERROR_LOG_FILE
    done
fi
#End
