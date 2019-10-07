#pragma once
// Essentially a C# like event
#include <cstddef> 
#include <mutex>
#include <atomic>

namespace DnCp
{
	// If the event is signaled before you wait then you will have to wait until the next pulse
	class PulsedEvent
	{
	public:
		PulsedEvent() {};
		PulsedEvent(const PulsedEvent&) = delete;
		PulsedEvent& operator=(const PulsedEvent&) = delete;
		void Wait()
		{
			std::unique_lock< std::mutex > lock(mainMutex_);
			condVar_.wait(lock);
		}
		void SignalAll()
		{
			condVar_.notify_all();
		}
		void SignalOne()
		{
			condVar_.notify_one();
		}
	protected:
		std::mutex				mainMutex_;
		std::condition_variable	condVar_;
	};

	// This event type opens and then closes again programatically. 
	class GateEvent
	{
	public:
		GateEvent() {};
		void Wait()
		{
			std::unique_lock< std::mutex > lock( mainMutex_ );
			if (barrierOpen_)
				return;
			condVar_.wait( lock );
		}
		void SignalAll()
		{
			barrierOpen_ = true;
			condVar_.notify_all();
		}
		void CloseEvent()
		{
			std::unique_lock< std::mutex > lock(mainMutex_);
			barrierOpen_ = false;
		}
	protected:
		std::mutex				mainMutex_;
		std::condition_variable	condVar_;
		std::atomic<bool>		barrierOpen_{ false };
	};
}
