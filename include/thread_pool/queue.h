#include <atomic>
#include <future>
#include <functional>
#include <tuple>
#include <utility>

//TODO(jpinyot): change to correct cache line size
constexpr uint32_t kCacheLineSize = 128;

// Lock-free Queue of Tasks:
//  - the first element is ALWAYS a dummy element that is deleted after executing the next element,
//    after deleting the first element, the executed elements becomes the dummy element.
//  - Producers threads adds new Tasks to the TAIL of the Queue.
//  - Consumer threads execute Task on the HEAD of the Queue.

namespace thread_pool {
    class Queue {
        public:
            Queue():
                _first(nullptr), _last(nullptr),
                _consumerLock(false), _producerLock(false),
                _taskCount(0) {
                    // init list with dummy element
                    _first = _last = new Task();
                }
            ~Queue() {
                // delete all pending tasks
                while (_first != nullptr) {
                    Task* tmp = _first;
                    _first = _first->next;
                    delete tmp;
                }
            }

        public:
            // add new task to the queue
            // returns a future
            template<class F, class ... Args>
            auto Produce(F&& function, Args&& ... args) {
                // get return type
                typedef decltype(function(args...)) RetType;
                // package the task
                std::packaged_task<RetType()> newTask(std::bind(function,
                            std::forward<Args>(args)...));
                // get the future from the task before the task is moved into the queue
                std::future<RetType> future = newTask.get_future();
                // cretate new Task
                Task* tmp = new AnyTask<RetType>(std::move(newTask));

                // wait until acquire exclusivity
                while (_producerLock.exchange(true)) {
                }

                // publish to consumers
                _last->next = tmp;
                // swing last forward
                _last = tmp;
                // increase task count
                _taskCount += 1;
                // release exclusivity
                _producerLock = false;

                // return the future
                return future;
            }

            // consume existing task
            void Consume() {
                // wait until acquire exclusivity
                while (_consumerLock.exchange(true)) {
                }

                Task* next = _first->next;
                Task* oldFirst = _first;
                // if queue is nonempty
                if (next != nullptr) {
                    // get function from the Task
                    auto function = next->GetFunction();
                    // move first pointer to the next pointer
                    _first = next;
                    // decrease task count
                    _taskCount -= 1;
                    // release exclusivity
                    _consumerLock.exchange(false);
                    // execute function if is valid
                    if (function.valid()) {
                        function();
                    }
                    // delete old first Task
                    delete oldFirst;
                }
                // queue is empty
                else {
                    // release exclusivity
                    _consumerLock.exchange(false);
                }
            }

            // return number of tasks remaining
            uint32_t TasksCount() {
                return _taskCount;
            }


        private:
            // class Task is a node of the Queue holding the task to be executed
            class Task {
                public:
                    Task():
                        next(nullptr) {
                    }
                    virtual ~Task() = default;
                public:
                    // pointer to the next object in the Queue
                    std::atomic<Task*> next;

                    // get function return a non valid packaged_task
                    virtual std::packaged_task<void()> GetFunction()
                    {return std::packaged_task<void()>();}
            };  // class Task

            // used polymorphism to store any type of funtion in the job
            template <typename RetType>
            class AnyTask : public Task {
                public:
                    AnyTask(std::packaged_task<RetType()> function):
                        Task(),
                        _function(std::move(function)) {
                }
                // get function
                std::packaged_task<void()>GetFunction() override {return std::move(_function);}

                private:
                    // task to be executed
                    std::packaged_task<void()> _function;
            };  // class AnyTask

            //TODO(jpinyot): add padding to each pointer
            // first element
            Task* _first;
            // last element
            Task* _last;
            // shared among consumers
            std::atomic<bool> _consumerLock;
            // shared among producers
            std::atomic<bool> _producerLock;
            // number of tasks remaining in the Queue
            std::atomic<uint32_t> _taskCount;
    };  // class Queue
};  // namespace thread_pool
