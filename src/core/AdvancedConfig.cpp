#include "AdvancedConfig.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>

namespace nlm {

// CommandLineParser implementation
AdvancedConfig::CommandLineParser::CommandLineParser(int argc, char** argv)
    : argc_(argc), argv_(argv) {
}

void AdvancedConfig::CommandLineParser::addArgument(const std::string& name, 
                                                   bool hasValue,
                                                   const std::string& description,
                                                   bool isRequired) {
    Argument arg;
    arg.name = name;
    arg.value = "";
    arg.description = description;
    arg.hasValue = hasValue;
    arg.isRequired = isRequired;
    arguments_.push_back(arg);
}

bool AdvancedConfig::CommandLineParser::parse() {
    parsedValues_.clear();
    
    for (int i = 1; i < argc_; ++i) {
        std::string arg = argv_[i];
        
        // Check if this argument matches any registered argument
        bool found = false;
        for (auto& argument : arguments_) {
            if (arg == "--" + argument.name) {
                found = true;
                if (argument.hasValue) {
                    if (i + 1 < argc_) {
                        std::string nextArg = argv_[i + 1];
                        // Check if next argument is not a flag (doesn't start with --)
                        if (nextArg.substr(0, 2) != "--") {
                            argument.value = nextArg;
                            parsedValues_[argument.name] = nextArg;
                            i++; // Skip next argument
                        } else {
                            // No value provided, use default or empty
                            argument.value = "";
                        }
                    }
                }
                break;
            }
        }
        
        if (!found) {
            // Unknown argument, could be a value without explicit flag
            // For simplicity, we'll treat it as a value for the first argument that requires one
            bool handled = false;
            for (auto& argument : arguments_) {
                if (argument.hasValue && argument.value.empty()) {
                    argument.value = arg;
                    parsedValues_[argument.name] = arg;
                    handled = true;
                    break;
                }
            }
            if (!handled) {
                // Store unknown argument anyway
                parsedValues_[arg] = "";
            }
        }
    }
    
    return true;
}

std::optional<std::string> AdvancedConfig::CommandLineParser::getArgument(const std::string& name) const {
    auto it = parsedValues_.find(name);
    if (it != parsedValues_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool AdvancedConfig::CommandLineParser::hasArgument(const std::string& name) const {
    return parsedValues_.find(name) != parsedValues_.end();
}

std::string AdvancedConfig::CommandLineParser::getHelpText() const {
    std::stringstream help;
    help << "Command-line arguments:" << std::endl;
    
    for (const auto& arg : arguments_) {
        help << "  --" << arg.name;
        if (arg.hasValue) {
            help << "=<value>";
        }
        help << std::endl;
        if (!arg.description.empty()) {
            help << "    " << arg.description << std::endl;
        }
        if (arg.isRequired) {
            help << "    (required)" << std::endl;
        }
        help << std::endl;
    }
    
    return help.str();
}

// ConfigFileManager implementation
AdvancedConfig::ConfigFileManager::ConfigFileManager() {
}

bool AdvancedConfig::ConfigFileManager::loadFromFile(const std::string& filename) {
    // Check file extension
    std::string ext = "";
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filename.substr(dotPos);
    }
    
    if (ext == ".cfg") {
        // Simple .cfg file format (key=value)
        std::ifstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open config file: " + filename);
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            size_t equalsPos = line.find('=');
            if (equalsPos != std::string::npos) {
                std::string key = line.substr(0, equalsPos);
                std::string value = line.substr(equalsPos + 1);
                
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t\r\n"));
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);
                
                // Store in config (placeholder - would need to integrate with actual config system)
                NLM_LOG_INFO("Loaded config parameter: " + key + " = " + value);
            }
        }
        
        currentFile_ = filename;
        return true;
    }
    
    NLM_LOG_ERROR("Unsupported config file format: " + ext);
    return false;
}

bool AdvancedConfig::ConfigFileManager::saveToFile(const std::string& filename) const {
    // For simplicity, save as .cfg format
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot write config file: " + filename);
        return false;
    }
    
    // Write placeholder configuration
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated by AdvancedConfig System" << std::endl;
    file << std::endl;
    
    // Write sample parameters
    file << "# Neural parameters" << std::endl;
    file << "neuron_count=1000" << std::endl;
    file << "region_count=1" << std::endl;
    file << "connection_probability=0.1" << std::endl;
    file << std::endl;
    
    file << "# Learning parameters" << std::endl;
    file << "stdp_ltp_weight=0.01" << std::endl;
    file << "stdp_ltd_weight=0.012" << std::endl;
    file << std::endl;
    
    file << "# Simulation parameters" << std::endl;
    file << "simulation_timestep=0.001" << std::endl;
    file << "random_seed=42" << std::endl;
    
    return true;
}

bool AdvancedConfig::ConfigFileManager::validateFile(const std::string& filename) const {
    // Basic validation - check if file exists and is readable
    std::ifstream file(filename);
    return file.good();
}

// RuntimeConfigManager implementation
AdvancedConfig::RuntimeConfigManager::RuntimeConfigManager() {
}

bool AdvancedConfig::RuntimeConfigManager::applyOverrides(const std::map<std::string, std::string>& overrides) {
    runtimeOverrides_ = overrides;
    
    // Log applied overrides
    for (const auto& override : overrides) {
        NLM_LOG_INFO("Applied runtime override: " + override.first + " = " + override.second);
    }
    
    return true;
}

std::string AdvancedConfig::RuntimeConfigManager::toString() const {
    std::stringstream ss;
    ss << "Runtime Configuration:" << std::endl;
    
    if (runtimeOverrides_.empty()) {
        ss << "  No runtime overrides applied." << std::endl;
    } else {
        for (const auto& override : runtimeOverrides_) {
            ss << "  " << override.first << " = " << override.second << std::endl;
        }
    }
    
    return ss.str();
}

std::string AdvancedConfig::RuntimeConfigManager::exportToJSON() const {
    // Simple JSON export
    std::stringstream json;
    json << "{" << std::endl;
    json << "  \"runtime_overrides\": {" << std::endl;
    
    bool first = true;
    for (const auto& override : runtimeOverrides_) {
        if (!first) {
            json << "," << std::endl;
        }
        json << "    \"" << override.first << "\": \"" << override.second << "\"";
        first = false;
    }
    
    json << std::endl << "  }" << std::endl;
    json << "}" << std::endl;
    
    return json.str();
}

std::string AdvancedConfig::RuntimeConfigManager::exportToYAML() const {
    // Simple YAML export
    std::stringstream yaml;
    yaml << "runtime_overrides:" << std::endl;
    
    for (const auto& override : runtimeOverrides_) {
        yaml << "  \"" << override.first << "\": \"" << override.second << "\"" << std::endl;
    }
    
    return yaml.str();
}

std::string AdvancedConfig::RuntimeConfigManager::exportToINI() const {
    // Simple INI export
    std::stringstream ini;
    ini << "[runtime_overrides]" << std::endl;
    
    for (const auto& override : runtimeOverrides_) {
        ini << override.first << " = " << override.second << std::endl;
    }
    
    return ini.str();
}

bool AdvancedConfig::RuntimeConfigManager::importFromJSON(const std::string& json) {
    // Simple JSON import (placeholder implementation)
    NLM_LOG_INFO("JSON import not fully implemented in this version");
    return true;
}

bool AdvancedConfig::RuntimeConfigManager::importFromYAML(const std::string& yaml) {
    // Simple YAML import (placeholder implementation)
    NLM_LOG_INFO("YAML import not fully implemented in this version");
    return true;
}

bool AdvancedConfig::RuntimeConfigManager::importFromINI(const std::string& ini) {
    // Simple INI import (placeholder implementation)
    NLM_LOG_INFO("INI import not fully implemented in this version");
    return true;
}

// AdvancedConfigManager implementation
AdvancedConfig::AdvancedConfigManager::AdvancedConfigManager() 
    : initialized_(false) {
}

AdvancedConfig::AdvancedConfigManager::~AdvancedConfigManager() {
}

bool AdvancedConfig::AdvancedConfigManager::initialize() {
    try {
        // Create basic configuration
        basicConfig_ = std::make_shared<Config>();
        
        // Initialize default values
        basicConfig_->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        basicConfig_->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        basicConfig_->set("connection_probability", 0.1f, ConfigSource::Default);
        basicConfig_->set("simulation_timestep", 0.001, ConfigSource::Default);
        basicConfig_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        // Create other managers
        parser_ = std::make_unique<CommandLineParser>(0, nullptr);  // Will be initialized with proper args
        fileManager_ = std::make_unique<ConfigFileManager>();
        runtimeManager_ = std::make_unique<RuntimeConfigManager>();
        
        // Initialize parameter descriptions
        parameters_ = {
            {
                "neuron_count",
                "Number of neurons in the network",
                Category::Neural,
                "int",
                "1000",
                "1000",
                {},
                false,
                nullptr
            },
            {
                "region_count",
                "Number of neural regions",
                Category::Neural,
                "int",
                "1",
                "1",
                {},
                false,
                nullptr
            },
            {
                "connection_probability",
                "Probability of connections between neurons",
                Category::Neural,
                "float",
                "0.1",
                "0.1",
                {},
                false,
                nullptr
            },
            {
                "simulation_timestep",
                "Simulation timestep in seconds",
                Category::Simulation,
                "float",
                "0.001",
                "0.001",
                {},
                false,
                nullptr
            },
            {
                "random_seed",
                "Random seed for reproducible results",
                Category::Simulation,
                "int",
                "42",
                "42",
                {},
                false,
                nullptr
            }
        };
        
        initialized_ = true;
        NLM_LOG_INFO("Advanced configuration manager initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to initialize advanced configuration manager: ") + e.what());
        return false;
    }
}

bool AdvancedConfig::AdvancedConfigManager::loadFromFile(const std::string& filename) {
    if (!initialized_) {
        NLM_LOG_ERROR("Advanced configuration manager not initialized");
        return false;
    }
    
    return fileManager_->loadFromFile(filename);
}

bool AdvancedConfig::AdvancedConfigManager::loadFromCommandLine(int argc, char** argv) {
    if (!initialized_) {
        NLM_LOG_ERROR("Advanced configuration manager not initialized");
        return false;
    }
    
    // Reinitialize parser with actual command-line arguments
    parser_ = std::make_unique<CommandLineParser>(argc, argv);
    
    // Add common command-line arguments
    parser_->addArgument("help", false, "Show help information", false);
    parser_->addArgument("config", true, "Configuration file path", false);
    parser_->addArgument("neurons", true, "Number of neurons (overrides config)", false);
    parser_->addArgument("regions", true, "Number of regions (overrides config)", false);
    parser_->addArgument("seed", true, "Random seed (overrides config)", false);
    parser_->addArgument("verbose", false, "Enable verbose logging", false);
    
    if (!parser_->parse()) {
        NLM_LOG_ERROR("Failed to parse command-line arguments");
        return false;
    }
    
    // Check for help argument
    if (parser_->hasArgument("help")) {
        NLM_LOG_INFO(parser_->getHelpText());
        return false;  // Exit program
    }
    
    // Load configuration file if specified
    if (parser_->hasArgument("config")) {
        auto configFile = parser_->getArgument("config");
        if (configFile.has_value()) {
            return loadFromFile(configFile.value());
        }
    }
    
    // Apply command-line overrides
    std::map<std::string, std::string> overrides;
    if (parser_->hasArgument("neurons")) {
        auto neurons = parser_->getArgument("neurons");
        if (neurons.has_value()) {
            overrides["neuron_count"] = neurons.value();
        }
    }
    
    if (parser_->hasArgument("regions")) {
        auto regions = parser_->getArgument("regions");
        if (regions.has_value()) {
            overrides["region_count"] = regions.value();
        }
    }
    
    if (parser_->hasArgument("seed")) {
        auto seed = parser_->getArgument("seed");
        if (seed.has_value()) {
            overrides["random_seed"] = seed.value();
        }
    }
    
    return applyOverrides(overrides);
}

bool AdvancedConfig::AdvancedConfigManager::applyOverrides(const std::map<std::string, std::string>& overrides) {
    if (!initialized_) {
        NLM_LOG_ERROR("Advanced configuration manager not initialized");
        return false;
    }
    
    return runtimeManager_->applyOverrides(overrides);
}

template<typename T>
T AdvancedConfig::AdvancedConfigManager::getParameter(const std::string& name, const T& defaultValue) const {
    // This is a simplified implementation
    // In a real implementation, this would look up the parameter in the config system
    return defaultValue;
}

template<typename T>
bool AdvancedConfig::AdvancedConfigManager::setParameter(const std::string& name, const T& value) {
    // This is a simplified implementation
    // In a real implementation, this would set the parameter in the config system
    return true;
}

std::string AdvancedConfig::AdvancedConfigManager::getParameterDescription(const std::string& name) const {
    auto it = parameters_.find(name);
    if (it != parameters_.end()) {
        return it->second.description;
    }
    return "Parameter not found.";
}

std::vector<AdvancedConfig::Parameter> AdvancedConfig::AdvancedConfigManager::getParametersByCategory(Category category) const {
    std::vector<Parameter> result;
    
    for (const auto& param : parameters_) {
        if (param.second.category == category) {
            result.push_back(param.second);
        }
    }
    
    return result;
}

std::vector<std::string> AdvancedConfig::AdvancedConfigManager::validateConfiguration() const {
    std::vector<std::string> errors;
    
    // Basic validation checks
    // This would be expanded in a real implementation
    
    if (parameters_.find("neuron_count") == parameters_.end()) {
        errors.push_back("Required parameter 'neuron_count' not found");
    }
    
    return errors;
}

std::string AdvancedConfig::AdvancedConfigManager::getConfigurationSummary() const {
    std::stringstream ss;
    ss << "NLM Advanced Configuration Summary" << std::endl;
    ss << "=================================" << std::endl;
    ss << "Loaded parameters: " << parameters_.size() << std::endl;
    ss << std::endl;
    
    for (const auto& param : parameters_) {
        ss << "  " << param.first << ": " << param.second.currentValue << std::endl;
    }
    
    ss << std::endl;
    ss << runtimeManager_->toString();
    
    return ss.str();
}

bool AdvancedConfig::AdvancedConfigManager::saveConfiguration(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot write configuration file: " + filename);
        return false;
    }
    
    file << getConfigurationSummary();
    return true;
}

std::string AdvancedConfig::AdvancedConfigManager::getHelpText() const {
    std::stringstream help;
    help << "NLM Advanced Configuration Help" << std::endl;
    help << "=============================" << std::endl;
    help << std::endl;
    
    help << "Command-line usage:" << std::endl;
    help << "  ./nlm [options]" << std::endl;
    help << std::endl;
    help << "Options:" << std::endl;
    help << "  --help                    Show this help message" << std::endl;
    help << "  --config=<file>          Load configuration from file" << std::endl;
    help << "  --neurons=<n>            Override number of neurons" << std::endl;
    help << "  --regions=<n>            Override number of regions" << std::endl;
    help << "  --seed=<n>               Override random seed" << std::endl;
    help << "  --verbose                 Enable verbose logging" << std::endl;
    help << std::endl;
    
    help << "Configuration Categories:" << std::endl;
    help << "  Simulation: Basic simulation parameters" << std::endl;
    help << "  Neural: Neural network parameters" << std::endl;
    help << "  Learning: Learning and plasticity parameters" << std::endl;
    help << "  Agent: Agent behavior parameters" << std::endl;
    help << "  Visualization: Visualization parameters" << std::endl;
    help << "  Performance: Performance tuning parameters" << std::endl;
    help << "  Debug: Debugging parameters" << std::endl;
    help << std::endl;
    
    return help.str();
}

void AdvancedConfig::AdvancedConfigManager::resetToDefaults() {
    // Reset to default values
    basicConfig_->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    basicConfig_->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    basicConfig_->set("connection_probability", 0.1f, ConfigSource::Default);
    basicConfig_->set("simulation_timestep", 0.001, ConfigSource::Default);
    basicConfig_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    
    runtimeManager_->applyOverrides({});
    
    NLM_LOG_INFO("Configuration reset to defaults");
}

bool AdvancedConfig::AdvancedConfigManager::exportConfiguration(const std::string& filename, 
                                                             const std::string& format) const {
    // Determine format from filename if not specified
    std::string actualFormat = format;
    if (actualFormat == "auto") {
        size_t dotPos = filename.find_last_of('.');
        if (dotPos != std::string::npos) {
            actualFormat = filename.substr(dotPos);
        } else {
            actualFormat = ".txt";
        }
    }
    
    if (actualFormat == ".json") {
        // Export to JSON (placeholder implementation)
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        file << "{\"configuration\": \"advanced config system not fully implemented\"}";
        return true;
    } else if (actualFormat == ".yaml" || actualFormat == ".yml") {
        // Export to YAML (placeholder implementation)
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        file << "configuration: advanced config system not fully implemented";
        return true;
    } else if (actualFormat == ".ini") {
        // Export to INI
        return saveConfiguration(filename + ".ini");
    }
    
    // Default: save as text
    return saveConfiguration(filename + ".txt");
}

bool AdvancedConfig::AdvancedConfigManager::importConfiguration(const std::string& filename) {
    // Import from file
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos) {
        std::string ext = filename.substr(dotPos);
        if (ext == ".cfg") {
            return loadFromFile(filename);
        }
    }
    
    NLM_LOG_ERROR("Unsupported configuration file format: " + ext);
    return false;
}

} // namespace nlm