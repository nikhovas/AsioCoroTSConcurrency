//#ifndef CORO_IMPL_CONDITION_VARIABLE
//#define CORO_IMPL_CONDITION_VARIABLE
#pragma once


namespace boost::asio::synchronization {

    condition_variable::condition_variable(io_context& executor)
    : timer_(executor) { }

    condition_variable::condition_variable(io_context::strand& executor)
    : timer_(executor) { }

    void condition_variable::notify_one() noexcept {
        timer_.cancel_one();
    }

    void condition_variable::notify_all() noexcept {
        timer_.cancel();
    }

    awaitable<void> condition_variable::wait() {
        co_await wait_until(std::chrono::time_point<std::chrono::steady_clock>::max());
        co_return;
    }

    template< class Clock, class Duration >
    awaitable<cv_status> condition_variable::wait_until(const std::chrono::time_point<Clock, Duration>& timeout_time) {
        timer_.expires_at(timeout_time);

        try {
            co_await timer_.async_wait(use_awaitable);
        } catch (...) {
            co_return cv_status::no_timeout;
        }

        co_return cv_status::timeout;
    }
}
//
//
//#endif