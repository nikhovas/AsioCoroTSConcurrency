#include "gtest/gtest.h"
#include <boost/asio.hpp>
#include "asio_corots_concur.h"


namespace net = boost::asio;


TEST(Channel, Default) {
    net::io_context executor{1};
    size_t signaled = 0;
    net::synchronization::channel<int> channel(executor);
    size_t i = 0;

    net::co_spawn(executor, [&]() -> net::awaitable<void> {
        while (auto j = co_await channel.get()) {
            EXPECT_EQ(j.value(), i);
            ++i;
        }
        co_return;
    }, net::detached);

    net::co_spawn(executor, [&]() -> net::awaitable<void> {
        for (size_t j = 0; j < 100; ++j) {
            bool res = co_await channel.put(j);
            EXPECT_EQ(res, true);

        }
        co_await channel.close();
        co_return;
    }, net::detached);

    executor.run();

    ASSERT_EQ(i, 100);
}
