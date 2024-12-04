// patterns.h
#include <vector>
#include <utility>

namespace GameOfLife {

struct Pattern {
    std::vector<std::pair<int, int>> cells;
};

const Pattern glider = {
    {{0, 1}, {1, 2}, {2, 0}, {2, 1}, {2, 2}}
};

const Pattern squareBlock = {
    {{0, 0}, {0, 1}, {1, 0}, {1, 1}}
};

}

