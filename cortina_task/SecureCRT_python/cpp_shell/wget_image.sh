#!/bin/bash

DestMailsList=( \
"pengpeng.chen@cortina-access.com"
);

LOG_FILE="g3_wget_image_daily.txt"
#CurTime=$(date "+%Y%m%d")
DayTimeTag=$(date "+%Y-%m-%d")
MonTimeTag=$(date "+%Y-%m")
G3_ImageType="g3-eng-major-image"
G3hgu_ImageType="g3hgu-eng-major-image"
G3hgu_epon_ImageType="g3hgu-eng_epon-major-image"
G3hgu_gpon_ImageType="g3hgu-eng_gpon-major-image"
Saturn_sfu_ImageType="saturn-sfu-eng-major-image"
Saturn_sfu_epon_ImageType="saturn-sfu-eng_epon-major-image"
Saturn_sfu_gpon_ImageType="saturn-sfu-eng_gpon-major-image"
Saturn_sfpplus_ImageType="saturn-sfpplus-eng-major-image"
YestTag=$(date -d '3 days ago' +%Y-%m-%d)
WORK_DIR="/home/pchen/daily_image_sanity_test"

echo $WORK_DIR
cd $WORK_DIR
mkdir $MonTimeTag
cd $MonTimeTag
echo $MonTimeTag
mkdir $DayTimeTag
mkdir $DayTimeTag/g3_images
mkdir $DayTimeTag/g3hgu_images
mkdir $DayTimeTag/g3hgu_epon_images
mkdir $DayTimeTag/g3hgu_gpon_images
mkdir $DayTimeTag/saturn_sfu_images
mkdir $DayTimeTag/saturn_sfu_epon_images
mkdir $DayTimeTag/saturn_sfu_gpon_images
mkdir $DayTimeTag/saturn_sfpplus_images

if [ -d $YestTag ]; then
    rm $YestTag -rf
else
   echo "balabala..."
fi
cd $WORK_DIR

#get G3 major-images
rm g3-gpt-nandinfo.img g3-uboot-env.bin g3-rootfs.mubi
wget http://192.168.65.235:18683/g3/$MonTimeTag/$DayTimeTag/$G3_ImageType/gpt-nandinfo.img
cp gpt-nandinfo.img $WORK_DIR/$MonTimeTag/$DayTimeTag/g3_images/
mv gpt-nandinfo.img g3-gpt-nandinfo.img
wget http://192.168.65.235:18683/g3/$MonTimeTag/$DayTimeTag/$G3_ImageType/uboot-env.bin
cp uboot-env.bin $WORK_DIR/$MonTimeTag/$DayTimeTag/g3_images/
mv uboot-env.bin g3-uboot-env.bin
wget http://192.168.65.235:18683/g3/$MonTimeTag/$DayTimeTag/$G3_ImageType/rootfs.mubi
cp rootfs.mubi $WORK_DIR/$MonTimeTag/$DayTimeTag/g3_images/
mv rootfs.mubi g3-rootfs.mubi

#get G3hgu images
# rm g3hgu-Image ca8279-engboard.dtb g3hgu-rootfs.mubi
# wget http://192.168.65.235:18683/g3hgu/$MonTimeTag/$DayTimeTag/$G3hgu_ImageType/Image 
# cp Image $WORK_DIR/$MonTimeTag/$DayTimeTag/g3hgu_images/
# mv Image g3hgu-Image
# wget http://192.168.65.235:18683/g3hgu/$MonTimeTag/$DayTimeTag/$G3hgu_ImageType/ca8279-engboard.dtb
# cp ca8279-engboard.dtb $WORK_DIR/$MonTimeTag/$DayTimeTag/g3hgu_images/
# mv ca8279-engboard.dtb g3hgu-ca8279-engboard.dtb
# wget http://192.168.65.235:18683/g3hgu/$MonTimeTag/$DayTimeTag/$G3hgu_ImageType/rootfs.mubi
# cp rootfs.mubi $WORK_DIR/$MonTimeTag/$DayTimeTag/g3hgu_images/
# mv rootfs.mubi g3hgu-rootfs.mubi

#get G3hgu_epon images
# rm g3hgu_epon-gpt.img g3hgu_epon-uboot-env.bin g3hgu_epon-Image g3hgu_epon-ca8279-engboard.dtb g3hgu_epon-rootfs.mubi
# wget http://192.168.65.235:18683/g3hgu/$MonTimeTag/$DayTimeTag/$G3hgu_epon_ImageType/gpt.img 
# cp gpt.img $WORK_DIR/$MonTimeTag/$DayTimeTag/g3hgu_epon_images/
# mv gpt.img g3hgu_epon-gpt.img
# wget http://192.168.65.235:18683/g3hgu/$MonTimeTag/$DayTimeTag/$G3hgu_epon_ImageType/uboot-env.bin
# cp uboot-env.bin $WORK_DIR/$MonTimeTag/$DayTimeTag/g3hgu_epon_images/
# mv uboot-env.bin g3hgu_epon-uboot-env.bin
# wget http://192.168.65.235:18683/g3hgu/$MonTimeTag/$DayTimeTag/$G3hgu_epon_ImageType/Image 
# cp Image $WORK_DIR/$MonTimeTag/$DayTimeTag/g3hgu_epon_images/
# mv Image g3hgu_epon-Image
# wget http://192.168.65.235:18683/g3hgu/$MonTimeTag/$DayTimeTag/$G3hgu_epon_ImageType/ca8279-engboard.dtb
# cp ca8279-engboard.dtb $WORK_DIR/$MonTimeTag/$DayTimeTag/g3hgu_epon_images/
# mv ca8279-engboard.dtb g3hgu_epon-ca8279-engboard.dtb
# wget http://192.168.65.235:18683/g3hgu/$MonTimeTag/$DayTimeTag/$G3hgu_epon_ImageType/rootfs.mubi
# cp rootfs.mubi $WORK_DIR/$MonTimeTag/$DayTimeTag/g3hgu_epon_images/
# mv rootfs.mubi g3hgu_epon-rootfs.mubi

#get G3hgu_gpon images
# rm g3hgu_gpon-Image g3hgu_gpon-ca8279-engboard.dtb g3hgu_gpon-rootfs.mubi
# wget http://192.168.65.235:18683/g3hgu/$MonTimeTag/$DayTimeTag/$G3hgu_gpon_ImageType/Image 
# cp Image $WORK_DIR/$MonTimeTag/$DayTimeTag/g3hgu_gpon_images/
# mv Image g3hgu_gpon-Image
# wget http://192.168.65.235:18683/g3hgu/$MonTimeTag/$DayTimeTag/$G3hgu_gpon_ImageType/ca8279-engboard.dtb
# cp ca8279-engboard.dtb $WORK_DIR/$MonTimeTag/$DayTimeTag/g3hgu_gpon_images/
# mv ca8279-engboard.dtb g3hgu_gpon-ca8279-engboard.dtb
# wget http://192.168.65.235:18683/g3hgu/$MonTimeTag/$DayTimeTag/$G3hgu_gpon_ImageType/rootfs.mubi
# cp rootfs.mubi $WORK_DIR/$MonTimeTag/$DayTimeTag/g3hgu_gpon_images/
# mv rootfs.mubi g3hgu_gpon-rootfs.mubi

#get Saturn-sfu images
# rm saturn*
# wget http://192.168.65.235:18683/saturn-sfu/$MonTimeTag/$DayTimeTag/$Saturn_sfu_ImageType/uImage 
# cp uImage $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_images/
# mv uImage saturn_sfu_eng-uImage
# wget http://192.168.65.235:18683/saturn-sfu/$MonTimeTag/$DayTimeTag/$Saturn_sfu_ImageType/saturn-sfu.dtb
# cp saturn-sfu.dtb $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_images/
# mv saturn-sfu.dtb saturn_sfu_eng.dtb
# wget http://192.168.65.235:18683/saturn-sfu/$MonTimeTag/$DayTimeTag/$Saturn_sfu_ImageType/rootfs.cpio.xz
# cp rootfs.cpio.xz $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_images/
# mv rootfs.cpio.xz saturn_sfu_eng-rootfs.cpio.xz

#get Saturn-sfu_epon images
rm saturn_sfu_eng_epon*
wget http://192.168.65.235:18683/saturn-sfu/$MonTimeTag/$DayTimeTag/$Saturn_sfu_epon_ImageType/gpt.img 
mv gpt.img $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_epon_images/
cp $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_epon_images/gpt.img saturn_sfu_eng_epon-gpt.img
wget http://192.168.65.235:18683/saturn-sfu/$MonTimeTag/$DayTimeTag/$Saturn_sfu_epon_ImageType/uboot-env.bin
mv uboot-env.bin $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_epon_images/
cp $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_epon_images/uboot-env.bin saturn_sfu_eng_epon-uboot-env.bin
wget http://192.168.65.235:18683/saturn-sfu/$MonTimeTag/$DayTimeTag/$Saturn_sfu_epon_ImageType/user.ubi 
mv user.ubi $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_epon_images/
cp $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_epon_images/user.ubi saturn_sfu_eng_epon-user.ubi
wget http://192.168.65.235:18683/saturn-sfu/$MonTimeTag/$DayTimeTag/$Saturn_sfu_epon_ImageType/uImage 
mv uImage $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_epon_images/
cp $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_epon_images/uImage saturn_sfu_eng_epon-uImage
wget http://192.168.65.235:18683/saturn-sfu/$MonTimeTag/$DayTimeTag/$Saturn_sfu_epon_ImageType/saturn-sfu.dtb
mv saturn-sfu.dtb $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_epon_images/
cp $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_epon_images/saturn-sfu.dtb saturn_sfu_eng_epon.dtb
wget http://192.168.65.235:18683/saturn-sfu/$MonTimeTag/$DayTimeTag/$Saturn_sfu_epon_ImageType/rootfs.cpio.xz
mv rootfs.cpio.xz $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_epon_images/
cp $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_epon_images/rootfs.cpio.xz saturn_sfu_eng_epon-rootfs.cpio.xz

#get Saturn-sfu_gpon images
rm saturn_sfu_eng_gpon*
wget http://192.168.65.235:18683/saturn-sfu/$MonTimeTag/$DayTimeTag/$Saturn_sfu_gpon_ImageType/gpt.img 
mv gpt.img $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_gpon_images/
cp $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_gpon_images/gpt.img saturn_sfu_eng_gpon-gpt.img
wget http://192.168.65.235:18683/saturn-sfu/$MonTimeTag/$DayTimeTag/$Saturn_sfu_gpon_ImageType/uboot-env.bin
mv uboot-env.bin $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_gpon_images/
cp $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_gpon_images/uboot-env.bin saturn_sfu_eng_gpon-uboot-env.bin
wget http://192.168.65.235:18683/saturn-sfu/$MonTimeTag/$DayTimeTag/$Saturn_sfu_gpon_ImageType/user.ubi 
mv user.ubi $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_gpon_images/
cp $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_gpon_images/user.ubi saturn_sfu_eng_gpon-user.ubi
wget http://192.168.65.235:18683/saturn-sfu/$MonTimeTag/$DayTimeTag/$Saturn_sfu_gpon_ImageType/uImage 
mv uImage $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_gpon_images/
cp $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_gpon_images/uImage saturn_sfu_eng_gpon-uImage
wget http://192.168.65.235:18683/saturn-sfu/$MonTimeTag/$DayTimeTag/$Saturn_sfu_gpon_ImageType/saturn-sfu.dtb
mv saturn-sfu.dtb $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_gpon_images/
cp $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_gpon_images/saturn-sfu.dtb saturn_sfu_eng_gpon.dtb
wget http://192.168.65.235:18683/saturn-sfu/$MonTimeTag/$DayTimeTag/$Saturn_sfu_gpon_ImageType/rootfs.cpio.xz
mv rootfs.cpio.xz $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_gpon_images/
cp $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfu_gpon_images/rootfs.cpio.xz saturn_sfu_eng_gpon-rootfs.cpio.xz

#get Saturn-sfp+ images
# wget http://192.168.65.235:18683/saturn-sfpplus/$MonTimeTag/$DayTimeTag/$Saturn_sfpplus_ImageType/uImage 
# cp uImage $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfpplus_images/
# mv uImage saturn_sfpplus_eng-uImage
# wget http://192.168.65.235:18683/saturn-sfpplus/$MonTimeTag/$DayTimeTag/$Saturn_sfpplus_ImageType/saturn_sfpplus.dtb
# cp saturn_sfpplus.dtb $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfpplus_images/
# mv saturn_sfpplus.dtb saturn_sfpplus_eng.dtb
# wget http://192.168.65.235:18683/saturn-sfpplus/$MonTimeTag/$DayTimeTag/$Saturn_sfpplus_ImageType/rootfs.jffs2
# cp rootfs.jffs2 $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfpplus_images/
# mv rootfs.jffs2 saturn_sfpplus_eng-rootfs.jff2
# wget http://192.168.65.235:18683/saturn-sfpplus/$MonTimeTag/$DayTimeTag/$Saturn_sfpplus_ImageType/rootfs.cpio.xz
# cp rootfs.cpio.xz $WORK_DIR/$MonTimeTag/$DayTimeTag/saturn_sfpplus_images/
# mv rootfs.cpio.xz saturn_sfpplus_eng-rootfs.cpio.xz

for m in ${DestMailsList[@]}; do
   echo $m
   #cat $LOG_FILE | mutt -s "G3 ASIC WGET Daily Images - "$DayTimeTag $m -a $LOG_FILE
done
#End
