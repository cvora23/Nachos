#!/bin/tcsh

directory_path=/home/scf-05/ptaskar/nachos-csci402/code/threads/terminal-logs

if[ -d $directory_path ]
then
    echo "DIRECTORY EXISTS"
else
    echo "DIRECTORY DOES NOT EXIST"
fi
