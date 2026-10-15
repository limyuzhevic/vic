#pragma once

#include "Brain.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Brain initialization declarations
// This provides the interface for Brain initialization
// All implementations are in BrainInitialization.cpp

class BrainInitialization {
public:
    // Initialize brain with configuration
    static bool initializeBrain(std::shared_ptr<Brain::Impl> pImpl);
};

}} // namespace nlm