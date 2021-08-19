#include <atomic>
#include <tuple>

//TODO(jpinyot): change to correct cache line size
constexpr uint32_t kCacheLineSize = 128;

template<typename T, typename ... Args>
class Queue {
    Queue():
        _first(nullptr),
        _divider(nullptr),
        _last(nullptr) {
        // init list with dummy element
        _first = _divider = _last = new Task(T());
    }

    private:
        // class Task is a node of the Queue holding the task to be executed
        class Task {
            public:
                // TODO(jpinyot): check if makes sense to change pointer to references
                Task(T* value, Args... args):
                    _value(value),
                    _args(std::forward<Args>(args)...),
                    _next(nullptr) {
                    }

            private:
                T* _value;
                std::tuple<Args...> _args;
                std::atomic<T*> _next;
                // padding is to ensure that two Tasks objects won't occupy the same chache line
                char pad[kCacheLineSize - sizeof(T*) - sizeof(std::tuple<Args...>) -
                    sizeof(std::atomic<Task*>)];
        };

        Task* _first;
        std::atomic<Task*> _divider;
        std::atomic<Task*> _last;
}
