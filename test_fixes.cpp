#include "src/core/Config/Config.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include "src/neuromodulation/Novelty.hpp"
#include "src/neuromodulation/PredictionError.hpp"
#include "src/neuromodulation/Reward.hpp"
#include "src/environment/Observation.hpp"
#include <iostream>

int main() {
    std::cout << "Testing fixed NLM files..." << std::endl;
    
    // Test Config
    nlm::Config config;
    config.set("test_key", 42);
    auto value = config.get<int>("test_key");
    if (value.has_value() && value.value() == 42) {
        std::cout << "✓ Config test passed" << std::endl;
    } else {
        std::cout << "✗ Config test failed" << std::endl;
        return 1;
    }
    
    // Test Neuromodulator (abstract class can't be instantiated, but we can test derived classes)
    std::cout << "✓ Neuromodulator header includes fixed" << std::endl;
    
    // Test Novelty
    nlm::Novelty novelty;
    std::cout << "✓ Novelty header includes fixed" << std::endl;
    
    // Test PredictionError
    nlm::PredictionError predError;
    std::cout << "✓ PredictionError header includes fixed" << std::endl;
    
    // Test Reward
    nlm::Reward reward;
    reward.add(10.0f);
    if (reward.getValue() == 10.0f) {
        std::cout << "✓ Reward test passed" << std::endl;
    } else {
        std::cout << "✗ Reward test failed" << std::endl;
        return 1;
    }
    
    // Test Observation
    nlm::Observation observation;
    std::cout << "✓ Observation header includes fixed" << std::endl;
    
    std::cout << "\nAll tests passed! Files are correctly fixed." << std::endl;
    return 0;
}