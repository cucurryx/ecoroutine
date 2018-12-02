## ecoroutine

A simple stackful asymmetric scheduling coroutine library for modern C++.

### How to use?

#### create a coroutine
```C++
        ecoroutine::CoroutineFunc func = [i](){};
        coroutine_t c = ecoroutine::create(func);
```
Notice: this `create` is different from the `pthread_create`, since the coroutine will
not begin to work immediatly. You need to call `ecoroutine::start`.


#### start a coroutine
```C++
ecoroutine::start(c);
```

#### yield a coroutine
```C++
ecoroutine::yield();
```
`yield` will yield the current running coroutine. If you try to yield the main coroutine,
it'll report an error. Because coroutine is non-preemptive, the only way to give up control of CPU is
yield. And the current coroutine's state will change to HangUp, and the scheduler will begin
to work.


#### example

```C++
int main() {
    vector<ecoroutine::coroutine_t> coroutines;
    for (int i = 0; i < 5; ++i) {
        ecoroutine::CoroutineFunc func = [i](){
            for (int j = 0; j < 5; ++j) {
                cout << "[coroutine_t " << ecoroutine::self() << "] " << j << endl;
                ecoroutine::yield();
            }
        };
        coroutines.push_back(ecoroutine::create(func));
    }

    for (auto x : coroutines) {
        ecoroutine::start(x);
    }
    return 0;
}
```

result:
```
[coroutine_t 1] 0
[coroutine_t 2] 0
[coroutine_t 3] 0
[coroutine_t 4] 0
[coroutine_t 5] 0
[coroutine_t 1] 1
[coroutine_t 2] 1
[coroutine_t 3] 1
[coroutine_t 4] 1
[coroutine_t 5] 1
[coroutine_t 1] 2
[coroutine_t 2] 2
[coroutine_t 3] 2
[coroutine_t 4] 2
[coroutine_t 5] 2
[coroutine_t 1] 3
[coroutine_t 2] 3
[coroutine_t 3] 3
[coroutine_t 4] 3
[coroutine_t 5] 3
[coroutine_t 1] 4
[coroutine_t 2] 4
[coroutine_t 3] 4
[coroutine_t 4] 4
[coroutine_t 5] 4
```