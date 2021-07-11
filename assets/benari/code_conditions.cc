#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

int main(void)
{
	volatile int var=0;
	std::mutex mtx;
	std::condition_variable cv;

	auto worker = [&] () {
		{
			std::unique_lock<std::mutex> lck(mtx);
			cv.wait(lck, []{return true;});
		}

		for( int i=0; i<10; i++) {
			int tmp=var;
			tmp++;
			var=tmp;
		}
	};

	std::thread threadA = std::thread(worker);
	std::thread threadB = std::thread(worker);

	{
		std::lock_guard<std::mutex> lck(mtx);
		cv.notify_all();
	}

	threadA.join();
	threadB.join();
	std::cout << var << std::endl;
}

