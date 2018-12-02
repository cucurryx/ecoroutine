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
    for (int i = 0; i < 10; ++i) {
        ecoroutine::CoroutineFunc func = [i](){
            for (int j = 0; j < 3; ++j) {
                cout << "[coroutine_t " << ecoroutine::self() << "] before yield:" << j << endl;
                ecoroutine::yield();
                cout << "[coroutine_t " << ecoroutine::self() << "] after yiled: " << j << endl;
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
[coroutine_t 1] before yield:0
[coroutine_t 2] before yield:0
[coroutine_t 3] before yield:0
[coroutine_t 4] before yield:0
[coroutine_t 5] before yield:0
[coroutine_t 6] before yield:0
[coroutine_t 7] before yield:0
[coroutine_t 8] before yield:0
[coroutine_t 9] before yield:0
[coroutine_t 10] before yield:0
[coroutine_t 1] after yiled: 0
[coroutine_t 1] before yield:1
[coroutine_t 2] after yiled: 0
[coroutine_t 2] before yield:1
[coroutine_t 3] after yiled: 0
[coroutine_t 3] before yield:1
[coroutine_t 4] after yiled: 0
[coroutine_t 4] before yield:1
[coroutine_t 5] after yiled: 0
[coroutine_t 5] before yield:1
[coroutine_t 6] after yiled: 0
[coroutine_t 6] before yield:1
[coroutine_t 7] after yiled: 0
[coroutine_t 7] before yield:1
[coroutine_t 8] after yiled: 0
[coroutine_t 8] before yield:1
[coroutine_t 9] after yiled: 0
[coroutine_t 9] before yield:1
[coroutine_t 10] after yiled: 0
[coroutine_t 10] before yield:1
[coroutine_t 1] after yiled: 1
[coroutine_t 1] before yield:2
[coroutine_t 2] after yiled: 1
[coroutine_t 2] before yield:2
[coroutine_t 3] after yiled: 1
[coroutine_t 3] before yield:2
[coroutine_t 4] after yiled: 1
[coroutine_t 4] before yield:2
[coroutine_t 5] after yiled: 1
[coroutine_t 5] before yield:2
[coroutine_t 6] after yiled: 1
[coroutine_t 6] before yield:2
[coroutine_t 7] after yiled: 1
[coroutine_t 7] before yield:2
[coroutine_t 8] after yiled: 1
[coroutine_t 8] before yield:2
[coroutine_t 9] after yiled: 1
[coroutine_t 9] before yield:2
[coroutine_t 10] after yiled: 1
[coroutine_t 10] before yield:2
[coroutine_t 1] after yiled: 2
[coroutine_t 2] after yiled: 2
[coroutine_t 3] after yiled: 2
[coroutine_t 4] after yiled: 2
[coroutine_t 5] after yiled: 2
[coroutine_t 6] after yiled: 2
[coroutine_t 7] after yiled: 2
[coroutine_t 8] after yiled: 2
[coroutine_t 9] after yiled: 2
[coroutine_t 10] after yiled: 2
```