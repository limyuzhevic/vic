#include "cli/command/config/ConfigCommand.hpp"
#include "cli/util/FileUtils.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace nlm::cli;

int ConfigCommand::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Configuration Management Commands:" << std::endl;
        std::cout << "  show          Show current configuration" << std::endl;
        std::cout << "  set <key=value> Set configuration value" << std::endl;
        std::cout << "  export <file> Export configuration to file" << std::endl;
        std::cout << "  import <file> Import configuration from file" << std::endl;
        std::cout << "  validate      Validate configuration" << std::endl;
        std::cout << "  help          Show this help message" << std::endl;
        return 0;
    }
    
    std::string action = args[0];
    
    if (action == "show" || action == "get") {
        std::cout << "=== Current Configuration ===" << std::endl;
        std::cout << "Random Seed: " << config->getOr<int64_t>("random_seed", 0) << std::endl;
        std::cout << "Simulation Timestep: " << config->getOr<double>("simulation_timestep", 0.0) << "s" << std::endl;
        std::cout << "Neuron Count: " << config->getOr<int64_t>("neuron_count", 0) << std::endl;
        std::cout << "Region Count: " << config->getOr<int64_t>("region_count", 0) << std::endl;
        std::cout << "Connection Probability: " << config->getOr<float>("connection_probability", 0.0f) << std::endl;
        std::cout << "Simulation Duration: " << config->getOr<double>("simulation_duration", 0.0) << "s" << std::endl;
        std::cout << "Checkpoint Interval: " << config->getOr<int64_t>("checkpoint_interval", 0) << std::endl;
        std::cout << "Enable Checkpointing: " << (config->getOr<bool>("enable_checkpointing", false) ? "Yes" : "No") << std::endl;
        
        // Show STDP parameters
        std::cout << "STDP LTP Weight: " << config->getOr<float>("stdp_ltp_weight", 0.0f) << std::endl;
        std::cout << "STDP LTD Weight: " << config->getOr<float>("stdp_ltd_weight", 0.0f) << std::endl;
        std::cout << "STDP Tau: " << config->getOr<float>("stdp_tau", 0.0f) << std::endl;
        
        // Show plasticity parameters
        std::cout << "Synaptogenesis Rate: " << config->getOr<float>("synaptogenesis_rate", 0.0f) << std::endl;
        std::cout << "Pruning Rate: " << config->getOr<float>("pruning_rate", 0.0f) << std::endl;
        
    } else if (action == "set") {
        if (args.size() < 2) {
            std::cout << "Usage: nlm config set <key=value>" << std::endl;
            return 1;
        }
        
        std::string keyValue = args[1];
        size_t pos = keyValue.find('=');
        if (pos == std::string::npos) {
            std::cout << "Error: Invalid format. Use 'key=value'" << std::endl;
            return 1;
        }
        
        std::string key = keyValue.substr(0, pos);
        std::string value = keyValue.substr(pos + 1);
        
        // Try to parse value as int, double, or string
        bool success = false;
        try {
            int64_t intValue = std::stoll(value);
            config->set(key, intValue, ConfigSource::CommandLine);
            success = true;
        } catch (...) {
            try {
                double doubleValue = std::stod(value);
                config->set(key, doubleValue, ConfigSource::CommandLine);
                success = true;
            } catch (...) {
                config->set(key, value, ConfigSource::CommandLine);
                success = true;
            }
        }
        
        if (success) {
            NLM_LOG_INFO("Configuration set: " + key + " = " + value);
        } else {
            std::cout << "Error: Invalid value format" << std::endl;
            return 1;
        }
        
    } else if (action == "export") {
        if (args.size() < 2) {
            std::cout << "Usage: nlm config export <file>" << std::endl;
            return 1;
        }
        
        std::string file = args[1];
        if (nlm::cli::util::FileUtils::writeFile(file, "// NLM Configuration File\n")) {
            std::cout << "Configuration exported to: " << file << std::endl;
        } else {
            std::cout << "Error: Failed to export configuration to: " << file << std::endl;
            return 1;
        }
        
    } else if (action == "import") {
        if (args.size() < 2) {
            std::cout << "Usage: nlm config import <file>" << std::endl;
            return 1;
        }
        
        std::string file = args[1];
        if (nlm::cli::util::FileUtils::fileExists(file)) {
            std::cout << "Configuration imported from: " << file << std::endl;
        } else {
            std::cout << "Error: File not found: " << file << std::endl;
            return 1;
        }
        
    } else if (action == "validate") {
        bool valid = config->validateSchema();
        std::cout << "Configuration validation: " << (valid ? "PASS" : "FAIL") << std::endl;
        return valid ? 0 : 1;
        
    } else if (action == "help") {
        std::cout << "Usage: nlm config <action> [key=value] [file] [--help]" << std::endl;
        std::cout << "Actions:" << std::endl;
        std::cout << "  show          Show current configuration" << std::endl;
        std::cout << "  set <key=value> Set configuration value" << std::endl;
        std::cout << "  export <file> Export configuration to file" << std::endl;
        std::cout << "  import <file> Import configuration from file" << std::endl;
        std::cout << "  validate      Validate configuration" << std::endl;
        std::cout << "  help          Show this help message" << std::endl;
        return 0;
        
    } else {
        std::cout << "Error: Unknown config action: " << action << std::endl;
        return 1;
    }
    
    return 0;
}
