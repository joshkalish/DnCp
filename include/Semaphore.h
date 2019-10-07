#pragma once
#pragma once
// Essentially a C# like event
#include <cstddef> 
#include <mutex>
#include <atomic>

namespace DnCp
{
	class Semaphore
	{
	protected:
		std::mutex mainMutex_;
		std::atomic<size_t> maxSize_;
		std::atomic<size_t> curClients_;
		std::condition_variable	condVar_;

	public:
		Semaphore(size_t maxClients) : maxSize_{ maxClients }, curClients_{ 0 }
		{

		}
		Semaphore(const Semaphore&) = delete;
		Semaphore& operator=(const Semaphore&) = delete;

		void Get()
		{
			std::unique_lock< std::mutex> lock(mainMutex_);
			if (curClients_ == maxSize_)
			{
				condVar_.wait(lock);
				//std::unique_lock< std::mutex> lock2(mainMutex_);
				curClients_++;
			}
			else
			{
				curClients_++;
			}
		}
		void Release()
		{
			std::unique_lock< std::mutex> lock(mainMutex_);
			curClients_--;
			if (curClients_ + 1 == maxSize_)
			{
				condVar_.notify_one();
			}
		}
	};
}