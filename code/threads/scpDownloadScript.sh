#!/bin/bash
#!/usr/bin
#!/usr/bin/expect -f

#SETTING CURRENT DATE TIME
set DATE [exec date "+%d-%m-%Y_%H-%M-%S"]

#SETTING CURRENT DIRECTORY PATH
set currentDirPath "/home/cvora/localNachosRepo/nachos-csci402/code/threads"

#SETTING TEST RESULT HOME DIRECTORY NAME
set testResultHomeDir "testResultHomeDir"

#CREATING TEST RESULT HOME DIRECTORY
if {!([file exists $currentDirPath/$testResultHomeDir])} then {
set MakeDir [exec mkdir $currentDirPath/$testResultHomeDir]}

#SETTING TEST RESULT DIRECTORY PATH
set testResultHomeDirPath "/home/cvora/localNachosRepo/nachos-csci402/code/threads/testResultHomeDir"

#CREATING SPECIFIC TEST RESULTS DIRECTORY
if {!([file exists $testResultHomeDirPath/$DATE])} then {
set MakeDir [exec mkdir $testResultHomeDirPath/$DATE]}

#SETTING REMOTE INI DIRECTORY
set remoteIniDir "/home/scf-05/ptaskar/nachos-csci402/code/threads"

#SETTING REMOTE LOG DIRECTORY
set remoteLogDir "/home/scf-05/ptaskar/nachos-csci402/code/threads/terminal-logs"

#CREATING A INTERACTIVE SCP SESSION FOR DOWNLOADING FILES 
spawn scp -r ptaskar@aludra.usc.edu:/{$remoteLogDir,$remoteIniDir/itemConfigFile.ini,$remoteIniDir/customerConfigFile.ini} \
                        $testResultHomeDirPath/$DATE

expect {
-re ".*es.*o.*" {
exp_send "yes\r"
exp_continue
}
-re ".*sword.*" {
exp_send "Taskar90\r"
}
}
interact 

