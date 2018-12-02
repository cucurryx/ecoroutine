#include <iostream>
#include <vector>

#include "coroutine.h"

using namespace std;


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