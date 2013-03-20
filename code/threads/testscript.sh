#!/bin/tcsh

#GETTING ALL THE COMMAND LINE ARGUMENTS
set nachosOption = $1
set testFile = $2
set enableDebugging = $3
set debugOptions = $4
set currentDateTime = `date +'%d-%m-%Y_%H-%M-%S'`

: << --COMMENT BLOCK--
echo "$nachosOption"
echo "$testFile"
echo "$enableDebugging"
echo "$debugOptions"
echo "$currentDateTime"
--END OF COMMENT BLOCK

#CREATING A NEW GDB SESSION LOG FILE
#set logFile = "$testFile"."$(date +"%F_%T")".log
set logFile = "Hello"."World".txt

echo "$logFile"

#STARTING GDB SESSION
#gdb nachos <<GDB_INPUT
#pwd
#run nachos -$nachosOption $testFile $enableDebugging $debugOptions >$logFile

#ENDING GDB SESSION 
#quit
#GDB_INPUT