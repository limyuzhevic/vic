// Phase2ConfigManager.cpp - Implementation of Phase 2 configuration manager
#include "core/Phase2ConfigManager.hpp"
#include "core/TestConstants.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {
namespace phase2 {

struct Phase2TestConfigManager::Impl {
    std::vector<std::string> parsedArgs;
    bool argsParsed;
    std::map<std::string, std::function<void(const std::string&)>> customOptions;
};

Phase2TestConfigManager::Phase2TestConfigManager() : pImpl(std::make_unique<Impl>()) {
    pImpl->argsParsed = false;
    applyDefaults();
}

Phase2TestConfigManager::~Phase2TestConfigManager() = default;

bool Phase2TestConfigManager::loadFromArgs(int argc, char** argv) {
    try {
        parseCommandLineArgs(argc, argv);
        validateConfig();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading configuration from arguments: " << e.what() << std::endl;
        return false;
    }
}

bool Phase2TestConfigManager::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON/YAML first
    // For now, use simple key=value parsing
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Map to config structure (simplified implementation)
            if (key == "randomSeed") config_.randomSeed = std::stoull(value);
            else if (key == "timestep") config_.timestep = std::stod(value);
            else if (key == "neuronCount") config_.neuronCount = std::stoull(value);
            else if (key == "regionCount") config_.regionCount = std::stoull(value);
            else if (key == "connectionProbability") config_.connectionProbability = std::stof(value);
        }
    }
    
    return true;
}

void Phase2TestConfigManager::parseCommandLineArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile_ = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile_ = argv[++i];
            }
        } else if (arg.substr(0, 8) == "--output=") {
            config_.outputFile = arg.substr(9);
        } else if (arg.substr(0, 7) == "--test=") {
            std::string testList = arg.substr(7);
            size_t pos = 0;
            while ((pos = testList.find(',')) != std::string::npos) {
                std::string test = testList.substr(0, pos);
                config_.runTests.push_back(test);
                testList.erase(0, pos + 1);
            }
            config_.runTests.push_back(testList);
        } else if (arg.substr(0, 10) == "--benchmark") {
            config_.enableBenchmark = true;
        } else if (arg.substr(0, 9) == "--verbose") {
            config_.verbose = true;
        } else if (arg.substr(0, 11) == "--neuron-count") {
            if (i + 1 < argc) config_.neuronCount = std::stoull(argv[++i]);
        } else if (arg.substr(0, 16) == "--plasticity-steps") {
            if (i + 1 < argc) config_.plasticitySteps = std::stoul(argv[++i]);
        } else if (arg.substr(0, 3) == "-s ") {
            config_.randomSeed = std::stoull(&arg[3]);
        }
    }
    
    pImpl->argsParsed = true;
}

void Phase2TestConfigManager::applyDefaults() {
    // Apply default values from constants
    config_.randomSeed = PHASE2_RANDOM_SEED_DEFAULT;
    config_.simulationTimestep = PHASE2_SIMULATION_TIMESTEP;
    config_.neuronCount = PHASE2_NEURON_COUNT;
    config_.regionCount = PHASE2_REGION_COUNT;
    config_.connectionProbability = PHASE2_CONNECTION_PROBABILITY;
    config_.stdpLtpWeight = PHASE2_STDP_LTP_WEIGHT;
    config_.stdpLtdWeight = PHASE2_STDP_LTD_WEIGHT;
    config_.stdpTau = PHASE2_STDP_TAU;
    config_.synaptogenesisRate = PHASE2_SYNAPTOGENESIS_RATE;
    config_.pruningRate = PHASE2_PRUNING_RATE;
    config_.plasticitySteps = PHASE2_TEST_STEPS;
    config_.connectivityTestSteps = PHASE2_CONNECTIVITY_TEST_STEPS;
    config_.stdpTestTrials = PHASE2_STDP_TEST_TRIALS;
    config_.checkpointFile = PHASE2_CHECKPOINT_FILE;
}

void Phase2TestConfigManager::validateConfig() {
    if (config_.neuronCount == 0) {
        throw std::runtime_error("Neuron count must be positive");
    }
    if (config_.regionCount == 0) {
        throw std::runtime_error("Region count must be positive");
    }
    if (config_.connectionProbability < 0.0 || config_.connectionProbability > 1.0) {
        throw std::runtime_error("Connection probability must be between 0 and 1");
    }
    if (config_.plasticitySteps == 0) {
        throw std::runtime_error("Plasticity steps must be positive");
    }
}

void Phase2TestConfigManager::printConfigSummary() const {
    std::cout << "=== Phase 2 Test Configuration Summary ===" << std::endl;
    std::cout << "Random Seed: " << config_.randomSeed << std::endl;
    std::cout << "Neuron Count: " << config_.neuronCount << std::endl;
    std::cout << "Region Count: " << config_.regionCount << std::endl;
    std::cout << "Connection Probability: " << config_.connectionProbability << std::endl;
    std::cout << "Plasticity Steps: " << config_.plasticitySteps << std::endl;
    std::cout << "Enable Benchmark: " << (config_.enableBenchmark ? "Yes" : "No") << std::endl;
    std::cout << "Enable Profiling: " << (config_.enableProfiling ? "Yes" : "No") << std::endl;
    std::cout << "Verbose: " << (config_.verbose ? "Yes" : "No") << std::endl;
    std::cout << "Run Connectivity Test: " << (config_.runConnectivityTest ? "Yes" : "No") << std::endl;
    std::cout << "Run Plasticity Test: " << (config_.runPlasticityTest ? "Yes" : "No") << std::endl;
    std::cout << "Run STDP Test: " << (config_.runStdpTest ? "Yes" : "No") << std::endl;
    std::cout << "Run Learning Experiment: " << (config_.runLearningExperiment ? "Yes" : "No") << std::endl;
    std::cout << "============================================" << std::endl;
}

Phase2TestConfigManager& Phase2TestConfigManager::getInstance() {
    static std::shared_ptr<Phase2TestConfigManager> instance = std::make_shared<Phase2TestConfigManager>();
    return *instance;
}

void Phase2TestConfigManager::setInstance(std::shared_ptr<Phase2TestConfigManager> manager) {
    // This is a simplified implementation; in practice you'd need to manage instance lifecycle
    std::shared_ptr<Phase2TestConfigManager>& instance = 
        const_cast<std::shared_ptr<Phase2TestConfigManager>&>(
            reinterpret_cast<const std::shared_ptr<Phase2TestConfigManager>&>(getInstance()));
    instance = manager;
}

std::shared_ptr<Config> Phase2TestConfigManager::toCoreConfig() const {
    auto config = std::make_shared<Config>();
    
    // Convert Phase 2 config to core config
    config->set("random_seed", static_cast<int64_t>(config_.randomSeed), ConfigSource::Runtime);
    config->set("simulation_timestep", config_.simulationTimestep, ConfigSource::Runtime);
    config->set("neuron_count", static_cast<int64_t>(config_.neuronCount), ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(config_.regionCount), ConfigSource::Runtime);
    config->set("connection_probability", config_.connectionProbability, ConfigSource::Runtime);
    config->set("stdp_ltp_weight", config_.stdpLtpWeight, ConfigSource::Runtime);
    config->set("stdp_ltd_weight", config_.stdpLtdWeight, ConfigSource::Runtime);
    config->set("stdp_tau", config_.stdpTau, ConfigSource::Runtime);
    
    return config;
}

std::string Phase2TestConfigManager::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

} // namespace phase2
} // namespace nlm