#pragma once


#include <boost/asio/strand.hpp>
#include "condition_variable.h"


namespace boost::asio::synchronization {
    class semaphore {
    public:
        explicit semaphore(size_t initial_value, io_context& executor) : counter_(initial_value), strand_(executor),
        cond_var_(strand_) {
        }

        awaitable<void> acquire() {
            co_await post(strand_, use_awaitable);
            ++waiting_num_;
            if (counter_ == 0) {
                co_await cond_var_.wait();
            }
            --waiting_num_;
            --counter_;
            co_await post(strand_.context(), use_awaitable);
        }

        void release() {
            ++counter_;
            if (waiting_num_) {
                cond_var_.notify_one();
            }
        }

//        bool try_acquire() noexcept {
//
//        }
//
//        template<class Rep, class Period>
//        bool try_acquire_for( const std::chrono::duration<Rep, Period>& rel_time );
//
//        template<class Clock, class Duration>
//        bool try_acquire_until( const std::chrono::time_point<Clock, Duration>& abs_time );

    private:
        size_t counter_;
        size_t waiting_num_{0};
        io_context::strand strand_;
        condition_variable cond_var_;
    };
}