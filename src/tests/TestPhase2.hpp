// Test Functions Module
// Phase 2: Real Neural Computation Tests

#pragma once

#include <memory>
#include "brain/Brain.hpp"

namespace tests {
    
    bool runBasicConnectivityTest(std::shared_ptr<Brain> brain);
    bool runPlasticityExperiment(std::shared_ptr<Brain> brain);
    bool runStdpVerification(std::shared_ptr<Brain> brain);
    
} // namespace tests
