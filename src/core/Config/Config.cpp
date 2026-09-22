#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <limits>
#include <map>
#include <sstream>
#include <iomanip>

namespace nlm {

// ConfigValueAccessor implementations
int ConfigValueAccessor::asInt(const ConfigValue& value, int defaultValue) {
    try {
        return std::get<int>(value);
    } catch (const std::bad_variant_access&) { return defaultValue; }
}

float ConfigValueAccessor::asFloat(const ConfigValue& value, float defaultValue) {
    try {
        // Handle all numeric types
        if (auto val = std::get_if<int>(&value)) return static_cast<float>(*val);
        if (auto val = std::get_if<int64_t>(&value)) return static_cast<float>(*val);
        if (auto val = std::get_if<double>(&value)) return static_cast<float>(*val);
        if (auto val = std::get_if<bool>(&value)) return *val ? 1.0f : 0.0f;
        return defaultValue;
    } catch (const std::bad_variant_access&) { return defaultValue; }
}

std::string ConfigValueAccessor::asString(const ConfigValue& value, const std::string& defaultValue) {
    try {
        return std::get<std::string>(value);
    } catch (const std::bad_variant_access&) { return defaultValue; }
}

bool ConfigValueAccessor::asBool(const ConfigValue& value, bool defaultValue) {
    try {
        // Handle bool and numeric (0=false, non-zero=true)
        if (auto val = std::get_if<bool>(&value)) return *val;
        if (auto val = std::get_if<int>(&value)) return *val != 0;
        if (auto val = std::get_if<int64_t>(&value)) return *val != 0;
        if (auto val = std::get_if<double>(&value)) return *val != 0.0;
        return defaultValue;
    } catch (const std::bad_variant_access&) { return defaultValue; }
}

template<typename T>
std::vector<T> ConfigValueAccessor::asVector(const ConfigValue& value, const std::vector<T>& defaultValue) {
    try {
        return std::get<std::vector<T>>(value);
    } catch (const std::bad_variant_access&) {
        return defaultValue;
    }
}

bool ConfigValueAccessor::isType(const ConfigValue& value, const std::string& typeName) {
    if (typeName == "int" && std::get_if<int>(&value)) return true;
    if (typeName == "int64_t" && std::get_if<int64_t>(&value)) return true;
    if (typeName == "double" && std::get_if<double>(&value)) return true;
    if (typeName == "bool" && std::get_if<bool>(&value)) return true;
    if (typeName == "string" && std::get_if<std::string>(&value)) return true;
    if (typeName == "vector<int>" && std::get_if<std::vector<int>>(&value)) return true;
    if (typeName == "vector<double>" && std::get_if<std::vector<double>>(&value)) return true;
    if (typeName == "vector<string>" && std::get_if<std::vector<std::string>>(&value)) return true;
    if (typeName == "vector<float>" && std::get_if<std::vector<float>>(&value)) return true;
    if (typeName == "vector<bool>" && std::get_if<std::vector<bool>>(&value)) return true;
    return false;
}

std::string ConfigValueAccessor::getTypeName(const ConfigValue& value) {
    if (std::get_if<int>(&value)) return "int";
    if (std::get_if<int64_t>(&value)) return "int64_t";
    if (std::get_if<double>(&value)) return "double";
    if (std::get_if<bool>(&value)) return "bool";
    if (std::get_if<std::string>(&value)) return "string";
    if (std::get_if<std::vector<int>>(&value)) return "vector<int>";
    if (std::get_if<std::vector<double>>(&value)) return "vector<double>";
    if (std::get_if<std::vector<std::string>>(&value)) return "vector<string>";
    if (std::get_if<std::vector<float>>(&value)) return "vector<float>";
    if (std::get_if<std::vector<bool>>(&value)) return "vector<bool>";
    return "unknown";
}

// Explicit template instantiations for vector types
template std::vector<int> ConfigValueAccessor::asVector<int>(const ConfigValue& value, const std::vector<int>& defaultValue);
template std::vector<double> ConfigValueAccessor::asVector<double>(const ConfigValue& value, const std::vector<double>& defaultValue);
template std::vector<std::string> ConfigValueAccessor::asVector<std::string>(const ConfigValue& value, const std::vector<std::string>& defaultValue);
template std::vector<float> ConfigValueAccessor::asVector<float>(const ConfigValue& value, const std::vector<float>& defaultValue);
template std::vector<bool> ConfigValueAccessor::asVector<bool>(const ConfigValue& value, const std::vector<bool>& defaultValue);

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigValue> defaults;
    std::vector<std::string> validationErrors;
    
    Impl() {
        // Initialize with default configuration values
        initializeDefaults();
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

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
                ((value.front() == '"' && value.back() == '"') ||
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
    
    file << "# NLM Configuration File\n";
    file << "# Generated on " << __DATE__ << " " << __TIME__ << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << " (Category: " << entry.category << ")\n";
        if (entry.isAdvanced) file << "# [Advanced setting]\n";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        
        file << "\n";
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

template<typename T>
void Config::set(const std::string& key, const T& value, ConfigSource source,
                 const std::string& description, const std::string& category,
                 bool isAdvanced) {
    // Validate based on type
    bool valid = true;
    if constexpr (std::is_same_v<T, int>) {
        valid = validateInt(key, value, std::numeric_limits<int>::min(), 
                           std::numeric_limits<int>::max());
    } else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
        valid = validateFloat(key, static_cast<float>(value), std::numeric_limits<float>::lowest(),
                             std::numeric_limits<float>::max());
    } else if constexpr (std::is_same_v<T, std::string>) {
        valid = validateString(key, value, 1024);
    }
    
    if (valid) {
        set<ConfigValue>(key, value, source);
        // Store metadata if this is the first time setting
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&key](const ConfigEntry& e) { return e.key == key; });
        if (it != pImpl->entries.end() && description.empty() && category == "General" && !isAdvanced) {
            // Already exists, don't overwrite metadata
        } else {
            it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                             [&key](const ConfigEntry& e) { return e.key == key; });
            if (it == pImpl->entries.end()) {
                // New entry, store metadata
                pImpl->entries.emplace_back(key, ConfigValue(value), source, 
                                          description.empty() ? "" : description,
                                          category.empty() ? "General" : category,
                                          isAdvanced);
            } else {
                // Existing entry, update only value and source
                it->value = ConfigValue(value);
                it->source = source;
            }
        }
    }
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

BrainConfig Config::getBrainConfig() const {
    BrainConfig config;
    
    // Get values with fallback to defaults
    config.neuronCount = getOr<int>("brain.neuron_count", config.neuronCount);
    config.regionCount = getOr<int>("brain.region_count", config.regionCount);
    config.connectionProbability = getOr<float>("brain.connection_probability", config.connectionProbability);
    config.initialWeightMean = getOr<float>("brain.initial_weight_mean", config.initialWeightMean);
    config.initialWeightStd = getOr<float>("brain.initial_weight_std", config.initialWeightStd);
    config.v_thresh = getOr<float>("brain.v_thresh", config.v_thresh);
    config.v_rest = getOr<float>("brain.v_rest", config.v_rest);
    config.v_reset = getOr<float>("brain.v_reset", config.v_reset);
    config.tau_mem = getOr<float>("brain.tau_mem", config.tau_mem);
    config.tau_ref = getOr<float>("brain.tau_ref", config.tau_ref);
    
    return config;
}

PlasticityConfig Config::getPlasticityConfig() const {
    PlasticityConfig config;
    
    config.stdpEnable = getOr<bool>("plasticity.stdp.enable", config.stdpEnable);
    config.hebbianEnable = getOr<bool>("plasticity.hebbian.enable", config.hebbianEnable);
    config.structuralEnable = getOr<bool>("plasticity.structural.enable", config.structuralEnable);
    config.stdpLtpWeight = getOr<float>("plasticity.stdp.ltp_weight", config.stdpLtpWeight);
    config.stdpLtdWeight = getOr<float>("plasticity.stdp.ltd_weight", config.stdpLtdWeight);
    config.stdpTau = getOr<float>("plasticity.stdp.tau", config.stdpTau);
    config.hebbianLearningRate = getOr<float>("plasticity.hebbian.learning_rate", config.hebbianLearningRate);
    config.synaptogenesisRate = getOr<float>("plasticity.structural.synaptogenesis_rate", config.synaptogenesisRate);
    config.pruningRate = getOr<float>("plasticity.structural.pruning_rate", config.pruningRate);
    
    return config;
}

NeuromodulationConfig Config::getNeuromodulationConfig() const {
    NeuromodulationConfig config;
    
    config.dopamineScale = getOr<float>("neuromod.dopamine.scale", config.dopamineScale);
    config.curiosityEnable = getOr<bool>("neuromod.curiosity.enable", config.curiosityEnable);
    config.noveltyEnable = getOr<bool>("neuromod.novelty.enable", config.noveltyEnable);
    config.dopamineLearningRate = getOr<float>("neuromod.dopamine.learning_rate", config.dopamineLearningRate);
    config.curiosityDecayRate = getOr<float>("neuromod.curiosity.decay_rate", config.curiosityDecayRate);
    
    return config;
}

void Config::setBrainConfig(const BrainConfig& config) {
    set("brain.neuron_count", config.neuronCount, ConfigSource::Runtime, 
        "Total number of neurons in the brain", "Brain", false);
    set("brain.region_count", config.regionCount, ConfigSource::Runtime,
        "Number of brain regions", "Brain", false);
    set("brain.connection_probability", config.connectionProbability, ConfigSource::Runtime,
        "Probability of connection between neurons", "Brain", false);
    set("brain.initial_weight_mean", config.initialWeightMean, ConfigSource::Runtime,
        "Mean initial synaptic weight", "Brain", false);
    set("brain.initial_weight_std", config.initialWeightStd, ConfigSource::Runtime,
        "Standard deviation of initial weights", "Brain", false);
    set("brain.v_thresh", config.v_thresh, ConfigSource::Runtime,
        "Neuron threshold potential (mV)", "Brain", false);
    set("brain.v_rest", config.v_rest, ConfigSource::Runtime,
        "Resting potential (mV)", "Brain", false);
    set("brain.v_reset", config.v_reset, ConfigSource::Runtime,
        "Reset potential after spike (mV)", "Brain", false);
    set("brain.tau_mem", config.tau_mem, ConfigSource::Runtime,
        "Membrane time constant (ms)", "Brain", false);
    set("brain.tau_ref", config.tau_ref, ConfigSource::Runtime,
        "Refractory period (ms)", "Brain", false);
}

void Config::setPlasticityConfig(const PlasticityConfig& config) {
    set("plasticity.stdp.enable", config.stdpEnable, ConfigSource::Runtime,
        "Enable Spike-Timing Dependent Plasticity", "Plasticity", false);
    set("plasticity.hebbian.enable", config.hebbianEnable, ConfigSource::Runtime,
        "Enable Hebbian learning", "Plasticity", false);
    set("plasticity.structural.enable", config.structuralEnable, ConfigSource::Runtime,
        "Enable structural plasticity", "Plasticity", false);
    set("plasticity.stdp.ltp_weight", config.stdpLtpWeight, ConfigSource::Runtime,
        "STDP potentiation weight", "Plasticity", true);
    set("plasticity.stdp.ltd_weight", config.stdpLtdWeight, ConfigSource::Runtime,
        "STDP depression weight", "Plasticity", true);
    set("plasticity.stdp.tau", config.stdpTau, ConfigSource::Runtime,
        "STDP time constant (ms)", "Plasticity", true);
    set("plasticity.hebbian.learning_rate", config.hebbianLearningRate, ConfigSource::Runtime,
        "Hebbian learning rate", "Plasticity", true);
    set("plasticity.structural.synaptogenesis_rate", config.synaptogenesisRate, ConfigSource::Runtime,
        "Rate of new synapse formation", "Plasticity", true);
    set("plasticity.structural.pruning_rate", config.pruningRate, ConfigSource::Runtime,
        "Rate of synapse pruning", "Plasticity", true);
}

void Config::setNeuromodulationConfig(const NeuromodulationConfig& config) {
    set("neuromod.dopamine.scale", config.dopamineScale, ConfigSource::Runtime,
        "Dopamine modulation scale", "Neuromodulation", true);
    set("neuromod.curiosity.enable", config.curiosityEnable, ConfigSource::Runtime,
        "Enable curiosity-driven exploration", "Neuromodulation", false);
    set("neuromod.novelty.enable", config.noveltyEnable, ConfigSource::Runtime,
        "Enable novelty detection", "Neuromodulation", false);
    set("neuromod.dopamine.learning_rate", config.dopamineLearningRate, ConfigSource::Runtime,
        "Dopamine learning rate", "Neuromodulation", true);
    set("neuromod.curiosity.decay_rate", config.curiosityDecayRate, ConfigSource::Runtime,
        "Curiosity decay rate", "Neuromodulation", true);
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

std::vector<std::string> Config::getKeysByCategory(const std::string& category) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.category == category) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "NLM Configuration Summary\n";
    oss << "========================\n\n";
    
    std::map<std::string, std::vector<const ConfigEntry*>> categorized;
    for (const auto& entry : pImpl->entries) {
        categorized[entry.category].push_back(&entry);
    }
    
    for (const auto& [category, entries] : categorized) {
        oss << "Category: " << category << "\n";
        for (const auto* entry : entries) {
            oss << "  " << entry->key << " = ";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    oss << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        oss << "\"" << arg[i] << "\"";
                        if (i < arg.size() - 1) oss << ", ";
                    }
                    oss << "]";
                } else {
                    oss << arg;
                }
            }, entry->value);
            oss << " (Source: " << static_cast<int>(entry->source) << ")";
            if (entry->isAdvanced) oss << " [ADVANCED]";
            oss << "\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::map<std::string, std::string> Config::toMap() const {
    std::map<std::string, std::string> map;
    for (const auto& entry : pImpl->entries) {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            oss << arg;
        }, entry.value);
        map[entry.key] = oss.str();
    }
    return map;
}

std::vector<std::string> Config::validate() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        bool valid = true;
        std::string error;
        
        try {
            if (auto val = std::get_if<int>(&entry.value)) {
                if (!validateInt(entry.key, *val, std::numeric_limits<int>::min(), std::numeric_limits<int>::max())) {
                    error = "Invalid int value for " + entry.key;
                    valid = false;
                }
            } else if (auto val = std::get_if<double>(&entry.value)) {
                if (!validateFloat(entry.key, static_cast<float>(*val), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max())) {
                    error = "Invalid float value for " + entry.key;
                    valid = false;
                }
            } else if (auto val = std::get_if<std::string>(&entry.value)) {
                if (!validateString(entry.key, *val, 1024)) {
                    error = "Invalid string value for " + entry.key + " (too long)";
                    valid = false;
                }
            }
            
            // Category-specific validations
            if (entry.category == "Brain") {
                if (entry.key == "brain.neuron_count" && *std::get_if<int>(&entry.value) <= 0) {
                    error = "brain.neuron_count must be positive";
                    valid = false;
                } else if (entry.key == "brain.region_count" && *std::get_if<int>(&entry.value) <= 0) {
                    error = "brain.region_count must be positive";
                    valid = false;
                } else if (entry.key == "brain.connection_probability" && 
                          (*std::get_if<double>(&entry.value) < 0.0 || *std::get_if<double>(&entry.value) > 1.0)) {
                    error = "brain.connection_probability must be between 0.0 and 1.0";
                    valid = false;
                }
            }
            
        } catch (const std::bad_variant_access&) {
            error = "Unknown type for " + entry.key;
            valid = false;
        }
        
        if (!valid) {
            errors.push_back(error);
        }
    }
    
    return errors;
}

std::optional<ConfigValue> Config::getDefault(const std::string& key) const {
    auto it = pImpl->defaults.find(key);
    if (it != pImpl->defaults.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool Config::isModified(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    auto defaultIt = pImpl->defaults.find(key);
    if (defaultIt == pImpl->defaults.end()) {
        return true; // No default but has a value, so it's modified
    }
    
    return it->value != defaultIt->second;
}

void Config::initializeDefaults() {
    // Set up default configuration values for backward compatibility
    defaults["random_seed"] = ConfigValue(static_cast<int64_t>(42));
    defaults["simulation_timestep"] = ConfigValue(0.001);
    defaults["neuron_count"] = ConfigValue(static_cast<int64_t>(1000));
    defaults["region_count"] = ConfigValue(static_cast<int64_t>(1));
    defaults["connection_probability"] = ConfigValue(0.1f);
    defaults["stdp_ltp_weight"] = ConfigValue(0.01f);
    defaults["stdp_ltd_weight"] = ConfigValue(0.012f);
    defaults["stdp_tau"] = ConfigValue(20.0f);
    defaults["synaptogenesis_rate"] = ConfigValue(0.0001f);
    defaults["pruning_rate"] = ConfigValue(0.00001f);
    
    // Brain defaults (convenience names)
    defaults["brain.neuron_count"] = defaults["neuron_count"];
    defaults["brain.region_count"] = defaults["region_count"];
    defaults["brain.connection_probability"] = defaults["connection_probability"];
    defaults["brain.initial_weight_mean"] = ConfigValue(0.5f);
    defaults["brain.initial_weight_std"] = ConfigValue(0.1f);
    defaults["brain.v_thresh"] = ConfigValue(-50.0f);
    defaults["brain.v_rest"] = ConfigValue(-70.0f);
    defaults["brain.v_reset"] = ConfigValue(-75.0f);
    defaults["brain.tau_mem"] = ConfigValue(20.0f);
    defaults["brain.tau_ref"] = ConfigValue(2.0f);
    
    // Plasticity defaults (convenience names)
    defaults["plasticity.stdp.enable"] = ConfigValue(true);
    defaults["plasticity.hebbian.enable"] = ConfigValue(true);
    defaults["plasticity.structural.enable"] = ConfigValue(true);
    defaults["plasticity.stdp.ltp_weight"] = defaults["stdp_ltp_weight"];
    defaults["plasticity.stdp.ltd_weight"] = defaults["stdp_ltd_weight"];
    defaults["plasticity.stdp.tau"] = defaults["stdp_tau"];
    defaults["plasticity.hebbian.learning_rate"] = ConfigValue(0.01f);
    defaults["plasticity.structural.synaptogenesis_rate"] = defaults["synaptogenesis_rate"];
    defaults["plasticity.structural.pruning_rate"] = defaults["pruning_rate"];
    
    // Neuromodulation defaults
    defaults["neuromod.dopamine.scale"] = ConfigValue(1.0f);
    defaults["neuromod.curiosity.enable"] = ConfigValue(true);
    defaults["neuromod.novelty.enable"] = ConfigValue(true);
    defaults["neuromod.dopamine.learning_rate"] = ConfigValue(0.001f);
    defaults["neuromod.curiosity.decay_rate"] = ConfigValue(0.01f);
}

bool Config::validateInt(const std::string& key, int value, int min, int max) {
    return value >= min && value <= max;
}

bool Config::validateFloat(const std::string& key, float value, float min, float max) {
    return value >= min && value <= max;
}

bool Config::validateString(const std::string& key, const std::string& value, size_t maxLength) {
    return value.length() <= maxLength;
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

std::string Config::toSnakeCase(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.size(); ++i) {
        if (std::isupper(str[i])) {
            if (i > 0 && !std::isupper(str[i - 1])) {
                result += '_';
            }
            result += std::tolower(str[i]);
        } else {
            result += str[i];
        }
    }
    return result;
}

std::string Config::toCamelCase(const std::string& str) {
    std::string result;
    bool capitalizeNext = false;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '_' || str[i] == '-') {
            capitalizeNext = true;
        } else if (capitalizeNext) {
            result += std::toupper(str[i]);
            capitalizeNext = false;
        } else {
            result += str[i];
        }
    }
    return result;
}

// Explicit template instantiations
namespace nlm {
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
    
    template void Config::set<int>(const std::string&, const int&, ConfigSource, const std::string&, const std::string&, bool);
    template void Config::set<float>(const std::string&, const float&, ConfigSource, const std::string&, const std::string&, bool);
    template void Config::set<double>(const std::string&, const double&, ConfigSource, const std::string&, const std::string&, bool);
    template void Config::set<std::string>(const std::string&, const std::string&, ConfigSource, const std::string&, const std::string&, bool);
    template void Config::set<bool>(const std::string&, const bool&, ConfigSource, const std::string&, const std::string&, bool);
}

} // namespace nlm

// Required includes for template implementations
#include <limits>
#include <stdexcept>
