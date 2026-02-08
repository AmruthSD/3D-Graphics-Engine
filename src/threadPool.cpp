#include <threadPool.hpp>

ThreadPool::ThreadPool(size_t threadCount) {
  for (size_t i = 0; i < threadCount; i++) {
    workers.emplace_back(&ThreadPool::workerLoop, this);
  }
}

ThreadPool::~ThreadPool() {
  stop = true;
  cv.notify_all();

  for (auto &w : workers) {
    if (w.joinable())
      w.join();
  }
}

void ThreadPool::enqueue(std::function<void()> job) {
  {
    std::lock_guard<std::mutex> lock(queueMutex);
    jobs.push(std::move(job));
  }
  cv.notify_one();
}

void ThreadPool::workerLoop() {
  while (true) {
    std::function<void()> job;

    {
      std::unique_lock<std::mutex> lock(queueMutex);
      cv.wait(lock, [&] { return stop || !jobs.empty(); });

      if (stop && jobs.empty())
        return;

      job = std::move(jobs.front());
      jobs.pop();
    }

    job();
  }
}