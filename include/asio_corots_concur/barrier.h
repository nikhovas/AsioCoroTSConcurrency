#pragma once


#include <boost/asio/strand.hpp>
#include "condition_variable.h"


namespace boost::asio::synchronization {

    class barrier {
    public:
        explicit barrier(const size_t &count, io_context& executor) : thread_count_(count), strand_(executor),
        cond_var_(strand_) { }

        awaitable<void> arrive_and_wait() {
            co_await post(strand_, use_awaitable);

            size_t loop = current_loop_;

            ++waiting_now_;
            if (waiting_now_ == thread_count_) {
                waiting_now_ = 0;
                ++current_loop_;
                cond_var_.notify_all();
            } else {
                cond_var_.wait();
            }

            co_await post(strand_.context(), use_awaitable);
        }

        void arrive() {
            ++waiting_now_;
            if (waiting_now_ == thread_count_) {
                waiting_now_ = 0;
                ++current_loop_;
                cond_var_.notify_all();
            }
        }

        awaitable<void> wait() {
            co_await post(strand_, use_awaitable);
            cond_var_.wait();
            co_await post(strand_.context(), use_awaitable);
        }

//        void arrive_and_drop();

    private:
        size_t thread_count_;
        size_t waiting_now_{0};
        size_t current_loop_{0};

        io_context::strand strand_;
        condition_variable cond_var_;
    };

}
