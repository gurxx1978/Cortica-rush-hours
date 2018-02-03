/*
*	© Superharmonic Technologies
*	Pavlos Sakoglou
*
*  ========================================================
*
*	Server Farm Resource Save-Mode Mechanism Implementation
*
*/

//*** ServerFarm class implementation ***//

// Include the class definition
#include "ServerFarm.hpp"


//*** Constructor and Destructor ***//

// The constructor and destructor simply instantiate the members without any
// default assumptions, thus we use the C++11 keyword "default" for optimization
// purposes. The constructor will create an std::set of std::tuples by calling their
// own constructors automatically, and the destructor will destroy these objects by
// calling their own destructors automatically. Same with the mutex and condition variables
ServerFarm::ServerFarm()	= default;
ServerFarm::~ServerFarm()	= default;


//*** Insert method implementation ***//

// Simply checks for legal input, and prints an error message in case it's out of bounds
void ServerFarm::AddTimeSpan(float start_time, float end_time) {

	// Make sure this method is used by one thread at the time
	std::unique_lock<std::mutex> lock(mt_insert);

	// In case the input is illegal
	if (!(start_time >= 0.0 && start_time <= 24.0 && end_time >= 0.0 && end_time <= 24.0 && start_time <= end_time)) {
		// 1) Print an error message
		// 2) Release the lock
		// 3) Notify waiting threads
		// 4) Return
		std::cerr << "Invalid input!\n";	
		lock.unlock();
		cv_insert.notify_one();
		return;
	}

	// In case the input range is legal
		// 1) Insert a tupled version of the input
		// 2) Release the lock
		// 3) Notify waiting threads
		// 4) Return
	rush_hours.insert(std::make_tuple(start_time, end_time));
	lock.unlock();
	cv_insert.notify_one();
	return;
}


//*** Auxiliary print method ***//

// Prints the contents of the rush hour database
// For demo only, thus no need for thread safety
void ServerFarm::printRushHours() {

	// If no rush hours are recorded, print an error message and return 
	if (rush_hours.empty()) {
		std::cerr << "No rush hours are specified!\n";
		return;
	}
		
	// Iterate and print all rush hours using std::for_each and a lambda inlined function
	std::for_each(std::begin(rush_hours), std::end(rush_hours), [&](auto tup) {
		std::cout << std::get<0>(tup) << " - " << std::get<1>(tup) << "\n";
	});

	return;
}