#!/bin/bash
read c
read d
read a
read b
while [ $a -ne $c -o $b -ne $d ]; do
s=""
if [ "$a" -lt "$c" ]; then
s="${s}S"
a=$((a+1))
fi
if [ "$c" -lt "$a" ]; then
s="${s}N"
a=$((a-1))
fi
if [ "$b" -lt "$d" ]; then
s="${s}E"
b=$((b+1))
fi
if [ "$d" -lt "$b" ]; then
s="${s}W"
b=$((b-1))
fi
echo "$s"
done