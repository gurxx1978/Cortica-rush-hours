/*
*	© Superharmonic Technologies
*	Pavlos Sakoglou
*
*  ================================================
*
*	Server Farm Resource Save-Mode Mechanism Test
*
*/

#include "TestCases.hpp"

#include <unordered_map>

int main() {

	// Basic test
	test1();

	// Bad input cases
	// test2();

	// Performance / Stress Testing
	// test3();

	/*** Bonus Question:

		How would your solution be different if we changed the definition of "time" to be an integer, representing the time of day in seconds?
		(answer in words)

		Answer:

			24 hours are equivalent to 24 * 60 * 60 = 86,400 seconds (integer time units)
			The idea of insertion and querying would still be the same, thus the implementation 
			wouldn't change. A client that inserts a rush time would simply specify the range in seconds
			where 0 seconds is the time at 00:00, and 86,400 seconds is the time at 24:00.
			An input tuple with values (10, 20) would correspond to the second 10 seconds of the day.
			A time representation in seconds would mostly be meaningful in case of insertion and querying 
			by machines, whereas an hours-minutes representation would work better with humans.

			Taking it a step further, it would be a good idea to get the user input in the form of integer
			input that would describe the hours and minutes separately for the range. Thus, instead of having 
			real numbers involved (which are meaningfuly uncountable) we could provide a method prototypes as 
			follows:

				void AddTime(unsigned start_hour, unsigned start_min, unsigned end_hour, unsigned end_min)

				void IsRushHour(unsigned hour, unsigned min)

			with restriction on the input as follows:

				start_hour, end_hour in {0, ..., 24}
				start_min, end_min in {0, ..., 60}

			This gives a total of 1,440 hour-minute combinations of querying, which is significantly less than 86,400
			Additionally, in that case we could sacrifice some space a create a hash table using STL's std::unordered_map
			which will hold all the past queried times, and the idea is that if a time is queried twice, the hash table 
			will look up and return in O(1) average and O(log n) worst case, which won't affect the current performance
			which is O(n) worst case. The benefit is that often queries will now be O(1) and that would significantly 
			improve performance in time.		

			Such an implementation will look like:

				private:
					std::unordered_map<long, bool> requested_minutes, requested_hours;

			and inside the query method we add:

				if (requested_hours[input_hours] == true)
					if (requested_minutes[input_minutes] == true)
						return true;
					else
						requested_minutes[input_minutes] = true
				else
					requested_hours[input_hours] = true

			This would work without initialization, cause upon construction the unordered map will assume 
			all key values to be false. You can confirm this by running the following snipped; having included
			the <unordered_map> header:

				***
					std::unordered_map<long, bool> req;

					// This will evaluate false
					if (req[10] == true)
						std::cout << "Fail!\n";

					// This will evaluate true
					if (req[10] == false)
						std::cout << "Success!\n";

				***

			Unfortunately, this cannot be implemented with a floating number representation of time.

			This was a fun exercise. I was glad to work on it. I would be happy to look into hubrid data structures
			to improve efficiency further i.e. constant time look up, but I believe this solution will still do well
			as it's asymptotically linear at worst case, and unless we start sacrificing space, there is no way to beat
			linear time in look-up with comparison operations.

			Current performance: 
			
				Queries		ms				Insertions		ms
				1'000		382				1'000			62
				10'000		2665			10'000			578		
				100'000		22490			100'000			4015	
				1'000'000	-				1'000'000		40,890

			Asymptotically linear:
								Queries ~ O(n) 
								Insertions ~ O(n)

	****************************************************************************************************************************/

	return 0;
}
