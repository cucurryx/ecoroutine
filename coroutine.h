//
// Created by xvvx on 18-12-2.
//

#ifndef ECOROUTINE_COROUTINE_H
#define ECOROUTINE_COROUTINE_H

#include <ucontext.h>
#include <stdlib.h>

#include <functional>
#include <list>
#include <string>
#include <sstream>
#include <memory>

//namespace ecoroutine {

class Coroutine;

constexpr u_int32_t kStackSize = 1024 * 1024;

using CoFunction = std::function<void()>;
using CoPtr = std::shared_ptr<Coroutine>;
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

coroutine_t create(CoFunction &func);
void yield();
void schedule(coroutine_t co);

//};


#endif //ECOROUTINE_COROUTINE_H
