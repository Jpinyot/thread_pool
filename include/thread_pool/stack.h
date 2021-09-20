#include <algorithm>
#include <atomic>
#include <future>
#include <functional>
#include <memory>
#include <tuple>
#include <utility>

//TODO(jpinyot): change to correct cache line size
constexpr uint32_t kCacheLineSize = 128;

// Lock-free Stack of Tasks:
namespace thread_pool {
    class Stack {
        public:
            Stack():
                _first(nullptr),
                _taskCount(0) {
            }
            ~Stack() = default;

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
                auto tmp = std::make_shared<AnyTask<RetType>>(std::move(newTask));
                // cast from AnyTast<T> to Task
                auto p = static_cast<std::shared_ptr<Task>>(tmp);
                // swap _first pointer to the new task
                p->next = _first;
                while(!std::atomic_compare_exchange_weak(&_first, &p->next, p)) {}
                // increase task counter
                _taskCount += 1;

                return future;
            }

            void Consume() {
                // get first task in the list
                auto task = getFront();
                if (task) {
                    // get function
                    auto function = task->GetFunction();
                    if (function.valid()) {
                        // execute function
                        function();
                        _taskCount -= 1;
                    }
                }
            }

            // return number of tasks remaining
            uint32_t TasksCount() {
                return _taskCount;
            }


        private:
            // class Task is a node of the Stack holding the task to be executed
            class Task {
                public:
                    Task():
                        next(nullptr)
                {}
                    virtual ~Task() = default;

                public:
                    // pointer to the next object in the Stack
                    /* std::atomic<Task*> next; */
                    std::shared_ptr<Task> next;

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
                    std::packaged_task<void()>GetFunction() override
                    {return std::move(_function);}

                private:
                    // task to be executed
                    std::packaged_task<void()> _function;
            };  // class AnyTask

            //TODO(jpinyot): add padding to each pointer
            // first element
            std::shared_ptr<Task> _first = nullptr;
            // number of tasks remaining in the Stack
            std::atomic<uint32_t> _taskCount;

        private:
            std::shared_ptr<Task> getFront() {
                auto p = std::atomic_load(&_first);
                while (p && !std::atomic_compare_exchange_weak(&_first, &p, p->next)) {
                }
                return std::move(p);
            }
    };  // class Stack
};  // namespace threadpool
