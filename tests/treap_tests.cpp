#include <iostream>
#include <chrono>
#include <random>

#include "../treap.h"
#include "timer.h"

int main() {
    Timer timer;
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

    int n;
    std::cin >> n;

    int number_of_tests;
    std::cin >> number_of_tests;

    Treap<int> treap;
    for (int i = 1; i < n; i++) {
        treap.insert(rng());
    }

    std::vector<int> inserted(number_of_tests);

    for (int i = 0; i < number_of_tests; i++) {
        int element = rng();

        timer.start();
        treap.insert(element);
        timer.stop();

        inserted[i] = element;
        std::cout << timer.get_elapsed() << ' ';
    }

    for (int i = 0; i < number_of_tests; i++) {
        timer.start();
        treap.find(inserted[i]);
        timer.stop();

        std::cout << timer.get_elapsed() << ' ';
    }

    for (int i = 0; i < number_of_tests; i++) {
        timer.start();
        treap.erase(inserted[i]);
        timer.stop();

        std::cout << timer.get_elapsed() << ' ';
    }
}
