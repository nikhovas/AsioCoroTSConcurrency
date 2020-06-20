//
// Created by Николай Васильев on 20.06.2020.
//

#ifndef COROUTINESTSADDONS_SYNC_GROUP_H
#define COROUTINESTSADDONS_SYNC_GROUP_H


#include <boost/asio/strand.hpp>
#include "condition_variable.h"


namespace boost::asio::synchronization {

    struct sync_group_core {
        io_context::strand strand;
        condition_variable cond_var;
        size_t added_elements{0};
        size_t ready_elements{0};

        explicit sync_group_core(io_context &executor) : strand(executor), cond_var(strand) {}
    };


    class sync_group_signaler {
    public:
        awaitable<void> signal() {
            co_await post(core_.strand, use_awaitable);
            if (core_.added_elements == ++core_.ready_elements) {
                core_.cond_var.notify_all();
            }
            co_await post(core_.strand.context(), use_awaitable);

            co_return;
        }

    private:
        friend class sync_group;
        explicit sync_group_signaler(sync_group_core &core) : core_(core) {}

        sync_group_core &core_;
    };


//    class sync_group_frame_signaler {
//    public:
//        sync_group_frame_signaler(sync_group_signaler signaler) : signaler_(std::move(signaler)) {}
//        ~sync_group_frame_signaler() {
//            co_await
//        }
//    private:
//        sync_group_signaler signaler_;
//    };


    class sync_group {
    public:
        explicit sync_group(io_context &executor) : core_(executor) { }

        awaitable<void> wait() {
            co_await post(core_.strand, use_awaitable);
            if (core_.added_elements != core_.ready_elements) {
                co_await core_.cond_var.wait();
            }
            co_await post(core_.strand.context(), use_awaitable);

            co_return;
        }

        sync_group_signaler signaler() {
            ++core_.added_elements;
            return sync_group_signaler(core_);
        }

    private:
        sync_group_core core_;
    };


}

#endif //COROUTINESTSADDONS_SYNC_GROUP_H
