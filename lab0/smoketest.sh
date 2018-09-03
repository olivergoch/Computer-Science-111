#!/bin/bash
#Oliver Goch
#123456789
#the.oliver.goch@gmail.com


echo "OOGY BOOGY" | ./lab0
if [ ! $? -eq 0 ]; then
    exit 1
fi

echo "OOGY BOOGY" > foo.txt | ./lab0 < foo.txt
if [ ! $? -eq 0 ]; then
    exit 1
fi

./lab0 --input=foo.txt
if [ ! $? -eq 0 ]; then
    exit 1
fi

./lab0 --output=out.txt < foo.txt
if [ ! $? -eq 0 ]; then
    exit 1
fi

./lab0 --input=foo.txt --output=out.txt
if [ ! $? -eq 0 ]; then
    exit 1
fi

./lab0 --fun
if [ ! $? -eq 1 ]; then
    exit 1
fi

./lab0 --input=foo.txt --fun
if [ ! $? -eq 1]; then
    exit 1
fi

./lab0 --input=
if [ ! $? -eq 2 ]; then
    exit 1
fi

./lab0 --output=
if [ ! $? -eq 3 ]; then
    exit 1
fi

./lab0 --input=foo.txt --output=
if [ ! $? -eq 3 ]; then
    exit 1
fi

./lab0 --input= --output=out.txt --segfault --catch
if [ ! $? -eq 2 ]; then
    exit 1
fi

./lab0 --input=foo.txt --output= --segfault --catch
if [ ! $? -eq 3 ]; then
    exit 1
fi

./lab0 --output=out.txt --input=foo.txt
if [ ! $? -eq 0 ]; then
    exit 1
fi

./lab0 --segfault --catch
if [ ! $? -eq 4 ]; then
    exit 1
fi

./lab0 --segfault --catch
if [ ! $? -eq 4 ]; then
    exit 1
fi

./lab0 --input=foo.txt --output=out.txt --catch
if [ ! $? -eq 0 ]; then
    exit 1
fi

rm -r foo.txt out.txt
echo "All Tests Passed!"
