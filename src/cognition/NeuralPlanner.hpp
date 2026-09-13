#pragma once

#include "NeuralPlanner.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>

namespace nlm {

NeuralPlanner::NeuralPlanner() : pImpl(new Impl) {}
NeuralPlanner::~NeuralPlanner() = default;

void NeuralPlanner::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->planningDepth = 5;
}

void NeuralPlanner::setPlanningDepth(int depth) {
    pImpl->planningDepth = depth;
}

void NeuralPlanner::update(double timestep) {
    // Placeholder implementation
}

} // namespace nlm
