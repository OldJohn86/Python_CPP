#!/bin/bash

DestMailsList=( \
"ch.hsu@cortina-access.com" \
"tony.liu@cortina-access.com" \
"junfeng.zhang@cortina-access.com" \
"yi.ding@cortina-access.com" \
"Lidong.chen@cortina-access.com" \
"pengpeng.chen@cortina-access.com" \
);
WORK_DIR=/space/pchen/Saturn_SFU
BUILD_DIR=ca-openwrt_Saturn_SFU
NORMAL_LOG_FILE="build_output_normal.txt"
ERROR_LOG_FILE="build_output_error.txt"
TAIL_FILE="tail.txt"
curTime=$(date "+%Y%m%d_%H%M%S")
DayTimeTag=$(date "+%Y%m%d")
MonTimeTag=$(date "+%Y%m")
Pre3_DayDate=$(date -d '3 days ago' "+%Y%m%d")
#echo $TimeTag
#echo $(($TimeTag-3))
#echo $Pre3_DayDate

cd $WORK_DIR
if [ -d $MonTimeTag ]; then
	echo "Have created dir "$MonTimeTag
else
	mkdir $MonTimeTag
fi
mkdir $MonTimeTag/$DayTimeTag

#Gitclone Source Codes of Titan.
if [ -d $BUILD_DIR-$Pre3_DayDate  ]; then
    echo "The Source Codes had git cloned before should be removed first."
    rm $BUILD_DIR-$Pre3_DayDate -rf
else
    echo "The First time to Git clone the Source Code and build. "
fi

git clone -b g3-master gitolite@cs-local-gitmirror:sw/platforms/cs-unified/distro/openwrt/trunk/ca-openwrt  $BUILD_DIR-$DayTimeTag
echo "Had done Git clone the Source codes and will build"
cd $BUILD_DIR-$DayTimeTag

#Make Configure
make target=saturn_sfu board=fpga images=ramdisk profile=basic-4.4 LINUX_VERSION=4.4 prepare

#Building and Save the compile logs
make V=99 1> $WORK_DIR/$MonTimeTag/$DayTimeTag/$NORMAL_LOG_FILE 2>$WORK_DIR/$MonTimeTag/$DayTimeTag/$ERROR_LOG_FILE
cd $WORK_DIR

tail -20 $MonTimeTag/$DayTimeTag/$ERROR_LOG_FILE > $TAIL_FILE
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
#service crond restart
#systemctl status sendmail.service

#Send Emails
grep -I "PASSED" $TAIL_FILE
if [ $? -eq 0 ]; then
    tail -20 $MonTimeTag/$DayTimeTag/$NORMAL_LOG_FILE >> $TAIL_FILE
    for m in ${DestMailsList[@]}; do
        echo $m
        #cat $TAIL_FILE | mutt -s "Saturn SFU Daily Building Result - "$DayTimeTag $m
    done
else
    tail -20 $MonTimeTag/$DayTimeTag/$ERROR_LOG_FILE >> $TAIL_FILE
    for m in ${DestMailsList[@]}; do
        echo $m
        cat $TAIL_FILE | mutt -s "Saturn SFU Daily Building Result - "$DayTimeTag $m -a $MonTimeTag/$DayTimeTag/$ERROR_LOG_FILE
    done
fi
#End
