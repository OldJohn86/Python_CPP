#!/usr/bin/expect -f
set timeout 30
set src_file [lindex $argv 0]
set dest_path [lindex $argv 1]

spawn scp -P 28683 $src_file cortina@192.168.65.235:/space/jflee/daily/assets/$dest_path/
expect "*password:"
send "Cortina123\r"

expect eof
