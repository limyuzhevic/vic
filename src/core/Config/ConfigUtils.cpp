#include "ConfigUtils.hpp"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <iomanip>

namespace nlm {

bool ConfigUtils::validate(const std::string& key, const ConfigValue& value) {
    // Basic validation - can be extended based on specific key requirements
    if (value.index() == std::variant_npos) {
        return false; // Invalid variant
    }
    return true;
}

std::string ConfigUtils::getValueType(const ConfigValue& value) {
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

bool ConfigUtils::hasRequiredKeys(const Config& config, const std::vector<std::string>& requiredKeys) {
    for (const auto& key : requiredKeys) {
        if (!config.has(key)) {
            return false;
        }
    }
    return true;
}

std::unordered_map<std::string, std::string> ConfigUtils::getAllStringValues(const Config& config) {
    std::unordered_map<std::string, std::string> result;
    auto keys = config.getKeys();
    
    for (const auto& key : keys) {
        std::visit([&result, &key](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                result[key] = arg;
            } else if constexpr (std::is_same_v<T, int>) {
                result[key] = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                result[key] = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(6) << arg;
                result[key] = oss.str();
            } else if constexpr (std::is_same_v<T, bool>) {
                result[key] = arg ? "true" : "false";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                std::ostringstream oss;
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
                result[key] = oss.str();
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                std::ostringstream oss;
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
                result[key] = oss.str();
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                std::ostringstream oss;
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
                result[key] = oss.str();
            }
        }, config.get<ConfigValue>(key).value());
    }
    
    return result;
}

std::shared_ptr<Config> ConfigUtils::parseFromString(const std::string& configString) {
    auto config = std::make_shared<Config>();
    std::istringstream stream(configString);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        line = Config::trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = Config::trim(line.substr(0, pos));
            std::string value = Config::trim(line.substr(pos + 1));
            
            // Remove quotes
            if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            config->set(key, value, ConfigSource::File);
        }
    }
    
    return config;
}

void ConfigUtils::merge(Config& target, const Config& source) {
    auto sourceKeys = source.getKeys();
    for (const auto& key : sourceKeys) {
        std::visit([&target, &key](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                target.set(key, arg, ConfigSource::Runtime);
            } else if constexpr (std::is_same_v<T, int>) {
                target.set(key, arg, ConfigSource::Runtime);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                target.set(key, arg, ConfigSource::Runtime);
            } else if constexpr (std::is_same_v<T, double>) {
                target.set(key, arg, ConfigSource::Runtime);
            } else if constexpr (std::is_same_v<T, bool>) {
                target.set(key, arg, ConfigSource::Runtime);
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                target.set(key, arg, ConfigSource::Runtime);
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                target.set(key, arg, ConfigSource::Runtime);
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                target.set(key, arg, ConfigSource::Runtime);
            }
        }, source.get<ConfigValue>(key).value());
    }
}

void ConfigUtils::applyProfile(Config& config, const std::string& profileName) {
    // Common profiles
    if (profileName == "fast") {
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Runtime);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
        config->set("connection_probability", 0.1f, ConfigSource::Runtime);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Runtime);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Runtime);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Runtime);
        config->set("pruning_rate", 0.00001f, ConfigSource::Runtime);
    } else if (profileName == "accurate") {
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Runtime);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Runtime);
        config->set("connection_probability", 0.15f, ConfigSource::Runtime);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Runtime);
        config->set("stdp_ltd_weight", 0.018f, ConfigSource::Runtime);
        config->set("synaptogenesis_rate", 0.00005f, ConfigSource::Runtime);
        config->set("pruning_rate", 0.000005f, ConfigSource::Runtime);
    } else if (profileName == "simple") {
        config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Runtime);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
        config->set("connection_probability", 0.05f, ConfigSource::Runtime);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Runtime);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Runtime);
        config->set("synaptogenesis_rate", 0.0002f, ConfigSource::Runtime);
        config->set("pruning_rate", 0.00002f, ConfigSource::Runtime);
    }
}

std::vector<std::string> ConfigUtils::getAvailableProfiles() {
    return {"fast", "accurate", "simple"};
}

std::shared_ptr<Config> ConfigUtils::createDefaultConfig() {
    auto config = std::make_shared<Config>();
    
    // Basic NLM Phase 2 configuration
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Random seed
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    
    return config;
}

std::shared_ptr<Config> ConfigUtils::createTestConfig() {
    auto config = std::make_shared<Config>();
    
    // Small test configuration
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.2f, ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    // Fast plasticity for testing
    config->set("stdp_ltp_weight", 0.05f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.06f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.001f, ConfigSource::Default);
    config->set("pruning_rate", 0.0001f, ConfigSource::Default);
    
    // Deterministic for testing
    config->set("random_seed", static_cast<int64_t>(123), ConfigSource::Default);
    
    return config;
}

} // namespace nlm
