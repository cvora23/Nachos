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

#Following commands writes the output only to the file and not to the screen (stdout)
#run nachos $nachosOption $testOption $enableDebugging $debugOptions >$logDir/$logFile

#Following commands writes the output only to the file and screen (stdout)
run nachos $nachosOption $testOption $enableDebugging $debugOptions | tee $logDir/$logFile

#Following commands writes the output only to the screen (stdout)
#run nachos $nachosOption $testOption $enableDebugging $debugOptions

#ENDING GDB SESSION 
quit
GDB_INPUT