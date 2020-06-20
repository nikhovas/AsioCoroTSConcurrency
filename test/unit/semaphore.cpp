#include "gtest/gtest.h"
#include <boost/asio.hpp>
#include "asio_corots_concur.h"


using namespace std::chrono_literals;
namespace net = boost::asio;


TEST(Semaphore, NonBlocking) {
    net::io_context executor;
    net::synchronization::semaphore semaphore(2, executor);

    net::co_spawn(executor, [&]() -> net::awaitable<void> {
        co_await semaphore.acquire();
        semaphore.release();

        co_await semaphore.acquire();
        co_await semaphore.acquire();
        semaphore.release();
        semaphore.release();
    }, net::detached);

    executor.run();
}

TEST(Semaphore, Blocking) {
    net::io_context executor;
    net::synchronization::semaphore semaphore(0, executor);
    bool touched = false;

    net::co_spawn(executor, [&]() -> net::awaitable<void> {
        co_await semaphore.acquire();
        touched = true;
        co_return;
    }, net::detached);

    net::co_spawn(executor, [&]() -> net::awaitable<void> {
        EXPECT_FALSE(touched);
        semaphore.release();
        co_return;
    }, net::detached);

    executor.run();

    ASSERT_TRUE(touched);
}

TEST(Semaphore, PingPong) {
    net::io_context executor;
    net::synchronization::semaphore my(1, executor);
    net::synchronization::semaphore that(0, executor);

    int step = 0;

    net::co_spawn(executor, [&]() -> net::awaitable<void> {
        co_await that.acquire();
        EXPECT_EQ(step, 1);
        step = 0;
        my.release();
        co_return;
    }, net::detached);

    net::co_spawn(executor, [&]() -> net::awaitable<void> {
        co_await my.acquire();
        EXPECT_EQ(step, 0);
        step = 1;
        that.release();

        co_await my.acquire();
        EXPECT_TRUE(step == 0);
        co_return;
    }, net::detached);

    executor.run();
}

//
//        SIMPLE_T_TEST(PingPong) {
//            solutions::Semaphore my{1};
//            solutions::Semaphore that{0};
//
//            int step = 0;
//
//            auto opponent_routine = [&]() {
//                that.Acquire();
//                ASSERT_EQ(step, 1);
//                step = 0;
//                my.Release();
//            };
//
//            twist::strand::thread opponent(opponent_routine);
//
//            my.Acquire();
//            ASSERT_EQ(step, 0);
//            step = 1;
//            that.Release();
//
//            my.Acquire();
//            ASSERT_TRUE(step == 0);
//
//            opponent.join();
//        }
//}
