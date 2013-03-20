#!/bin/tcsh

#CREATING TERMINAL LOG DIRECTORY

set logDir = ./terminal-logs

    if (! -d $logDir) then
        mkdir $logDir
   endif
    
#GETTING ALL THE COMMAND LINE ARGUMENTS
set nachosOption = $1
set testOption = $2
#set testFile = $2
set enableDebugging = $3
set debugOptions = $4
set currentDateTime = `date +'%d-%m-%Y_%H-%M-%S'`

: << --END-COMMENT--
echo "$nachosOption"
echo "$testOption"
echo "$enableDebugging"
echo "$debugOptions"
echo "$currentDateTime"
--END-COMMENT--

#CREATING A NEW GDB SESSION LOG FILE
#set logFile = "currentTest"."$(date +"%F_%T")".log
set logFile = "currentTest".$currentDateTime.log

#echo "$logFile"

#STARTING GDB SESSION
gdb nachos <<GDB_INPUT
pwd
run nachos -$nachosOption $testOption -$enableDebugging $debugOptions >$logDir/$logFile

#ENDING GDB SESSION 
quit
GDB_INPUT