#ifndef COROUTINESTSADDONS_LATCH_H
#define COROUTINESTSADDONS_LATCH_H


#include <boost/asio/strand.hpp>
#include "condition_variable.h"


namespace boost::asio::synchronization {

    class latch {
    public:
        explicit latch(const size_t &count, io_context& executor) : thread_count_(count), strand_(executor),
        cond_var_(strand_) { }

        awaitable<void> arrive_and_wait() {
            co_await post(strand_, use_awaitable);

            if (thread_count_ >= 1) {
                --thread_count_;

                if (thread_count_ == 0) {
                    cond_var_.notify_all();
                } else {
                    cond_var_.wait();
                }
            }

            co_await post(strand_.context(), use_awaitable);
        }

        void arrive() {
            if (thread_count_ >= 1) {
                --thread_count_;

                if (thread_count_ == 0) {
                    cond_var_.notify_all();
                }
            }
        }

        awaitable<void> wait() {
            co_await post(strand_, use_awaitable);
            cond_var_.wait();
            co_await post(strand_.context(), use_awaitable);
        }

    private:
        size_t thread_count_;
        io_context::strand strand_;
        condition_variable cond_var_;
    };

}

#endif //COROUTINESTSADDONS_LATCH_H
