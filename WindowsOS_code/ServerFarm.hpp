/*
*	© Superharmonic Technologies
*	Pavlos Sakoglou
*
*  ========================================================
*
*	Server Farm Resource Save-Mode Mechanism Definition
*
*/

// Multiple inclusion guard
#ifndef SERVER_FARM_HPP
#define SERVER_FARM_HPP

// Import standard libraries
#include <set>			// Set data structure. Implements a binary tree
						// and supports in order insertion in O(log n) time.
						// A better choice would be std::unordered_set but that 
						// would require a hash function overload that supports std::tuples
						// which is the key. For our purposes this will still work
						// but ideally we would work around an unordered set 

#include <tuple>		// C++11 data structure that models an N-dimensional std::pair
						// Provides a n-tuple interface with n being a meta parameter
						// and elements are accessible via the std::get<i>(tuple) method,
						// where i is the ith element of the tuple

#include <iostream>		// Standard I/O etc.

#include <mutex>		// C++11 interface that provides mutual exclusion guards in case
						// of multi-threading. It's good to implement this mechanism in case
						// our server has async requests of high volume. See test3() for demo

#include <condition_variable>	// C++11 synchronization primitive that notifies waiting threads
								// that are currently locked out of the method. NotifyOne() method 
								// wakes up a single waiting thread of the waiting pool

#include <algorithm>	// Includes for_each(). Notice that for_each is valid until C++20. Then it will
						// be deprecated since an equivalent looping method is provided by 
						// the C++11 range-based loop

//*** ServerFarm class definition ***// 

// This class models a request/query interface for our server farm.
// Client computers can query the rush hour database to check whether or
// not a particular hour is considered rush hour.
// 
// * Data structure model: O(log n) insertion on average, O(n) worst case
// The rush hour database is modeled by an ordered binary tree (std::set) of 2-dimensional
// tuples and does not allow duplicates -- this is an attribute of an std::set. Thus, whenever
// we attempt to insert an existing tuple the STL data structure will prevent it for us internally.
// The limitation of std::set is that it does in-order insertion, which takes up to logarithmic
// time, when ideally we would like constant insertion time. As mentioned above, this can be 
// takcled by using a unordered set instead, in which case we would have to overload the 
// key hash method so it can hash std::tuples.
//
// * Query method: O(n) worst case
// The query method uses a range based for loop to iterate in all time span tuples
// and check if the queried time is in that range. Returns true or false accordingly.
// The query method might be called by multiple client computers, thus it is safer 
// to implement synchronization primitives and mutual exclusion mechanisms to make
// sure there is no confusion in the output. Additionally, we need to unlock the mutex
// in all cases before the function returns, otherwise we get deadlock. In case an 
// exception is thrown before we unlock, the std::unique_lock wrapper class will make
// sure to unlock the mutex as well, thus preventing a deadlock in all cases. Alternatively, 
// we would also have to unlock in the destructor.
//
// * Auxiliary print method
// I decided to add a print method as well for better testing and demo convenience.
// It simply prints all tuples in the database
class ServerFarm {
public:
	// Default constructors and destructors are enough.
	ServerFarm();
	~ServerFarm();

	// Insertion method: O(log n) worst case
	void AddTimeSpan(float start_time, float end_time);

	// Auxiliary print method
	void printRushHours();

	// Since the query method is called frequently, we inline it
	// O(n) worst case
	inline bool IsRushHour(float time) {

		// Make sure this method is used by one thread at the time
		std::unique_lock<std::mutex> lock(mt_query);

		// In case of illegal input:
			//	1) Alert the user
			//	2) Release the lock 
			//	3) Notify waiting threads
			//	4) Return false
		if (!(time >= 0.0 && time <= 24.0)) {
			std::cerr << "Invalid input!\n";
			lock.unlock();
			cv_query.notify_one();
			return false;
		}

		// In case there are no rush times defined:
			// 1) Release the lock 
			// 2) Notify waiting threads
			// 3) Return false
		if (rush_hours.empty()) {
			lock.unlock();
			cv_query.notify_one();
			return false;
		}
			
		// Iterate the tuples to check for rush hour
		// This is a C++11 range-based loop and is as efficient and optimized
		// as the std::for_each algorithm
		for (auto e : rush_hours) 
			// In case the queried time is a rush hour
				// 1) Release the lock
				// 2) Notify waiting threads
				// 3) Return true
			if (time >= std::get<0>(e) && time <= std::get<1>(e)) {
				lock.unlock();
				cv_query.notify_one();
				return true;
			}
			
		// In case the queried time was not found in the database
			// 1) Release the lock
			// 2) Notify waiting threads
			// 3) Return false
		lock.unlock();
		cv_query.notify_one();
		return false;
	}

private:
	// Rush hour database: a set of distinct time-span tuples
	std::set<std::tuple<float, float>> rush_hours;

	// Multithreading protection mechanisms
	std::mutex mt_query, mt_insert;
	std::condition_variable cv_query, cv_insert;

	// No need for assignment operator or copy constructor
	// so we implement them as private to prevent the compiler
	// to generate them internally and use them without our
	// intention. If we want multiple copies of the server
	// we can implement them as public members
	ServerFarm(const ServerFarm&);
	ServerFarm& operator=(const ServerFarm&);
};

#endif // !SERVER_FARM_HPP