// Include headers
#include "ConfigurationManager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <numeric>
#include <cctype>

namespace nlm {
    std::shared_ptr<Config> config;
    std::unordered_map<std::string, ParameterInfo> parameterDefinitions;
    std::string configFile;
    std::vector<std::string> validationErrors;
    bool helpRequested;
    bool versionRequested;
    
    Impl() : config(std::make_shared<Config>()), helpRequested(false), versionRequested(false) {
        // Add default parameters with descriptions
        addDefaultParameters();
    }
    
    void addDefaultParameters() {
        // Core simulation parameters
        addParameter("random_seed", ConfigValue(static_cast<int64_t>(42)), 
                    "Random seed for reproducible experiments", ConfigSource::Default);
        
        addParameter("simulation_timestep", ConfigValue(0.001), 
                    "Simulation timestep in seconds", ConfigSource::Default, 1e-6, 0.1);
        
        addParameter("neuron_count", ConfigValue(static_cast<int64_t>(500)), 
                    "Number of neurons in the brain (smaller for faster testing)", ConfigSource::Default, 100, 10000);
        
        addParameter("region_count", ConfigValue(static_cast<int64_t>(1)), 
                    "Number of brain regions", ConfigSource::Default, 1, 10);
        
        addParameter("connection_probability", ConfigValue(0.15f), 
                    "Connection probability for random connectivity", ConfigSource::Default, 0.01f, 1.0f);
        
        // Plasticity parameters
        addParameter("stdp_ltp_weight", ConfigValue(0.02f), 
                    "STDP long-term potentiation weight", ConfigSource::Default, 0.001f, 0.1f);
        
        addParameter("stdp_ltd_weight", ConfigValue(0.015f), 
                    "STDP long-term depression weight", ConfigSource::Default, 0.001f, 0.1f);
        
        addParameter("stdp_tau", ConfigValue(20.0f), 
                    "STDP time constant", ConfigSource::Default, 1.0f, 100.0f);
        
        // Structural plasticity parameters
        addParameter("synaptogenesis_rate", ConfigValue(0.0001f), 
                    "Structural synaptogenesis rate", ConfigSource::Default, 1e-6f, 0.001f);
        
        addParameter("pruning_rate", ConfigValue(0.00001f), 
                    "Structural pruning rate", ConfigSource::Default, 1e-7f, 0.001f);
        
        // Experiment-specific parameters
        addParameter("max_simulation_steps", ConfigValue(1000), 
                    "Maximum simulation steps per experiment", ConfigSource::Default, 10, 100000);
        
        addParameter("learning_threshold", ConfigValue(0.01f), 
                    "Weight change threshold for learning detection", ConfigSource::Default, 1e-4f, 0.1f);
        
        addParameter("spike_threshold", ConfigValue(1.0f), 
                    "Neuron membrane potential threshold for spiking", ConfigSource::Default, 0.0f, 10.0f);
        
        // Input/output parameters
        addParameter("input_strength", ConfigValue(50.0f), 
                    "Strength of injected current for input stimulation", ConfigSource::Default, 0.0f, 100.0f);
        
        addParameter("output_neurons", ConfigValue(10), 
                    "Number of output neurons to stimulate", ConfigSource::Default, 1, 100);
        
        // Logging parameters
        addParameter("log_level", ConfigValue(static_cast<int>(2)), 
                    "Log level (0=Error, 1=Warning, 2=Info, 3=Debug)", ConfigSource::Default, 0, 3);
        
        addParameter("verbose", ConfigValue(false), 
                    "Enable verbose logging", ConfigSource::Default);
        
        addParameter("log_filename", ConfigValue("nlm.log"), 
                    "Filename for log output (empty for console only)", ConfigSource::Default);
    }
};

ConfigurationManager::ConfigurationManager() : pImpl(std::make_unique<Impl>()) {}

ConfigurationManager::~ConfigurationManager() = default;

ConfigurationManager::ConfigurationManager(ConfigurationManager&&) noexcept = default;

ConfigurationManager& ConfigurationManager::operator=(ConfigurationManager&&) noexcept = default;

bool ConfigurationManager::loadFromFile(const std::string& filepath) {
    if (!pImpl->config->loadFromFile(filepath)) {
        NLM_LOG_WARNING("Failed to load configuration from file: " + filepath);
        return false;
    }
    pImpl->configFile = filepath;
    NLM_LOG_INFO("Loaded configuration from file: " + filepath);
    return true;
}

bool ConfigurationManager::loadFromCommandLine(int argc, char** argv) {
    bool hasValidArgs = pImpl->config->loadFromArgs(argc, argv);
    
    if (!hasValidArgs) {
        NLM_LOG_WARNING("No valid command-line arguments found");
    }
    
    // Check for special flags
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            pImpl->helpRequested = true;
        } else if (arg == "--version" || arg == "-v") {
            pImpl->versionRequested = true;
        }
    }
    
    return hasValidArgs;
}

template<typename T>
T ConfigurationManager::getOr(const std::string& key, const T& defaultValue) const {
    auto configValue = pImpl->config->getOr<T>(key, defaultValue);
    
    // Get parameter info for validation
    auto it = pImpl->parameterDefinitions.find(key);
    if (it != pImpl->parameterDefinitions.end()) {
        const auto& info = it->second;
        if (!pImpl->config->has(key)) {
            NLM_LOG_INFO("Using default for " + key + ": " + std::to_string(defaultValue));
        }
    }
    
    return configValue;
}

bool ConfigurationManager::validate() const {
    bool isValid = true;
    std::vector<std::string> errors;
    
    for (const auto& [name, info] : pImpl->parameterDefinitions) {
        if (info.isRequired && !pImpl->config->has(name)) {
            std::string error = "Required parameter missing: " + name;
            errors.push_back(error);
            NLM_LOG_ERROR(error);
            isValid = false;
        }
        
        if (pImpl->config->has(name)) {
            auto configValue = pImpl->config->get(name);
            if (configValue.has_value()) {
                if (!validateParameterValue(name, configValue.value(), info, errors)) {
                    std::string error = "Invalid value for parameter: " + name;
                    errors.push_back(error);
                    NLM_LOG_ERROR(error);
                    isValid = false;
                }
            }
        }
    }
    
    // Store errors in pImpl for later retrieval
    pImpl->validationErrors = errors;
    
    return isValid;
}

void ConfigurationManager::printSummary() const {
    std::cout << "Configuration Summary:" << std::endl;
    std::cout << "=====================" << std::endl;
    
    auto keys = pImpl->config->getKeys();
    std::sort(keys.begin(), keys.end());
    
    for (const auto& key : keys) {
        auto configValue = pImpl->config->get(key);
        if (configValue.has_value()) {
            std::cout << "  " << std::left << std::setw(25) << key << " = ";
            
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    std::cout << "\"" << arg << "\"";
                } else {
                    std::cout << arg;
                }
            }, configValue.value());
            
            std::cout << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void ConfigurationManager::printHelp() {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Usage: ./nlm [--config <file>] [options]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --version, -v           Show version information" << std::endl;
    std::cout << "  --config <file>         Load configuration from file" << std::endl;
    std::cout << "  --key value             Set configuration parameter" << std::endl;
    std::cout << "  -k value                Short form of --key" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Configuration Parameters:" << std::endl;
    std::cout << "------------------------" << std::endl;
    
    // Print parameter definitions
    for (const auto& [name, info] : ConfigurationManager().pImpl->parameterDefinitions) {
        std::cout << "  " << std::left << std::setw(20) << name << " - " << info.description;
        
        if (info.isRequired) {
            std::cout << " (required)";
        }
        
        if (info.minValue != std::numeric_limits<double>::lowest()) {
            std::cout << " [min=" << info.minValue << "]";
        }
        
        if (info.maxValue != std::numeric_limits<double>::max()) {
            std::cout << " [max=" << info.maxValue << "]";
        }
        
        std::cout << std::endl;
    }
}

void ConfigurationManager::addParameter(const std::string& name, const ConfigValue& defaultValue,
                                      const std::string& description, ConfigSource source,
                                      double minValue, double maxValue, bool isRequired) {
    ParameterInfo info;
    info.name = name;
    info.defaultValue = defaultValue;
    info.description = description;
    info.source = source;
    info.minValue = minValue;
    info.maxValue = maxValue;
    info.isRequired = isRequired;
    
    pImpl->parameterDefinitions[name] = info;
    
    // Set default value if not already set
    if (!pImpl->config->has(name)) {
        pImpl->config->set(name, defaultValue, ConfigSource::Default);
    }
}

std::shared_ptr<Config> ConfigurationManager::getConfig() const {
    return pImpl->config;
}

ParameterType ConfigurationManager::getParameterType(const std::string& name) const {
    return inferParameterType(pImpl->config->getOr<ConfigValue>(name, ConfigValue(std::string())));
}

bool ConfigurationManager::hasParameter(const std::string& name) const {
    return pImpl->parameterDefinitions.find(name) != pImpl->parameterDefinitions.end();
}

ParameterType ConfigurationManager::inferParameterType(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return ParameterType::Integer;
    if (std::holds_alternative<int64_t>(value)) return ParameterType::Integer64;
    if (std::holds_alternative<double>(value)) return ParameterType::Float;
    if (std::holds_alternative<bool>(value)) return ParameterType::Boolean;
    if (std::holds_alternative<std::string>(value)) return ParameterType::String;
    if (std::holds_alternative<std::vector<int>>(value)) return ParameterType::VectorInt;
    if (std::holds_alternative<std::vector<double>>(value)) return ParameterType::VectorDouble;
    if (std::holds_alternative<std::vector<std::string>>(value)) return ParameterType::VectorString;
    
    return ParameterType::String; // Default
}

bool ConfigurationManager::validateParameterValue(const std::string& name, const ConfigValue& value,
                                                 const ParameterInfo& info) {
    // Type validation
    ParameterType valueType = inferParameterType(value);
    ParameterType expectedType = inferParameterType(info.defaultValue);
    
    if (valueType != expectedType) {
        return false;
    }
    
    // Range validation (for numeric types)
    if (expectedType == ParameterType::Integer || expectedType == ParameterType::Integer64 ||
        expectedType == ParameterType::Float) {
        
        double val = 0.0;
        if (std::holds_alternative<int>(value)) val = static_cast<double>(std::get<int>(value));
        else if (std::holds_alternative<int64_t>(value)) val = static_cast<double>(std::get<int64_t>(value));
        else if (std::holds_alternative<double>(value)) val = std::get<double>(value);
        
        if (val < info.minValue || val > info.maxValue) {
            return false;
        }
    }
    
    return true;
}

bool ConfigurationManager::validateParameterValue(const std::string& name, const ConfigValue& value,
                                                 const ParameterInfo& info, std::vector<std::string>& errors) {
    // Type validation
    ParameterType valueType = inferParameterType(value);
    ParameterType expectedType = inferParameterType(info.defaultValue);
    
    if (valueType != expectedType) {
        errors.push_back("Type mismatch for parameter " + name + ": expected " + 
                        typeToString(expectedType) + ", got " + typeToString(valueType));
        return false;
    }
    
    // Range validation (for numeric types)
    if (expectedType == ParameterType::Integer || expectedType == ParameterType::Integer64 ||
        expectedType == ParameterType::Float) {
        
        double val = 0.0;
        if (std::holds_alternative<int>(value)) val = static_cast<double>(std::get<int>(value));
        else if (std::holds_alternative<int64_t>(value)) val = static_cast<double>(std::get<int64_t>(value));
        else if (std::holds_alternative<double>(value)) val = std::get<double>(value);
        
        if (val < info.minValue || val > info.maxValue) {
            errors.push_back("Value out of range for parameter " + name + ": " + 
                           std::to_string(val) + " not in [" + std::to_string(info.minValue) + ", " +
                           std::to_string(info.maxValue) + "]");
            return false;
        }
    }
    
    return true;
}

std::string ConfigurationManager::typeToString(ParameterType type) {
    switch (type) {
        case ParameterType::Integer: return "int";
        case ParameterType::Integer64: return "int64_t";
        case ParameterType::Float: return "float/double";
        case ParameterType::Boolean: return "bool";
        case ParameterType::String: return "string";
        case ParameterType::VectorInt: return "vector<int>";
        case ParameterType::VectorDouble: return "vector<double>";
        case ParameterType::VectorString: return "vector<string>";
        default: return "unknown";
    }
}

std::vector<std::string> ConfigurationManager::getValidationErrors() const {
    return pImpl->validationErrors;
}

void ConfigurationManager::printValidationErrors() const {
    if (!pImpl->validationErrors.empty()) {
        std::cout << "Configuration Validation Errors:" << std::endl;
        std::cout << "================================" << std::endl;
        for (const auto& error : pImpl->validationErrors) {
            std::cout << "  - " << error << std::endl;
        }
        std::cout << std::endl;
    } else {
        std::cout << "No validation errors found." << std::endl;
        std::cout << std::endl;
    }
}

} // namespace nlm