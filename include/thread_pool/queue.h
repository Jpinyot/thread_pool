#include <atomic>
#include <future>
#include <functional>
#include <tuple>

//TODO(jpinyot): change to correct cache line size
constexpr uint32_t kCacheLineSize = 128;

template<typename F, typename ... Args>
class Queue {
    public:
        Queue():
            _first(nullptr), _divider(nullptr), _last(nullptr),
            _consumerLock(false), _producerLock(false) {
            // init list with dummy element
            _first = _divider = _last = new Task(nullptr, nullptr);
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
        void Produce(const F&& function, const Args&& ... args) {
            Task* tmp = new Task(std::forward<F>(function), std::forward<Args>(args)...);
            // wait until acquire exclusivity
            while (_producerLock.exchange(true)) {
            }

            // publish to consumers
            _last->next = tmp;
            // swing last forward
            _last = tmp;
            // release exclusivity
            _producerLock = false;
        }

        // consume existing task
        // TODO(jpinyot): return the return value of the function
        auto Consume(F& result) {
            // wait until acquire exclusivity
            while (_consumerLock.exchange(true)) {
            }

            Task* next = _first->next;
            // if queue is nonempty
            if (next != nullptr) {
                // copy function and arguments from the Task
                F function = next->_function;
                std::tuple<Args ...> args = next->_args;
                _first = next;
                // release exclusivity
                _consumerLock.exchange(false);
                //TODO(jpinyot): execute function if exist
                return true;
            }
            // queue is empty
            else {
                // release exclusivity
                _consumerLock.exchange(false);
                return false;
            }
        }


    private:
        // class Task is a node of the Queue holding the task to be executed
        class Task {
            public:
                // TODO(jpinyot): check if makes sense to change pointer to references
                Task(F&& function, Args&& ... args):
                    _function(std::forward<F>(function)),
                    _args(std::forward<Args>(args)...),
                    _next(nullptr) {
                }
                ~Task() = default;

            private:
                // TODO(jpinyot): pass to raw pointers or unique_ptr to avoid blocking
                // while copyng the argument
                std::function<F(Args ...)> _function;
                std::tuple<Args ...> _args;
                std::atomic<F> _next;
                //TODO(jpinyot): add padd to each pointer
        };

        //TODO(jpinyot): add padd to each pointer
        // first element
        Task* _first;
        // dummy element
        Task* _divider;
        // last element
        Task* _last;
        // shared among consumers
        std::atomic<bool> _consumerLock;
        // shared among producers
        std::atomic<bool> _producerLock;
};
