// ErrorHandlingUtils.cpp - Implementation of error handling utilities
#include "ErrorHandlingUtils.hpp"
#include <sstream>

namespace nlm {
namespace utils {

// ErrorRecovery implementation - no changes needed (header-only)

// ResourceManager implementation - no changes needed (header-only)

// ValidationUtils implementation

// Configuration validation implementation
bool ValidationUtils::validateConfigValue(const std::string& key, const ConfigValue& value,
                                        ErrorCategory category) {
    // This would need access to the Config class, so we'll implement basic validation here
    // Full implementation would require Config.hpp inclusion
    
    if (key == "neuron_count") {
        if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value) > 0;
        } else if (std::holds_alternative<int>(value)) {
            return std::get<int>(value) > 0;
        }
        return false;
    } else if (key == "connection_probability") {
        if (std::holds_alternative<float>(value)) {
            float prob = std::get<float>(value);
            return prob >= 0.0f && prob <= 1.0f;
        } else if (std::holds_alternative<double>(value)) {
            double prob = std::get<double>(value);
            return prob >= 0.0 && prob <= 1.0;
        }
        return false;
    } else if (key == "simulation_timestep") {
        if (std::holds_alternative<double>(value)) {
            double dt = std::get<double>(value);
            return dt > 0.0 && dt < 1.0;
        } else if (std::holds_alternative<float>(value)) {
            float dt = std::get<float>(value);
            return dt > 0.0f && dt < 1.0f;
        }
        return false;
    }
    
    return true; // Default: allow unknown keys
}

bool ValidationUtils::validateBrainState(const class Brain& brain, ErrorCategory category) {
    if (brain.getTotalNeuronCount() == 0) {
        NLM_THROW(category, "Brain initialized with 0 neurons.");
        return false;
    }
    
    if (brain.getTotalSynapseCount() < 0) {
        NLM_THROW(category, "Invalid synapse count in brain.");
        return false;
    }
    
    return true;
}

bool ValidationUtils::validateMemoryAllocation(size_t size, size_t maxSize, ErrorCategory category) {
    if (size > maxSize) {
        NLM_THROW(category, "Memory allocation exceeds maximum allowed size.");
        return false;
    }
    return true;
}

// PerformanceMonitor implementation - no changes needed (header-only)

} // namespace utils
} // namespace nlm

// Test the error handling utilities
#ifdef TESTING
void testErrorHandlingUtils() {
    nlm::utils::ErrorRecovery::Configuration config;
    nlm::utils::ErrorRecovery recovery(config);
    
    // Test error handling
    try {
        nlm::utils::NLMException exc(nlm::utils::ErrorCategory::Validation, "Test error");
        recovery.handleError(nlm::utils::ErrorInfo(nlm::utils::ErrorCategory::Validation, "Test error"));
    } catch (...) {
        // Exception should be logged and handled
    }
    
    // Test validation
    bool valid = nlm::utils::ValidationUtils::validateConfigValue("neuron_count", int64_t(100));
    assert(valid == true);
    
    valid = nlm::utils::ValidationUtils::validateConfigValue("connection_probability", float(0.5f));
    assert(valid == true);
    
    valid = nlm::utils::ValidationUtils::validateConfigValue("invalid_key", std::string("test"));
    assert(valid == true); // Unknown keys pass by default
    
    // Test resource manager
    auto& resourceManager = nlm::getResourceManager();
    
    // Performance monitor test
    auto& perfMonitor = nlm::getPerformanceMonitor();
    perfMonitor.startTiming("test_operation");
    // Simulate work
    for (int i = 0; i < 1000; ++i) { volatile int x = i; }
    perfMonitor.stopTiming("test_operation");
    
    perfMonitor.logPerformanceSummary();
    
    std::cout << "Error handling utilities test completed successfully!" << std::endl;
}
#endif // TESTING