#include "gtest/gtest.h"
#include <thread>
#include "thread_pool/queue.h"

using thread_pool::Queue;

class QueueUt : public ::testing::Test{
    public:
        QueueUt():
            _queue(nullptr) {

        }

        void SetUp() {
            // code here will execute just before the test ensues
            _queue = new Queue();
        }

        void TearDown() {
            // code here will be called just after the test completes
            // ok to through exceptions from here if need be
            if (_queue) {
                delete _queue;
            }
        }

        ~QueueUt() {}

    protected:
        Queue* _queue;
};

// bad test
TEST_F(QueueUt, setupTearDown)
{
    _queue = new Queue();

    EXPECT_EQ(1,1);
}

// produce and consume one Task using only one thread
TEST_F(QueueUt, OneThreadProduceAndConsume)
{

    int num = 1;

    auto sumOne = [](int &n) {return n + 1;};

    auto retNum = _queue->Produce(sumOne, num);
    _queue->Consume();

    EXPECT_EQ(retNum.get(), 2);
}
