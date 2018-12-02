#include <iostream>

#include "coroutine.h"

using namespace std;


void test() {
    for (int i = 0; i < 10; ++i) {
        ecoroutine::CoFunction func = [i](){
            cout << "begin" << i << endl;
            ecoroutine::yield();
            cout << "end" << i << endl;

        };
        ecoroutine::create(func);
    }
}

int main() {
    test();
    return 0;
}