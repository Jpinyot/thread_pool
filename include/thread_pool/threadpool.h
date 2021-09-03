#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include "thread_pool/mdlist.h"

namespace thread_pool {
    class ThreadPool {
        public:
            ThreadPool(const uint32_t& priorityCount, const uint32_t& threadsCount):
                _tasks(priorityCount),
                _condVar(),
                _mutexWorkers(),
                _workers(),
                _running(true) {
                // create threads
                createWorkers(threadsCount);
            }
            ~ThreadPool() {
                _running = false;
                // destroy threads
                destroyWorkers();
            }

        public:
            // add new Task
            template<class F, class ... Args>
            auto Push(const uint32_t& p, F&& function, Args&& ... args) {
                // produce new task
                auto retVal = _tasks.Produce(std::bind(function, std::forward<Args>(args)...));
                // notify one
                _condVar.notify_one();

                return retVal;
            }

        private:
            // two dimension list with ...TODO
            MDList _tasks;
            std::condition_variable _condVar;
            std::mutex _mutexWorkers;
            std::vector<std::thread> _workers;
            std::atomic<bool> _running;

        private:
            // create all requested threads
            void createWorkers(const uint32_t& threadsCount) {
                for (uint32_t i = 0; i < threadsCount; i++) {
                    std::thread(&ThreadPool::worker, this);
                }
            }
            // destroy all threads
            void destroyWorkers() {
                for (auto& worker : _workers) {
                    worker.join();
                }
            }
            // worker
            void worker() {
                std::unique_lock<std::mutex> lock(_mutexWorkers);
                while (_running) {
                    if (!_tasks.Consume()) {
                        _condVar.wait(lock);
                    }
                }
            }
    };  // class ThreadPool
}  // namespace thread_pool
