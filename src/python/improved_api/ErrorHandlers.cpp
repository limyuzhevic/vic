#include "NLMImprovedAPI.hpp"
#include <iostream>
#include <sstream>
#include <thread>

namespace nlm {
namespace ErrorHandling {

NLMError::NLMError(const std::string& message) : std::runtime_error(message) {}

BrainInitializationError::BrainInitializationError(const std::string& message) 
    : NLMError("Brain initialization failed: " + message) {}

ConfigurationError::ConfigurationError(const std::string& message)
    : NLMError("Configuration error: " + message) {}

SimulationError::SimulationError(const std::string& message)
    : NLMError("Simulation error: " + message) {}

MemoryError::MemoryError(const std::string& message)
    : NLMError("Memory error: " + message) {}

ValidationError::ValidationError(const std::string& message)
    : NLMError("Validation error: " + message) {}

bool attempt_recovery(std::shared_ptr<Brain> brain, int max_attempts) {
    for (int attempt = 1; attempt <= max_attempts; ++attempt) {
        brain->reset();
        if (brain->initialize()) {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100 * attempt)); // Exponential backoff
    }
    return false;
}

std::string get_error_context(const std::exception& e) {
    std::stringstream ss;
    ss << "Error context: " << e.what() << std::endl;
    ss << "Recommendation: Check configuration and system resources" << std::endl;
    return ss.str();
}

void log_error_safely(const std::exception& e) {
    std::string error_msg = "Error: " + std::string(e.what());
    // Simple safe logging - don't crash on logging failures
    try {
        std::cerr << error_msg << std::endl;
    } catch (...) {
        // If even stderr fails, silently continue
    }
}

} // namespace ErrorHandling

} // namespace nlm