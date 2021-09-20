#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include "thread_pool/mdlist.h"

// default value for the number of prioritys
constexpr uint32_t kPriorityCount = 1;
// determine how many cores a machine has
// may return 0 when not able to detect
const uint32_t kProcessorCount = std::thread::hardware_concurrency();

namespace thread_pool {
    class ThreadPool {
        public:
            ThreadPool(const uint32_t& priorityCount = 1,
                    const uint32_t& threadsCount = (kProcessorCount != 0)? kProcessorCount : 1):
                _tasks(priorityCount),
                _condVar(),
                _mutexWorkers(),
                _workers(),
                _running(true) {
                // create threads
                createWorkers(threadsCount);
            }
            ~ThreadPool() {
                // exit running
                _running = false;
                // notify all
                _condVar.notify_all();
                // destroy threads
                destroyWorkers();
            }

        public:
            // add new Task
            template<class F, class ... Args>
            auto Push(const uint32_t& p, F&& function, Args&& ... args) {
                // produce new task
                auto retVal = _tasks.Produce(p, std::bind(function, std::forward<Args>(args)...));
                // notify one
                _condVar.notify_one();

                return retVal;
            }

        private:
            MDList _tasks;
            std::condition_variable _condVar;
            std::mutex _mutexWorkers;
            std::vector<std::thread> _workers;
            std::atomic<bool> _running;

        private:
            // create all requested threads
            void createWorkers(const uint32_t& threadsCount) {
                for (uint32_t i = 0; i < threadsCount; i++) {
                    _workers.emplace_back(std::thread(&ThreadPool::worker, this));
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
                        // TODO(jpinyot): Prevent all thread to wait with task on the queue
                        _condVar.wait(lock);
                        /* std::this_thread::yield(); */
                    }
                }
            }
    };  // class ThreadPool
}  // namespace thread_pool
