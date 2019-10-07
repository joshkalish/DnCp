#pragma once
#include <cstddef> 
#include <mutex>
#include <atomic>

// Simple barrier - just enough for what I need to do.
// 
namespace DnCp
{

	class Barrier
	{
	protected:
		std::size_t				threadsToSync_;
		std::size_t				threadsReady_;
		std::mutex				mainMutex_;
		std::condition_variable	condVar_;
		std::atomic<bool>		barrierOpen_{ false };
	public:
		// Copy or assign doesn't make sense
		Barrier(const Barrier&) = delete;
		Barrier& operator=(const Barrier&) = delete;

		Barrier(std::size_t threads) : threadsToSync_{ threads }, threadsReady_{ 0 }
		{
		}
		// Each thread calls this and will wait until all treads are here
		void RegisterAndWait()
		{
			// Increase the count 
			{
				std::unique_lock< std::mutex > lock(mainMutex_);
				threadsReady_++;
				if (threadsReady_ == threadsToSync_)
				{
					barrierOpen_ = true;
				}
				else if (threadsReady_ > threadsToSync_)
				{
					// WTF - too many clients
					std::cerr << "This barrier is set for " << threadsToSync_ << " threads but " << threadsReady_ <<
						" threads signaled.  Noticed from thread ID: " << std::this_thread::get_id() << std::endl;

				}
			}
			// This thread was the winner -- signal all and just return
			if (barrierOpen_)
			{
				condVar_.notify_all();
			}
			// No, we need to wait - check the flag to make sure we didn't just unlock before in another thread
			else if ( !barrierOpen_ )
			{
				std::unique_lock< std::mutex > lock(mainMutex_);
				condVar_.wait(lock);
			}
		}
	};

}