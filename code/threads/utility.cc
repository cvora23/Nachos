// utility.cc 
//	Debugging routines.  Allows users to control whether to 
//	print DEBUG statements, based on a command line argument.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "utility.h"

#include <stdarg.h>

static char *enableFlags = NULL; // controls which DEBUG messages are printed 

//----------------------------------------------------------------------
// DebugInit
//      Initialize so that only DEBUG messages with a flag in flagList 
//	will be printed.
//
//	If the flag is "+", we enable all DEBUG messages.
//
// 	"flagList" is a string of characters for whose DEBUG messages are 
//		to be enabled.
//----------------------------------------------------------------------

void
DebugInit(char *flagList)
{
    enableFlags = flagList;
}

//----------------------------------------------------------------------
// DebugIsEnabled
//      Return TRUE if DEBUG messages with "flag" are to be printed.
//----------------------------------------------------------------------

bool
DebugIsEnabled(char flag)
{
    if (enableFlags != NULL)
       return (strchr(enableFlags, flag) != 0) 
		|| (strchr(enableFlags, '+') != 0);
    else
      return FALSE;
}

//----------------------------------------------------------------------
// DEBUG
//      Print a debug message, if flag is enabled.  Like printf,
//	only with an extra argument on the front.
//----------------------------------------------------------------------

void 
DEBUG(char flag, char *format, ...)
{
    if (DebugIsEnabled(flag)) {
	va_list ap;
	// You will get an unused variable message here -- ignore it.
	va_start(ap, format);
	vfprintf(stdout, format, ap);
	va_end(ap);
	fflush(stdout);
    }
}

char* xitoa(int n)
{
	char* ret = NULL;
	int numChars = 0;
	// Determine if integer is negative
	bool isNegative = false;
	if (n < 0)
	{
		n = -n;
		isNegative = true;
		numChars++;
	}
	// Count how much space we will need for the string
	int temp = n;
	do
	{
		numChars++;
		temp /= 10;
	} while ( temp );
	//Allocate space for the string (1 for negative sign, 1 for each digit, and 1 for null terminator)
	ret = new char[ numChars + 1 ];
	ret[numChars] = 0;
	//Add the negative sign if needed
	if (isNegative) ret[0] = '-';
	// Copy digits to string in reverse order
	int i = numChars - 1;
	do
	{
		ret[i--] = n%10 + '0';
		n /= 10;
	} while (n);
	return ret;
}
