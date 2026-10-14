// Test utilities and helper functions for NLM

#pragma once

#include <memory>
#include <string>
#include "brain/Brain.hpp"

namespace nlm {
namespace tests {

// Basic neural connectivity test
void runBasicConnectivityTest(std::shared_ptr<Brain> brain);

// Plasticity learning experiment test
void runPlasticityExperiment(std::shared_ptr<Brain> brain);

// STDP verification test
void runStdpVerification(std::shared_ptr<Brain> brain);

// Common test utilities
class TestUtils {
public:
    static void logTestHeader(const std::string& testName);
    static void logTestResult(const std::string& testName, bool passed, const std::string& message = "");
    static void resetBrain(std::shared_ptr<Brain> brain);
    static size_t getActiveSynapseCount(std::shared_ptr<Brain> brain);
    static float getAverageSynapseWeight(std::shared_ptr<Brain> brain);
};

} // namespace tests
} // namespace nlm