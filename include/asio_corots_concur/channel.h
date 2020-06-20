//
// Created by Николай Васильев on 20.06.2020.
//

#ifndef COROUTINESTSADDONS_CHANNEL_H
#define COROUTINESTSADDONS_CHANNEL_H

#include <boost/asio/strand.hpp>
#include <queue>
#include "condition_variable.h"


namespace boost::asio::synchronization {

    template<typename T>
    class channel {
    public:
        channel(io_context& executor) : strand_(executor) {}

        awaitable<void> close() {
            co_await post(strand_, use_awaitable);
            closed_ = true;
            cond_var_.notify_all();
            co_await post(strand_.context(), use_awaitable);
        }

        awaitable<void> closed() {
            co_await post(strand_, use_awaitable);
            bool closed = closed_;
            co_await post(strand_.context(), use_awaitable);
            co_return closed;
        }

        awaitable<bool> put(const T &i) {
            co_await post(strand_, use_awaitable);
            if (closed_) {
                co_await post(strand_.context(), use_awaitable);
                co_return false;
            }

            queue_.push(i);
            ++put_;
            co_await cond_var_.notify_one();
            co_await post(strand_.context(), use_awaitable);
            co_return true;
        }

        awaitable<std::optional<T>> get() {
            co_await post(strand_, use_awaitable);

            if (queue_.empty() && !closed_) {
                co_await  cond_var_.wait();
            }

            if (queue_.empty()) {
                co_await post(strand_.context(), use_awaitable);
                co_return std::nullopt;
            }

            T result = queue_.front();
            queue_.pop();
            ++got_;

            co_await post(strand_.context(), use_awaitable);
            co_return result;
        }

        size_t size() const {
            return put_ - got_;
        }

    private:
        std::queue<T> queue_;
        io_context::strand strand_;
        condition_variable cond_var_;
        bool closed_{false};
        size_t put_{0};
        size_t got_{0};
    };

}

#endif //COROUTINESTSADDONS_CHANNEL_H
