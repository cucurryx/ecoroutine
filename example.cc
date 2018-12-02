#include <iostream>
#include <vector>

#include "coroutine.h"

using namespace std;


int main() {
    vector<ecoroutine::coroutine_t> coroutines;
    for (int i = 0; i < 5; ++i) {
        ecoroutine::CoroutineFunc func = [i](){
            for (int j = 0; j < 5; ++j) {
                cout << "[coroutine_t " << ecoroutine::self() << "] before yield:" << j << endl;
                ecoroutine::yield();
            }
        };
        coroutines.push_back(ecoroutine::create(func));
    }

    for (int i = 0; i < 10; ++i) {
        for (auto x : coroutines) {
            ecoroutine::run(x);
        }
    }

    return 0;
}