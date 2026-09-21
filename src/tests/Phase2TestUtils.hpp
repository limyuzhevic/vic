// Phase 2 Test Utilities Header
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"

namespace nlm {

// Test function declarations
void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
void runPlasticityExperiment(std::shared_ptr<Brain> brain, uint64_t seed = 42);
void runStdpVerification(std::shared_ptr<Brain> brain);
void runPhase2CompleteTest(std::shared_ptr<Brain> brain);

// Configuration utilities
class Phase2ConfigBuilder {
public:
    static std::shared_ptr<Config> createDefaultConfig();
    static std::shared_ptr<Config> createSmallConfig();
    static std::shared_ptr<Config> createLargeConfig();
    static std::shared_ptr<Config> createConfigFromArgs(int argc, char** argv);
};

// Logging utilities
class TestLogger {
public:
    static void initializeLogging(bool enableConsole = true, LogLevel level = LogLevel::Info);
    static void logSection(const std::string& title);
    static void logTestResult(const std::string& testName, bool success, const std::string& message = "");
    static void logBrainStats(std::shared_ptr<Brain> brain);
};

// Test result structure
struct TestResult {
    std::string testName;
    bool passed;
    std::string details;
    std::chrono::duration<double> executionTime;
};

} // namespace nlm
