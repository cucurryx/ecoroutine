//
// Created by xvvx on 18-12-2.
//

#ifndef ECOROUTINE_COROUTINE_H
#define ECOROUTINE_COROUTINE_H

#include <stdlib.h>

#include <functional>
#include <memory>


namespace ecoroutine {

constexpr u_int32_t kStackSize = 1024 * 1024;

using CoroutineFunc = std::function<void()>;
using coroutine_t = u_int32_t;

enum class CoState {
    kReady,
    kRunning,
    kHangUp,
    kDead
};

static void HandleError(const std::string &msg) {
    perror(msg.c_str());
    exit(EXIT_FAILURE);
}

/*-------------------------------------------------------------------------------------*/

//predeclerations of functions

coroutine_t create(CoroutineFunc &func);

coroutine_t self();

void start(coroutine_t c);

void yield();

};


#endif //ECOROUTINE_COROUTINE_H
