#pragma once

#include "core/Types/Types.hpp"
#include "core/Config/Config.hpp"
#include "core/TestConfig.hpp"
#include "brain/Brain.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <chrono>

namespace nlm {

class TestHelper {
public:
    // Configuration loading and validation
    static bool loadConfigFromArgs(int argc, char** argv, std::shared_ptr<Config> config);
    static bool validateConfig(const std::shared_ptr<Config>& config);
    static void applyPhase6Defaults(std::shared_ptr<Config> config);
    
    // Brain initialization and error handling
    static std::shared_ptr<Brain> createAndInitializeBrain(std::shared_ptr<Config> config);
    static bool safeBrainReset(std::shared_ptr<Brain> brain);
    
    // Test orchestration
    static void runParameterizedTest(const TestConfig& testConfig, 
                                     std::shared_ptr<Brain> brain,
                                     const std::string& testName,
                                     std::function<void(std::shared_ptr<Brain>)> testFunc);
    
    // Progress reporting
    static void logProgress(const std::string& message, size_t current, size_t total);
    static void logTestResult(const std::string& testName, bool passed, const std::string& details = "");
    
    // Utility functions
    static std::string getTimestamp();
    static double getElapsedSeconds(std::chrono::high_resolution_clock::time_point start);
};

} // namespace nlm