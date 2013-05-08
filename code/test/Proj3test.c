/**
 * This testcase file tests the functionalities for project 3 - Part 1 & 2
 */

#include "syscall.h"

/**
 * executes a single instance of matmult
 */
void testcase1()
{
	Exec("../test/matmult",15);
}

/**
 * Forks 2 instances of matmult functionality
 */
void testcase2()
{
	Exec("../test/forkMatmult",19);
}

/**
 * Exec 2 instance of matmult functionality
 */
void testcase3()
{
	Exec("../test/matmult",15);
	Exec("../test/matmult",15);
}

/**
 * execute a single instance of sort functionality
 */
void testcase4()
{
	Exec("../test/sort",12);
}

/**
 * Forks 2 instances of sort functionality
 */
void testcase5()
{
	Exec("../test/forkSort",16);
}

/**
 * Exec 2 instances of sort functionality
 */
void testcase6()
{

	Exec("../test/sort",12);
	Exec("../test/sort",12);
}

/**
 * main function for the test-suite to demonstrate the functionalities are working for the
 * Project 3 - Part 1 and Part 2
 */
void main()
{
	int testcase;

	Print(" 1.Single instance of matmult\n");
	Print(" 2.Fork 2 instance of matmult\n");
	Print(" 3.Exec 2 instance of matmult\n");
	Print(" 4.Single instance of sort\n");
	Print(" 5.Fork 2 instance of sort\n");
	Print(" 6.Exec 2 instance of sort\n");
	Print("\n");

	Print("Enter Choice:\n");

	testcase=Scan();

	Print("\n\n");

	switch(testcase)
	{
	case 1:
		testcase1();
		break;
	case 2:
		testcase2();
		break;
	case 3:
		testcase3();
		break;
	case 4:
		testcase4();
		break;
	case 5:
		testcase5();
		break;
	case 6:
		testcase6();
		break;
	default:
		Print("Please select a proper test-case choice\n");
	}

	Exit(0);
}
