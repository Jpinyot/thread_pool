#include <future>
#include <thread>
#include <atomic>

#include "gtest/gtest.h"
#include "thread_pool/mdlist.h"

using thread_pool::MDList;

constexpr uint32_t kPriorityCount = 3;

class MDListUt : public ::testing::Test{
    public:
        MDListUt():
            _list(nullptr) {

        }

        void SetUp() {
            // code here will execute just before the test ensues
            _list = new MDList(kPriorityCount);
        }

        void TearDown() {
            // code here will be called just after the test completes
            // ok to through exceptions from here if need be
            if (_list) {
                delete _list;
            }
        }

        ~MDListUt() {}

    protected:
        MDList* _list;
};

// produce and consume one Task using only one thread
TEST_F(MDListUt, OneThreadProduceAndConsume)
{
    auto sumOne = [](const int& n) {return n + 1;};

    auto retNum = _list->Produce(2, sumOne, 1);
    _list->Consume();

    EXPECT_EQ(retNum.get(), 2);
}

// return a vector of futures from the Tasks added to the Queue
auto getFutures(MDList* list, const int& numOfTasks) {

    auto sumOne = [](const int& n) {return n + 1;};
    // first value is espacted value and second value is the return
    std::vector<std::pair<uint32_t, std::future<int>>> futures;

    for (int i = 0; i < numOfTasks; i++) {
        for (int j = 0; j < kPriorityCount; j++) {
            futures.emplace_back(std::pair<uint32_t, std::future<int>>
                    (i + 1, list->Produce(j, sumOne, i)));
        }
    }
    return futures;
}

// multiple producers and multiple consumers with multiple Tasks
TEST_F(MDListUt, MultiProducersMultiConsumersMultiTasks)
{
    std::atomic<bool> exit;
    exit.exchange(false);
    const int numOfTasks = 1000;

    auto consume = [&exit](MDList* list) {while(!exit) {list->Consume();}};

    std::thread t1(consume, this->_list);
    std::thread t2(consume, this->_list);
    std::thread t3(consume, this->_list);
    std::thread t4(consume, this->_list);

    auto futures1 = std::async(getFutures, _list, numOfTasks);
    auto futures2 = std::async(getFutures, _list, numOfTasks);
    auto futures3 = std::async(getFutures, _list, numOfTasks);

    auto futuresV1 = futures1.get();
    auto futuresV2 = futures2.get();
    auto futuresV3 = futures3.get();

    for (int i = 0; i < numOfTasks; i++) {
        EXPECT_EQ(futuresV1.at(i).second.get(), futuresV1.at(i).first);
        EXPECT_EQ(futuresV2.at(i).second.get(), futuresV2.at(i).first);
        EXPECT_EQ(futuresV3.at(i).second.get(), futuresV3.at(i).first);
    }

    exit.exchange(true);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
}
