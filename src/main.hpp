#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include "core/Logger/Logger.hpp"

namespace nlm {

// Forward declarations
class Config;
class Brain;
class Logger;

// Error types
enum class NLMError {
    Success = 0,
    InitializationFailed = 1,
    ConfigLoadFailed = 2,
    BrainInitializationFailed = 3,
    InvalidParameters = 4,
    MemoryError = 5
};

// Type aliases for better readability
using SimulationStep = int64_t;
using NeuronId = int64_t;
using RegionId = int64_t;
using NeuronType = int;

// Result type for error handling
class Result {
public:
    Result(NLMError error = NLMError::Success, const std::string& message = "")
        : error_(error), message_(message) {}
    
    bool success() const { return error_ == NLMError::Success; }
    bool failed() const { return !success(); }
    NLMError error() const { return error_; }
    const std::string& message() const { return message_; }
    
private:
    NLMError error_;
    std::string message_;
};

// Configuration initialization function
Result setupConfiguration(int argc, char** argv, std::shared_ptr<Config>& config);

// Brain initialization function
Result setupBrain(std::shared_ptr<Config> config, std::shared_ptr<Brain>& brain);

// Test functions
Result runBasicConnectivityTest(std::shared_ptr<Brain> brain);
Result runPlasticityExperiment(std::shared_ptr<Brain> brain);
Result runStdpVerification(std::shared_ptr<Brain> brain);

// Results reporting function
void printFinalResults(const std::shared_ptr<Brain>& brain);

// Utility functions
bool parseCommandLineArgs(int argc, char** argv, std::string& configFile, 
                         std::map<std::string, std::string>& customConfig);

} // namespace nlm