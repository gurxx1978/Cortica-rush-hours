/*
*	© Superharmonic Technologies
*	Pavlos Sakoglou
*
*  ========================================================
*
*	Server Farm Resource Save-Mode Mechanism Definition
*
*/

//*** Three test cases:	
//						1) Simple insertion and querying
//						2) Bad input cases
//						3) Performance and stress testing
//
//******************************************************************

// Include the class definition and supporting STL libraries
#include "ServerFarm.hpp"

#include <chrono>		// System clock and C++11 time measurement features such as 
						// std::chrono::system_clock and time_since_epoch()::count()
						// which returns a long long value of *seconds

#include <thread>		// C++11 threading library implementing std::thread and std::thread::join 
						// std::thread launches the thread upon creation, and std::thread::join makes
						// the current thread of execution to wait for the parallel thread to return

#include <vector>		// Dynamic array wrapper class and interface

#include <random>		// C++11 random number generating classes and distributions


//** Test 1: Basic cases ***//

void test1() {

	std::cout << "*** Test 1 ***\n\n";

	// Init
	std::cout << "== Init() ServerFarm\n\n";
	ServerFarm farm;

	// Check rush hours
	farm.printRushHours();	// Nothing to print
	std::cout << "\n";

	// Check for some bad and some good input queries
	float N = -2.0;

	unsigned i = 0;
	for (; i < 5; ++i) {
		std::cout << std::boolalpha << N << " is rush hour? " << farm.IsRushHour(N) << "\n";
		N += 1.0;
	}

	// Add rush hours
	farm.AddTimeSpan(10, 12);

	std::cout << "\nAdding time span: 10:00 - 12:00\n";
	std::cout << "Is 10.30 rush hour? " << farm.IsRushHour(10.30) << "\n";		// TRUE
	std::cout << "Is 11:59 rush hour? " << farm.IsRushHour(11.59) << "\n";		// TRUE
	std::cout << "Is 12.01 rush hour? " << farm.IsRushHour(12.01) << "\n\n";	// FALSE

	// Add rush hours
	farm.AddTimeSpan(8, 15);

	std::cout << "Adding time span: 08:00 - 15:00\n";
	std::cout << "Is 10.30 rush hour? " << farm.IsRushHour(10.30)	<< "\n";	// TRUE
	std::cout << "Is 11:59 rush hour? " << farm.IsRushHour(11.59)	<< "\n";	// TRUE
	std::cout << "Is 12.01 rush hour? " << farm.IsRushHour(12.01)	<< "\n";	// TRUE
	std::cout << "Is 08.00 rush hour? " << farm.IsRushHour(8.0)		<< "\n";	// TRUE
	std::cout << "Is 14:59 rush hour? " << farm.IsRushHour(14.59)	<< "\n";	// TRUE
	std::cout << "Is 06:00 rush hour? " << farm.IsRushHour(6.0)		<< "\n\n";	// FALSE

	// Add rush hours
	farm.AddTimeSpan(24.0, 24.0);

	std::cout << "Adding time span: 24:00 - 24:00\n";
	std::cout << "Is 01:00 rush hour? " << farm.IsRushHour(1.0)		<< "\n";	// FALSE	
	std::cout << "Is 02:00 rush hour? " << farm.IsRushHour(2.0)		<< "\n";	// FALSE
	std::cout << "Is 12.01 rush hour? " << farm.IsRushHour(12.01)	<< "\n";	// TRUE
	std::cout << "Is 08.00 rush hour? " << farm.IsRushHour(8.0)		<< "\n";	// TRUE
	std::cout << "Is 14:59 rush hour? " << farm.IsRushHour(14.59)	<< "\n";	// TRUE
	std::cout << "Is 24:00 rush hour? " << farm.IsRushHour(24.0)	<< "\n\n";	// TRUE

	// Print all recorded rush hours
	std::cout << "All rush hours:\n";
	farm.printRushHours();
	
	return;
}

//=======================================================================================================


//*** Test 2: Bad input cases ***//

void test2() {

	std::cout << "\n\n*** Test 2 ***\n\n";

	// Init
	std::cout << "== Init() ServerFarm\n\n";
	ServerFarm farm;


	// Add 10 duplicates time spans
	unsigned i = 0; 
	for (; i < 10; ++i)
		farm.AddTimeSpan(10.0, 13.0);


	// Print available rush hours 
	farm.printRushHours();		// Only one time span should be printed


	// Attempt to add illegal ranges
	farm.AddTimeSpan(FLT_MAX, FLT_MIN);
	farm.AddTimeSpan(FLT_MIN, FLT_MAX);
	farm.AddTimeSpan(100.0, 200.0);
	farm.AddTimeSpan(-10.0, -1.0);
	farm.AddTimeSpan(10.0, -1.0);
	farm.AddTimeSpan(24.0, 45.0);
	farm.AddTimeSpan(10.0, 9.0);
	farm.AddTimeSpan(-1.0, 4.0);


	// Attempt query illegal times
	std::cout << "Is -01:00 rush hour? "		<< farm.IsRushHour(-1.0)		<< "\n";	// FALSE	
	std::cout << "Is FLOAT_MAX rush hour? "		<< farm.IsRushHour(FLT_MAX)		<< "\n";	// FALSE
	std::cout << "Is FLOAT_MIN rush hour? "		<< farm.IsRushHour(FLT_MIN)		<< "\n";	// FALSE
	std::cout << "Is 24:0001 rush hour? "		<< farm.IsRushHour(24.0001)		<< "\n";	// FALSE
	std::cout << "Is 300:54 rush hour? "		<< farm.IsRushHour(300.54)		<< "\n";	// FALSE
	std::cout << "Is -1000000:00 rush hour? "	<< farm.IsRushHour(-1000000.0)	<< "\n\n";	// FALSE

	// Print available rush hours 
	farm.printRushHours();		// Only one time span should be printed

	return;
}

//=======================================================================================================


//*** Test 3: Performance and stress testing  ***//

void test3() {

	std::cout << "\n\n*** Test 3 ***\n\n";

	// Init
	std::cout << "== Init() ServerFarm\n\n";
	ServerFarm farm;


	// Init the time measuring functionality
	long long start, end;

	// now() is a C++11 function object that returns the current time in milliseconds
	std::function<double()> now = [&]() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock().now().time_since_epoch()).count();
	};

	// RNG of reals in [0.0, 24.0]
	std::uniform_real<float> dist(0.0, 24.0);		// Uniform real distribution
	std::mt19937 eng;								// RNG engine to trigger randomness


	// *** Insertion

	// Add N rush time tuples and measure time
	long long INSERT = 1000;
	start = now();
	for (auto i = 0; i < INSERT; ++i) {
		eng.seed(std::chrono::system_clock::now().time_since_epoch().count());		// Recharge the RNG engine
		farm.AddTimeSpan(dist(eng), dist(eng));										// Add time span tuple
	}
	end = now();

	// Elapsed time
	std::cout << "Attempted to add " << INSERT << " time spans. Time elapsed: " << end - start << "ms\n";



	// *** Querying

	// Query asynchronously with a pool thread
	std::vector<std::thread> query_pool;

	// N querries
	long QUERIES = 1000;
	unsigned i = 0;

	// Launch and start measuring time
	start = now();
	for (; i < QUERIES; ++i) {
		query_pool.push_back(std::thread(&ServerFarm::IsRushHour, &farm, float(i)));
	}

	// Wait for all queries to execute and stop measuring time
	i = 0;
	for (; i < QUERIES; ++i)
		query_pool.at(i).join();
	end = now();

	// Time elapsed
	std::cout << "Attempted to query " << QUERIES << " times. Time elapsed: " << end - start << "ms\n";

	return;
}