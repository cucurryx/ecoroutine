//
// Created by xvvx on 18-12-2.
//

#include "coroutine.h"

#include <ucontext.h>

#include <iostream>
#include <list>
#include <sstream>


namespace ecoroutine {

    namespace impl {

        // states of coroutine
        // initialized state is kReady, and the current running coroutine's state is kRunning.
        // a coroutine's state will change to kHangUp if calling ecoroutine::yield().
        // in the end, it will be dead.
        enum class CoroutineState {
            kReady,
            kRunning,
            kHangUp,
            kDead
        };

        // tool function for handling error
        static inline void HandleError(const std::string &msg) {
            perror(msg.c_str());
            exit(EXIT_FAILURE);
        }

        // pre-decleration
        class Scheduler;
        class Coroutine;


        static void schedule();

        //default size of coroutine stack
        constexpr u_int32_t kStackSize = 1024 * 1024;

        class Coroutine {
        public:
            Coroutine(coroutine_t id, CoroutineFunc &func);

            ~Coroutine() {
                delete stack_;
            }

            static void RunCoroutineFunc(uint32_t high, uint32_t low) {
                auto *c= reinterpret_cast<Coroutine*>((static_cast<size_t>(high) << 32) + low);
                c->func_();
                c->state_ = CoroutineState::kDead;
                schedule();
            }

            char *stack_;
            CoroutineFunc func_;
            coroutine_t id_;
            ucontext_t context_;
            CoroutineState state_;
        };

        Coroutine::Coroutine(coroutine_t id, CoroutineFunc &func):
                id_(id),
                func_(func),
                state_(CoroutineState::kReady) {
            stack_ = (char*)malloc(kStackSize);
            getcontext(&context_);
            context_.uc_stack.ss_sp = stack_;
            context_.uc_stack.ss_size = kStackSize;
            context_.uc_link = nullptr;
            makecontext(&context_, reinterpret_cast<void(*)()>(RunCoroutineFunc), 2,
                        reinterpret_cast<size_t>(this) >> 32, this);
        }


        class Scheduler {
        public:
            using CoroutinePtr = std::shared_ptr<Coroutine>;
            using CoroutinePtrList = std::list<CoroutinePtr>;
            using CoroutinePtrHashMap = std::unordered_map<coroutine_t, CoroutinePtr>;

        public:
            Scheduler() = default;

            ~Scheduler() = default;

            coroutine_t Create(CoroutineFunc &func);

            void Run(coroutine_t c);

            void Yield();

            void DoSchedule();

            coroutine_t RunningId() {
                return running_id_;
            }

        private:
            CoroutinePtr GetCurrentCoroutine();

        private:
            CoroutinePtrHashMap id_map_;
            CoroutinePtrList all_coroutines_;
            CoroutinePtrList runnable_coroutines_;

            ucontext_t main_context_;
            coroutine_t next_id_ { 1 };
            coroutine_t running_id_ { 0 };
        };


        coroutine_t Scheduler::Create(CoroutineFunc &func) {
            auto new_coroutine_ptr = std::make_shared<Coroutine>(next_id_, func);
            runnable_coroutines_.push_back(new_coroutine_ptr);
            all_coroutines_.push_back(new_coroutine_ptr);
            id_map_[new_coroutine_ptr->id_] = new_coroutine_ptr;
            ++next_id_;
            return new_coroutine_ptr->id_;
        }

        void Scheduler::DoSchedule() {
            //current is main coroutine
            if (running_id_ == 0) {
                if (!runnable_coroutines_.empty()) {
                    CoroutinePtr next = runnable_coroutines_.front();
                    runnable_coroutines_.pop_front();
                    next->state_ = CoroutineState::kRunning;
                    running_id_ = next->id_;
                    swapcontext(&main_context_, &next->context_);

                    //if come back from dead coroutine
                    if (next->state_ == CoroutineState::kDead) {
                        id_map_.erase(next->id_);
                        runnable_coroutines_.remove(next);
                        all_coroutines_.remove(next);
                    }
                }
            } else {
                CoroutinePtr curr = GetCurrentCoroutine();
                ucontext_t *curr_context = &curr->context_;
                if(curr->state_==CoroutineState::kDead){
                    curr.reset();
                }
                //switch to main coroutine
                running_id_ = 0;
                swapcontext(curr_context, &main_context_);
            }
        }

        void Scheduler::Yield() {
            CoroutinePtr curr = GetCurrentCoroutine();

            if (curr == nullptr) {
                HandleError("can't yield main coroutine!\n");
            }

            curr->state_ = CoroutineState::kHangUp;
            runnable_coroutines_.push_back(curr);
            running_id_ = 0;

            swapcontext(&curr->context_, &main_context_);
        }

        void Scheduler::Run(coroutine_t c) {
            CoroutinePtr p = nullptr;

            if (id_map_.count(c)) {
                p = id_map_[c];
            } else {
                HandleError("no such coroutine_t");
            }
            p->state_ = CoroutineState::kReady;
            DoSchedule();
        }

        Scheduler::CoroutinePtr Scheduler::GetCurrentCoroutine() {
            CoroutinePtr curr = nullptr;
            if (id_map_.count(running_id_)) {
                curr = id_map_[running_id_];
            }
            return curr;
        }


        static Scheduler scheduler;

        static void schedule() {
            scheduler.DoSchedule();
        }
    }; // namespace impl


//function for users
    
coroutine_t create(CoroutineFunc &func) {
    return impl::scheduler.Create(func);
}

coroutine_t self() {
    return impl::scheduler.RunningId();
}

void yield() {
    impl::scheduler.Yield();
}

void run(coroutine_t c) {
    impl::scheduler.Run(c);
}

};
