// TestHarness.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <sstream> 
#include <chrono>
#include <ctime>
#include <algorithm>

#include <Barrier.h>
#include <Event.h>
#include <Semaphore.h>
#include <CountDownEvent.h>
#include <SpinLock.h>
#include <ThreadPool.h>

void myThreadBarrierTest(DnCp::Barrier & barrier)
{
	{
		std::ostringstream strm;
		strm << "In a thread - waiting" << std::this_thread::get_id() << "\n";
		std::cout << strm.str();
	}
	barrier.RegisterAndWait();
	{
		std::ostringstream strm;
		strm << "In a thread - going" << std::this_thread::get_id() << "\n";
		std::cout << strm.str();
	}
}

void TestBarrier()
{
	std::size_t threadCount{ 10 };
	DnCp::Barrier bar{ threadCount };
	std::vector<std::thread> threads;

	std::cout << "Starting barrier test with " << threadCount  << " threads.\n";

	//create threads
	for (std::size_t i = 1; i <= threadCount; ++i)
		threads.emplace_back(std::thread(myThreadBarrierTest, std::ref(bar)));
	//wait for them to complete
	for (auto& th : threads)
		th.join();

	std::cout << "Completed barrier test with " << threadCount << " threads.\n";
}

void evtThreadWaiter(DnCp::PulsedEvent& evt)
{
	{
		std::ostringstream strm;
		strm << "In a thread - waiting" << std::this_thread::get_id() << "\n";
		std::cout << strm.str();
	}
	evt.Wait();
	{
		std::ostringstream strm;
		strm << "In a thread - got my signal to go" << std::this_thread::get_id() << "\n";
		std::cout << strm.str();
	}
}

void evtThreadPulser(DnCp::PulsedEvent& evt)
{
	for (int i = 0; i < 10; i++)
	{
		{
			std::ostringstream strm;
			strm << "In a thread - pulisng " << std::this_thread::get_id() << "\n";
			std::cout << strm.str();
		}
		evt.SignalOne();

		std::chrono::milliseconds timespan(100); // or whatever
		std::this_thread::sleep_for(timespan);
	}
}

void TestPulsedEvent()
{
	DnCp::PulsedEvent evt;
	std::vector<std::thread> threads;

	auto threadCount{ 5 };

	std::cout << "Testing pulsed events with " << threadCount << " threads.\n";
	
	//create threads
	threads.emplace_back(std::thread(evtThreadPulser, std::ref(evt)));
	std::chrono::milliseconds timespan(100); // or whatever
	std::this_thread::sleep_for(timespan);
	for (std::size_t i = 1; i <= threadCount; ++i)
		threads.emplace_back(std::thread(evtThreadWaiter, std::ref(evt)));

	for (auto& th : threads)
		th.join();

	std::cout << "Completed testing pulsed events with " << threadCount << " threads.\n";

}


void evtThreadWaiterGt(DnCp::GateEvent& evt)
{
	{
		std::ostringstream strm;
		strm << "In a thread - waiting" << std::this_thread::get_id() << "\n";
		std::cout << strm.str();
	}
	evt.Wait();
	{
		std::ostringstream strm;
		strm << "In a thread - got my signal to go" << std::this_thread::get_id() << "\n";
		std::cout << strm.str();
	}
}

void evtThreadPulserGt(DnCp::GateEvent& evt)
{
	for (int i = 0; i < 1; i++)
	{
		{
			std::ostringstream strm;
			strm << "In a thread - pulisng " << std::this_thread::get_id() << "\n";
			std::cout << strm.str();
		}
		evt.SignalAll();

		std::chrono::milliseconds timespan(100); // or whatever
		std::this_thread::sleep_for(timespan);
	}
}

void TestGatedThread()
{
	DnCp::GateEvent evt;
	std::vector<std::thread> threads;
	auto threadCount{ 5 };

	std::cout << "Testing gated events for " << threadCount << " threads\n";

	//create threads
	threads.emplace_back(std::thread(evtThreadPulserGt, std::ref(evt)));
	std::chrono::milliseconds timespan(100); // or whatever
	std::this_thread::sleep_for(timespan);
	for (std::size_t i = 1; i <= threadCount; ++i)
		threads.emplace_back(std::thread(evtThreadWaiterGt, std::ref(evt)));

	for (auto& th : threads)
		th.join();

	std::cout << "Completing gated events for " << threadCount << " threads\n";
}

#define _CRT_SECURE_NO_WARNINGS

void evtThreadWaiterSem(DnCp::Semaphore& sem)
{
	char timeStr[30];
	{
		auto timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		ctime_s(timeStr, sizeof timeStr, &timenow);
		std::ostringstream strm;
		strm << timeStr << " In a thread - getting " << std::this_thread::get_id() << "\n";
		std::cout << strm.str();
	}
	sem.Get();
	{
		auto timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		ctime_s(timeStr, sizeof timeStr, &timenow);
		std::ostringstream strm;
		strm << timeStr << " In a thread - got " << std::this_thread::get_id() << "\n";
		std::cout << strm.str();
	}

	std::chrono::milliseconds timespan(100); // or whatever
	std::this_thread::sleep_for(timespan);
	{
		auto timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		ctime_s(timeStr, sizeof timeStr, &timenow);
		std::ostringstream strm;
		strm << timeStr << " In a thread - releasing " << std::this_thread::get_id() << "\n";
		std::cout << strm.str();
	}
	sem.Release();
	{
		auto timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		ctime_s(timeStr, sizeof timeStr, &timenow);
		std::ostringstream strm;
		strm << timeStr << " In a thread - released " << std::this_thread::get_id() << "\n";
		std::cout << strm.str();
	}
}

void SemaphoreTester()
{
	DnCp::Semaphore sem{ 5 };
	std::vector<std::thread> threads;
	//create threads
	auto threadCount{ 20 };

	std::cout << "Testing semaphores for " << threadCount << " threads.\n";
	for (std::size_t i = 1; i <= threadCount; ++i)
		threads.emplace_back(std::thread(evtThreadWaiterSem, std::ref(sem)));

	for (auto& th : threads)
		th.join();
	std::cout << "Done semaphores for " << threadCount << " threads.\n";
}

void testSpinLock(DnCp::SpinLock & slok)
{
	char timeStr[40];
	{
		auto timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		ctime_s(timeStr, sizeof timeStr, &timenow);
		std::ostringstream strm;
		strm << timeStr << " In a thread - spinlock getting " << std::this_thread::get_id() << "\n";
		std::cout << strm.str();
	}
//	DnCp::SpinGuard sg(slok);
	std::lock_guard<DnCp::SpinLock> lk(slok);
	{
		auto timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		ctime_s(timeStr, sizeof timeStr, &timenow);
		std::ostringstream strm;
		strm << timeStr << " In a thread - got spinlock " << std::this_thread::get_id() << "\n";
		std::cout << strm.str();
	}
	std::chrono::milliseconds timespan(1500); // or whatever
	std::this_thread::sleep_for(timespan);
	{
		auto timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		ctime_s(timeStr, sizeof timeStr, &timenow);
		std::ostringstream strm;
		strm << timeStr << " In a thread - releasing spinlock " << std::this_thread::get_id() << "\n";
		std::cout << strm.str();
	}
}

void SpinTest()
{
	DnCp::SpinLock sl;

	std::vector<std::thread> threads;
	//create threads
	auto threadCount{ 15 };

	std::cout << "Testing spin lock for " << threadCount << " threads.\n";
	for (std::size_t i = 1; i <= threadCount; ++i)
		threads.emplace_back(std::thread(testSpinLock, std::ref(sl)));

	for (auto& th : threads)
		th.join();

	std::cout << "Done Testing spin lock for " << threadCount << " threads.\n";
}

void testCountdownEvent(DnCp::CountDownEvent& ce)
{
	std::ostringstream strm;
	strm << " In a thread - releasing count down event " << std::this_thread::get_id() << "\n";

	std::chrono::milliseconds timespan(500); // or whatever
	std::this_thread::sleep_for(timespan);
	std::cout << strm.str();
	ce.Signal();
}

void countDownEventTest()
{
	DnCp::CountDownEvent ce{ 10 };

	std::vector<std::thread> threads;
	//create threads
	auto threadCount{ 10 };

	std::cout << "Testing count-down event for " << threadCount << " threads.\n";

	for (std::size_t i = 1; i <= threadCount; ++i)
		threads.emplace_back(std::thread(testCountdownEvent, std::ref(ce)));
	ce.WaitForAll();

	for (auto& th : threads)
		th.join();

	std::cout << "Done Testing count-down event for " << threadCount << " threads.\n";

}

class VectorSorterQ : public DnCp::ThreadPool< std::vector< int > >
{
public:
	VectorSorterQ() {};
	virtual void ProcessItem(std::vector< int > & vec) override
	{
		std::sort(vec.begin(), vec.end());
	}
};

void TestWorkQueue()
{
	std::cout << "Testing work queue\n";
	size_t vecLength{ 10000 };
	size_t numVecs{ 30 };
	VectorSorterQ vq;
	for (size_t i = 0; i < numVecs; i++)
	{
		std::vector< int > rvec(vecLength);
		std::generate(rvec.begin(), rvec.end(),
			[]() {return rand() % 1000; });
		vq.AddItem(rvec);
	}
	std::cout << "Testing work queue - generated " << numVecs << " vectors \n";
	vq.StartWorking(5);
	vq.WaitUntilFinish();
	std::cout << "Testing work queue - done \n";
}

int main()
{
	TestWorkQueue();
	TestBarrier();
	TestPulsedEvent();
	TestGatedThread();
	SemaphoreTester();
	SpinTest();
	countDownEventTest();
}
