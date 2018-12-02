//
// Created by xvvx on 18-12-2.
//

#include "coroutine.h"

#include <iostream>
#include <list>
#include <string>
#include <sstream>


namespace ecoroutine {

static void schedule();

struct Coroutine {

    Coroutine(coroutine_t id, CoFunction &func) {
        func_ = func;

        getcontext(&context_);

        stack_ = (char*)malloc(kStackSize);
        context_.uc_stack.ss_sp = stack_;
        context_.uc_stack.ss_size = kStackSize;
        context_.uc_link = NULL;

        makecontext(&context_, reinterpret_cast<void(*)()>(RunCoroutineFunc), 2,
                    reinterpret_cast<size_t>(this) >> 32, this);

        state_ = CoState::kReady;
    }

    ~Coroutine() {
        delete stack_;
    }

    CoFunction func_;
    CoState state_;
    coroutine_t id_;
    ucontext_t context_;
    char *stack_;

private:
    static void RunCoroutineFunc(uint32_t high, uint32_t low) {
        auto *c= reinterpret_cast<Coroutine*>((static_cast<size_t>(high) << 32) + low);
        c->func_();
        c->state_ = CoState::kDead;
        schedule();
    }

};

class Scheduler {
public:

    using CoroutinePtr = std::shared_ptr<Coroutine>;

public:

    Scheduler() { }

    coroutine_t Create(CoFunction &func) {
        auto p = std::make_shared<Coroutine>(next_id_, func);
        ++next_id_;

        all_coroutines_.push_back(p);
        ready_coroutines_.push_back(p);

        Schedule();

        return p->id_;
    }

    coroutine_t RunningId() {
        return running_id_;
    }

    void Yield();


    void Schedule();

private:

    CoroutinePtr GetCurrentCoroutine() {
        CoroutinePtr curr = nullptr;
        for (auto x : all_coroutines_) {
            if (x->id_ == running_id_) {
                curr = x;
                break;
            }
        }
        return curr;
    }

private:

    std::list<CoroutinePtr> all_coroutines_;
    std::list<CoroutinePtr> ready_coroutines_;

    ucontext_t main_context_;
    coroutine_t next_id_ { 1 };
    coroutine_t running_id_ { 0 };
};

void Scheduler::Schedule() {
    CoroutinePtr curr = GetCurrentCoroutine();

    if (ready_coroutines_.empty()) {
        swapcontext(&curr->context_, &main_context_);
    }

    auto next = ready_coroutines_.front();
    ready_coroutines_.pop_front();

    if (curr->state_ == CoState::kHangUp) {
        ready_coroutines_.push_front(curr);
    }

    if (curr->state_ == CoState::kDead) {
        all_coroutines_.remove(curr);
    }

    if (next->state_ == CoState::kReady) {
        next->state_ = CoState::kRunning;
        next->context_.uc_link = &main_context_;

        running_id_ = next->id_;
        swapcontext(&main_context_, &next->context_);

    } else if (next->state_ == CoState::kHangUp) {

        next->state_ = CoState::kRunning;
        next->context_.uc_link = &curr->context_;

        running_id_ = next->id_;
        swapcontext(&curr->context_, &next->context_);

    }
}

void Scheduler::Yield() {
    CoroutinePtr curr = GetCurrentCoroutine();
    curr->state_ = CoState::kHangUp;
    Schedule();
}

/*-------------------------------------------------------------------------------------*/

static Scheduler scheduler;

coroutine_t create(CoFunction &func) {
    return scheduler.Create(func);
}

coroutine_t self() {
    return scheduler.RunningId();
}

void yield() {
    scheduler.Yield();
}

static void schedule() {
    scheduler.Schedule();
}


};
