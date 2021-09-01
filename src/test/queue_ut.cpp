#include <thread>
#include <atomic>

#include "gtest/gtest.h"
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
    auto sumOne = [](int &n) {return n + 1;};

    auto retNum = _queue->Produce(sumOne, 1);
    _queue->Consume();

    EXPECT_EQ(retNum.get(), 2);
}

// one producer thread and one consumer thread with multiple Tasks
TEST_F(QueueUt, OneConsumerOneProducerMultiTasks)
{
    std::atomic<bool> exit;
    exit.exchange(false);

    auto consume = [&exit](Queue* q) {while(!exit) {q->Consume();}};
    auto sumOne = [](int &n) {return n + 1;};

    std::thread t1(consume, this->_queue);

    auto retTwo = _queue->Produce(sumOne, 1);
    auto retThree = _queue->Produce(sumOne, 2);
    auto retFour = _queue->Produce(sumOne, 3);
    auto retFive = _queue->Produce(sumOne, 4);

    EXPECT_EQ(retTwo.get(), 2);
    EXPECT_EQ(retThree.get(), 3);
    EXPECT_EQ(retFour.get(), 4);
    EXPECT_EQ(retFive.get(), 5);

    exit.exchange(true);
    t1.join();
}
