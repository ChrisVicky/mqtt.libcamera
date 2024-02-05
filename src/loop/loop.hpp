#ifndef __LOOP_HPP__
#define __LOOP_HPP__

#include <atomic>
#include <functional>
#include <list>
#include <mutex>
#include <thread>

class Loop {
  // Loop
  std::mutex loopMutex_;
  std::atomic<bool> loopRun_{false};
  std::atomic<bool> loopBreak_{false};
  std::list<std::function<void()>> calls_;
  std::function<void()> routine_;
  std::thread loopThread_;
  void loopExec_();

public:
  Loop() { loopThread_ = std::thread(&Loop::loopExec_, this); }
  void start();
  void stop();
  void exit();
  void callLater(const std::function<void()> func);
  void routine(const std::function<void()> func);
};
#endif // !__LOOP_HPP__
