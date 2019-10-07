#pragma once

#include <cstddef> 
#include <mutex>
#include <atomic>
#include <deque>

namespace DnCp
{
	/*
	This lets you define a class based on a type of work item.  It can be a
	*/
	template <class T_>
	class ThreadPool
	{
	public:
		ThreadPool() {};
		void AddItem(T_ item) {
			std::lock_guard<std::mutex> lk(queueLock_);
			q_.push_back(item);
		};
		std::vector<std::thread> workers_;
		void StartWorking(size_t threads)
		{
			for (int i = 0; i < threads; i++) {
				workers_.emplace_back(std::thread([this] {WorkerThread(); }));
			}
		}
		void WaitUntilFinish() {
			for (auto& th : workers_)
				th.join();
		}
	protected:
		std::mutex queueLock_;
		std::deque<T_> q_;
		virtual void ProcessItem(T_&) = 0;
		void WorkerThread() {
			while (true)
			{
				T_ item;
				{
					std::lock_guard<std::mutex> lk(queueLock_);
					if (q_.size() == 0)
						return;
					item = q_.back();
					q_.pop_back();
				}
				ProcessItem(item);
			}
		}
	};
}
