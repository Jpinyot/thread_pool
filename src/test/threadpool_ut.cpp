#include <future>
#include <thread>
#include <atomic>

#include "gtest/gtest.h"
#include <vector>
#include "thread_pool/threadpool.h"

using thread_pool::ThreadPool;

constexpr uint32_t kPriorityCount = 3;
constexpr uint32_t kThreadCount = 4;

class ThreadPoolUt : public ::testing::Test{
    public:
        ThreadPoolUt():
            _threadPool(nullptr) {

        }

        void SetUp() {
            // code here will execute just before the test ensues
            _threadPool = new ThreadPool(kPriorityCount, kThreadCount);
        }

        void TearDown() {
            // code here will be called just after the test completes
            // ok to through exceptions from here if need be
            if (_threadPool) {
                delete _threadPool;
            }
        }

        ~ThreadPoolUt() {}

    protected:
        ThreadPool* _threadPool;
};

// one Task pushed
TEST_F(ThreadPoolUt, OneTaskPushed)
{
    auto sumOne = [](const int& n) {return n + 1;};

    auto retNum = _threadPool->Push(2, sumOne, 1);

    EXPECT_EQ(retNum.get(), 2);
}

// multiple Task pushed
TEST_F(ThreadPoolUt, MultiTasksPushed)
{
    int itrerationCount = 100;
    auto sumOne = [](const int& n) {return n + 1;};

    std::vector<std::future<int>> retVal;

    for (int i = 0; i < itrerationCount; i++) {
        retVal.push_back(_threadPool->Push(0, sumOne, i));
    }

    for (int i = 0; i < itrerationCount; i++) {
        EXPECT_EQ(retVal.at(i).get(), i + 1);
    }
}
