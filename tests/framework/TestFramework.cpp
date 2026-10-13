// Test Framework Implementation

#include "TestFramework.hpp"

namespace nlm {
namespace tests {

// Factory function for creating test configurations
std::shared_ptr<Config> createTestConfig(uint64_t seed = 42) {
    auto config = std::make_shared<Config>();
    
    // Set test-specific defaults
    config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Performance constraints
    config->set("max_test_duration", 10000.0, ConfigSource::Default); // ms
    
    return config;
}

// Performance measurement utilities
struct PerformanceMetrics {
    double duration_ms = 0.0;
    size_t neuronsProcessed = 0;
    size_t synapsesProcessed = 0;
    float opsPerMs = 0.0f;
    
    float calculateScore() const {
        if (duration_ms == 0.0) return 0.0f;
        return (static_cast<float>(neuronsProcessed) + 
                static_cast<float>(synapsesProcessed) * 10.0f) / 
               static_cast<float>(duration_ms);
    }
};

PerformanceMetrics measurePerformance(std::function<void()> operation) {
    PerformanceMetrics metrics;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    operation();
    auto endTime = std::chrono::high_resolution_clock::now();
    
    metrics.duration_ms = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    return metrics;
}

} // namespace tests
} // namespace nlm
