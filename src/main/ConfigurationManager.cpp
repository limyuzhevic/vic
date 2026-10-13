// ==============================================================================
// Main Function Implementations for Configuration Manager
// ==============================================================================

#include "main/ConfigurationManager.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

struct ConfigurationManager::Impl {
    std::shared_ptr<Config> config_;
    std::string experimentMode_ = "default";
    bool benchmarksEnabled_ = false;
    bool experimentsEnabled_ = false;
};

ConfigurationManager::ConfigurationManager() : pImpl(std::make_unique<Impl>()) {
    pImpl->config_ = std::make_shared<Config>();
}

ConfigurationManager::~ConfigurationManager() = default;

bool ConfigurationManager::load(int argc, char** argv) {
    try {
        // Default configuration for Phase 2
        pImpl->config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        pImpl->config_->set("simulation_timestep", 0.001, ConfigSource::Default);
        pImpl->config_->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        pImpl->config_->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        pImpl->config_->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // STDP parameters
        pImpl->config_->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        pImpl->config_->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        pImpl->config_->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        pImpl->config_->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        pImpl->config_->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Override with command line args
        pImpl->config_->loadFromArgs(argc, argv);
        
        // Check for special command line options
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg == "--benchmarks") {
                pImpl->benchmarksEnabled_ = true;
            } else if (arg == "--experiments") {
                pImpl->experimentsEnabled_ = true;
            } else if (arg.find("--mode=") == 0) {
                pImpl->experimentMode_ = arg.substr(7);
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception in ConfigurationManager::load: " + std::string(e.what()));
        return false;
    }
}

bool ConfigurationManager::loadFromFile(const std::string& filepath) {
    return pImpl->config_->loadFromFile(filepath);
}

template<typename T>
T ConfigurationManager::get(const std::string& key, const T& defaultValue) const {
    auto value = pImpl->config_->getOr<T>(key, defaultValue);
    return value.value_or(defaultValue);
}

void ConfigurationManager::set(const std::string& key, const std::string& value) {
    try {
        // Parse value based on type
        if (value.find('.') != std::string::npos) {
            double floatValue = std::stod(value);
            pImpl->config_->set(key, floatValue, ConfigSource::Runtime);
        } else {
            // Try to parse as int first, then fallback to string
            try {
                int intValue = std::stoi(value);
                pImpl->config_->set(key, intValue, ConfigSource::Runtime);
            } catch (...) {
                // If not int, treat as string
                pImpl->config_->set(key, value, ConfigSource::Runtime);
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error setting configuration key '" + key + "' to value '" + value + ": " + std::string(e.what()));
        throw; // Re-throw to let caller handle it
    }
}

std::vector<std::string> ConfigurationManager::getKeys() const {
    return pImpl->config_->getKeys();
}

void ConfigurationManager::printSummary() const {
    std::cout << "=== Configuration Summary ===" << std::endl;
    std::cout << pImpl->config_->summary() << std::endl;
    
    std::cout << "=== Additional Settings ===" << std::endl;
    std::cout << "  Experiment mode: " << pImpl->experimentMode_ << std::endl;
    std::cout << "  Benchmarks enabled: " << (pImpl->benchmarksEnabled_ ? "yes" : "no") << std::endl;
    std::cout << "  Experiments enabled: " << (pImpl->experimentsEnabled_ ? "yes" : "no") << std::endl;
}

bool ConfigurationManager::shouldRunBenchmarks() const {
    return pImpl->benchmarksEnabled_;
}

bool ConfigurationManager::shouldRunExperiments() const {
    return pImpl->experimentsEnabled_;
}

} // namespace nlm