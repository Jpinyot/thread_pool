#include "gtest/gtest.h"
#include "thread_pool/queue.h"

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
        }

        ~QueueUt() {}

    protected:
        Queue* _queue;
};

TEST_F(QueueUt, setupTearDown)
{
    _queue = new Queue();

    delete _queue;
    EXPECT_EQ(1,1);
}
