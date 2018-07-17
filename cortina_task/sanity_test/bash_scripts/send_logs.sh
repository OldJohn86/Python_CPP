#!/bin/bash

daytag=$(date "+%Y-%m-%d")
montag=$(date "+%Y-%m")

#send g3 log
expect expect_scp.sh dailylogs/g3-santiy-test_$daytag.txt g3/$montag/$daytag/g3-eng-major-image

#send epon log
expect expect_scp.sh dailylogs/saturn_epon-santiy-test_$daytag.txt saturn-sfu/$montag/$daytag/saturn-sfu-eng_epon-major-image

#send gpon log
expect expect_scp.sh dailylogs/saturn_gpon-santiy-test_$daytag.txt saturn-sfu/$montag/$daytag/saturn-sfu-eng_gpon-major-image
#end
