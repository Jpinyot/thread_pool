#include <future>
#include <thread>
#include <atomic>

#include "gtest/gtest.h"
#include "thread_pool/mdlist.h"

using thread_pool::MDList;

class MDListUt : public ::testing::Test{
    public:
        MDListUt():
            list(nullptr) {

        }

        void SetUp() {
            // code here will execute just before the test ensues
            list = new MDList(3);
        }

        void TearDown() {
            // code here will be called just after the test completes
            // ok to through exceptions from here if need be
            if (list) {
                delete list;
            }
        }

        ~MDListUt() {}

    protected:
        MDList* list;
};

// bad test
TEST_F(MDListUt, setupTearDown)
{
    list = new MDList(1);

    EXPECT_EQ(1,1);
}

// produce and consume one Task using only one thread
TEST_F(MDListUt, OneThreadProduceAndConsume)
{
    auto sumOne = [](const int& n) {return n + 1;};

    auto retNum = list->Produce(0, sumOne, 1);
    /* EXPECT_EQ(list->TasksCount(), 1); */
    list->Consume();

    EXPECT_EQ(retNum.get(), 2);
}
