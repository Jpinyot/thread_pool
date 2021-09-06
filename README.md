# A C++11/C++14 Low-lock Thread Pool with Priority Tasks

# Abstract
Presenting a C++11/C++14-compatible low-lock thread pool implementation, built from scrach with high-performance embedded systems in mind. The library is implemented as a main class ThreadPool and helper classes containing the data strucuters to store the Tasks, With the only dependencies beeing the C++11 or C++14 standard librarys.

# Introduction
Thread pool implementation using c++14/c++11 threads and a lock-free skip list.
## Working in c++14
This library should successfully compile on any C++14 standard-compilant compiler.
## Working in c++11
This library should successfully compile on any C++11 standard-compilant compiler. If we want to build the library using C++11 we will not be able to use auto to return type deduction by the compiler.Thus we will need to specify the return type of the method:
`template<class RetType, class F, class ... Args>
std::future<RetType> ThreadPool::Push(F&&, Args&& ...)`


# Usage
## Including the library
## Working with prioritys
## Setting number of threads
The constructor of the main class `ThreadPool` have to input arguments `PriorityCount` and `ThreadCount`
## Setting padding size
## Blocking vs reschedule the execution

## Submitting and waiting for tasks
A task can be any function, with or without arguments and/or return value. Once a task has been submitted to the queue, it will be executed as soon as a thread becomes avalible. Tasks are executed respecting the priority order(beeing 0 the first priority up to n beeing the last to be exectued) and int the order that they were submitted(FIFO) if they have the same priority.

C++14 example:
```cpp
// Submit a task without arguments to the queue, and get a future for it.
auto my_future = pool.push(task);
// Submit a task with arguments to the queue, and get a future for it.
auto my_future = pool.push(task, arg);
// Submit a task with arguments to the queue, and no return value.
pool.push(task, arg1, arg2);
```

C++11 example:
```cpp
// Submit a task without arguments to the queue, and get a future for it.
auto my_future = pool.push<int>(task);
// Submit a task with arguments to the queue, and get a future for it.
std::future<int> my_future = pool.push<int>(task, arg);
// Submit a task with arguments to the queue, and no return value.
pool.push<void>(task, arg1, arg2);
```
### Submitting tasks to the queue with method with reference
If a task have to modify a given argument a reference is spected as an input/output value, to be modify one of the argument we will need to specify using `std::ref'.

Example:
```cpp
// Submit a task with reference of a argument to the queue, and get a future for it.
auto my_future = pool.push(task, std::ref(arg));
```

# Testing the package

# References
https://codereview.stackexchange.com/questions/240937/easy-to-use-thread-pool-implementation
https://www.drdobbs.com/article/print?articleId=210604448&siteSectionName=parallel
https://www.drdobbs.com/article/print?articleId=211601363&siteSectionName=parallel
https://www.drdobbs.com/article/print?articleId=212201163&siteSectionName=parallel
https://www.osti.gov/pages/servlets/purl/1237474
