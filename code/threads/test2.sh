#!/bin/tcsh

dirVar="/home/scf-05/ptaskar/nachos-csci402/code/threads/terminal-logs"

if [ "$(ls -A $dirVar)" ]; then
  echo "$dirVar not empty"
else
  echo "$dirVar is empty"
fi