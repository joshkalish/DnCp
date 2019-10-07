#pragma once
#include <cstddef> 
#include <mutex>
#include <atomic>

// Basically taking from the C++ 
namespace DnCp
{
	class SpinLock
	{
	protected:
		std::atomic_flag lock_;
	public:
		SpinLock() : lock_{ ATOMIC_FLAG_INIT }
		{
		};
		SpinLock(const SpinLock&) = delete;
		SpinLock& operator=(const SpinLock&) = delete;
		bool lock()
		{
			while ( lock_.test_and_set(std::memory_order_acquire) ) {};

			return true;
		}
		void unlock()
		{
			lock_.clear(std::memory_order_release);
		}
	};

	class SpinGuard
	{
	protected:
		SpinLock & spinLock_;
	public:
		SpinGuard(SpinLock& spinLock) : spinLock_{ spinLock }
		{
			spinLock_.lock();
		}
		~SpinGuard()
		{
			spinLock_.unlock();
		}
	};
}