#ifndef MINESWEEPER_RNG_H
#define MINESWEEPER_RNG_H

#include <random>

class RNG {
private:
    std::mt19937 gen;
    int min_, max_;
    std::uniform_int_distribution<> dis;

public:
    RNG():
            gen(std::random_device{}()),
            min_(0),
            max_(0) {
        set_range(min_, max_);
    }

    void set_range(int min, int max) {
        dis = std::uniform_int_distribution<>(min, max);
    }

    int generate() {
        return dis(gen);
    }
    int generate(int min, int max) {
        set_range(min, max);
        return dis(gen);
    }
};

#endif //MINESWEEPER_RNG_H
