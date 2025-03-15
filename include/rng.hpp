#ifndef MINESWEEPER_RNG_H
#define MINESWEEPER_RNG_H

#include <random>

template <typename numType, class dist>
class RNG {
private:
    std::mt19937 gen;
    numType min_, max_;
    dist dis;

public:
    RNG():
            gen(std::random_device{}()),
            min_(0),
            max_(0) {
        set_range(min_, max_);
    }

    void set_range(numType min, numType max) {
        dis = dist(min, max);
    }

    numType generate() {
        return dis(gen);
    }
    numType generate(numType min, numType max) {
        set_range(min, max);
        return dis(gen);
    }
};

class RNG_int: public RNG<int, std::uniform_int_distribution<>> {
};

class RNG_real: public RNG<double, std::uniform_real_distribution<>> {
};

#endif //MINESWEEPER_RNG_H
