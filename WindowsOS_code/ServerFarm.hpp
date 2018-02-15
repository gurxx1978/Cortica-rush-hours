#pragma once

// Import standard libraries
#include <set>
#include <tuple>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <algorithm>
// * Data structure model: O(log n) insertion on average, O(n) worst case
// * Query method: O(n) worst case
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
		for (const auto &e : rush_hours) 
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
