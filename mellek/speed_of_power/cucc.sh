#!/bin/bash
read c
read d
read a
read b
while [ $a -ne $c -o $b -ne $d ]; do
s=""
if [ "$a" -lt "$c" ]; then
s="${s}E"
a=$((a+1))
fi
if [ "$c" -lt "$a" ]; then
s="${s}W"
a=$((a-1))
fi
if [ "$b" -lt "$d" ]; then
s="${s}S"
b=$((b+1))
fi
if [ "$d" -lt "$b" ]; then
s="${s}N"
b=$((b-1))
fi
echo "$s"
done