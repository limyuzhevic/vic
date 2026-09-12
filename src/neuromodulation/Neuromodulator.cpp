#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <algorithm>
#include <cmath>

namespace nlm {

// Initialize Serotonin implementation  
void Serotonin::initialize(class Brain* brain) {
    // Serotonin initialization if needed
}

// Initialize Acetylcholine implementation  
void Acetylcholine::initialize(class Brain* brain) {
    // Acetylcholine initialization if needed
}

void Norepinephrine::initialize(class Brain* brain) {
    // Norepinephrine initialization if needed
}

// Dopamine.cpp implementation (existing, but fixed)
void Dopamine::initialize(class Brain* brain) {
    // Dopamine initialization if needed
}

// Curiosity.cpp implementation
void Curiosity::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized");
}

// PredictionError.cpp implementation
void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError system initialized");
}

// Novelty.cpp implementation
void Novelty::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty system initialized");
}

} // namespace nlm