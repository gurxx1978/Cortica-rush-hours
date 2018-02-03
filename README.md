# Server Rush Times

This application descirbes a query/insertion interface that allow client machines to record rush hours in a server and then query a timestamp to check if its a rush hour or not. The solution is asymptotically linear with the current description and limitations, but can be improved to constant query time with a few modifications and some space sacrifice. 

# Input Interface

- AddTimeSpan(float start\_time, float end\_time)

- IsRushHour(float time);

# Components

- ServerFarm class: encapsulates a database of distinct time span tuples, and multithreading guards to prevent race conditions.

- Test cases: performs basic tests, bad input tests, and stress testing and performance analysis

# Example output

### Simple test

![Data-Flow](/img/test1.jpg)

### Bad input test

![Data-Flow](/img/test2.jpg)

### Stress Testing / Performance analysis

![Data-Flow](/img/test3.jpg)

# Complexity

- Insertion in O(n) asymptotically

- Querying in O(n) asymptotically

# Improvements

It would be a good idea to get the user input in the form of integer input that would describe the hours and minutes separately for the range. Thus, instead of having real numbers involved (which are meaningfuly uncountable) we could provide a method prototypes as follows:

	void AddTime(unsigned start_hour, unsigned start_min, unsigned end_hour, unsigned end_min)
	
	void IsRushHour(unsigned hour, unsigned min)

with restriction on the input as follows:

	start_hour, end_hour in {0, ..., 24}
	start_min, end_min in {0, ..., 60}

This gives a total of 1,440 hour-minute combinations of querying, which is significantly less than 86,400. Additionally, in that case we could sacrifice some space a create a hash table using STL's std::unordered\_map which will hold all the past queried times, and the idea is that if a time is queried twice, the hash table will look up and return in O(1) average and O(log n) worst case, which won't affect the current performance which is O(n) worst case. The benefit is that often queries will now be O(1) and that would significantly improve performance in time.		

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

This would work without initialization, cause upon construction the unordered map will assume all key values to be false.

# How to run

1) Visual Studio 2015 or newer and CodeBlocks

Create a new project and include all code files as is. Run the program to see the default test cases. Modify Test.cpp by commenting in the other test cases, re-build, and run.

You can get Visual Studio Community version for free here: https://www.visualstudio.com/downloads/

You can get CodeBlocks and installation instructions here: https://www.ntu.edu.sg/home/ehchua/programming/howto/CodeBlocks_HowTo.html

2) Windows command line 

Instructions can be found here: https://www.youtube.com/watch?v=SvEPC3d85ZA


