#include "loop.hpp"
void Loop::start() { loopRun_.store(true); }

void Loop::stop() { loopRun_.store(false); }

void Loop::exit() {
  loopBreak_.store(true);
  if (loopThread_.joinable())
    loopThread_.join();
}

void Loop::loopExec_() {
  while (!loopBreak_.load()) {
    while (loopRun_.load()) {
      std::unique_lock<std::mutex> locker(loopMutex_);
      auto iter = calls_.begin();
      if (iter == calls_.end())
        continue;
      auto call = std::move(*iter);
      calls_.erase(iter);
      locker.unlock();
      call();
      if (routine_) {
        locker.lock();
        calls_.push_back(routine_);
      }
    }
  }
}

void Loop::routine(const std::function<void()> func) {
  routine_ = func;
  std::unique_lock<std::mutex> locker(loopMutex_);
  calls_.push_back(routine_);
}

void Loop::callLater(const std::function<void()> func) {
  {
    std::unique_lock<std::mutex> locker(loopMutex_);
    calls_.push_back(func);
  }
}
