#!/bin/bash
#NAME:Oliver Goch
#EMAIL:the.oliver.goch@gmail.com
#ID:123456789
IFS=

./lab4b --period=2 --scale=C --log=logfile <<-EOF
SCALE=F

PERIOD=1

START

STOP

LOG OOGY BOOGY

OFF
EOF

if [ ! $? -eq 0 ]; then
    exit 1
fi

if [ ! -s logfile ]; then
    exit 1
fi

grep "SCALE=F" logfile
if [ ! $? -eq 0 ]; then
    exit 1
fi

grep "PERIOD=1" logfile
if [ ! $? -eq 0 ]; then
    exit 1
fi

grep "START" logfile
if [ ! $? -eq 0 ]; then
    exit 1
fi

grep "STOP" logfile
if [ ! $? -eq 0 ]; then
    exit 1
fi

grep "LOG OOGY BOOGY" logfile
if [ ! $? -eq 0 ]; then
    exit 1
fi

grep "OFF" logfile
if [ ! $? -eq 0 ]; then
    exit 1
fi

grep "SHUTDOWN" logfile
if [ ! $? -eq 0 ]; then
    exit 1
fi

grep -e '[0-9][0-9]:[0-9][0-9]:[0-9][0-9] [0-9][0-9].[0-9]' logfile
if [ ! $? -eq 0 ]; then
    exit 1
fi

./lab4b --fake
if [ ! $? -eq 1 ]; then
    exit 1
fi


rm -rf logfile
echo "All Tests Passed!"
