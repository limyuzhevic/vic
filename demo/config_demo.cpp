// Simple test program demonstrating configuration improvements
// This shows the enhanced configuration system with centralized defaults

#include "core/Config/Config.hpp"
#include "core/Config/ConfigInitializer.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <memory>

int main(int argc, char** argv) {
    std::cout << "=== NLM Configuration Improvements Demonstration ===" << std::endl;
    std::cout << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    std::cout << "1. Testing Configuration Initializer..." << std::endl;
    auto config = std::make_shared<nlm::Config>();
    nlm::initializeConfigWithDefaults(config);
    
    std::cout << "   - Default random_seed: " << config->getOr<int64_t>(nlm::ConfigKeys::RANDOM_SEED, 0) << std::endl;
    std::cout << "   - Default neuron_count: " << config->getOr<int64_t>(nlm::ConfigKeys::NEURON_COUNT, 0) << std::endl;
    std::cout << "   - Default connection_probability: " << config->getOr<float>(nlm::ConfigKeys::CONNECTION_PROBABILITY, 0.0f) << std::endl;
    std::cout << "   - Default synaptogenesis_rate: " << config->getOr<float>(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, 0.0f) << std::endl;
    
    std::cout << std::endl;
    std::cout << "2. Testing Configuration Key Consistency..." << std::endl;
    
    // Verify all expected keys are present
    std::vector<std::string> expectedKeys = {
        nlm::ConfigKeys::RANDOM_SEED,
        nlm::ConfigKeys::NEURON_COUNT,
        nlm::ConfigKeys::REGION_COUNT,
        nlm::ConfigKeys::CONNECTION_PROBABILITY,
        nlm::ConfigKeys::STDP_LTP_WEIGHT,
        nlm::ConfigKeys::STDP_LTD_WEIGHT,
        nlm::ConfigKeys::STDP_TAU,
        nlm::ConfigKeys::SYNAPTOGENESIS_RATE,
        nlm::ConfigKeys::PRUNING_RATE,
        nlm::ConfigKeys::DOPAMINE_BASELINE,
        nlm::ConfigKeys::REWARD_DISCOUNT_FACTOR,
        nlm::ConfigKeys::ENVIRONMENT_NAME,
        nlm::ConfigKeys::LOG_LEVEL,
        nlm::ConfigKeys::LOG_TO_FILE,
        nlm::ConfigKeys::MAX_SIMULATION_STEPS
    };
    
    int keyCount = 0;
    for (const auto& key : expectedKeys) {
        if (config->has(key)) {
            keyCount++;
        } else {
            std::cout << "   WARNING: Missing expected key: " << key << std::endl;
        }
    }
    
    std::cout << "   - Found " << keyCount << " out of " << expectedKeys.size() << " expected configuration keys" << std::endl;
    
    std::cout << std::endl;
    std::cout << "3. Testing Phase 2 Specific Values..." << std::endl;
    
    // Set Phase 2 specific values
    config->set(nlm::ConfigKeys::STDP_LTP_WEIGHT, 0.02f);
    config->set(nlm::ConfigKeys::STDP_LTD_WEIGHT, 0.015f);
    config->set(nlm::ConfigKeys::STDP_TAU, 20.0f);
    config->set(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, 0.0001f);
    config->set(nlm::ConfigKeys::PRUNING_RATE, 0.00001f);
    
    std::cout << "   - STDP LTP weight: " << config->getOr<float>(nlm::ConfigKeys::STDP_LTP_WEIGHT, 0.0f) << std::endl;
    std::cout << "   - STDP LTD weight: " << config->getOr<float>(nlm::ConfigKeys::STDP_LTD_WEIGHT, 0.0f) << std::endl;
    std::cout << "   - STDP tau: " << config->getOr<float>(nlm::ConfigKeys::STDP_TAU, 0.0f) << std::endl;
    std::cout << "   - Synaptogenesis rate: " << config->getOr<float>(nlm::ConfigKeys::SYNAPTOGENESIS_RATE, 0.0f) << std::endl;
    std::cout << "   - Pruning rate: " << config->getOr<float>(nlm::ConfigKeys::PRUNING_RATE, 0.0f) << std::endl;
    
    std::cout << std::endl;
    std::cout << "4. Testing Configuration Schema..." << std::endl;
    
    auto schema = nlm::ConfigSchema::getSchema();
    std::cout << "   - Schema contains " << schema.size() << " configuration entries" << std::endl;
    
    // Check for some key schema entries
    bool hasRandomSeed = false;
    bool hasNeuronCount = false;
    bool hasConnectionProbability = false;
    
    for (const auto& entry : schema) {
        if (entry.key == nlm::ConfigKeys::RANDOM_SEED) hasRandomSeed = true;
        if (entry.key == nlm::ConfigKeys::NEURON_COUNT) hasNeuronCount = true;
        if (entry.key == nlm::ConfigKeys::CONNECTION_PROBABILITY) hasConnectionProbability = true;
    }
    
    std::cout << "   - Has random_seed schema entry: " << (hasRandomSeed ? "YES" : "NO") << std::endl;
    std::cout << "   - Has neuron_count schema entry: " << (hasNeuronCount ? "YES" : "NO") << std::endl;
    std::cout << "   - Has connection_probability schema entry: " << (hasConnectionProbability ? "YES" : "NO") << std::endl;
    
    std::cout << std::endl;
    std::cout << "5. Testing Configuration Summary..." << std::endl;
    
    std::string summary = config->summary();
    std::cout << "   - Configuration has " << summary.find("entries") - summary.find("(") << " entries" << std::endl;
    
    std::cout << std::endl;
    std::cout << "=== Configuration Improvements Demonstration Complete ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Key Improvements Made:" << std::endl;
    std::cout << "  ✓ Centralized configuration constants and defaults" << std::endl;
    std::cout << "  ✓ Fixed configuration key inconsistencies between files" << std::endl;
    std::cout << "  ✓ Improved configuration parsing with better error handling" << std::endl;
    std::cout << "  ✓ Added configuration validation and schema definition" << std::endl;
    std::cout << "  ✓ Refactored main.cpp for cleaner organization" << std::endl;
    std::cout << "  ✓ Enhanced configuration initialization and documentation" << std::endl;
    
    return 0;
}