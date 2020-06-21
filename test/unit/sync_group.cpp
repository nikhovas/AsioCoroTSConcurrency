#include "gtest/gtest.h"
#include <boost/asio.hpp>
#include "asio_corots_concur.h"


using namespace std::chrono_literals;
namespace net = boost::asio;


TEST(SyncGroup, SyncDefault) {
    net::io_context executor;
    size_t signaled = 0;
    net::synchronization::sync_group sync_group(executor);

    auto worker = [&](net::synchronization::sync_group_signaler signaler) -> net::awaitable<void> {
        ++signaled;
        co_await signaler.signal();
        co_return;
    };

    net::co_spawn(executor, [&]() -> net::awaitable<void> {
        for (size_t i = 0; i < 100; ++i) {
            net::co_spawn(executor, std::bind(worker, sync_group.signaler()), net::detached);
        }

        co_await sync_group.wait();
        EXPECT_EQ(signaled, 100);

    }, net::detached);

    executor.run();
}


TEST(SyncGroup, SyncDefaultMonoThread) {
    net::io_context executor{1};
    size_t signaled = 0;
    net::synchronization::sync_group sync_group(executor);

    auto worker = [&](net::synchronization::sync_group_signaler signaler) -> net::awaitable<void> {
        ++signaled;
        co_await signaler.signal();
        co_return;
    };

    net::co_spawn(executor, [&]() -> net::awaitable<void> {
        for (size_t i = 0; i < 100; ++i) {
            net::co_spawn(executor, std::bind(worker, sync_group.signaler()), net::detached);
        }

        EXPECT_EQ(signaled, 0);
        co_await sync_group.wait();
        EXPECT_EQ(signaled, 100);

    }, net::detached);

    executor.run();
}


//TEST(SyncGroup, SyncFrame) {
//    net::io_context executor;
//    size_t signaled = 0;
//    net::synchronization::sync_group sync_group(executor);
//
//    auto worker = [&](net::synchronization::sync_group_frame_signaler) -> net::awaitable<void> {
//        ++signaled;
//        co_return;
//    };
//
//    net::co_spawn(executor, [&]() -> net::awaitable<void> {
//        for (size_t i = 0; i < 100; ++i) {
//            net::co_spawn(executor, std::bind(worker, sync_group.signaler()), net::detached);
//        }
//
//        co_await sync_group.wait();
//        EXPECT_EQ(signaled, 100);
//
//    }, net::detached);
//
//    executor.run();
//}