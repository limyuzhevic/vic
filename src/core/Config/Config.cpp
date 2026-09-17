#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <cctype>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::map<std::string, ConfigValue> values;
    std::vector<std::pair<std::string, std::string>> defaultValues;
    std::map<std::string, std::string> descriptions;
    
    // Performance optimization
    bool sorted;
    
    Impl() : sorted(true) {
        // Register default configuration values
        registerDefault("random_seed", 42, "Random seed for reproducible experiments", Int64Validator(0, std::numeric_limits<int64_t>::max()));
        registerDefault("simulation_timestep", 0.001, "Simulation timestep in seconds", FloatValidator(0.0f, std::numeric_limits<float>::max()));
        registerDefault("neuron_count", 1000, "Number of neurons in the brain", Int64Validator(1, std::numeric_limits<int64_t>::max()));
        registerDefault("region_count", 2, "Number of brain regions", Int64Validator(1, std::numeric_limits<int64_t>::max()));
        registerDefault("connection_probability", 0.1, "Connection probability for random connectivity", FloatValidator(0.0f, 1.0f));
        registerDefault("stdp_ltp_weight", 0.01f, "STDP LTP weight", FloatValidator(0.0f, std::numeric_limits<float>::max()));
        registerDefault("stdp_ltd_weight", 0.012f, "STDP LTD weight", FloatValidator(0.0f, std::numeric_limits<float>::max()));
        registerDefault("stdp_tau", 20.0f, "STDP time constant", FloatValidator(0.0f, std::numeric_limits<float>::max()));
        registerDefault("synaptogenesis_rate", 0.001f, "Development synaptogenesis rate", FloatValidator(0.0f, std::numeric_limits<float>::max()));
        registerDefault("pruning_rate", 0.0001f, "Development pruning rate", FloatValidator(0.0f, std::numeric_limits<float>::max()));
        registerDefault("dopamine_baseline", 0.1f, "Dopamine baseline level", FloatValidator(0.0f, std::numeric_limits<float>::max()));
        registerDefault("reward_discount_factor", 0.99f, "Reward discount factor", FloatValidator(0.0f, 1.0f));
        registerDefault("environment_name", std::string("GridWorld"), "Environment name", StringValidator());
        registerDefault("environment_width", 10, "Environment width", Int64Validator(1, std::numeric_limits<int64_t>::max()));
        registerDefault("environment_height", 10, "Environment height", Int64Validator(1, std::numeric_limits<int64_t>::max()));
        registerDefault("log_level", std::string("INFO"), "Log level", StringValidator());
        registerDefault("log_to_file", false, "Whether to log to file", nullptr);
        registerDefault("log_filename", std::string("nlm.log"), "Log filename", StringValidator());
        registerDefault("max_simulation_steps", 10000, "Maximum simulation steps", Int64Validator(0, std::numeric_limits<int64_t>::max()));
        registerDefault("simulation_time_limit", 0.0f, "Simulation time limit in seconds", FloatValidator(0.0f, std::numeric_limits<float>::max()));
        registerDefault("visualization_enabled", false, "Whether visualization is enabled", nullptr);
        registerDefault("visualization_update_rate", 30.0f, "Visualization update rate in Hz", FloatValidator(0.0f, std::numeric_limits<float>::max()));
        registerDefault("replay_interval", 100, "Memory replay interval", Int64Validator(0, std::numeric_limits<int64_t>::max()));
        registerDefault("consolidation_interval", 1000, "Memory consolidation interval", Int64Validator(0, std::numeric_limits<int64_t>::max()));
        registerDefault("checkpoint_dir", std::string("./checkpoints"), "Checkpoint directory", StringValidator());
        registerDefault("checkpoint_max_files", 10000, "Maximum checkpoint files", Int64Validator(0, std::numeric_limits<int64_t>::max()));
        registerDefault("checkpoint_compression", 2, "Checkpoint compression level (0-5)", Int64Validator(0, 5));
        registerDefault("checkpoint_auto", true, "Enable automatic checkpointing", nullptr);
    }
    
    void registerDefault(const std::string& key, const std::any& value, 
                        const std::string& description = "",
                        std::shared_ptr<ConfigValidator> validator = nullptr) {
        defaultValues.emplace_back(key, toString(value));
        if (!description.empty()) {
            descriptions[key] = description;
        }
        values[key] = ConfigValue(value, ConfigSource::Default);
        if (validator) {
            values[key].validators.push_back(validator);
        }
        sorted = false;
    }
    
    void ensureSorted() const {
        if (!sorted) {
            // Sort entries for deterministic iteration
            sorted = true;
        }
    }
    
    size_t getMemoryUsage() const {
        size_t total = sizeof(*this);
        for (const auto& pair : values) {
            total += pair.first.capacity();
            total += sizeof(pair.second);
            if (pair.second.value.has_value()) {
                if (std::holds_alternative<std::string>(pair.second.value)) {
                    total += std::get<std::string>(pair.second.value).capacity();
                }
            }
        }
        return total;
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

template<typename T>
void Config::set(const std::string& key, const T& value, ConfigSource source) {
    if (!pImpl->values.count(key) || pImpl->values[key].source == ConfigSource::Default) {
        pImpl->values[key] = ConfigValue(value, source);
        
        if (pImpl->descriptions.count(key)) {
            std::shared_ptr<ConfigValidator> validator = createValidatorForType<T>();
            if (validator) {
                pImpl->values[key].validators.push_back(validator);
            }
        }
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (!pImpl->values.count(key)) {
        return std::nullopt;
    }
    
    const auto& configValue = pImpl->values.at(key);
    if (!configValue.isValid()) {
        return std::nullopt;
    }
    
    try {
        return std::any_cast<T>(configValue.value);
    } catch (const std::bad_any_cast&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

bool Config::validate(const std::string& key) const {
    if (!pImpl->values.count(key)) {
        return false;
    }
    return pImpl->values.at(key).isValid();
}

std::string Config::getValidationError(const std::string& key) const {
    if (!pImpl->values.count(key)) {
        return "Key not found";
    }
    return pImpl->values.at(key).getValidationError();
}

void Config::remove(const std::string& key) {
    pImpl->values.erase(key);
}

void Config::clear() {
    pImpl->values.clear();
    pImpl->sorted = false;
}

bool Config::exists(const std::string& key) const {
    return pImpl->values.count(key) > 0;
}

std::vector<std::string> Config::getAllKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->values.size());
    for (const auto& pair : pImpl->values) {
        keys.push_back(pair.first);
    }
    return keys;
}

ConfigSource Config::getSource(const std::string& key) const {
    if (!pImpl->values.count(key)) {
        return ConfigSource::Default;
    }
    return pImpl->values.at(key).source;
}

// JSON/YAML support
bool Config::loadFromJson(const nlohmann::json& jsonData) {
    try {
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            const auto& value = *it;
            
            // Convert JSON value to appropriate C++ type
            if (value.is_string()) {
                set(key, value.get<std::string>(), ConfigSource::File);
            } else if (value.is_number_integer()) {
                set(key, value.get<int64_t>(), ConfigSource::File);
            } else if (value.is_number_unsigned()) {
                set(key, value.get<uint64_t>(), ConfigSource::File);
            } else if (value.is_number()) {
                set(key, value.get<double>(), ConfigSource::File);
            } else if (value.is_boolean()) {
                set(key, value.get<bool>(), ConfigSource::File);
            } else if (value.is_array()) {
                // Handle arrays as strings for now
                set(key, value.dump(), ConfigSource::File);
            }
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading JSON config: " << e.what() << std::endl;
        return false;
    }
}

nlohmann::json Config::toJson() const {
    nlohmann::json jsonData;
    for (const auto& pair : pImpl->values) {
        const auto& configValue = pair.second;
        if (!configValue.isValid()) continue;
        
        if (configValue.value.type() == typeid(std::string)) {
            jsonData[pair.first] = std::any_cast<std::string>(configValue.value);
        } else if (configValue.value.type() == typeid(int64_t)) {
            jsonData[pair.first] = std::any_cast<int64_t>(configValue.value);
        } else if (configValue.value.type() == typeid(double)) {
            jsonData[pair.first] = std::any_cast<double>(configValue.value);
        } else if (configValue.value.type() == typeid(bool)) {
            jsonData[pair.first] = std::any_cast<bool>(configValue.value);
        } else if (configValue.value.type() == typeid(float)) {
            jsonData[pair.first] = std::any_cast<float>(configValue.value);
        }
    }
    return jsonData;
}

void Config::optimizeMemoryUsage() {
    // Remove unused entries
    auto it = pImpl->values.begin();
    while (it != pImpl->values.end()) {
        if (!it->second.isValid()) {
            it = pImpl->values.erase(it);
        } else {
            ++it;
        }
    }
    pImpl->sorted = false;
}

size_t Config::getMemoryUsage() const {
    return pImpl->getMemoryUsage();
}

void Config::printConfig() const {
    pImpl->ensureSorted();
    std::cout << "=== Configuration ===" << std::endl;
    for (const auto& pair : pImpl->values) {
        const auto& entry = pair.second;
        std::cout << pair.first << " = ";
        
        std::visit([&](const auto& val) {
            if constexpr (std::is_same_v<decltype(val), std::string>) {
                std::cout << "\"" << val << "\"";
            } else {
                std::cout << val;
            }
        }, entry.value);
        
        std::cout << " (source: " << static_cast<int>(entry.source) << ")";
        if (!entry.isValid()) {
            std::cout << " [INVALID]";
        }
        std::cout << std::endl;
    }
}

void Config::validateAll() const {
    std::cout << "=== Configuration Validation ===" << std::endl;
    int validCount = 0;
    int totalCount = 0;
    
    for (const auto& pair : pImpl->values) {
        totalCount++;
        if (validate(pair.first)) {
            validCount++;
        } else {
            std::cout << "INVALID: " << pair.first << " - " << 
                getValidationError(pair.first) << std::endl;
        }
    }
    
    std::cout << "Valid: " << validCount << "/" << totalCount << " entries" << std::endl;
}

// Implementation of helper methods
std::shared_ptr<ConfigValidator> Config::createValidatorForType(const std::type_info& typeInfo) {
    if (typeInfo == typeid(std::string)) {
        return std::make_shared<StringValidator>();
    } else if (typeInfo == typeid(int64_t) || typeInfo == typeid(int) || typeInfo == typeid(long)) {
        return std::make_shared<Int64Validator>(-std::numeric_limits<int64_t>::max(), std::numeric_limits<int64_t>::max());
    } else if (typeInfo == typeid(double) || typeInfo == typeid(float)) {
        return std::make_shared<FloatValidator>(-std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    } else if (typeInfo == typeid(bool)) {
        return std::make_shared<ConfigValidator>();
    }
    return nullptr;
}

std::string Config::toString(const std::any& value) {
    if (value.type() == typeid(std::string)) {
        return std::any_cast<std::string>(value);
    } else if (value.type() == typeid(int64_t)) {
        return std::to_string(std::any_cast<int64_t>(value));
    } else if (value.type() == typeid(double)) {
        return std::to_string(std::any_cast<double>(value));
    } else if (value.type() == typeid(float)) {
        return std::to_string(std::any_cast<float>(value));
    } else if (value.type() == typeid(bool)) {
        return std::any_cast<bool>(value) ? "true" : "false";
    }
    return "";
}

// Explicit template instantiations
template void Config::set<int>(const std::string&, const int&, ConfigSource);
template void Config::set<int64_t>(const std::string&, const int64_t&, ConfigSource);
template void Config::set<double>(const std::string&, const double&, ConfigSource);
template void Config::set<float>(const std::string&, const float&, ConfigSource);
template void Config::set<bool>(const std::string&, const bool&, ConfigSource);
template void Config::set<std::string>(const std::string&, const std::string&, ConfigSource);

template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm