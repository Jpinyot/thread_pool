#include <atomic>
#include <future>
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
        void Produce(const F&& function, const Args&& ... args) {
            Task* tmp = new Task(function, args);
        }

        std::future<F> Consume() {
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
                F _function;
                std::tuple<Args ...> _args;
                std::atomic<F> _next;
                // padding is to ensure that two Tasks objects won't occupy the same chache line
                /* char pad[kCacheLineSize - sizeof(F*) - sizeof(std::tuple<Args...>) - */
                /*     sizeof(std::atomic<Task*>)]; */
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
}
