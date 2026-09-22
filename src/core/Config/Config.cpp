#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>  // Added for setprecision

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>  // Added for setprecision
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>

namespace nlm {

namespace {

// Simple JSON/YAML-like parser for basic key=value pairs
// This is a lightweight implementation that supports:
// - Basic key=value pairs
// - String values with quotes
// - Number values (integers and floats)
// - Boolean values
// - Comments (lines starting with # or //)

struct JsonLikeValue {
    std::variant<int, int64_t, double, bool, std::string> value;
    std::string type;
};

std::optional<JsonLikeValue> parseLine(const std::string& line) {
    std::string lineCopy = line;
    lineCopy = trim(lineCopy);
    
    if (lineCopy.empty() || lineCopy[0] == '#' || lineCopy[0] == '/') {
        return std::nullopt;
    }
    
    size_t pos = lineCopy.find('=');
    if (pos == std::string::npos) {
        return std::nullopt;
    }
    
    std::string key = trim(lineCopy.substr(0, pos));
    std::string valueStr = trim(lineCopy.substr(pos + 1));
    
    // Remove quotes
    if (valueStr.size() >= 2 && ((valueStr.front() == '"' && valueStr.back() == '"') || 
                               (valueStr.front() == \'\' && valueStr.back() == \'\'))) {
        valueStr = valueStr.substr(1, valueStr.size() - 2);
    }
    
    // Try to parse as number
    try {
        if (valueStr.find('.') != std::string::npos) {
            double num = std::stod(valueStr);
            return JsonLikeValue{num, "double"};
        } else {
            size_t pos64;
            long long int64 = std::stoll(valueStr, &pos64);
            if (pos64 == valueStr.size()) {
                return JsonLikeValue{int64, "int64_t"};
            }
            int num = std::stoi(valueStr);
            return JsonLikeValue{num, "int"};
        }
    } catch (...) {
        // Not a number
    }
    
    // Try to parse as boolean
    if (valueStr == "true" || valueStr == "1") {
        return JsonLikeValue{true, "bool"};
    }
    if (valueStr == "false" || valueStr == "0") {
        return JsonLikeValue{false, "bool"};
    }
    
    // Assume it's a string
    return JsonLikeValue{valueStr, "string"};
}

std::string serializeValue(const JsonLikeValue& value) {
    if (value.type == "string") {
        return "\"" + std::get<std::string>(value.value) + "\"";
    } else if (value.type == "int64_t") {
        return std::to_string(std::get<int64_t>(value.value));
    } else if (value.type == "int") {
        return std::to_string(std::get<int>(value.value));
    } else if (value.type == "double") {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(6) << std::get<double>(value.value);
        return oss.str();
    } else if (value.type == "bool") {
        return std::get<bool>(value.value) ? "true" : "false";
    }
    return "";
}

} // namespace

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>
#include <unordered_map>

namespace nlm {

namespace {

// Configuration parameter definitions with validation rules
struct ParameterDefinition {
    std::string name;
    std::string type;  // "int", "double", "bool", "string"
    std::string description;
    std::variant<int, double, bool, std::string> defaultValue;
    std::optional<std::pair<double, double>> range;  // min, max for numeric types
    std::function<bool(const ConfigValue&)> validator;
    bool required = false;
};

// Global parameter registry
std::vector<ParameterDefinition> getDefaultParameters() {
    return {
        // Core simulation parameters
        {"random_seed", "int64_t", "Random seed for reproducible simulations", 42, std::nullopt, nullptr, false},
        {"simulation_timestep", "double", "Simulation timestep in seconds", 0.001, std::make_pair(0.0001, 0.1), nullptr, true},
        {"neuron_count", "int64_t", "Number of neurons in the network", 1000, std::make_pair(1, 100000), nullptr, true},
        {"region_count", "int64_t", "Number of neural regions", 1, std::make_pair(1, 100), nullptr, false},
        {"connection_probability", "double", "Probability of synaptic connections", 0.1, std::make_pair(0.0, 1.0), nullptr, false},
        
        // LIF neuron parameters
        {"lif_resting_potential", "double", "Neuron resting potential (mV)", -70.0, std::make_pair(-100.0, -40.0), nullptr, false},
        {"lif_threshold", "double", "Neuron firing threshold (mV)", -55.0, std::make_pair(-80.0, -20.0), nullptr, false},
        {"lif_reset_potential", "double", "Post-spike reset potential (mV)", -70.0, std::make_pair(-100.0, -40.0), nullptr, false},
        {"lif_leak_conductance", "double", "Neuron leak conductance (nS)", 10.0, std::make_pair(1.0, 100.0), nullptr, false},
        {"lif_time_constant", "double", "Membrane time constant (ms)", 20.0, std::make_pair(5.0, 100.0), nullptr, false},
        {"lif_refractory_period", "int64_t", "Refractory period in steps", 5, std::make_pair(0, 100), nullptr, false},
        {"lif_adaptation_strength", "double", "Spike-frequency adaptation strength", 0.1, std::make_pair(0.0, 1.0), nullptr, false},
        {"lif_adaptation_decay", "double", "Adaptation decay rate", 0.95, std::make_pair(0.0, 1.0), nullptr, false},
        {"lif_voltage_clamp_min", "double", "Minimum voltage clamp (mV)", -100.0, std::make_pair(-200.0, 0.0), nullptr, false},
        {"lif_voltage_clamp_max", "double", "Maximum voltage clamp (mV)", 50.0, std::make_pair(0.0, 200.0), nullptr, false},
        
        // STDP parameters
        {"stdp_ltp_weight", "double", "STDP LTP weight", 0.02, std::make_pair(0.0, 0.1), nullptr, false},
        {"stdp_ltd_weight", "double", "STDP LTD weight", 0.015, std::make_pair(0.0, 0.1), nullptr, false},
        {"stdp_tau", "double", "STDP time constant (ms)", 20.0, std::make_pair(1.0, 100.0), nullptr, false},
        
        // Structural plasticity parameters
        {"synaptogenesis_rate", "double", "Synaptogenesis rate per step", 0.0001, std::make_pair(0.0, 0.001), nullptr, false},
        {"pruning_rate", "double", "Pruning rate per step", 0.00001, std::make_pair(0.0, 0.0001), nullptr, false},
        
        // Memory system parameters
        {"working_memory_capacity", "int64_t", "Working memory capacity", 100, std::make_pair(10, 10000), nullptr, false},
        {"episode_max_length", "int64_t", "Maximum episode length", 100, std::make_pair(1, 10000), nullptr, false},
        {"episode_history", "int64_t", "Maximum episodes in history", 1000, std::make_pair(10, 10000), nullptr, false},
        
        // Neuromodulation parameters
        {"dopamine_level", "double", "Dopamine level (0.0-1.0)", 1.0, std::make_pair(0.0, 1.0), nullptr, false},
        {"curiosity_level", "double", "Curiosity level (0.0-1.0)", 1.0, std::make_pair(0.0, 1.0), nullptr, false},
        {"novelty_level", "double", "Novelty level (0.0-1.0)", 1.0, std::make_pair(0.0, 1.0), nullptr, false},
        {"prediction_error_level", "double", "Prediction error level (0.0-1.0)", 1.0, std::make_pair(0.0, 1.0), nullptr, false},
        
        // Development parameters
        {"developmental_stage", "string", "Developmental stage", "Initial", std::nullopt, nullptr, false},
        {"critical_period_end", "int64_t", "End of critical period (steps)", 1000, std::make_pair(100, 10000), nullptr, false},
        {"plasticity_decay_rate", "double", "Plasticity decay rate", 0.95, std::make_pair(0.0, 1.0), nullptr, false},
        {"development_plasticity_multiplier", "double", "Development plasticity multiplier", 2.0, std::make_pair(1.0, 5.0), nullptr, false},
        
        // Cognitive system parameters
        {"attention_inhibition_strength", "double", "Attention inhibition strength", 0.5, std::make_pair(0.0, 2.0), nullptr, false},
        {"attention_excitation_strength", "double", "Attention excitation strength", 1.5, std::make_pair(0.0, 3.0), nullptr, false},
        {"attention_window_size", "int64_t", "Attention window size (steps)", 50, std::make_pair(10, 200), nullptr, false},
        
        // Prediction system parameters
        {"prediction_horizon", "int64_t", "Prediction horizon (steps)", 10, std::make_pair(1, 100), nullptr, false},
        {"prediction_confidence_threshold", "double", "Prediction confidence threshold", 0.5, std::make_pair(0.0, 1.0), nullptr, false},
        
        // Concept formation parameters
        {"concept_threshold", "double", "Concept formation threshold", 0.7, std::make_pair(0.0, 1.0), nullptr, false},
        {"concept_decay_rate", "double", "Concept decay rate", 0.98, std::make_pair(0.0, 1.0), nullptr, false},
        
        // Working memory parameters
        {"working_memory_decay_rate", "double", "Working memory decay rate", 0.9, std::make_pair(0.0, 1.0), nullptr, false},
        
        // Episodic memory parameters
        {"episode_similarity_threshold", "double", "Episode similarity threshold", 0.8, std::make_pair(0.0, 1.0), nullptr, false},
        
        // Checkpoint parameters
        {"checkpoint_enabled", "bool", "Enable checkpoint saving", true, std::nullopt, nullptr, false},
        {"checkpoint_interval", "int64_t", "Checkpoint saving interval (steps)", 1000, std::make_pair(100, 10000), nullptr, false},
        {"checkpoint_max_files", "int64_t", "Maximum checkpoint files to keep", 5, std::make_pair(1, 50), nullptr, false},
        
        // System constraints
        {"max_neurons", "int64_t", "Maximum neurons allowed", 10000, std::make_pair(1000, 100000), nullptr, false},
        {"max_synapses", "int64_t", "Maximum synapses allowed", 100000, std::make_pair(10000, 1000000), nullptr, false},
        
        // Learning rates
        {"learning_rate", "double", "General learning rate", 0.001, std::make_pair(0.0, 0.1), nullptr, false},
        {"hebbian_learning_rate", "double", "Hebbian learning rate", 0.01, std::make_pair(0.0, 0.1), nullptr, false},
        {"STDP_learning_rate", "double", "STDP learning rate", 0.01, std::make_pair(0.0, 0.1), nullptr, false},
    };
}

// Validate configuration against parameter definitions
bool validateConfiguration(const Config& config) {
    const auto& params = getDefaultParameters();
    bool valid = true;
    
    for (const auto& paramDef : params) {
        if (!config.has(paramDef.name)) {
            if (paramDef.required) {
                NLM_LOG_WARNING("Missing required parameter: " + paramDef.name);
                valid = false;
            }
            continue;
        }
        
        // Validate parameter type
        std::optional<ConfigValue> value = config.get(paramDef.name);
        if (!value) {
            NLM_LOG_WARNING("Parameter type mismatch: " + paramDef.name);
            valid = false;
            continue;
        }
        
        // Validate range if specified
        if (paramDef.range.has_value()) {
            auto& range = paramDef.range.value();
            double numValue = 0.0;
            
            if (paramDef.type == "int" || paramDef.type == "int64_t") {
                try {
                    if (std::holds_alternative<int>(*value)) {
                        numValue = static_cast<double>(std::get<int>(*value));
                    } else if (std::holds_alternative<int64_t>(*value)) {
                        numValue = static_cast<double>(std::get<int64_t>(*value));
                    }
                } catch (...) {
                    NLM_LOG_WARNING("Parameter " + paramDef.name + " cannot be parsed as number");
                    valid = false;
                    continue;
                }
            } else if (paramDef.type == "double") {
                if (std::holds_alternative<double>(*value)) {
                    numValue = std::get<double>(*value);
                } else {
                    NLM_LOG_WARNING("Parameter " + paramDef.name + " should be a number");
                    valid = false;
                    continue;
                }
            }
            
            if (numValue < range.first || numValue > range.second) {
                NLM_LOG_WARNING("Parameter " + paramDef.name + " out of range [" + 
                               std::to_string(range.first) + ", " + std::to_string(range.second) + "]");
                valid = false;
            }
        }
        
        // Apply custom validator if provided
        if (paramDef.validator && !paramDef.validator(*value)) {
            NLM_LOG_WARNING("Parameter " + paramDef.name + " failed validation");
            valid = false;
        }
    }
    
    return valid;
}

// Apply default values for missing parameters
void applyDefaultParameters(Config& config) {
    const auto& params = getDefaultParameters();
    
    for (const auto& paramDef : params) {
        if (!config.has(paramDef.name)) {
            // Apply default value
            switch (paramDef.defaultValue.index()) {
                case 0: // int
                    config.set(paramDef.name, std::get<int>(paramDef.defaultValue), ConfigSource::Default);
                    break;
                case 1: // int64_t
                    config.set(paramDef.name, std::get<int64_t>(paramDef.defaultValue), ConfigSource::Default);
                    break;
                case 2: // double
                    config.set(paramDef.name, std::get<double>(paramDef.defaultValue), ConfigSource::Default);
                    break;
                case 3: // bool
                    config.set(paramDef.name, std::get<bool>(paramDef.defaultValue), ConfigSource::Default);
                    break;
                case 4: // string
                    config.set(paramDef.name, std::get<std::string>(paramDef.defaultValue), ConfigSource::Default);
                    break;
            }
        }
    }
}

// Parameter consistency validation
bool validateParameterConsistency(const Config& config) {
    bool valid = true;
    
    // Validate that neuron count is reasonable
    if (auto neuronsOpt = config.get<int64_t>("neuron_count")) {
        if (*neuronsOpt > 10000) {
            NLM_LOG_WARNING("Large neuron count may impact performance: " + std::to_string(*neuronsOpt));
        }
    }
    
    // Validate that connection probability is reasonable
    if (auto connProbOpt = config.get<double>("connection_probability")) {
        if (*connProbOpt < 0.01 || *connProbOpt > 0.5) {
            NLM_LOG_WARNING("Connection probability seems extreme: " + std::to_string(*connProbOpt));
        }
    }
    
    // Validate voltage parameters
    if (auto vMinOpt = config.get<double>("lif_voltage_clamp_min")) {
        if (auto vMaxOpt = config.get<double>("lif_voltage_clamp_max")) {
            if (*vMinOpt >= *vMaxOpt) {
                NLM_LOG_WARNING("Voltage clamp min must be less than max");
                valid = false;
            }
        }
    }
    
    // Validate developmental stage consistency
    if (auto stageOpt = config.get<std::string>("developmental_stage")) {
        std::string stage = *stageOpt;
        if (stage != "Initial" && stage != "CriticalPeriod" && stage != "Maturation" && 
            stage != "Adult" && stage != "Aging") {
            NLM_LOG_WARNING("Unknown developmental stage: " + stage);
            valid = false;
        }
    }
    
    return valid;
}

// Get parameter descriptions for documentation
def
getParameterDescriptions() {
    const auto& params = getDefaultParameters();
    std::ostringstream oss;
    oss << "NLM Configuration Parameters:\n\n";
    
    for (const auto& param : params) {
        oss << "Parameter: " << param.name << "\n";
        oss << "  Type: " << param.type << "\n";
        oss << "  Description: " << param.description << "\n";
        oss << "  Default: ";
        switch (param.defaultValue.index()) {
            case 0: oss << std::get<int>(param.defaultValue); break;
            case 1: oss << std::get<int64_t>(param.defaultValue); break;
            case 2: oss << std::get<double>(param.defaultValue); break;
            case 3: oss << (std::get<bool>(param.defaultValue) ? "true" : "false"); break;
            case 4: oss << "\"" << std::get<std::string>(param.defaultValue) << "\""; break;
        }
        
        if (param.range.has_value()) {
            oss << "  Range: [" << param.range->first << ", " << param.range->second << "]";
        }
        oss << "\n\n";
    }
    
    return oss.str();
}

} // namespace

bool Config::loadFromFile(const std::string& filepath) {
    // First, try to load as JSON using RapidJSON
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try JSON parsing first
    rapidjson::IStreamWrapper isw(file);
    rapidjson::Document doc;
    
    if (doc.ParseStream(isw).HasParseError()) {
        // JSON parsing failed, try the old format
        file.clear();
        file.seekg(0, std::ios::beg);
        
        std::string line;
        while (std::getline(file, line)) {
            auto parsed = parseLine(line);
            if (parsed) {
                set(parsed->type == "string" ? std::get<std::string>(parsed->value) :
                    parsed->type == "int64_t" ? std::get<int64_t>(parsed->value) :
                    parsed->type == "int" ? std::get<int>(parsed->value) :
                    parsed->type == "double" ? std::get<double>(parsed->value) :
                    std::get<bool>(parsed->value), ConfigSource::File);
            }
        }
    } else {
        // JSON parsing successful, extract values
        for (rapidjson::Value::ConstMemberIterator itr = doc.MemberBegin(); 
             itr != doc.MemberEnd(); ++itr) {
            const std::string key = itr->name.GetString();
            
            if (itr->value.IsString()) {
                set(key, itr->value.GetString(), ConfigSource::File);
            } else if (itr->value.IsInt()) {
                set(key, itr->value.GetInt(), ConfigSource::File);
            } else if (itr->value.IsInt64()) {
                set(key, itr->value.GetInt64(), ConfigSource::File);
            } else if (itr->value.IsDouble()) {
                set(key, itr->value.GetDouble(), ConfigSource::File);
            } else if (itr->value.IsBool()) {
                set(key, itr->value.GetBool(), ConfigSource::File);
            }
        }
    }
    
    // Apply default values and validate
    applyDefaultParameters(*this);
    validateParameterConsistency(*this);
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Serialize the actual value based on type
        std::visit([&file](auto&& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << value << "\"";
            } else if constexpr (std::is_same_v<T, int> || 
                                 std::is_same_v<T, int64_t>) {
                file << value;
            } else if constexpr (std::is_same_v<T, double>) {
                // Format with reasonable precision
                file << std::fixed << std::setprecision(6) << value;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (value ? "true" : "false");
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

bool Config::validate() const {
    return validateConfiguration(*this);
}

std::string Config::getParameterDocumentation() {
    return getParameterDescriptions();
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::printParameterValidationReport() const {
    std::cout << "=== NLM Configuration Validation Report ===\n\n";
    
    bool allValid = validateConfiguration(*this);
    
    std::cout << "Validation: " << (allValid ? "PASS" : "FAIL") << "\n\n";
    
    std::cout << "Parameters:\n";
    for (const auto& entry : pImpl->entries) {
        std::cout << "  " << entry.key << " = ";
        std::visit([&](auto&& val) {
            if constexpr (std::is_same_v<decltype(val), std::string>) {
                std::cout << "\"" << val << "\"";
            } else {
                std::cout << val;
            }
        }, entry.value);
        std::cout << " (from " << static_cast<int>(entry.source) << ")\n";
    }
    
    std::cout << "\n=== End Report ===\n";
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
        file << entry.key << " = ";
        
        // Serialize the actual value based on type
        std::visit([&file](auto&& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << value << "\"";
            } else if constexpr (std::is_same_v<T, int> || 
                                 std::is_same_v<T, int64_t>) {
                file << value;
            } else if constexpr (std::is_same_v<T, double>) {
                // Format with reasonable precision
                file << std::fixed << std::setprecision(6) << value;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (value ? "true" : "false");
            }
        }, entry.value);
        
        file << "\n\n";
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
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
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
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

} // namespace nlm
