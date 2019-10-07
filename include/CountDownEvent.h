#pragma once
#include <cstddef> 
#include <mutex>
#include <atomic>

namespace DnCp
{
	class CountDownEvent
	{
	protected:
		std::mutex mainMutex_;
		std::atomic<size_t> numberToCount_;
		std::condition_variable	condVar_;

	public:
		CountDownEvent(size_t numSignals) : numberToCount_{ numSignals }
		{

		}
		CountDownEvent(const CountDownEvent&) = delete;
		CountDownEvent& operator=(const CountDownEvent&) = delete;

		void WaitForAll()
		{
			std::unique_lock< std::mutex> lock( mainMutex_ );
			if (numberToCount_ == 0)
				return;
			condVar_.wait(lock);
		}
		// Signal only fires when all threads Signal()
		void Signal()
		{
			bool timeToSignal{ false };
			{
				std::unique_lock< std::mutex> lock(mainMutex_);
				numberToCount_--;
				if (numberToCount_ == 0)
					timeToSignal = true;
			}
			// Kept out of the lock to prevent any race
			if ( timeToSignal )
				condVar_.notify_all();

		}
	};
}