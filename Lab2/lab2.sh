#!/bin/bash

#file=`find $1 -name $2 -type f`
#find $1 -type f -printf "%f\n" | wc -l
#find $1 -type d -printf "%f\n" | wc -l
#stat --printf="%n %s %w %A %i\n" $file 

find $1 -type f -printf "%f\n" | wc -l
find $1 -type d -printf "%f\n" | wc -l
find $1 -name $2 -type f -printf "%p %f %s %M %i %a\n"
