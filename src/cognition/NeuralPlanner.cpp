#include "NeuralPlanner.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuralPlanner::Impl {
    Brain* brain;
    
    Impl() : brain(nullptr) {}
};