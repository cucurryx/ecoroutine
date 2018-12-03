## ecoroutine

A simple stackful asymmetric scheduling coroutine library for modern C++.

- schedule algorithm: asymmetric coroutine sheduling, same as [libgo](https://github.com/yyzybb537/libgo)
- coroutine stack: virtual memory stack


### How to use?

#### build
```
cmake .
make example && make benchmark
```

#### create a coroutine
```C++
ecoroutine::CoroutineFunc func = [i](){};
coroutine_t c = ecoroutine::create(func);
```
Notice: this `create` is different from the `pthread_create`, since the coroutine will
not begin to work immediatly. You need to call `ecoroutine::start`.


#### run a coroutine
If a coroutie has been created but not running yet, you need run. If a coroutine has called yield,
you need run to make it running again.

```C++
ecoroutine::run(c);
```

#### yield a coroutine
```C++
ecoroutine::yield();
```

`yield` will yield the current running coroutine. If you try to yield the main coroutine,
it'll report an error. Because coroutine is non-preemptive, the only way to give up control of CPU is
yield. And the current coroutine's state will change to HangUp, and the scheduler will begin
to work.

### How's the performance?
Comparing to C++11 std::thread, ecoroutine doesn't need to do context switch in kernel mode. So it's quietly faster than std::thread.

I create and destroy thread/coroutine for 10,000,000 times to show the difference of their performance.

![./pics/bench.png](./pics/bench.png)

benchmark code：

```C++
constexpr uint32_t loop_times = 500 * 1000;

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

```

### future
- maybe a channel for data transfer
- maybe hook for networking I/O
