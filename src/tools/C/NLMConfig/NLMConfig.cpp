#include "NLMConfig.hpp"
#include <cstring>
#include <regex>
#include <sstream>

namespace nlm {
namespace tools {

int NLMConfig::run(int argc, char* argv[]) {
    // Simple argument parsing
    std::string configFile;
    bool validate = false;
    std::string generatePath;
    std::string comparePath;
    bool analyze = false;
    std::string exportJSON;
    std::string exportYAML;
    std::string exportSimple;
    bool list = false;
    bool help = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            help = true;
        } else if (arg == "--validate" || arg == "-v") {
            validate = true;
        } else if (arg == "--generate" || arg == "-g") {
            if (i + 1 < argc) {
                generatePath = argv[++i];
            }
        } else if (arg == "--compare" || arg == "-c") {
            if (i + 1 < argc) {
                comparePath = argv[++i];
            }
        } else if (arg == "--analyze" || arg == "-a") {
            analyze = true;
        } else if (arg == "--export-json") {
            if (i + 1 < argc) {
                exportJSON = argv[++i];
            }
        } else if (arg == "--export-yaml") {
            if (i + 1 < argc) {
                exportYAML = argv[++i];
            }
        } else if (arg == "--export-simple") {
            if (i + 1 < argc) {
                exportSimple = argv[++i];
            }
        } else if (arg == "--list") {
            list = true;
        } else if (!configFile.empty() && configFile == "" && arg.substr(0, 2) != "--") {
            configFile = arg;
        } else if (configFile.empty() && arg.substr(0, 2) != "--") {
            configFile = arg;
        }
    }
    
    if (help) {
        printHelp();
        return 0;
    }
    
    // Load configuration if provided
    std::map<std::string, std::string> config;
    if (!configFile.empty()) {
        if (!loadConfig(configFile, config)) {
            std::cerr << "Error: Failed to load configuration file: " << configFile << std::endl;
            return 2;
        }
        std::cout << "Loaded configuration from: " << configFile << std::endl;
    }
    
    // Generate configuration
    if (!generatePath.empty()) {
        auto generatedConfig = generateConfig(generatePath);
        saveConfig(generatedConfig, generatePath);
        std::cout << "Generated configuration saved to: " << generatePath << std::endl;
    }
    
    // Validate configuration
    if (validate && !config.empty()) {
        if (!validateConfig(config)) {
            return 1;
        }
    }
    
    // Compare configurations
    if (!comparePath.empty() && !config.empty()) {
        std::map<std::string, std::string> compareConfig;
        if (loadConfig(comparePath, compareConfig)) {
            compareConfigs(config, compareConfig);
        }
    }
    
    // Analyze parameters
    if (analyze && !config.empty()) {
        analyzeParameters(config);
    }
    
    // Export configurations
    if (!config.empty()) {
        if (!exportJSON.empty()) {
            saveConfig(config, exportJSON);
            std::cout << "Configuration exported to JSON: " << exportJSON << std::endl;
        }
        
        if (!exportYAML.empty()) {
            saveConfig(config, exportYAML);
            std::cout << "Configuration exported to YAML: " << exportYAML << std::endl;
        }
        
        if (!exportSimple.empty()) {
            saveConfig(config, exportSimple);
            std::cout << "Configuration exported to simple format: " << exportSimple << std::endl;
        }
        
        if (list) {
            std::cout << "=== Configuration Contents ===" << std::endl;
            for (const auto& pair : config) {
                std::cout << "  " << pair.first << ": " << pair.second << std::endl;
            }
        }
    }
    
    return 0;
}

bool NLMConfig::loadConfig(const std::string& filepath, std::map<std::string, std::string>& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Detect file format
    if (isJSONFile(content)) {
        // Parse JSON (simplified)
        std::istringstream iss(content);
        std::string key, value;
        while (iss >> key) {
            if (iss.get() == '=') {
                std::getline(iss, value);
                value = trim(value);
                config[key] = value;
            }
        }
    } else if (isYAMLFile(content)) {
        // Parse YAML (simplified - treat as key=value)
        std::istringstream iss(content);
        std::string line;
        while (std::getline(iss, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') {
                continue;
            }
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                config[key] = value;
            }
        }
    } else {
        // Parse simple key=value format
        std::istringstream iss(content);
        std::string line;
        while (std::getline(iss, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') {
                continue;
            }
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                config[key] = value;
            }
        }
    }
    
    return true;
}

bool NLMConfig::saveConfig(const std::map<std::string, std::string>& config, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& pair : config) {
        file << pair.first << " = " << pair.second << std::endl;
    }
    
    file.close();
    return true;
}

bool NLMConfig::validateConfig(const std::map<std::string, std::string>& config) {
    std::cout << "Validating configuration..." << std::endl;
    
    // Basic validation
    if (config.empty()) {
        std::cout << "Warning: Configuration is empty" << std::endl;
    }
    
    // Check for critical parameters
    std::vector<std::string> criticalParams = {
        "neuron_count",
        "region_count",
        "connection_probability",
        "plasticity_learning_rate"
    };
    
    bool valid = true;
    for (const auto& param : criticalParams) {
        if (config.find(param) == config.end()) {
            std::cout << "Warning: Critical parameter not found: " << param << std::endl;
        }
    }
    
    // Validate parameter values
    for (const auto& pair : config) {
        const std::string& key = pair.first;
        const std::string& value = pair.second;
        
        try {
            if (key == "neuron_count" || key == "region_count") {
                int intValue = std::stoi(value);
                if (intValue <= 0) {
                    std::cerr << "Error: " << key << " must be positive" << std::endl;
                    valid = false;
                }
            } else if (key == "connection_probability") {
                double doubleValue = std::stod(value);
                if (doubleValue < 0.0 || doubleValue > 1.0) {
                    std::cerr << "Error: " << key << " must be between 0.0 and 1.0" << std::endl;
                    valid = false;
                }
            } else if (key == "plasticity_learning_rate") {
                double doubleValue = std::stod(value);
                if (doubleValue < 0.0) {
                    std::cerr << "Error: " << key << " must be non-negative" << std::endl;
                    valid = false;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid value for " << key << ": " << e.what() << std::endl;
            valid = false;
        }
    }
    
    if (valid) {
        std::cout << "Configuration validation PASSED" << std::endl;
    } else {
        std::cout << "Configuration validation FAILED" << std::endl;
    }
    
    return valid;
}

std::map<std::string, std::string> NLMConfig::generateConfig(const std::string& templatePath) {
    std::map<std::string, std::string> config;
    
    if (!templatePath.empty() && std::filesystem::exists(templatePath)) {
        // Load from template
        loadConfig(templatePath, config);
        std::cout << "Generated configuration from template: " << templatePath << std::endl;
    } else {
        // Default configuration
        config = {
            {"random_seed", "42"},
            {"simulation_timestep", "0.001"},
            {"neuron_count", "1000"},
            {"region_count", "2"},
            {"connection_probability", "0.1"},
            {"plasticity_learning_rate", "0.01"},
            {"stdp_ltp_weight", "0.01"},
            {"stdp_ltd_weight", "0.012"},
            {"stdp_time_constant", "20.0"},
            {"development_synaptogenesis_rate", "0.001"},
            {"development_pruning_rate", "0.0001"},
            {"dopamine_baseline", "0.1"},
            {"reward_discount_factor", "0.99"},
            {"environment_name", "GridWorld"},
            {"environment_width", "10"},
            {"environment_height", "10"},
            {"log_level", "INFO"},
            {"log_to_file", "false"},
            {"log_filename", "nlm.log"},
            {"max_simulation_steps", "10000"},
            {"simulation_time_limit", "0.0"},
            {"visualization_enabled", "false"},
            {"visualization_update_rate", "30.0"}
        };
        std::cout << "Generated default NLM configuration template" << std::endl;
    }
    
    return config;
}

void NLMConfig::compareConfigs(const std::map<std::string, std::string>& config1, const std::map<std::string, std::string>& config2) {
    std::cout << "=== Configuration Comparison ===" << std::endl;
    
    std::set<std::string> keys1, keys2;
    for (const auto& pair : config1) keys1.insert(pair.first);
    for (const auto& pair : config2) keys2.insert(pair.first);
    
    std::set<std::string> onlyIn1, onlyIn2, common;
    std::set_intersection(keys1.begin(), keys1.end(), keys2.begin(), keys2.end(), std::inserter(common, common.begin()));
    std::set_difference(keys1.begin(), keys1.end(), keys2.begin(), keys2.end(), std::inserter(onlyIn1, onlyIn1.begin()));
    std::set_difference(keys2.begin(), keys2.end(), keys1.begin(), keys1.end(), std::inserter(onlyIn2, onlyIn2.begin()));
    
    std::cout << "Keys only in config1: " << onlyIn1.size() << std::endl;
    for (const auto& key : onlyIn1) std::cout << "  " << key << std::endl;
    
    std::cout << "Keys only in config2: " << onlyIn2.size() << std::endl;
    for (const auto& key : onlyIn2) std::cout << "  " << key << std::endl;
    
    std::cout << "Common keys: " << common.size() << std::endl;
    
    std::vector<std::pair<std::string, std::string, std::string>> differences;
    for (const auto& key : common) {
        if (config1.at(key) != config2.at(key)) {
            differences.emplace_back(key, config1.at(key), config2.at(key));
        }
    }
    
    std::cout << "Value differences: " << differences.size() << std::endl;
    for (const auto& diff : differences) {
        std::cout << "  " << diff.first << ":" << std::endl;
        std::cout << "    Config1: " << diff.second << std::endl;
        std::cout << "    Config2: " << diff.third << std::endl;
    }
    
    if (onlyIn1.empty() && onlyIn2.empty() && differences.empty()) {
        std::cout << "\n✓ Configurations are identical" << std::endl;
    } else {
        std::cout << "\n✗ Configurations differ" << std::endl;
    }
}

void NLMConfig::analyzeParameters(const std::map<std::string, std::string>& config) {
    std::cout << "=== Parameter Sensitivity Analysis ===" << std::endl;
    
    std::cout << "Analysis of critical parameters:" << std::endl;
    
    std::vector<std::string> criticalParams = {
        "neuron_count", "region_count", "connection_probability", 
        "plasticity_learning_rate", "max_simulation_steps", "simulation_timestep"
    };
    
    for (const auto& param : criticalParams) {
        auto it = config.find(param);
        if (it != config.end()) {
            std::string info = getParameterInfo(param, it->second);
            std::cout << "  " << param << ": " << it->second << " - " << info << std::endl;
        } else {
            std::cout << "  " << param << ": [NOT SET] - Critical parameter" << std::endl;
        }
    }
    
    std::cout << "\nRecommendations:" << std::endl;
    std::cout << "  - Higher neuron_count improves capacity but increases computation time" << std::endl;
    std::cout << "  - Optimal region_count: 1-4 for most simulations" << std::endl;
    std::cout << "  - connection_probability around 0.1-0.2 for balanced connectivity" << std::endl;
    std::cout << "  - plasticity_learning_rate affects learning speed and stability" << std::endl;
    std::cout << "  - max_simulation_steps should balance training and computation time" << std::endl;
}

void NLMConfig::printHelp() {
    std::cout << "NLMConfig - Configuration Management Tool" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "USAGE: nlmconfig [OPTIONS] [CONFIG_FILE]" << std::endl;
    std::cout << std::endl;
    std::cout << "DESCRIPTION:" << std::endl;
    std::cout << "  Advanced configuration management for NLM (Neural Learning Machine)." << std::endl;
    std::cout << "  Supports validation, generation, comparison, and analysis of configurations." << std::endl;
    std::cout << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "  --validate, -v           Validate configuration file" << std::endl;
    std::cout << "  --generate, -g PATH      Generate configuration from template" << std::endl;
    std::cout << "  --compare, -c FILE       Compare current config with another config" << std::endl;
    std::cout << "  --analyze, -a            Perform parameter sensitivity analysis" << std::endl;
    std::cout << "  --export-json FILE       Export configuration as JSON" << std::endl;
    std::cout << "  --export-yaml FILE       Export configuration as YAML" << std::endl;
    std::cout << "  --export-simple FILE     Export configuration as simple key=value format" << std::endl;
    std::cout << "  --list                   List all configuration keys and values" << std::endl;
    std::cout << "  --help, -h               Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  nlmconfig --validate configs/default.cfg" << std::endl;
    std::cout << "  nlmconfig --generate my_config.cfg" << std::endl;
    std::cout << "  nlmconfig --compare configs/default.cfg my_config.cfg" << std::endl;
    std::cout << "  nlmconfig --analyze configs/default.cfg" << std::endl;
    std::cout << "  nlmconfig --export-json config.json configs/default.cfg" << std::endl;
    std::cout << std::endl;
    std::cout << "CONFIGURATION FILES:" << std::endl;
    std::cout << "  Supported formats: Simple .cfg files (key=value), JSON, YAML" << std::endl;
}

std::string NLMConfig::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

bool NLMConfig::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string NLMConfig::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string NLMConfig::getFileExtension(const std::string& filename) {
    size_t pos = filename.rfind('.');
    if (pos != std::string::npos) {
        return filename.substr(pos + 1);
    }
    return "";
}

std::string NLMConfig::getParameterInfo(const std::string& param, const std::string& value) {
    std::map<std::string, std::string> infoMap = {
        {"neuron_count", "Controls network size and capacity"},
        {"region_count", "Number of brain regions affecting modular processing"},
        {"connection_probability", "Controls sparsity of connectivity"},
        {"plasticity_learning_rate", "Controls rate of synaptic adaptation"},
        {"max_simulation_steps", "Controls simulation duration"},
        {"simulation_timestep", "Controls temporal resolution"},
        {"random_seed", "Ensures reproducible results"},
        {"development_synaptogenesis_rate", "Controls structural development"},
        {"development_pruning_rate", "Controls synaptic pruning"}
    };
    
    auto it = infoMap.find(param);
    if (it != infoMap.end()) {
        return it->second;
    }
    return "General configuration parameter";
}

bool NLMConfig::isJSONFile(const std::string& content) {
    return content.find('{') != std::string::npos && content.find('}') != std::string::npos;
}

bool NLMConfig::isYAMLFile(const std::string& content) {
    return content.find(":") != std::string::npos && (content.find(" ") != std::string::npos || content.find("\t") != std::string::npos);
}

bool NLMConfig::isSimpleFormat(const std::string& content) {
    return content.find('=') != std::string::npos;
}

} // namespace tools
} // namespace nlm
