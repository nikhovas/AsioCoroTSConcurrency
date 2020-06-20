#include "gtest/gtest.h"
#include <boost/asio.hpp>
#include "asio_corots_concur.h"


using namespace std::chrono_literals;
namespace net = boost::asio;


TEST(ConditionVariable, NotifyOne) {
    net::io_context executor;
    bool passed = false;

    net::co_spawn(executor, [&]() -> net::awaitable<void> {
        net::synchronization::condition_variable cond_var(executor);

        auto wait_routine = [&]() -> net::awaitable<void> {
            co_await cond_var.wait();
            passed = true;
            co_return;
        };

        net::co_spawn(executor, wait_routine, net::detached);

        net::deadline_timer t(executor, boost::posix_time::seconds(1));
        co_await t.async_wait(net::use_awaitable);
        cond_var.notify_one();

    }, net::detached);

    executor.run();

    ASSERT_EQ(passed, true);
}

TEST(ConditionVariable, BlockingWait) {
    net::io_context executor;
    int state = 0;
    net::synchronization::condition_variable cond_var(executor);

    cond_var.notify_one();
    cond_var.notify_all();

    net::co_spawn(executor, [&]() -> net::awaitable<void> {
        EXPECT_EQ(state, 0);
        co_await cond_var.wait();
        EXPECT_EQ(state, 1);
        co_return;
    }, net::detached);

    net::co_spawn(executor, [&]() -> net::awaitable<void> {
        EXPECT_EQ(state, 0);
        state = 1;
        cond_var.notify_one();
        co_return;
    }, net::detached);

    executor.run();
}

TEST(ConditionVariable, NotifyAll) {
    net::io_context executor;
    bool pass = false;
    int got = 0;
    net::synchronization::condition_variable cond_var(executor);

    auto wait_routine = [&]() -> net::awaitable<void> {
        co_await cond_var.wait();
        ++got;
        co_return;
    };

    net::co_spawn(executor, wait_routine, net::detached);
    net::co_spawn(executor, wait_routine, net::detached);

    net::co_spawn(executor, [&]() -> net::awaitable<void> {
        pass = true;
        cond_var.notify_all();
        co_return;
    }, net::detached);

    executor.run();

    ASSERT_EQ(got, 2);
}