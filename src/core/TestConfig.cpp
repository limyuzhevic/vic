#include "core/TestConfig.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace nlm {

TestConfig::TestConfig() {
    // Load from default config file
    loadFromFile("configs/default.cfg");
}

bool TestConfig::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Test configuration file not found: " + filename);
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Parse key=value lines
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            // Parse numeric values and apply to parameters
            parseKeyValue(key, value);
        }
    }
    
    NLM_LOG_INFO("Test configuration loaded from: " + filename);
    return true;
}

void TestConfig::loadFromArgs(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    parseArgs(args);
}

void TestConfig::parseArgs(const std::vector<std::string>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg == "--random_seed" && i + 1 < args.size()) {
            set<uint64_t>("random_seed", std::stoull(args[++i]));
        }
        else if (arg == "--timestep" && i + 1 < args.size()) {
            set<double>("timestep", std::stod(args[++i]));
        }
        else if (arg == "--neuron_count" && i + 1 < args.size()) {
            set<int64_t>("neuron_count", std::stoll(args[++i]));
        }
        else if (arg == "--region_count" && i + 1 < args.size()) {
            set<int64_t>("region_count", std::stoll(args[++i]));
        }
        else if (arg == "--connection_probability" && i + 1 < args.size()) {
            set<float>("connection_probability", std::stof(args[++i]));
        }
        else if (arg == "--verbose" || arg == "--quiet") {
            params_.verboseLogging = (arg == "--verbose");
        }
        else if (arg == "--debug") {
            params_.enableDebugOutput = true;
        }
        else if (arg == "--config" && i + 1 < args.size()) {
            params_.configFile = args[++i];
        }
    }
}

void TestConfig::parseKeyValue(const std::string& key, const std::string& value) {
    // Parse numeric values and apply to parameters
    if (key == "random_seed") {
        set<uint64_t>("random_seed", std::stoull(value));
    }
    else if (key == "timestep") {
        set<double>("timestep", std::stod(value));
    }
    else if (key == "neuron_count") {
        set<int64_t>("neuron_count", std::stoll(value));
    }
    else if (key == "region_count") {
        set<int64_t>("region_count", std::stoll(value));
    }
    else if (key == "connection_probability") {
        set<float>("connection_probability", std::stof(value));
    }
    else if (key == "stdp_ltp_weight") {
        set<float>("stdp_ltp_weight", std::stof(value));
    }
    else if (key == "stdp_ltd_weight") {
        set<float>("stdp_ltd_weight", std::stof(value));
    }
    else if (key == "stdp_tau") {
        set<float>("stdp_tau", std::stof(value));
    }
    else if (key == "synaptogenesis_rate") {
        set<float>("synaptogenesis_rate", std::stof(value));
    }
    else if (key == "pruning_rate") {
        set<float>("pruning_rate", std::stof(value));
    }
    else if (key == "neurons_to_inject") {
        set<size_t>("neurons_to_inject", std::stoul(value));
    }
    else if (key == "steps_for_plasticity") {
        set<size_t>("steps_for_plasticity", std::stoul(value));
    }
}

void TestConfig::printConfigSummary() const {
    NLM_LOG_INFO("=== Test Configuration Summary ===");
    NLM_LOG_INFO("Random seed: " + std::to_string(params_.randomSeed));
    NLM_LOG_INFO("Timestep: " + std::to_string(params_.timestep) + "s");
    NLM_LOG_INFO("Neuron count: " + std::to_string(params_.neuronCount));
    NLM_LOG_INFO("Region count: " + std::to_string(params_.regionCount));
    NLM_LOG_INFO("Connection probability: " + std::to_string(params_.connectionProbability));
    NLM_LOG_INFO("STDP LTP weight: " + std::to_string(params_.stdpLtpWeight));
    NLM_LOG_INFO("STDP LTD weight: " + std::to_string(params_.stdpLtdWeight));
    NLM_LOG_INFO("STDP tau: " + std::to_string(params_.stdpTau));
    NLM_LOG_INFO("Synaptogenesis rate: " + std::to_string(params_.synaptogenesisRate));
    NLM_LOG_INFO("Pruning rate: " + std::to_string(params_.pruningRate));
    NLM_LOG_INFO("Neurons to inject: " + std::to_string(params_.neuronsToInject));
    NLM_LOG_INFO("Steps for plasticity: " + std::to_string(params_.stepsForPlasticity));
    NLM_LOG_INFO("Steps for STDP: " + std::to_string(params_.stepsForStdp));
    NLM_LOG_INFO("Verbose logging: " + std::string(params_.verboseLogging ? "Yes" : "No"));
    NLM_LOG_INFO("Debug output: " + std::string(params_.enableDebugOutput ? "Yes" : "No"));
    NLM_LOG_INFO("====================================");
}

} // namespace nlm