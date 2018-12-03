//
// Created by xvvx on 18-12-2.
//

#include "coroutine.h"

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <thread>

constexpr uint32_t loop_times = 500 * 100;

uint64_t coroutine_test() {
    ecoroutine::CoroutineFunc func = [](){};

    clock_t start = clock();
    for (int i = 0; i < loop_times; ++i) {
        ecoroutine::coroutine_t coroutine1 = ecoroutine::create(func);
        ecoroutine::coroutine_t coroutine2 = ecoroutine::create(func);
        ecoroutine::run(coroutine1);
        ecoroutine::run(coroutine2);
    }
    clock_t end = clock();

    return end - start;
}

uint64_t thread_test() {
    auto func = [](){};
    clock_t start = clock();
    for (int i = 0; i < loop_times; ++i) {
        std::thread thread1(func);
        std::thread thread2(func);
        thread1.join();
        thread2.join();
    }
    clock_t end = clock();

    return end - start;
}

int main(int argc, char* argv[]) {
    const int kMicrosecondsPerSecond = 1000 * 1000;
    printf("coroutine: %d times %f s\n", loop_times * 2,
           static_cast<double>(coroutine_test()) / kMicrosecondsPerSecond);
    printf("thread: %d times %f s\n", loop_times * 2,
           static_cast<double>(thread_test()) / kMicrosecondsPerSecond);
    return 0;
}
