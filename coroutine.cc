//
// Created by xvvx on 18-12-2.
//

#include "coroutine.h"

#include <ucontext.h>

#include <iostream>
#include <list>
#include <sstream>


namespace ecoroutine {

static void schedule();

struct Coroutine {

    Coroutine(coroutine_t id, CoroutineFunc &func) {
        id_ = id;
        func_ = func;

        getcontext(&context_);
        stack_ = (char*)malloc(kStackSize);
        context_.uc_stack.ss_sp = stack_;
        context_.uc_stack.ss_size = kStackSize;
        context_.uc_link = NULL;
        state_ = CoState::kReady;

        makecontext(&context_, reinterpret_cast<void(*)()>(RunCoroutineFunc), 2,
                    reinterpret_cast<size_t>(this) >> 32, this);
    }

    ~Coroutine() {
        delete stack_;
    }

    CoroutineFunc func_;
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

    Scheduler() = default;

    coroutine_t Create(CoroutineFunc &func);

    coroutine_t RunningId() {
        return running_id_;
    }

    void Start(coroutine_t c);

    void Yield();

    void DoSchedule();

private:

    CoroutinePtr GetCurrentCoroutine() {
        CoroutinePtr curr = nullptr;
        if (id_map_.count(running_id_)) {
            curr = id_map_[running_id_];
        }
        return curr;
    }

private:

    std::unordered_map<coroutine_t, CoroutinePtr> id_map_;
    std::list<CoroutinePtr> all_coroutines_;
    std::list<CoroutinePtr> ready_coroutines_;

    ucontext_t main_context_;
    coroutine_t next_id_ { 1 };
    coroutine_t running_id_ { 0 };

};


coroutine_t Scheduler::Create(CoroutineFunc &func) {
    auto p = std::make_shared<Coroutine>(next_id_, func);
    ++next_id_;

    ready_coroutines_.push_back(p);
    all_coroutines_.push_back(p);
    id_map_[p->id_] = p;

    return p->id_;
}

void Scheduler::DoSchedule() {
    CoroutinePtr curr = GetCurrentCoroutine();

    //current is main coroutine
    if (curr == nullptr) {
        if (!ready_coroutines_.empty()) {
            CoroutinePtr next = ready_coroutines_.front();
            ready_coroutines_.pop_front();
            next->state_ = CoState::kRunning;
            running_id_ = next->id_;
            swapcontext(&main_context_, &next->context_);
        }
    } else {
        //not main coroutines
        if (curr->state_ == CoState::kDead) {
            all_coroutines_.remove(curr);
        }
        if (curr->state_ == CoState::kHangUp) {
            ready_coroutines_.push_back(curr);
        }

        if (ready_coroutines_.empty()) {
            running_id_ = 0;
            swapcontext(&curr->context_, &main_context_);
        } else {
            CoroutinePtr next = ready_coroutines_.front();
            ready_coroutines_.pop_front();

            if (next->state_ == CoState::kReady || next->state_ == CoState::kHangUp) {
                next->state_ = CoState::kRunning;
                next->context_.uc_link = &main_context_;
                running_id_ = next->id_;
                swapcontext(&curr->context_, &next->context_);
            }
        }
    }
}

void Scheduler::Yield() {
    CoroutinePtr curr = GetCurrentCoroutine();

    if (curr == nullptr) {
        HandleError("can't yield main coroutine!\n");
    }

    curr->state_ = CoState::kHangUp;
    DoSchedule();
}

void Scheduler::Start(coroutine_t c) {
    CoroutinePtr p = nullptr;

    if (id_map_.count(c)) {
        p = id_map_[c];
    } else {
        HandleError("no such coroutine_t");
    }

    p->state_ = CoState::kReady;
    DoSchedule();
}

/*-------------------------------------------------------------------------------------*/

static Scheduler scheduler;

coroutine_t create(CoroutineFunc &func) {
    return scheduler.Create(func);
}

coroutine_t self() {
    return scheduler.RunningId();
}

void yield() {
    scheduler.Yield();
}

void start(coroutine_t c) {
    scheduler.Start(c);
}

static void schedule() {
    scheduler.DoSchedule();
}

};
