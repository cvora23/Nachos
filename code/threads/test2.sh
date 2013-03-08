s#!/bin/tcsh

dir="/home/scf-05/ptaskar/nachos-csci402/code/threads/terminal-logs"

if [ "$(ls -A $dir)" ]; then
  echo "$dir not empty"
else
  echo "$dir is empty"
fi