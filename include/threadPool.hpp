#pragma once
#include <common.hpp>

class ThreadPool {
public:
  ThreadPool(size_t threadCount = std::thread::hardware_concurrency());
  ~ThreadPool();

  void enqueue(std::function<void()> job);

private:
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> jobs;

  std::mutex queueMutex;
  std::condition_variable cv;
  std::atomic<bool> stop{false};

  void workerLoop();
};