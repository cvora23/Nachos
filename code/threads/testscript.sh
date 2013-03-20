#!/bin/tcsh

#GETTING ALL THE COMMAND LINE ARGUMENTS
nachosOption=$1
testFile=$2
enableDebugging=$3
debugOptions=$4

#CREATING A NEW GDB SESSION LOG FILE
logFile=$testFile.$(date +"%F_%T").log

#STARTING GDB SESSION
gdb nachos <<GDB_INPUT
pwd
run nachos -$nachosOption $testFile $enableDebugging $debugOptions >$logFile
#ENDING GDB SESSION 
quit
GDB_INPUT