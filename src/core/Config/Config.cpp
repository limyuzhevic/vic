#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <thread>

// Add std::filesystem namespace support for older C++ standards
namespace fs = std::filesystem;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()), 
                  lastError_(), 
                  lastOperationSuccess_(false) {
    // Load default values from schema
    initializeDefaults();
    lastOperationSuccess_ = true;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, std::string& error) {
    lastOperationSuccess_ = false;
    error.clear();
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        lastError_ = "Failed to open configuration file: " + filepath;
        error = lastError_;
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    bool success = true;
    
    while (std::getline(file, line)) {
        lineNum++;
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            ConfigValue configValue;
            if (parseConfigValue(value, configValue)) {
                auto schemaIt = schema_.find(key);
                if (schemaIt != schema_.end()) {
                    if (!validateKeyValue(key, configValue)) {
                        error = "Invalid value for key '" + key + "': " + 
                                getValidationError(key);
                        success = false;
                        continue;
                    }
                }
                
                set(key, configValue, ConfigSource::File, true);
            } else {
                error = "Failed to parse value at line " + std::to_string(lineNum) + ": " + value;
                success = false;
            }
        } else {
            error = "Invalid format at line " + std::to_string(lineNum) + ": missing '='";
            success = false;
        }
    }
    
    lastOperationSuccess_ = success;
    if (!success && error.empty()) {
        lastError_ = "Failed to load configuration file: " + filepath;
        error = lastError_;
    }
    
    return success;
}

bool Config::loadFromArgs(int argc, char** argv, std::string& error) {
    lastOperationSuccess_ = false;
    error.clear();
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                ConfigValue configValue;
                if (parseConfigValue(value, configValue)) {
                    auto schemaIt = schema_.find(key);
                    if (schemaIt != schema_.end()) {
                        if (!validateKeyValue(key, configValue)) {
                            error = "Invalid command line value for key '" + key + "': " + 
                                    getValidationError(key);
                            success = false;
                            continue;
                        }
                    }
                    
                    set(key, configValue, ConfigSource::CommandLine, true);
                } else {
                    error = "Failed to parse command line value for key '" + key + "': " + value;
                    success = false;
                }
            }
        }
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            ConfigValue configValue;
            if (parseConfigValue(value, configValue)) {
                auto schemaIt = schema_.find(key);
                if (schemaIt != schema_.end()) {
                    if (!validateKeyValue(key, configValue)) {
                        error = "Invalid command line value for key '" + key + "': " + 
                                getValidationError(key);
                        success = false;
                        continue;
                    }
                }
                
                set(key, configValue, ConfigSource::CommandLine, true);
            } else {
                error = "Failed to parse command line value for key '" + key + "': " + value;
                success = false;
            }
        }
    }
    
    lastOperationSuccess_ = success;
    if (!success && error.empty()) {
        lastError_ = "Failed to parse command line arguments";
        error = lastError_;
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath, std::string& error) const {
    lastOperationSuccess_ = false;
    error.clear();
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        lastError_ = "Failed to open file for writing: " + filepath;
        error = lastError_;
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        std::string valueStr;
        std::visit([&valueStr](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStr = "\"" + arg + "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                valueStr = arg ? "true" : "false";
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                valueStr = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                valueStr = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                valueStr = "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) valueStr += ", ";
                    valueStr += std::to_string(arg[i]);
                }
                valueStr += "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                valueStr = "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) valueStr += ", ";
                    valueStr += std::to_string(arg[i]);
                }
                valueStr += "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                valueStr = "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) valueStr += ", ";
                    valueStr += "\"" + arg[i] + "\"";
                }
                valueStr += "]";
            } else {
                valueStr = std::to_string(arg);
            }
        }, entry.value);
        
        file << entry.key << " = " << valueStr << "\n\n";
    }
    
    lastOperationSuccess_ = true;
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

void Config::set(const std::string& key, const ConfigValue& value, 
                 ConfigSource source, bool validate) {
    if (validate) {
        if (!validateKeyValue(key, value)) {
            lastOperationSuccess_ = false;
            lastError_ = "Invalid value for key '" + key + "': " + getValidationError(key);
            return;
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
        
        auto schemaIt = schema_.find(key);
        if (schemaIt != schema_.end()) {
            pImpl->entries.back().description = schemaIt->second.description;
        }
    }
    
    lastOperationSuccess_ = true;
    lastError_.clear();
}

void Config::set(const std::string& key, const std::string& value,
                 ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, int value,
                 ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, int64_t value,
                 ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, double value,
                 ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, bool value,
                 ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::addValidationRule(const std::string& key, std::shared_ptr<ValidationRule> rule) {
    auto schemaIt = schema_.find(key);
    if (schemaIt != schema_.end()) {
        schemaIt->second.validators.push_back(rule);
    }
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
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::fixed << std::setprecision(3) << arg;
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
                    oss << std::fixed << std::setprecision(3) << arg[i];
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
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (!entry.getValidationError().empty()) {
            oss << " [INVALID: " << entry.getValidationError() << "]";
        }
        oss << "\n";
    }
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool Config::parseConfigValue(const std::string& str, ConfigValue& outValue) {
    std::string value = trim(str);
    
    if ((value.front() == '"' && value.back() == '"') ||
        (value.front() == '\'' && value.back() == '\'')) {
        value = value.substr(1, value.size() - 2);
    }
    
    if (value == "true") {
        outValue = true;
        return true;
    } else if (value == "false") {
        outValue = false;
        return true;
    }
    
    try {
        size_t pos;
        int64_t intVal = std::stoll(value, &pos);
        if (pos == value.size()) {
            outValue = intVal;
            return true;
        }
    } catch (...) {}
    
    try {
        size_t pos;
        double doubleVal = std::stod(value, &pos);
        if (pos == value.size()) {
            outValue = doubleVal;
            return true;
        }
    } catch (...) {}
    
    outValue = value;
    return true;
}

void Config::initializeDefaults() {
    // Advanced Neural Network Configuration
    schema_["neural.network.layer_count"] = ConfigSchemaEntry{
        "neural.network.layer_count", "int64", static_cast<int64_t>(3),
        "Number of neural layers in the network", true,
        {std::make_shared<RangeValidationRule>(1, 100, "layer count")}
    };
    
    schema_["neural.network.neurons_per_layer"] = ConfigSchemaEntry{
        "neural.network.neurons_per_layer", "int64", static_cast<int64_t>(100),
        "Number of neurons per layer", true,
        {std::make_shared<RangeValidationRule>(10, 10000, "neurons per layer")}
    };
    
    schema_["neural.network.connectivity_mode"] = ConfigSchemaEntry{
        "neural.network.connectivity_mode", "string", std::string("dense"),
        "Network connectivity mode: 'dense' or 'sparse'", true,
        {std::make_shared<EnumValidationRule>(std::vector<std::string>{"dense", "sparse"}, "connectivity mode")}
    };
    
    schema_["neural.network.region_count"] = ConfigSchemaEntry{
        "neural.network.region_count", "int64", static_cast<int64_t>(1),
        "Number of neural regions", true,
        {std::make_shared<RangeValidationRule>(1, 100, "region count")}
    };
    
    schema_["neural.network.population_count"] = ConfigSchemaEntry{
        "neural.network.population_count", "int64", static_cast<int64_t>(5),
        "Number of neuron populations per region", true,
        {std::make_shared<RangeValidationRule>(1, 50, "population count")}
    };
    
    schema_["neural.network.developmental_stage"] = ConfigSchemaEntry{
        "neural.network.developmental_stage", "string", std::string("adult"),
        "Current developmental stage", true,
        {std::make_shared<EnumValidationRule>(std::vector<std::string>{"initial", "critical_period", "maturation", "adult", "aging"}, "developmental stage")}
    };
    
    // Advanced Learning and Plasticity Configuration
    schema_["learning.stdp.learning_rate"] = ConfigSchemaEntry{
        "learning.stdp.learning_rate", "double", 0.02,
        "STDP learning rate for potentiation", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "STDP learning rate")}
    };
    
    schema_["learning.stdp.ltp_weight"] = ConfigSchemaEntry{
        "learning.stdp.ltp_weight", "double", 0.02,
        "STDP Long-Term Potentiation weight", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "STDP LTP weight")}
    };
    
    schema_["learning.stdp.ltd_weight"] = ConfigSchemaEntry{
        "learning.stdp.ltd_weight", "double", 0.015,
        "STDP Long-Term Depression weight", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "STDP LTD weight")}
    };
    
    schema_["learning.stdp.tau"] = ConfigSchemaEntry{
        "learning.stdp.tau", "double", 20.0,
        "STDP trace decay time constant", true,
        {std::make_shared<RangeValidationRule>(1.0, 100.0, "STDP tau")}
    };
    
    schema_["learning.stdp.eligibility_trace_decay"] = ConfigSchemaEntry{
        "learning.stdp.eligibility_trace_decay", "double", 0.9,
        "Eligibility trace decay factor (0.0-1.0)", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "eligibility trace decay")}
    };
    
    schema_["learning.hebbian.strength"] = ConfigSchemaEntry{
        "learning.hebbian.strength", "double", 0.01,
        "Hebbian learning strength", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "Hebbian strength")}
    };
    
    schema_["learning.adaptive.learning_rate"] = ConfigSchemaEntry{
        "learning.adaptive.learning_rate", "double", 0.01,
        "Adaptive learning rate base value", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "adaptive learning rate")}
    };
    
    schema_["learning.adaptive.decay_rate"] = ConfigSchemaEntry{
        "learning.adaptive.decay_rate", "double", 0.95,
        "Adaptive learning rate decay factor", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "adaptive decay rate")}
    };
    
    schema_["learning.meta_learning.rate"] = ConfigSchemaEntry{
        "learning.meta_learning.rate", "double", 0.001,
        "Meta-learning rate for hyperparameter optimization", true,
        {std::make_shared<RangeValidationRule>(0.0, 0.1, "meta-learning rate")}
    };
    
    // Advanced Neuromodulation Configuration
    schema_["neuromodulation.multi_neuromodulator.enabled"] = ConfigSchemaEntry{
        "neuromodulation.multi_neuromodulator.enabled", "bool", true,
        "Enable multi-neuromodulator system", true
    };
    
    schema_["neuromodulation.dopamine.baseline"] = ConfigSchemaEntry{
        "neuromodulation.dopamine.baseline", "double", 0.5,
        "Dopamine baseline level (0.0-1.0)", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "dopamine baseline")}
    };
    
    schema_["neuromodulation.serotonin.baseline"] = ConfigSchemaEntry{
        "neuromodulation.serotonin.baseline", "double", 0.5,
        "Serotonin baseline level (0.0-1.0)", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "serotonin baseline")}
    };
    
    schema_["neuromodulation.norepinephrine.baseline"] = ConfigSchemaEntry{
        "neuromodulation.norepinephrine.baseline", "double", 0.5,
        "Norepinephrine baseline level (0.0-1.0)", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "norepinephrine baseline")}
    };
    
    schema_["neuromodulation.acetylcholine.baseline"] = ConfigSchemaEntry{
        "neuromodulation.acetylcholine.baseline", "double", 0.5,
        "Acetylcholine baseline level (0.0-1.0)", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "acetylcholine baseline")}
    };
    
    schema_["neuromodulation.interaction_strength"] = ConfigSchemaEntry{
        "neuromodulation.interaction_strength", "double", 0.3,
        "Interaction strength between neuromodulators", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "neuromodulator interaction")}
    };
    
    // Advanced Memory Systems Configuration
    schema_["memory.multi_tier.system_enabled"] = ConfigSchemaEntry{
        "memory.multi_tier.system_enabled", "bool", true,
        "Enable multi-tier memory system", true
    };
    
    schema_["memory.short_term.capacity"] = ConfigSchemaEntry{
        "memory.short_term.capacity", "int64", static_cast<int64_t>(100),
        "Short-term memory capacity (number of entries)", true,
        {std::make_shared<RangeValidationRule>(10, 1000, "short-term capacity")}
    };
    
    schema_["memory.long_term.capacity"] = ConfigSchemaEntry{
        "memory.long_term.capacity", "int64", static_cast<int64_t>(10000),
        "Long-term memory capacity (number of entries)", true,
        {std::make_shared<RangeValidationRule>(100, 100000, "long-term capacity")}
    };
    
    schema_["memory.consolidation.threshold"] = ConfigSchemaEntry{
        "memory.consolidation.threshold", "double", 0.7,
        "Consolidation threshold for memory stabilization", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "consolidation threshold")}
    };
    
    schema_["memory.replay.frequency"] = ConfigSchemaEntry{
        "memory.replay.frequency", "int64", static_cast<int64_t>(10),
        "Replay frequency (steps between replay events)", true,
        {std::make_shared<RangeValidationRule>(1, 1000, "replay frequency")}
    };
    
    schema_["memory.prioritization.importance_decay"] = ConfigSchemaEntry{
        "memory.prioritization.importance_decay", "double", 0.9,
        "Importance decay rate for memory prioritization", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "importance decay")}
    };
    
    schema_["memory.decay.rate"] = ConfigSchemaEntry{
        "memory.decay.rate", "double", 0.01,
        "General memory decay rate (0.0-1.0)", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "memory decay rate")}
    };
    
    // Advanced Cognition Systems Configuration
    schema_["cognition.planning.algorithm"] = ConfigSchemaEntry{
        "cognition.planning.algorithm", "string", std::string("a_star"),
        "Planning algorithm: 'a_star', 'dijkstra', or 'heuristic'", true,
        {std::make_shared<EnumValidationRule>(std::vector<std::string>{"a_star", "dijkstra", "heuristic"}, "planning algorithm")}
    };
    
    schema_["cognition.planning.max_depth"] = ConfigSchemaEntry{
        "cognition.planning.max_depth", "int64", static_cast<int64_t>(20),
        "Maximum planning depth", true,
        {std::make_shared<RangeValidationRule>(1, 100, "planning depth")}
    };
    
    schema_["cognition.concept.formulation.threshold"] = ConfigSchemaEntry{
        "cognition.concept.formulation.threshold", "double", 0.5,
        "Concept formation threshold (0.0-1.0)", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "concept formation threshold")}
    };
    
    schema_["cognition.attention.mechanism"] = ConfigSchemaEntry{
        "cognition.attention.mechanism", "string", std::string("global"),
        "Attention mechanism: 'global', 'local', or 'hybrid'", true,
        {std::make_shared<EnumValidationRule>(std::vector<std::string>{"global", "local", "hybrid"}, "attention mechanism")}
    };
    
    schema_["cognition.social.learning.enabled"] = ConfigSchemaEntry{
        "cognition.social.learning.enabled", "bool", false,
        "Enable social learning from other agents", true
    };
    
    schema_["cognition.self_model.development_rate"] = ConfigSchemaEntry{
        "cognition.self_model.development_rate", "double", 0.1,
        "Self-model development rate (0.0-1.0)", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "self-model development rate")}
    };
    
    // Advanced Simulation Features Configuration
    schema_["simulation.event_driven.enabled"] = ConfigSchemaEntry{
        "simulation.event_driven.enabled", "bool", false,
        "Enable event-driven simulation instead of fixed timestep", true
    };
    
    schema_["simulation.parallel.processing_threads"] = ConfigSchemaEntry{
        "simulation.parallel.processing_threads", "int", std::thread::hardware_concurrency(),
        "Number of parallel processing threads", true,
        {std::make_shared<RangeValidationRule>(1, 64, "parallel threads")}
    };
    
    schema_["simulation.distributed.computing.enabled"] = ConfigSchemaEntry{
        "simulation.distributed.computing.enabled", "bool", false,
        "Enable distributed computing for large-scale simulations", true
    };
    
    schema_["simulation.heterogeneous.architecture.enabled"] = ConfigSchemaEntry{
        "simulation.heterogeneous.architecture.enabled", "bool", false,
        "Enable heterogeneous architecture simulation", true
    };
    
    schema_["simulation.real_time.enabled"] = ConfigSchemaEntry{
        "simulation.real_time.enabled", "bool", false,
        "Enable real-time simulation mode", true
    };
    
    schema_["simulation.real_time.max_fps"] = ConfigSchemaEntry{
        "simulation.real_time.max_fps", "int", 60,
        "Maximum frames per second for real-time simulation", true,
        {std::make_shared<RangeValidationRule>(1, 1000, "max FPS")}
    };
    
    // Advanced Experiment Configuration
    schema_["experiments.multi_experiment.framework.enabled"] = ConfigSchemaEntry{
        "experiments.multi_experiment.framework.enabled", "bool", false,
        "Enable multi-experiment framework", true
    };
    
    schema_["experiments.parameter_space.exploration_rate"] = ConfigSchemaEntry{
        "experiments.parameter_space.exploration_rate", "double", 0.1,
        "Parameter space exploration rate (0.0-1.0)", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "exploration rate")}
    };
    
    schema_["experiments.adaptive.design.enabled"] = ConfigSchemaEntry{
        "experiments.adaptive.design.enabled", "bool", false,
        "Enable adaptive experiment design", true
    };
    
    schema_["experiments.result.analysis.precision"] = ConfigSchemaEntry{
        "experiments.result.analysis.precision", "string", std::string("high"),
        "Result analysis precision: 'low', 'medium', or 'high'", true,
        {std::make_shared<EnumValidationRule>(std::vector<std::string>{"low", "medium", "high"}, "analysis precision")}
    };
    
    schema_["experiments.reproducibility.enabled"] = ConfigSchemaEntry{
        "experiments.reproducibility.enabled", "bool", true,
        "Enable experiment reproducibility features", true
    };
    
    // Performance Optimization Profiles
    schema_["performance.profile.type"] = ConfigSchemaEntry{
        "performance.profile.type", "string", std::string("balanced"),
        "Performance profile type: 'speed', 'memory', or 'balanced'", true,
        {std::make_shared<EnumValidationRule>(std::vector<std::string>{"speed", "memory", "balanced"}, "performance profile")}
    };
    
    schema_["performance.speed.optimization_level"] = ConfigSchemaEntry{
        "performance.speed.optimization_level", "int", 3,
        "Speed optimization level (0-3, where 3 is maximum)", true,
        {std::make_shared<RangeValidationRule>(0, 3, "speed optimization level")}
    };
    
    schema_["performance.memory.optimization_level"] = ConfigSchemaEntry{
        "performance.memory.optimization_level", "int", 3,
        "Memory optimization level (0-3, where 3 is maximum)", true,
        {std::make_shared<RangeValidationRule>(0, 3, "memory optimization level")}
    };
    
    schema_["performance.balanced.optimization_level"] = ConfigSchemaEntry{
        "performance.balanced.optimization_level", "int", 2,
        "Balanced optimization level (0-3, where 2 is medium)", true,
        {std::make_shared<RangeValidationRule>(0, 3, "balanced optimization level")}
    };
    
    // Configuration Management Tools
    schema_["config.management.auto_save.enabled"] = ConfigSchemaEntry{
        "config.management.auto_save.enabled", "bool", false,
        "Enable automatic configuration saving", true
    };
    
    schema_["config.management.auto_save.interval"] = ConfigSchemaEntry{
        "config.management.auto_save.interval", "int64", static_cast<int64_t>(3600),
        "Auto-save interval in seconds", true,
        {std::make_shared<RangeValidationRule>(60, 86400, "auto-save interval")}
    };
    
    schema_["config.comparison.diff_threshold"] = ConfigSchemaEntry{
        "config.comparison.diff_threshold", "double", 0.01,
        "Configuration comparison difference threshold (0.0-1.0)", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "comparison threshold")}
    };
    
    schema_["config.validation.strict_mode"] = ConfigSchemaEntry{
        "config.validation.strict_mode", "bool", false,
        "Enable strict configuration validation", true
    };
    
    schema_["config.optimization.enabled"] = ConfigSchemaEntry{
        "config.optimization.enabled", "bool", false,
        "Enable automatic configuration optimization", true
    };
    
    // Legacy/Existing Configuration (for backward compatibility)
    schema_["network.layer_count"] = ConfigSchemaEntry{
        "network.layer_count", "int64", static_cast<int64_t>(3),
        "Number of neural layers in the network", true,
        {std::make_shared<RangeValidationRule>(1, 100, "layer count")}
    };
    
    schema_["network.neurons_per_layer"] = ConfigSchemaEntry{
        "network.neurons_per_layer", "int64", static_cast<int64_t>(100),
        "Number of neurons per layer", true,
        {std::make_shared<RangeValidationRule>(10, 10000, "neurons per layer")}
    };
    
    schema_["network.connectivity_mode"] = ConfigSchemaEntry{
        "network.connectivity_mode", "string", std::string("dense"),
        "Network connectivity mode: 'dense' or 'sparse'", true,
        {std::make_shared<EnumValidationRule>(std::vector<std::string>{"dense", "sparse"}, "connectivity mode")}
    };
    
    schema_["performance.optimization_level"] = ConfigSchemaEntry{
        "performance.optimization_level", "int", 2,
        "Performance optimization level (0-3, where 3 is maximum)", true,
        {std::make_shared<RangeValidationRule>(0, 3, "optimization level")}
    };
    
    schema_["performance.max_memory_mb"] = ConfigSchemaEntry{
        "performance.max_memory_mb", "int64", static_cast<int64_t>(2048),
        "Maximum memory usage in MB", true,
        {std::make_shared<RangeValidationRule>(256, 16384, "memory limit")}
    };
    
    schema_["performance.thread_count"] = ConfigSchemaEntry{
        "performance.thread_count", "int", std::thread::hardware_concurrency(),
        "Number of CPU threads to use", true,
        {std::make_shared<RangeValidationRule>(1, 64, "thread count")}
    };
    
    schema_["learning.stdp_learning_rate"] = ConfigSchemaEntry{
        "learning.stdp_learning_rate", "double", 0.02,
        "STDP learning rate for potentiation", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "STDP learning rate")}
    };
    
    schema_["learning.hebbian_strength"] = ConfigSchemaEntry{
        "learning.hebbian_strength", "double", 0.01,
        "Hebbian learning strength", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "Hebbian strength")}
    };
    
    schema_["learning.eligibility_trace_decay"] = ConfigSchemaEntry{
        "learning.eligibility_trace_decay", "double", 0.9,
        "Eligibility trace decay factor (0.0-1.0)", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "eligibility trace decay")}
    };
    
    schema_["simulation.timestep"] = ConfigSchemaEntry{
        "simulation.timestep", "double", 0.001,
        "Simulation timestep in seconds", true,
        {std::make_shared<RangeValidationRule>(0.0001, 1.0, "timestep")}
    };
    
    schema_["simulation.max_steps"] = ConfigSchemaEntry{
        "simulation.max_steps", "int64", static_cast<int64_t>(10000),
        "Maximum number of simulation steps", true,
        {std::make_shared<RangeValidationRule>(10, 1000000, "max steps")}
    };
    
    schema_["debug.enable_breakpoints"] = ConfigSchemaEntry{
        "debug.enable_breakpoints", "bool", false,
        "Enable debugging breakpoints", true
    };
    
    schema_["debug.log_level"] = ConfigSchemaEntry{
        "debug.log_level", "string", std::string("INFO"),
        "Log level: DEBUG, INFO, WARN, ERROR", true,
        {std::make_shared<EnumValidationRule>(std::vector<std::string>{"DEBUG", "INFO", "WARN", "ERROR"}, "log level")}
    };
    
    schema_["debug.profiling_enabled"] = ConfigSchemaEntry{
        "debug.profiling_enabled", "bool", false,
        "Enable profiling", true
    };
    
    schema_["neuron_count"] = ConfigSchemaEntry{
        "neuron_count", "int64", static_cast<int64_t>(500),
        "Total number of neurons", true,
        {std::make_shared<RangeValidationRule>(10, 100000, "neuron count")}
    };
    
    schema_["region_count"] = ConfigSchemaEntry{
        "region_count", "int64", static_cast<int64_t>(1),
        "Number of neural regions", true,
        {std::make_shared<RangeValidationRule>(1, 100, "region count")}
    };
    
    schema_["connection_probability"] = ConfigSchemaEntry{
        "connection_probability", "double", 0.15,
        "Probability of connections between neurons (0.0-1.0)", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "connection probability")}
    };
    
    schema_["random_seed"] = ConfigSchemaEntry{
        "random_seed", "int64", static_cast<int64_t>(42),
        "Random seed for reproducible results", true,
        {std::make_shared<RangeValidationRule>(0, 2147483647, "random seed")}
    };
    
    schema_["stdp_ltp_weight"] = ConfigSchemaEntry{
        "stdp_ltp_weight", "double", 0.02,
        "STDP Long-Term Potentiation weight", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "STDP LTP weight")}
    };
    
    schema_["stdp_ltd_weight"] = ConfigSchemaEntry{
        "stdp_ltd_weight", "double", 0.015,
        "STDP Long-Term Depression weight", true,
        {std::make_shared<RangeValidationRule>(0.0, 1.0, "STDP LTD weight")}
    };
    
    schema_["stdp_tau"] = ConfigSchemaEntry{
        "stdp_tau", "double", 20.0,
        "STDP trace decay time constant", true,
        {std::make_shared<RangeValidationRule>(1.0, 100.0, "STDP tau")}
    };
    
    schema_["synaptogenesis_rate"] = ConfigSchemaEntry{
        "synaptogenesis_rate", "double", 0.0001,
        "Rate of new synapse formation", true,
        {std::make_shared<RangeValidationRule>(0.0, 0.01, "synaptogenesis rate")}
    };
    
    schema_["pruning_rate"] = ConfigSchemaEntry{
        "pruning_rate", "double", 0.00001,
        "Rate of synapse removal", true,
        {std::make_shared<RangeValidationRule>(0.0, 0.01, "pruning rate")}
    };
}}

// Template function definitions
template std::optional<int> nlm::Config::get<int>(const std::string&) const;
template std::optional<int64_t> nlm::Config::get<int64_t>(const std::string&) const;
template std::optional<double> nlm::Config::get<double>(const std::string&) const;
template std::optional<bool> nlm::Config::get<bool>(const std::string&) const;
template std::optional<std::string> nlm::Config::get<std::string>(const std::string&) const;

template int nlm::Config::getOr<int>(const std::string&, const int&) const;
template int64_t nlm::Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double nlm::Config::getOr<double>(const std::string&, const double&) const;
template bool nlm::Config::getOr<bool>(const std::string&, const bool&) const;
template std::string nlm::Config::getOr<std::string>(const std::string&, const std::string&) const;
