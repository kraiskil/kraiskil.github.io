#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

int main(void)
{
	volatile int var=0;

	auto worker = [&] () {
		for( int i=0; i<10; i++) {
			var++;
		}
	};

	std::thread threadA = std::thread(worker);
	std::thread threadB = std::thread(worker);

	threadA.join();
	threadB.join();
	std::cout << var << std::endl;
}

