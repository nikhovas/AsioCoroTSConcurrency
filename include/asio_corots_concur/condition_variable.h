#pragma once


#include <chrono>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/steady_timer.hpp>


namespace boost::asio::synchronization {
    enum class cv_status {
        no_timeout,
        timeout
    };

    class condition_variable {
    public:
        inline condition_variable(io_context& executor);
        inline condition_variable(io_context::strand& executor);
        inline condition_variable(const condition_variable&) = delete;
        inline ~condition_variable() = default;

        inline void notify_one() noexcept;
        inline void notify_all() noexcept;
        inline awaitable<void> wait();

        template< class Clock, class Duration >
        inline awaitable<cv_status> wait_until(const std::chrono::time_point<Clock, Duration>& timeout_time);

    private:
        steady_timer timer_;
    };
}


#include "impl/condition_variable.ipp"
