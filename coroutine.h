//
// Created by xvvx on 18-12-2.
//

#ifndef ECOROUTINE_COROUTINE_H
#define ECOROUTINE_COROUTINE_H

#include <stdlib.h>

#include <functional>
#include <memory>


namespace ecoroutine {

    using CoroutineFunc = std::function<void()>;
    using coroutine_t = u_int32_t;

    // create a coroutine and return the id of this coroutine
    // param1: func, coroutine function
    coroutine_t create(CoroutineFunc &func);

    // like pthread_self(), get the id of the coroutine itself
    coroutine_t self();

    // run a coroutine whose state is kReady or kHangUp
    void run(coroutine_t c);

    // give up control of CPU
    void yield();

};


#endif //ECOROUTINE_COROUTINE_H
