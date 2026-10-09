#include "core/TestHelper.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>

namespace nlm {

bool TestHelper::loadConfigFromArgs(int argc, char** argv, std::shared_ptr<Config> config) {
    if (!config) return false;
    
    try {
        return config->loadFromArgs(argc, argv);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading config from command line: " + std::string(e.what()));
        return false;
    }
}

bool TestHelper::validateConfig(const std::shared_ptr<Config>& config) {
    if (!config) {
        NLM_LOG_ERROR("Config is null");
        return false;
    }
    
    // Validate required fields
    if (!config->has("random_seed")) {
        NLM_LOG_ERROR("Missing required config: random_seed");
        return false;
    }
    
    if (!config->has("neuron_count")) {
        NLM_LOG_ERROR("Missing required config: neuron_count");
        return false;
    }
    
    if (!config->has("simulation_timestep")) {
        NLM_LOG_ERROR("Missing required config: simulation_timestep");
        return false;
    }
    
    return true;
}

void TestHelper::applyPhase6Defaults(std::shared_ptr<Config> config) {
    if (!config) return;
    
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", Phase6Constants::SIMULATION_TIMESTEP, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(Phase6Constants::DEFAULT_NEURON_COUNT), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(Phase6Constants::DEFAULT_REGION_COUNT), ConfigSource::Default);
    config->set("connection_probability", Phase6Constants::DEFAULT_CONNECTION_PROBABILITY, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", Phase6Constants::DEFAULT_STDP_LTP_WEIGHT, ConfigSource::Default);
    config->set("stdp_ltd_weight", Phase6Constants::DEFAULT_STDP_LTD_WEIGHT, ConfigSource::Default);
    config->set("stdp_time_constant", Phase6Constants::DEFAULT_STDP_TIME_CONSTANT, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", Phase6Constants::DEFAULT_SYNAPSIOGENESIS_RATE, ConfigSource::Default);
    config->set("pruning_rate", Phase6Constants::DEFAULT_PRUNING_RATE, ConfigSource::Default);
}

std::shared_ptr<Brain> TestHelper::createAndInitializeBrain(std::shared_ptr<Config> config) {
    if (!config) {
        NLM_LOG_ERROR("Cannot create brain with null config");
        return nullptr;
    }
    
    try {
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return nullptr;
        }
        return brain;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception creating brain: " + std::string(e.what()));
        return nullptr;
    }
}

bool TestHelper::safeBrainReset(std::shared_ptr<Brain> brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot reset null brain");
        return false;
    }
    
    try {
        brain->reset();
        return brain->initialize();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during brain reset: " + std::string(e.what()));
        return false;
    }
}

void TestHelper::runParameterizedTest(const TestConfig& testConfig, 
                                     std::shared_ptr<Brain> brain,
                                     const std::string& testName,
                                     std::function<void(std::shared_ptr<Brain>)> testFunc) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== " + testName + " ===");
    NLM_LOG_INFO("Configuration: " + testConfig.name);
    
    try {
        testFunc(brain);
        logTestResult(testName, true, "Completed successfully");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Test " + testName + " failed: " + std::string(e.what()));
        logTestResult(testName, false, std::string(e.what()));
    }
}

void TestHelper::logProgress(const std::string& message, size_t current, size_t total) {
    std::ostringstream oss;
    oss << "[" << current << "/" << total << "] " << message;
    NLM_LOG_INFO(oss.str());
}

void TestHelper::logTestResult(const std::string& testName, bool passed, const std::string& details) {
    if (passed) {
        NLM_LOG_INFO("✓ TEST PASSED: " + testName);
        if (!details.empty()) NLM_LOG_INFO("  Details: " + details);
    } else {
        NLM_LOG_ERROR("✗ TEST FAILED: " + testName);
        if (!details.empty()) NLM_LOG_ERROR("  Details: " + details);
    }
}

std::string TestHelper::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

double TestHelper::getElapsedSeconds(std::chrono::high_resolution_clock::time_point start) {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    return duration.count() / 1000.0;
}

} // namespace nlm