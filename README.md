## ecoroutine

A simple pthread style coroutine library for modern C++.

### How to use?

#### create a coroutine
```C++
auto c = Coroutine([](){
    cout << "this is coroutine function";
});
```

#### start a coroutine
```C++
c.Start();
```

#### yield a coroutine
```C++
c.Yield();
```

#### example

```C++
a example show the use of coroutine
```
