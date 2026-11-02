#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

ValidationResult Config::validate() const {
    ValidationResult result;
    
    // Check for required keys
    std::vector<std::string> requiredKeys = {
        "neuron_count",
        "region_count",
        "connection_probability",
        "simulation_timestep",
        "random_seed"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            result.errors.push_back("Missing required configuration key: " + key);
        }
    }
    
    // Validate each key
    for (const auto& entry : pImpl->entries) {
        std::vector<std::string> keyErrors = validateKey(entry.key);
        if (!keyErrors.empty()) {
            for (const auto& error : keyErrors) {
                result.errors.push_back("Key \"" + entry.key + "\": " + error);
            }
        }
        
        // Add to validated values
        std::ostringstream valueStr;
        std::visit([&valueStr](auto&& arg) {
            valueStr << arg;
        }, entry.value);
        result.validatedValues[entry.key] = valueStr.str();
    }
    
    // Check consistency between related keys
    std::vector<std::string> consistencyErrors = validateConsistency();
    if (!consistencyErrors.empty()) {
        for (const auto& error : consistencyErrors) {
            result.errors.push_back("Consistency error: " + error);
        }
    }
    
    // Check for warnings
    for (const auto& entry : pImpl->entries) {
        std::vector<std::string> keyWarnings = validateKeyWarnings(entry.key);
        if (!keyWarnings.empty()) {
            for (const auto& warning : keyWarnings) {
                result.warnings.push_back("Key \"" + entry.key + "\": " + warning);
            }
        }
    }
    
    result.valid = result.errors.empty();
    
    return result;
}

std::vector<std::string> Config::validateKey(const std::string& key) const {
    std::vector<std::string> errors;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return errors; // Key doesn't exist, don't report error here (handled by required keys check)
    }
    
    const auto& entry = *it;
    
    // Type-specific validation
    if (key == "neuron_count") {
        auto value = std::get_if<int64_t>(&entry.value);
        if (value) {
            if (*value <= 0 || *value > 100000) {
                errors.push_back("neuron_count " + std::to_string(*value) + " is outside valid range (1-100000)");
            }
        }
    } else if (key == "region_count") {
        auto value = std::get_if<int64_t>(&entry.value);
        if (value) {
            if (*value <= 0 || *value > 100) {
                errors.push_back("region_count " + std::to_string(*value) + " is outside valid range (1-100)");
            }
        }
    } else if (key == "connection_probability") {
        auto value = std::get_if<double>(&entry.value);
        if (value) {
            if (*value < 0.0 || *value > 1.0) {
                errors.push_back("connection_probability " + std::to_string(*value) + " is outside valid range (0.0-1.0)");
            }
        }
    } else if (key == "simulation_timestep") {
        auto value = std::get_if<double>(&entry.value);
        if (value) {
            if (*value <= 0.0 || *value > 1.0) {
                errors.push_back("simulation_timestep " + std::to_string(*value) + " is outside valid range (0.0001-1.0)");
            }
        }
    } else if (key == "random_seed") {
        auto value = std::get_if<int64_t>(&entry.value);
        if (value) {
            if (*value < 0) {
                errors.push_back("random_seed " + std::to_string(*value) + " must be non-negative");
            }
        }
    } else if (key == "plasticity_learning_rate") {
        auto value = std::get_if<double>(&entry.value);
        if (value) {
            if (*value < 0.0 || *value > 1.0) {
                errors.push_back("plasticity_learning_rate " + std::to_string(*value) + " is outside valid range (0.0-1.0)");
            }
        }
    } else if (key == "stdp_ltp_weight") {
        auto value = std::get_if<double>(&entry.value);
        if (value) {
            if (*value < 0.0 || *value > 1.0) {
                errors.push_back("stdp_ltp_weight " + std::to_string(*value) + " is outside valid range (0.0-1.0)");
            }
        }
    } else if (key == "stdp_ltd_weight") {
        auto value = std::get_if<double>(&entry.value);
        if (value) {
            if (*value < 0.0 || *value > 1.0) {
                errors.push_back("stdp_ltd_weight " + std::to_string(*value) + " is outside valid range (0.0-1.0)");
            }
        }
    } else if (key == "stdp_tau") {
        auto value = std::get_if<double>(&entry.value);
        if (value) {
            if (*value <= 0.0) {
                errors.push_back("stdp_tau " + std::to_string(*value) + " must be positive");
            }
        }
    } else if (key == "synaptogenesis_rate") {
        auto value = std::get_if<double>(&entry.value);
        if (value) {
            if (*value < 0.0 || *value > 0.1) {
                errors.push_back("synaptogenesis_rate " + std::to_string(*value) + " is outside typical range (0.0-0.1)");
            }
        }
    } else if (key == "pruning_rate") {
        auto value = std::get_if<double>(&entry.value);
        if (value) {
            if (*value < 0.0 || *value > 0.01) {
                errors.push_back("pruning_rate " + std::to_string(*value) + " is outside typical range (0.0-0.01)");
            }
        }
    } else if (key == "replay_interval") {
        auto value = std::get_if<int64_t>(&entry.value);
        if (value) {
            if (*value <= 0) {
                errors.push_back("replay_interval " + std::to_string(*value) + " must be positive");
            }
        }
    } else if (key == "consolidation_interval") {
        auto value = std::get_if<int64_t>(&entry.value);
        if (value) {
            if (*value <= 0) {
                errors.push_back("consolidation_interval " + std::to_string(*value) + " must be positive");
            }
        }
    } else if (key == "dopamine_baseline") {
        auto value = std::get_if<double>(&entry.value);
        if (value) {
            if (*value < 0.0 || *value > 10.0) {
                errors.push_back("dopamine_baseline " + std::to_string(*value) + " is outside typical range (0.0-10.0)");
            }
        }
    } else if (key == "dopamine_scale") {
        auto value = std::get_if<double>(&entry.value);
        if (value) {
            if (*value < 0.0 || *value > 10.0) {
                errors.push_back("dopamine_scale " + std::to_string(*value) + " is outside typical range (0.0-10.0)");
            }
        }
    } else if (key == "curiosity_enable") {
        auto value = std::get_if<bool>(&entry.value);
        if (value) {
            // No range validation for boolean values
        }
    } else if (key == "novelty_enable") {
        auto value = std::get_if<bool>(&entry.value);
        if (value) {
            // No range validation for boolean values
        }
    } else if (key == "structural_plasticity_enable") {
        auto value = std::get_if<bool>(&entry.value);
        if (value) {
            // No range validation for boolean values
        }
    } else if (key == "hebbian_enable") {
        auto value = std::get_if<bool>(&entry.value);
        if (value) {
            // No range validation for boolean values
        }
    } else if (key == "reward_discount_factor") {
        auto value = std::get_if<double>(&entry.value);
        if (value) {
            if (*value < 0.0 || *value > 1.0) {
                errors.push_back("reward_discount_factor " + std::to_string(*value) + " is outside valid range (0.0-1.0)");
            }
        }
    } else if (key == "environment_width") {
        auto value = std::get_if<int64_t>(&entry.value);
        if (value) {
            if (*value <= 0 || *value > 1000) {
                errors.push_back("environment_width " + std::to_string(*value) + " is outside valid range (1-1000)");
            }
        }
    } else if (key == "environment_height") {
        auto value = std::get_if<int64_t>(&entry.value);
        if (value) {
            if (*value <= 0 || *value > 1000) {
                errors.push_back("environment_height " + std::to_string(*value) + " is outside valid range (1-1000)");
            }
        }
    } else if (key == "visualization_update_rate") {
        auto value = std::get_if<double>(&entry.value);
        if (value) {
            if (*value <= 0.0) {
                errors.push_back("visualization_update_rate " + std::to_string(*value) + " must be positive");
            }
        }
    } else if (key == "max_simulation_steps") {
        auto value = std::get_if<int64_t>(&entry.value);
        if (value) {
            if (*value <= 0 || *value > 1000000) {
                errors.push_back("max_simulation_steps " + std::to_string(*value) + " is outside valid range (1-1000000)");
            }
        }
    }
    
    return errors;
}

std::vector<std::string> Config::validateKeyWarnings(const std::string& key) const {
    std::vector<std::string> warnings;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return warnings; // Key doesn't exist
    }
    
    const auto& entry = *it;
    
    // Warning validation for performance-related parameters
    if (key == "neuron_count") {
        auto value = std::get_if<int64_t>(&entry.value);
        if (value && *value > 10000) {
            warnings.push_back("Large neuron count may impact performance");
        }
    } else if (key == "region_count") {
        auto value = std::get_if<int64_t>(&entry.value);
        if (value && *value > 10) {
            warnings.push_back("Large region count may impact performance");
        }
    } else if (key == "connection_probability") {
        auto value = std::get_if<double>(&entry.value);
        if (value && *value > 0.5) {
            warnings.push_back("High connection probability may create dense connectivity, impacting performance");
        }
    } else if (key == "simulation_timestep") {
        auto value = std::get_if<double>(&entry.value);
        if (value && *value < 0.001) {
            warnings.push_back("Very small timestep may require more simulation steps");
        }
    } else if (key == "stdp_ltp_weight") {
        auto value = std::get_if<double>(&entry.value);
        if (value && *value > 0.05) {
            warnings.push_back("High STDP LTP weight may cause rapid weight saturation");
        }
    } else if (key == "synaptogenesis_rate") {
        auto value = std::get_if<double>(&entry.value);
        if (value && *value > 0.01) {
            warnings.push_back("High synaptogenesis rate may create excessive new connections");
        }
    } else if (key == "pruning_rate") {
        auto value = std::get_if<double>(&entry.value);
        if (value && *value < 0.00001) {
            warnings.push_back("Low pruning rate may lead to excessive synaptic growth");
        }
    }
    
    return warnings;
}

std::vector<std::string> Config::validateConsistency() const {
    std::vector<std::string> errors;
    
    // Check consistency between neuron_count and region_count
    auto neuronCount = getOr<int64_t>("neuron_count", 1000);
    auto regionCount = getOr<int64_t>("region_count", 1);
    
    if (neuronCount && regionCount) {
        double neuronsPerRegion = static_cast<double>(*neuronCount) / *regionCount;
        if (neuronsPerRegion < 10) {
            errors.push_back("neuron_count and region_count create too few neurons per region (should be at least 10)");
        } else if (neuronsPerRegion > 10000) {
            errors.push_back("neuron_count and region_count create too many neurons per region (should be at most 10000)");
        }
    }
    
    // Check consistency between connection_probability and structural_plasticity_enable
    auto connectionProb = getOr<float>("connection_probability", 0.1f);
    auto structuralEnable = getOr<bool>("structural_plasticity_enable", true);
    
    if (structuralEnable && connectionProb > 0.5) {
        errors.push_back("high connection_probability combined with structural_plasticity_enable may create excessive connectivity";
    }
    
    // Check consistency between plasticity parameters
    auto stdpLtp = getOr<float>("stdp_ltp_weight", 0.01f);
    auto stdpLtd = getOr<float>("stdp_ltd_weight", 0.012f);
    
    if (stdpLtp && stdpLtd) {
        if (*stdpLtp <= *stdpLtd) {
            errors.push_back("stdp_ltp_weight should be greater than stdp_ltd_weight for proper STDP function");
        }
        if (std::abs(*stdpLtp - *stdpLtd) < 0.001) {
            errors.push_back("stdp_ltp_weight and stdp_ltd_weight are too close, may cause unstable learning");
        }
    }
    
    // Check consistency between neuromodulation parameters
    auto dopamineBaseline = getOr<double>("dopamine_baseline", 0.1);
    auto dopamineScale = getOr<double>("dopamine_scale", 1.0);
    
    if (dopamineBaseline && dopamineScale) {
        if (*dopamineBaseline > 1.0) {
            warnings.push_back("dopamine_baseline is high, may affect neural excitability significantly");
        }
    }
    
    // Check consistency between development and plasticity parameters
    auto synaptogenesisRate = getOr<double>("synaptogenesis_rate", 0.0001);
    auto pruningRate = getOr<double>("pruning_rate", 0.00001);
    
    if (synaptogenesisRate && pruningRate) {
        if (*synaptogenesisRate < *pruningRate * 10) {
            warnings.push_back("synaptogenesis_rate is much lower than pruning_rate, may lead to net synapse loss");
        }
    }
    
    // Check consistency between simulation and checkpoint parameters
    auto maxSteps = getOr<int64_t>("max_simulation_steps", 10000);
    auto checkpointInterval = getOr<int64_t>("checkpoint_interval", 1000);
    
    if (maxSteps && checkpointInterval) {
        if (*checkpointInterval > *maxSteps / 2) {
            warnings.push_back("checkpoint_interval is too large compared to max_simulation_steps");
        }
    }
    
    return errors;
}

std::map<std::string, int64_t> Config::getStatistics() const {
    std::map<std::string, int64_t> stats;
    
    stats["total_entries"] = pImpl->entries.size();
    
    size_t intCount = 0, int64Count = 0, doubleCount = 0, boolCount = 0, stringCount = 0;
    size_t vectorCount = 0;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                intCount++;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                int64Count++;
            } else if constexpr (std::is_same_v<T, double>) {
                doubleCount++;
            } else if constexpr (std::is_same_v<T, bool>) {
                boolCount++;
            } else if constexpr (std::is_same_v<T, std::string>) {
                stringCount++;
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                vectorCount++;
            }
        }, entry.value);
    }
    
    stats["int_values"] = intCount;
    stats["int64_values"] = int64Count;
    stats["double_values"] = doubleCount;
    stats["bool_values"] = boolCount;
    stats["string_values"] = stringCount;
    stats["vector_values"] = vectorCount;
    
    // Calculate value ranges
    int64_t minValue = std::numeric_limits<int64_t>::max();
    int64_t maxValue = std::numeric_limits<int64_t>::min();
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> ||
                          std::is_same_v<T, int64_t> ||
                          std::is_same_v<T, double>) {
                int64_t intVal = static_cast<int64_t>(arg);
                minValue = std::min(minValue, intVal);
                maxValue = std::max(maxValue, intVal);
            }
        }, entry.value);
    }
    
    stats["min_value"] = minValue;
    stats["max_value"] = maxValue;
    
    return stats;
}

bool Config::isComplete(const std::vector<std::string>& requiredKeys) const {
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            return false;
        }
    }
    return true;
}

void Config::normalize() {
    // Normalize values to ensure they're within valid ranges
    
    // Normalize neuron_count
    auto neuronCount = getOr<int64_t>("neuron_count", 1000);
    if (neuronCount) {
        if (*neuronCount < 1) {
            set("neuron_count", static_cast<int64_t>(1), ConfigSource::Runtime);
        } else if (*neuronCount > 100000) {
            set("neuron_count", static_cast<int64_t>(100000), ConfigSource::Runtime);
        }
    }
    
    // Normalize region_count
    auto regionCount = getOr<int64_t>("region_count", 1);
    if (regionCount) {
        if (*regionCount < 1) {
            set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
        } else if (*regionCount > 100) {
            set("region_count", static_cast<int64_t>(100), ConfigSource::Runtime);
        }
    }
    
    // Normalize connection_probability
    auto connectionProb = getOr<float>("connection_probability", 0.1f);
    if (connectionProb) {
        if (*connectionProb < 0.0f) {
            set("connection_probability", 0.0f, ConfigSource::Runtime);
        } else if (*connectionProb > 1.0f) {
            set("connection_probability", 1.0f, ConfigSource::Runtime);
        }
    }
    
    // Normalize simulation_timestep
    auto timestep = getOr<double>("simulation_timestep", 0.001);
    if (timestep) {
        if (*timestep <= 0.0) {
            set("simulation_timestep", 0.0001f, ConfigSource::Runtime);
        } else if (*timestep > 1.0) {
            set("simulation_timestep", 0.001f, ConfigSource::Runtime);
        }
    }
    
    // Normalize plasticity parameters
    auto plasticityLearningRate = getOr<float>("plasticity_learning_rate", 0.01f);
    if (plasticityLearningRate) {
        if (*plasticityLearningRate < 0.0f) {
            set("plasticity_learning_rate", 0.01f, ConfigSource::Runtime);
        } else if (*plasticityLearningRate > 1.0f) {
            set("plasticity_learning_rate", 1.0f, ConfigSource::Runtime);
        }
    }
    
    auto stdpLtp = getOr<float>("stdp_ltp_weight", 0.01f);
    if (stdpLtp) {
        if (*stdpLtp < 0.0f) {
            set("stdp_ltp_weight", 0.01f, ConfigSource::Runtime);
        } else if (*stdpLtp > 1.0f) {
            set("stdp_ltp_weight", 1.0f, ConfigSource::Runtime);
        }
    }
    
    auto stdpLtd = getOr<float>("stdp_ltd_weight", 0.012f);
    if (stdpLtd) {
        if (*stdpLtd < 0.0f) {
            set("stdp_ltd_weight", 0.01f, ConfigSource::Runtime);
        } else if (*stdpLtd > 1.0f) {
            set("stdp_ltd_weight", 0.012f, ConfigSource::Runtime);
        }
    }
    
    auto stdpTau = getOr<float>("stdp_tau", 20.0f);
    if (stdpTau) {
        if (*stdpTau <= 0.0f) {
            set("stdp_tau", 1.0f, ConfigSource::Runtime);
        }
    }
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Check if key already exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == pImpl->entries.end()) {
            // Key doesn't exist, add it
            pImpl->entries.push_back(entry);
        } else {
            // Key exists, overwrite value if source is higher priority
            if (source >= it->source) {
                it->value = entry.value;
                it->source = source;
                it->description = entry.description;
            }
        }
    }
}

std::map<std::string, std::string> Config::getSchema() const {
    std::map<std::string, std::string> schema;
    
    // Define schema for known keys
    const std::map<std::string, std::string> knownSchema = {
        {"neuron_count", "int64_t:positive"},
        {"region_count", "int64_t:positive"},
        {"connection_probability", "float:range(0.0-1.0)"},
        {"simulation_timestep", "float:range(0.0001-1.0)"},
        {"random_seed", "int64_t:nonnegative"},
        {"plasticity_learning_rate", "float:range(0.0-1.0)"},
        {"stdp_ltp_weight", "float:range(0.0-1.0)"},
        {"stdp_ltd_weight", "float:range(0.0-1.0)"},
        {"stdp_tau", "float:positive"},
        {"synaptogenesis_rate", "float:range(0.0-0.1)"},
        {"pruning_rate", "float:range(0.0-0.01)"},
        {"replay_interval", "int64_t:positive"},
        {"consolidation_interval", "int64_t:positive"},
        {"dopamine_baseline", "float:range(0.0-10.0)"},
        {"dopamine_scale", "float:range(0.0-10.0)"},
        {"curiosity_enable", "bool"},
        {"novelty_enable", "bool"},
        {"structural_plasticity_enable", "bool"},
        {"hebbian_enable", "bool"},
        {"reward_discount_factor", "float:range(0.0-1.0)"},
        {"environment_width", "int64_t:positive"},
        {"environment_height", "int64_t:positive"},
        {"visualization_update_rate", "float:positive"},
        {"max_simulation_steps", "int64_t:range(1-1000000)"},
        {"log_level", "string:enum(DEBUG,INFO,WARNING,ERROR,CRITICAL)"},
        {"log_to_file", "bool"},
        {"enable_multithreading", "bool"}
    };
    
    // Copy known schema
    for (const auto& pair : knownSchema) {
        schema[pair.first] = pair.second;
    }
    
    return schema;
}

bool Config::isConsistent() const {
    return validateConsistency().empty();
}

bool Config::validateConstraint(const std::string& key, const std::string& constraint) const {
    // Simple constraint validation
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false; // Key doesn't exist
    }
    
    // Parse constraint
    std::vector<std::string> constraintParts;
    std::istringstream iss(constraint);
    std::string part;
    while (std::getline(iss, part, ':')) {
        constraintParts.push_back(part);
    }
    
    if (constraintParts.empty()) {
        return true;
    }
    
    const std::string& constraintType = constraintParts[0];
    
    if (constraintType == "range") {
        if (constraintParts.size() < 2) {
            return true;
        }
        
        const std::string& rangeStr = constraintParts[1];
        size_t dashPos = rangeStr.find('-');
        if (dashPos == std::string::npos) {
            return true;
        }
        
        std::string minStr = rangeStr.substr(0, dashPos);
        std::string maxStr = rangeStr.substr(dashPos + 1);
        
        double minVal = (minStr == "negative_infinity" || minStr == "-inf") ? 
                        std::numeric_limits<double>::lowest() : 
                        std::stod(minStr);
        double maxVal = (maxStr == "positive_infinity" || maxStr == "inf") ? 
                        std::numeric_limits<double>::max() : 
                        std::stod(maxStr);
        
        auto value = get<double>(key);
        if (!value) {
            return false;
        }
        
        return *value >= minVal && *value <= maxVal;
    } else if (constraintType == "enum") {
        // Check if value is in allowed enum values
        // This would require knowing the allowed enum values for each key
        return true; // Simplified
    } else if (constraintType == "positive") {
        auto intVal = get<int64_t>(key);
        auto doubleVal = get<double>(key);
        if (intVal) {
            return *intVal > 0;
        } else if (doubleVal) {
            return *doubleVal > 0.0;
        }
        return false;
    } else if (constraintType == "nonnegative") {
        auto intVal = get<int64_t>(key);
        auto doubleVal = get<double>(key);
        if (intVal) {
            return *intVal >= 0;
        } else if (doubleVal) {
            return *doubleVal >= 0.0;
        }
        return false;
    } else if (constraintType == "max") {
        if (constraintParts.size() < 2) {
            return true;
        }
        
        double maxVal = std::stod(constraintParts[1]);
        
        auto intVal = get<int64_t>(key);
        auto doubleVal = get<double>(key);
        if (intVal) {
            return *intVal <= maxVal;
        } else if (doubleVal) {
            return *doubleVal <= maxVal;
        }
        return false;
    }
    
    return true; // Unknown constraint type
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
