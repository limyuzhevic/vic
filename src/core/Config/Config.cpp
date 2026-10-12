#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <limits>
#include <chrono>
#include <numeric>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ParameterInfo> parameterSchema;
    ConfigStats stats;
    std::chrono::system_clock::time_point sessionTimestamp;
    
    Impl() : sessionTimestamp(std::chrono::system_clock::now()) {}
};

// Helper functions
namespace {
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    std::string toString(const ConfigValue& value) {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, value);
        return oss.str();
    }
    
    bool isTypeValid(const ConfigValue& value, const std::string& type) {
        if (type == "int") return std::holds_alternative<int>(value);
        if (type == "int64_t") return std::holds_alternative<int64_t>(value);
        if (type == "double") return std::holds_alternative<double>(value);
        if (type == "bool") return std::holds_alternative<bool>(value);
        if (type == "string") return std::holds_alternative<std::string>(value);
        if (type == "vector<int>") return std::holds_alternative<std::vector<int>>(value);
        if (type == "vector<double>") return std::holds_alternative<std::vector<double>>(value);
        if (type == "vector<string>") return std::holds_alternative<std::vector<std::string>>(value);
        return false;
    }
    
    // Initialize default schema
    void initializeDefaultSchema(Config::Impl* impl) {
        // Basic simulation parameters
        impl->parameterSchema["random_seed"] = ParameterInfo(
            "random_seed", "Random seed for reproducible simulations", 42,
            "int", "", 0, std::numeric_limits<int>::max(), false
        );
        
        impl->parameterSchema["simulation_timestep"] = ParameterInfo(
            "simulation_timestep", "Time step size in seconds", 0.001,
            "double", "seconds", 1e-6, 1.0, false
        );
        
        // Network parameters
        impl->parameterSchema["neuron_count"] = ParameterInfo(
            "neuron_count", "Total number of neurons in the network", 500,
            "int", "", 1, 10000, false
        );
        
        impl->parameterSchema["region_count"] = ParameterInfo(
            "region_count", "Number of brain regions", 1,
            "int", "", 1, 100, false
        );
        
        impl->parameterSchema["connection_probability"] = ParameterInfo(
            "connection_probability", "Probability of connection between neurons", 0.15,
            "double", "", 0.0, 1.0, false
        );
        
        // Plasticity parameters
        impl->parameterSchema["stdp_ltp_weight"] = ParameterInfo(
            "stdp_ltp_weight", "STDP Long-Term Potentiation weight", 0.02,
            "double", "", 0.0, 1.0, false
        );
        
        impl->parameterSchema["stdp_ltd_weight"] = ParameterInfo(
            "stdp_ltd_weight", "STDP Long-Term Depression weight", 0.015,
            "double", "", 0.0, 1.0, false
        );
        
        impl->parameterSchema["stdp_tau"] = ParameterInfo(
            "stdp_tau", "STDP time constant", 20.0,
            "double", "ms", 1.0, 1000.0, false
        );
        
        // Structural plasticity
        impl->parameterSchema["synaptogenesis_rate"] = ParameterInfo(
            "synaptogenesis_rate", "Rate of new synapse formation", 0.0001,
            "double", "", 0.0, 1.0, false
        );
        
        impl->parameterSchema["pruning_rate"] = ParameterInfo(
            "pruning_rate", "Rate of synapse pruning", 0.00001,
            "double", "", 0.0, 1.0, false
        );
        
        // Development parameters
        impl->parameterSchema["developmental_stage"] = ParameterInfo(
            "developmental_stage", "Current developmental stage", std::string("Adult"),
            "string", "", 0.0, 0.0, false
        );
        
        impl->parameterSchema["critical_period_end"] = ParameterInfo(
            "critical_period_end", "End time of critical period (seconds)", 100.0,
            "double", "seconds", 0.0, 1000.0, false
        );
        
        // Learning parameters
        impl->parameterSchema["learning_rate"] = ParameterInfo(
            "learning_rate", "General learning rate", 0.1,
            "double", "", 0.0, 1.0, false
        );
        
        impl->parameterSchema["reward_scaling"] = ParameterInfo(
            "reward_scaling", "Reward signal scaling factor", 1.0,
            "double", "", 0.0, 10.0, false
        );
        
        // Neuromodulation parameters
        impl->parameterSchema["dopamine_level"] = ParameterInfo(
            "dopamine_level", "Dopamine neuromodulator level", 0.5,
            "double", "", 0.0, 1.0, false
        );
        
        impl->parameterSchema["curiosity_level"] = ParameterInfo(
            "curiosity_level", "Curiosity neuromodulator level", 0.3,
            "double", "", 0.0, 1.0, false
        );
        
        impl->parameterSchema["novelty_threshold"] = ParameterInfo(
            "novelty_threshold", "Novelty detection threshold", 0.1,
            "double", "", 0.0, 1.0, false
        );
        
        // Memory parameters
        impl->parameterSchema["working_memory_capacity"] = ParameterInfo(
            "working_memory_capacity", "Working memory capacity", 100,
            "int", "items", 1, 1000, false
        );
        
        impl->parameterSchema["episodic_memory_size"] = ParameterInfo(
            "episodic_memory_size", "Episodic memory size (episodes)", 50,
            "int", "", 1, 1000, false
        );
        
        // Performance parameters
        impl->parameterSchema["max_energy"] = ParameterInfo(
            "max_energy", "Maximum agent energy", 100.0,
            "double", "energy", 0.0, 1000.0, false
        );
        
        impl->parameterSchema["energy_decay_rate"] = ParameterInfo(
            "energy_decay_rate", "Energy decay rate per second", 0.01,
            "double", "1/s", 0.0, 1.0, false
        );
        
        // Visualization parameters
        impl->parameterSchema["enable_visualization"] = ParameterInfo(
            "enable_visualization", "Enable visualization output", false,
            "bool", "", 0.0, 0.0, false
        );
        
        impl->parameterSchema["visualization_fps"] = ParameterInfo(
            "visualization_fps", "Visualization frames per second", 30,
            "int", "Hz", 1, 1000, false
        );
    }
}}

Config::Config() : pImpl(std::make_unique<Impl>()) {
    initializeDefaultSchema(pImpl.get());
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << toString(entry.value) << "\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

bool Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    return setWithValidation(key, value, source);
}

bool Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    return setWithValidation(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, int value, ConfigSource source) {
    return setWithValidation(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, double value, ConfigSource source) {
    return setWithValidation(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, bool value, ConfigSource source) {
    return setWithValidation(key, ConfigValue(value), source);
}

template<typename T>
bool Config::set(const std::string& key, const T& value, ConfigSource source) {
    return setWithValidation(key, ConfigValue(value), source);
}

bool Config::setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Check parameter schema if it exists
    auto paramInfo = findParameterInfo(key);
    if (paramInfo) {
        // Type validation
        std::string type = paramInfo->type;
        if (type == "auto") {
            type = inferType(value);
        }
        if (!isTypeValid(value, type)) {
            std::cerr << "Error: Value type mismatch for parameter \"" << key << "\". "
                      << "Expected type: " << type << ", got type " << typeid(value).name() << std::endl;
            return false;
        }
        
        // Range validation
        if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (val < paramInfo->min || val > paramInfo->max) {
                std::cerr << "Error: Value out of range for parameter \"" << key << "\": "
                          << val << " (valid range: [" << paramInfo->min << ", " << paramInfo->max << "])" << std::endl;
                return false;
            }
        } else if (std::holds_alternative<int>(value)) {
            double val = static_cast<double>(std::get<int>(value));
            if (val < paramInfo->min || val > paramInfo->max) {
                std::cerr << "Error: Value out of range for parameter \"" << key << "\": "
                          << val << " (valid range: [" << paramInfo->min << ", " << paramInfo->max << "])" << std::endl;
                return false;
            }
        } else if (std::holds_alternative<int64_t>(value)) {
            double val = static_cast<double>(std::get<int64_t>(value));
            if (val < paramInfo->min || val > paramInfo->max) {
                std::cerr << "Error: Value out of range for parameter \"" << key << "\": "
                          << val << " (valid range: [" << paramInfo->min << ", " << paramInfo->max << "])" << std::endl;
                return false;
            }
        }
        
        // Custom validator
        if (paramInfo->validator && !paramInfo->validator(value)) {
            std::cerr << "Error: Validation failed for parameter \"" << key << "\"" << std::endl;
            return false;
        }
    }
    
    // Update or add the entry
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->timestamp = std::chrono::system_clock::now();
        
        // Update statistics
        if (source == ConfigSource::Runtime) {
            pImpl->stats.setByUser++;
        } else if (source == ConfigSource::File) {
            pImpl->stats.setByFile++;
        } else if (source == ConfigSource::CommandLine) {
            pImpl->stats.setByCommandLine++;
        }
    } else {
        pImpl->entries.emplace_back(key, value, source);
        
        // Update statistics
        pImpl->stats.totalParameters++;
        if (source == ConfigSource::Runtime) {
            pImpl->stats.setByUser++;
        } else if (source == ConfigSource::File) {
            pImpl->stats.setByFile++;
        } else if (source == ConfigSource::CommandLine) {
            pImpl->stats.setByCommandLine++;
        }
        
        // Update first set time if this is the first entry
        if (pImpl->stats.firstSet.time_since_epoch().count() == 0) {
            pImpl->stats.firstSet = std::chrono::system_clock::now();
        }
    }
    
    pImpl->stats.lastModified = std::chrono::system_clock::now();
    
    return true;
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    // Reset statistics
    pImpl->stats = ConfigStats();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary:" << std::endl;
    oss << "Total parameters: " << pImpl->entries.size() << std::endl;
    oss << "Set by user: " << pImpl->stats.setByUser << std::endl;
    oss << "Set by file: " << pImpl->stats.setByFile << std::endl;
    oss << "Set by command line: " << pImpl->stats.setByCommandLine << std::endl;
    
    if (pImpl->stats.firstSet.time_since_epoch().count() > 0) {
        auto duration = std::chrono::system_clock::now() - pImpl->stats.firstSet;
        auto hours = std::chrono::duration_cast<std::chrono::hours>(duration).count();
        oss << "First set: " << hours << " hours ago" << std::endl;
    }
    
    oss << std::endl << "Parameters:" << std::endl;
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [" << toString(entry.value) << "] (" 
            << static_cast<int>(entry.source) << ")" << std::endl;
    }
    return oss.str();
}

ConfigStats Config::getStats() const {
    return pImpl->stats;
}

void Config::registerParameter(const ParameterInfo& param) {
    pImpl->parameterSchema[param.name] = param;
}

bool Config::hasParameter(const std::string& key) const {
    return pImpl->parameterSchema.find(key) != pImpl->parameterSchema.end();
}

std::optional<ParameterInfo> Config::getParameterInfo(const std::string& key) const {
    auto it = pImpl->parameterSchema.find(key);
    if (it != pImpl->parameterSchema.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<ParameterInfo> Config::getAllParameterInfo() const {
    std::vector<ParameterInfo> params;
    params.reserve(pImpl->parameterSchema.size());
    for (const auto& pair : pImpl->parameterSchema) {
        params.push_back(pair.second);
    }
    return params;
}

std::vector<ParameterInfo> Config::getParametersByCategory(const std::string& category) const {
    std::vector<ParameterInfo> params;
    for (const auto& pair : pImpl->parameterSchema) {
        if (pair.second.category == category) {
            params.push_back(pair.second);
        }
    }
    return params;
}

bool Config::validate() const {
    std::vector<std::string> errors = getValidationErrors();
    if (!errors.empty()) {
        std::cerr << "Configuration validation errors:" << std::endl;
        for (const auto& error : errors) {
            std::cerr << "  " << error << std::endl;
        }
        return false;
    }
    return true;
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    for (const auto& param : pImpl->parameterSchema) {
        if (param.second.required && !has(param.first)) {
            errors.push_back("Required parameter missing: " + param.first);
        }
        
        if (has(param.first)) {
            auto valueOpt = getAny(param.first);
            if (valueOpt) {
                // Check type
                std::string type = param.second.type;
                if (type == "auto") {
                    type = inferType(valueOpt.value());
                }
                if (!isTypeValid(valueOpt.value(), type)) {
                    errors.push_back("Type mismatch for parameter \"" + param.first + "\": " +
                                   "expected " + type + ", got " + getTypeName(valueOpt.value()));
                }
                
                // Check range
                if (std::holds_alternative<double>(valueOpt.value())) {
                    double val = std::get<double>(valueOpt.value());
                    if (val < param.second.min || val > param.second.max) {
                        errors.push_back("Range violation for parameter \"" + param.first + "\": " +
                                       std::to_string(val) + " not in [" +
                                       std::to_string(param.second.min) + ", " +
                                       std::to_string(param.second.max) + "]");
                    }
                } else if (std::holds_alternative<int>(valueOpt.value())) {
                    double val = static_cast<double>(std::get<int>(valueOpt.value()));
                    if (val < param.second.min || val > param.second.max) {
                        errors.push_back("Range violation for parameter \"" + param.first + "\": " +
                                       std::to_string(val) + " not in [" +
                                       std::to_string(param.second.min) + ", " +
                                       std::to_string(param.second.max) + "]");
                    }
                }
                
                // Custom validator
                if (param.second.validator && !param.second.validator(valueOpt.value())) {
                    errors.push_back("Custom validation failed for parameter \"" + param.first + "\"");
                }
            }
        }
    }
    
    return errors;
}

void Config::setValidator(const std::string& key, std::function<bool(const ConfigValue&)> validator) {
    auto it = pImpl->parameterSchema.find(key);
    if (it != pImpl->parameterSchema.end()) {
        it->second.validator = validator;
    }
}

bool Config::isValid(const std::string& key) const {
    auto paramInfo = findParameterInfo(key);
    if (!paramInfo || !has(key)) {
        return false;
    }
    
    auto valueOpt = getAny(key);
    if (!valueOpt) {
        return false;
    }
    
    // Type validation
    std::string type = paramInfo->type;
    if (type == "auto") {
        type = inferType(valueOpt.value());
    }
    if (!isTypeValid(valueOpt.value(), type)) {
        return false;
    }
    
    // Range validation
    if (std::holds_alternative<double>(valueOpt.value())) {
        double val = std::get<double>(valueOpt.value());
        if (val < paramInfo->min || val > paramInfo->max) {
            return false;
        }
    } else if (std::holds_alternative<int>(valueOpt.value())) {
        double val = static_cast<double>(std::get<int>(valueOpt.value()));
        if (val < paramInfo->min || val > paramInfo->max) {
            return false;
        }
    }
    
    // Custom validator
    if (paramInfo->validator && !paramInfo->validator(valueOpt.value())) {
        return false;
    }
    
    return true;
}

template<typename T>
bool Config::setFloat(const std::string& key, T value, const std::string& unit, double min, double max) {
    return setWithValidation(key, ConfigValue(static_cast<double>(value)), ConfigSource::Runtime);
}

template<typename T>
bool Config::setInt(const std::string& key, T value, double min, double max) {
    return setWithValidation(key, ConfigValue(static_cast<int>(value)), ConfigSource::Runtime);
}

template<typename T>
bool Config::setBool(const std::string& key, T value) {
    return setWithValidation(key, ConfigValue(static_cast<bool>(value)), ConfigSource::Runtime);
}

template<typename T>
bool Config::setString(const std::string& key, T value) {
    return setWithValidation(key, ConfigValue(std::string(value)), ConfigSource::Runtime);
}

void Config::setMultiple(const std::map<std::string, ConfigValue>& values, ConfigSource source) {
    for (const auto& pair : values) {
        setWithValidation(pair.first, pair.second, source);
    }
}

bool Config::setMultipleWithValidation(const std::map<std::string, ConfigValue>& values, ConfigSource source) {
    for (const auto& pair : values) {
        if (!setWithValidation(pair.first, pair.second, source)) {
            return false;
        }
    }
    return true;
}

// Static factory methods
Config Config::createDefaultConfig() {
    Config config;
    
    // Set sensible defaults
    config.set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config.set("simulation_timestep", 0.001, ConfigSource::Default);
    config.set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config.set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config.set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config.set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config.set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config.set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    return config;
}

Config Config::createResearchConfig() {
    Config config = createDefaultConfig();
    
    // Research-oriented settings
    config.set("neuron_count", static_cast<int64_t>(2000), ConfigSource::User);
    config.set("learning_rate", 0.05, ConfigSource::User);
    config.set("reward_scaling", 2.0, ConfigSource::User);
    config.set("working_memory_capacity", 500, ConfigSource::User);
    
    return config;
}

Config Config::createPerformanceConfig() {
    Config config = createDefaultConfig();
    
    // Performance-optimized settings
    config.set("neuron_count", static_cast<int64_t>(1000), ConfigSource::User);
    config.set("connection_probability", 0.1f, ConfigSource::User);
    config.set("enable_visualization", false, ConfigSource::User);
    
    return config;
}

Config Config::createValidationConfig() {
    Config config = createDefaultConfig();
    
    // Validation-focused settings
    config.set("stdp_ltp_weight", 0.01f, ConfigSource::User);
    config.set("stdp_ltd_weight", 0.005f, ConfigSource::User);
    config.set("novelty_threshold", 0.05, ConfigSource::User);
    config.set("reward_scaling", 0.5, ConfigSource::User);
    
    return config;
}

// Helper functions
std::string Config::getHelp(const std::string& key) const {
    auto paramInfo = findParameterInfo(key);
    if (!paramInfo) {
        return "Parameter not found: " + key;
    }
    
    std::ostringstream oss;
    oss << paramInfo->name << ": " << paramInfo->description;
    if (!paramInfo->unit.empty()) {
        oss << " (" << paramInfo->unit << ")";
    }
    oss << "\n  Type: " << paramInfo->type << "\n";
    oss << "  Default: " << toString(paramInfo->defaultValue);
    if (paramInfo->type == "double" || paramInfo->type == "int") {
        oss << " [" << paramInfo->min << ", " << paramInfo->max << "]";
    }
    return oss.str();
}

std::string Config::getParameterList() const {
    std::ostringstream oss;
    oss << "Available parameters:" << std::endl;
    for (const auto& param : pImpl->parameterSchema) {
        oss << "  " << param.first << "\n";
    }
    return oss.str();
}

std::map<std::string, std::string> Config::getCategoryDescriptions() const {
    std::map<std::string, std::string> categories;
    categories["simulation"] = "Basic simulation parameters";
    categories["plasticity"] = "Neural plasticity and learning";
    categories["development"] = "Brain development and maturation";
    categories["neuromodulation"] = "Modulation signals and states";
    categories["memory"] = "Memory system parameters";
    categories["performance"] = "Performance and optimization";
    categories["visualization"] = "Visualization and output";
    return categories;
}

void Config::setTimestamp(const std::chrono::system_clock::time_point& timestamp) {
    pImpl->sessionTimestamp = timestamp;
}

std::chrono::system_clock::time_point Config::getTimestamp() const {
    return pImpl->sessionTimestamp;
}

// Private helper methods
std::optional<ParameterInfo> Config::findParameterInfo(const std::string& key) const {
    auto it = pImpl->parameterSchema.find(key);
    if (it != pImpl->parameterSchema.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<ConfigValue> Config::getAny(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->value;
    }
    return std::nullopt;
}

std::string Config::inferType(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
    return "unknown";
}

std::string Config::getTypeName(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
    return "unknown";
}

// Explicit template instantiations

template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

template bool Config::set(const std::string&, const int&, ConfigSource);
template bool Config::set(const std::string&, const double&, ConfigSource);
template bool Config::set(const std::string&, const bool&, ConfigSource);
template bool Config::set(const std::string&, const std::string&, ConfigSource);

template bool Config::setFloat(const std::string&, double, const std::string&, double, double);
template bool Config::setInt(const std::string&, int64_t, double, double);
template bool Config::setBool(const std::string&, bool);
template bool Config::setString(const std::string&, const std::string&);

} // namespace nlm