#include <functional>
#include <future>
#include <thread>
#include <atomic>

#include "gtest/gtest.h"
#include "thread_pool/stack.h"

using thread_pool::Stack;

class StackUt : public ::testing::Test{
    public:
        StackUt():
            _stack(nullptr) {

        }

        void SetUp() {
            // code here will execute just before the test ensues
            _stack = new Stack();
        }

        void TearDown() {
            // code here will be called just after the test completes
            // ok to through exceptions from here if need be
            if (_stack) {
                delete _stack;
            }
        }

        ~StackUt() {}

    protected:
        Stack* _stack;
};

// bad test
TEST_F(StackUt, setupTearDown)
{
    _stack = new Stack();

    EXPECT_EQ(1,1);
}

// produce and consume one Task using only one thread modify reference
TEST_F(StackUt, OneThreadProduceAndConsumeModifyReference)
{
    auto makeItTwo = [](int& n) {n = 2;};

    int n = 0;
    auto ret = _stack->Produce(makeItTwo, std::ref(n));
    EXPECT_EQ(_stack->TasksCount(), 1);
    _stack->Consume();

    EXPECT_EQ(n, 2);
}

// produce and consume one Task using only one thread
TEST_F(StackUt, OneThreadProduceAndConsume)
{
    auto sumOne = [](const int& n) {return n + 1;};

    auto retNum = _stack->Produce(sumOne, 1);
    EXPECT_EQ(_stack->TasksCount(), 1);
    _stack->Consume();

    EXPECT_EQ(retNum.get(), 2);
}

// one producer thread and one consumer thread with multiple Tasks
TEST_F(StackUt, OneProducerOneConsumerMultiTasks)
{
    std::atomic<bool> exit;
    exit.exchange(false);

    auto consume = [&exit](Stack* q) {while(!exit) {q->Consume();}};
    auto sumOne = [](const int& n) {return n + 1;};

    std::thread t1(consume, this->_stack);

    auto retTwo = _stack->Produce(sumOne, 1);
    auto retThree = _stack->Produce(sumOne, 2);
    auto retFour = _stack->Produce(sumOne, 3);
    auto retFive = _stack->Produce(sumOne, 4);

    EXPECT_EQ(retTwo.get(), 2);
    EXPECT_EQ(retThree.get(), 3);
    EXPECT_EQ(retFour.get(), 4);
    EXPECT_EQ(retFive.get(), 5);
    EXPECT_EQ(_stack->TasksCount(), 0);

    exit.exchange(true);
    t1.join();
}

// one producer and multi consumers with multiple Tasks
TEST_F(StackUt, OneProducerMultiConsumerMultiTasks)
{
    std::atomic<bool> exit;
    exit.exchange(false);

    auto consume = [&exit](Stack* q) {while(!exit) {q->Consume();}};
    auto sumOne = [](const int& n) {return n + 1;};

    std::thread t1(consume, this->_stack);
    std::thread t2(consume, this->_stack);
    std::thread t3(consume, this->_stack);

    auto retTwo = _stack->Produce(sumOne, 1);
    auto retThree = _stack->Produce(sumOne, 2);
    auto retFour = _stack->Produce(sumOne, 3);
    auto retFive = _stack->Produce(sumOne, 4);
    auto retSix = _stack->Produce(sumOne, 5);
    auto retSeven = _stack->Produce(sumOne, 6);

    EXPECT_EQ(retTwo.get(), 2);
    EXPECT_EQ(retThree.get(), 3);
    EXPECT_EQ(retFour.get(), 4);
    EXPECT_EQ(retFive.get(), 5);
    EXPECT_EQ(retSix.get(), 6);
    EXPECT_EQ(retSeven.get(), 7);
    EXPECT_EQ(_stack->TasksCount(), 0);

    exit.exchange(true);
    t1.join();
    t2.join();
    t3.join();
}

// one producer and multi consumers with multiple big Tasks
TEST_F(StackUt, OneProducerMultiConsumerMultiBigTasks)
{
    std::function<int(const int&)> fibonacci = [&fibonacci](const int& n) {
        if (n == 0) {return 0;}
        if (n == 1) {return 1;}
        return fibonacci(n-1) + fibonacci(n-2);
    };
    std::atomic<bool> exit;
    exit.exchange(false);

    auto consume = [&exit](Stack* q) {while(!exit) {q->Consume();}};

    std::thread t1(consume, this->_stack);
    std::thread t2(consume, this->_stack);
    std::thread t3(consume, this->_stack);
    std::thread t4(consume, this->_stack);

    auto fib37 = _stack->Produce(fibonacci, 37);
    auto fib36 = _stack->Produce(fibonacci, 36);
    auto fib35 = _stack->Produce(fibonacci, 35);
    auto fib34 = _stack->Produce(fibonacci, 34);
    auto fib33 = _stack->Produce(fibonacci, 33);
    auto fib32 = _stack->Produce(fibonacci, 32);
    auto fib25 = _stack->Produce(fibonacci, 25);
    auto fib22 = _stack->Produce(fibonacci, 22);
    auto fib12 = _stack->Produce(fibonacci, 12);

    EXPECT_EQ(fib12.get(), 144);
    EXPECT_EQ(fib22.get(), 17711);
    EXPECT_EQ(fib25.get(), 75025);
    EXPECT_EQ(fib32.get(), 2178309);
    EXPECT_EQ(fib33.get(), 3524578);
    EXPECT_EQ(fib34.get(), 5702887);
    EXPECT_EQ(fib35.get(), 9227465);
    EXPECT_EQ(fib36.get(), 14930352);
    EXPECT_EQ(fib37.get(), 24157817);

    exit.exchange(true);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
}

// return a vector of futures from the Tasks added to the Stack
auto getFutures(Stack* q, const int& numOfTasks) {

    auto sumOne = [](const int& n) {return n + 1;};
    std::vector<std::future<int>> futures;

    for (int i = 0; i < numOfTasks; i++) {
        futures.emplace_back(q->Produce(sumOne, i));
    }
    return futures;
}

// multi producers and multi consumers with multiple Tasks
TEST_F(StackUt, MultiProducersMultiConsumersMultiTasks)
{
    std::atomic<bool> exit;
    exit.exchange(false);
    const int numOfTasks = 1000;

    auto consume = [&exit](Stack* q) {while(!exit) {q->Consume();}};

    std::thread t1(consume, this->_stack);
    std::thread t2(consume, this->_stack);
    std::thread t3(consume, this->_stack);
    std::thread t4(consume, this->_stack);

    auto futures1 = std::async(getFutures, _stack, numOfTasks);
    auto futures2 = std::async(getFutures, _stack, numOfTasks);
    auto futures3 = std::async(getFutures, _stack, numOfTasks);

    auto futuresV1 = futures1.get();
    auto futuresV2 = futures2.get();
    auto futuresV3 = futures3.get();

    for (int i = 0; i < numOfTasks; i++) {
        EXPECT_EQ(futuresV1.at(i).get(), i + 1);
        EXPECT_EQ(futuresV2.at(i).get(), i + 1);
        EXPECT_EQ(futuresV3.at(i).get(), i + 1);
    }

    exit.exchange(true);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
}
