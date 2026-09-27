#include "cli/ConfigCommand.hpp"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "core/Config/Config.hpp"

using json = nlohmann::json;

using namespace nlm;
namespace cli {

ConfigCommand::ConfigCommand() 
    : CommandBase("config", "Configuration management (create, validate, view)"),
      action_("view"),
      output_file_(""),
      validate_only_(false) {
}

bool ConfigCommand::execute(const std::map<std::string, std::string>& args) {
    std::cout << "Configuration command: " << action_ << std::endl;
    
    if (action_ == "create") {
        return createConfiguration(output_file_);
    } else if (action_ == "view") {
        return viewConfiguration(output_file_);
    } else if (action_ == "validate") {
        return validateConfiguration(output_file_);
    } else if (action_ == "export") {
        return exportConfiguration(output_file_);
    } else {
        std::cerr << "Error: Unknown config action: " << action_ << std::endl;
        return false;
    }
}

void ConfigCommand::printHelp() const {
    std::cout << "Usage: nlm-cli config [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Manage configuration files." << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -a, --action <act>    Action: create, view, validate, export" << std::endl;
    std::cout << "  -o, --output <file>   Output file for configuration export" << std::endl;
    std::cout << "  -V, --validate        Only validate configuration, don't modify" << std::endl;
    std::cout << "  -q, --quiet           Suppress console output" << std::endl;
    std::cout << "  -v, --verbose         Enable verbose logging" << std::endl;
}

void ConfigCommand::printOptions() const {
    std::cout << "  [command options]" << std::endl;
}

bool ConfigCommand::createConfiguration(const std::string& output_file) {
    json config;
    
    config["enabled"] = true;
    config["random_seed"] = 42;
    config["neuron_count"] = 100;
    config["region_count"] = 1;
    config["simulation_timestep"] = 0.001;
    config["connection_probability"] = 0.1f;
    config["stdp_ltp_weight"] = 0.01f;
    config["stdp_ltd_weight"] = 0.012f;
    config["synaptogenesis_rate"] = 0.0001f;
    config["pruning_rate"] = 0.00001f;
    
    if (output_file.empty()) {
        output_file = "configs/default_config.json";
    }
    
    std::ofstream file(output_file);
    if (!file.is_open()) {
        std::cerr << "Error: Could not create configuration file: " << output_file << std::endl;
        return false;
    }
    
    file << config.dump(2);
    file.close();
    
    std::cout << "Configuration created successfully: " << output_file << std::endl;
    return true;
}

bool ConfigCommand::viewConfiguration(const std::string& input_file) {
    if (input_file.empty()) {
        input_file = "configs/default_config.json";
    }
    
    if (!fileExists(input_file)) {
        std::cerr << "Error: Configuration file not found: " << input_file << std::endl;
        return false;
    }
    
    std::ifstream file(input_file);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open configuration file: " << input_file << std::endl;
        return false;
    }
    
    json config;
    try {
        file >> config;
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid JSON in configuration file: " << e.what() << std::endl;
        return false;
    }
    
    file.close();
    
    std::cout << "Configuration file content:" << std::endl;
    std::cout << config.dump(2) << std::endl;
    
    return true;
}

bool ConfigCommand::validateConfiguration(const std::string& input_file) {
    if (input_file.empty()) {
        input_file = "configs/default_config.json";
    }
    
    if (!fileExists(input_file)) {
        std::cerr << "Error: Configuration file not found: " << input_file << std::endl;
        return false;
    }
    
    std::ifstream file(input_file);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open configuration file: " << input_file << std::endl;
        return false;
    }
    
    json config;
    try {
        file >> config;
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid JSON in configuration file: " << e.what() << std::endl;
        return false;
    }
    
    file.close();
    
    // Basic validation
    json validation_result;
    validation_result["valid"] = true;
    validation_result["errors"] = json::array();
    validation_result["warnings"] = json::array();
    
    if (!config.contains("neuron_count")) {
        validation_result["errors"].push_back("Missing required parameter: neuron_count");
        validation_result["valid"] = false;
    }
    
    if (config.contains("neuron_count")) {
        int neuron_count = config["neuron_count"];
        if (neuron_count < 10) {
            validation_result["warnings"].push_back("Low neuron count may affect results");
        } else if (neuron_count > 10000) {
            validation_result["errors"].push_back("Neuron count too high, may cause performance issues");
            validation_result["valid"] = false;
        }
    }
    
    if (config.contains("simulation_timestep")) {
        double timestep = config["simulation_timestep"];
        if (timestep <= 0.0 || timestep > 1.0) {
            validation_result["errors"].push_back("Invalid simulation timestep");
            validation_result["valid"] = false;
        }
    }
    
    std::cout << "Validation Results:" << std::endl;
    std::cout << validation_result.dump(2) << std::endl;
    
    return validation_result["valid"].get<bool>();
}

bool ConfigCommand::exportConfiguration(const std::string& input_file) {
    if (input_file.empty()) {
        input_file = "configs/default_config.json";
    }
    
    if (!fileExists(input_file)) {
        std::cerr << "Error: Configuration file not found: " << input_file << std::endl;
        return false;
    }
    
    // Create a new file with .txt extension
    std::string txt_file = input_file.substr(0, input_file.find_last_of('.')) + ".txt";
    
    // Simple export to text format
    std::ifstream in_file(input_file);
    std::ofstream out_file(txt_file);
    
    if (!in_file.is_open() || !out_file.is_open()) {
        std::cerr << "Error: Could not open files for export" << std::endl;
        return false;
    }
    
    json config;
    try {
        in_file >> config;
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid JSON in configuration file: " << e.what() << std::endl;
        return false;
    }
    
    in_file.close();
    
    // Export as human-readable text
    out_file << "NLM Configuration Export" << std::endl;
    out_file << "=======================" << std::endl;
    
    if (config.contains("enabled")) {
        out_file << "enabled: " << (config["enabled"].get<bool>() ? "true" : "false") << std::endl;
    }
    if (config.contains("random_seed")) {
        out_file << "random_seed: " << config["random_seed"].get<int64_t>() << std::endl;
    }
    if (config.contains("neuron_count")) {
        out_file << "neuron_count: " << config["neuron_count"].get<int64_t>() << std::endl;
    }
    if (config.contains("region_count")) {
        out_file << "region_count: " << config["region_count"].get<int64_t>() << std::endl;
    }
    if (config.contains("simulation_timestep")) {
        out_file << "simulation_timestep: " << config["simulation_timestep"].get<double>() << std::endl;
    }
    
    out_file.close();
    
    std::cout << "Configuration exported to: " << txt_file << std::endl;
    
    return true;
}

} // namespace cli
} // namespace nlm
