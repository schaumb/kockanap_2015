#!bin/bash

read c
read d
read a
read b
while [ $a -ne $c -o $b -ne $d ]; do
s=""
if [ "$a"<"$c" ]; then
s="$sS"
a=$((a+1))
fi
if [ $a>$c ]; then
s="$sN"
a=$((a-1))
fi
if [ $b<$d ]; then
s="$sE"
b=$((b+1))
fi
if [ $b>$d ]; then
s="$sW"
b=$((b-1))
fi
echo $s
done
