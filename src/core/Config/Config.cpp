#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<std::string> validationErrors;
};

struct ConfigValidationRule {
    std::string paramName;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    
    ConfigValidationRule(const std::string& name, std::function<bool(const ConfigValue&)> val, const std::string& error)
        : paramName(name), validator(val), errorMessage(error) {}
    
    bool validate(const ConfigValue& value) const {
        return validator(value);
    }
};

class ConfigValidator {
public:
    static ConfigValidator& instance() {
        static ConfigValidator validator;
        return validator;
    }
    
    void addRule(const ConfigValidationRule& rule) {
        rules_.push_back(rule);
    }
    
    std::vector<std::string> validate(const std::vector<ConfigEntry>& entries) const {
        std::vector<std::string> errors;
        for (const auto& entry : entries) {
            auto it = std::find_if(rules_.begin(), rules_.end(),
                                  [&entry](const ConfigValidationRule& rule) {
                                      return rule.paramName == entry.key;
                                  });
            if (it != rules_.end()) {
                if (!it->validate(entry.value)) {
                    errors.push_back("Invalid value for parameter '" + entry.key + "': " + it->errorMessage);
                }
            }
        }
        return errors;
    }
    
    bool validateRequired(const std::vector<ConfigEntry>& entries, const std::set<std::string>& requiredParams) const {
        bool allPresent = true;
        for (const auto& param : requiredParams) {
            if (!std::any_of(entries.begin(), entries.end(),
                           [&param](const ConfigEntry& e) { return e.key == param; })) {
                allPresent = false;
                break;
            }
        }
        return allPresent;
    }
    
private:
    ConfigValidator() {
        // Initialize validation rules for common NLM parameters
        
        // Numeric parameters that should be positive
        addRule(ConfigValidationRule("random_seed", 
            [](const ConfigValue& v) {
                if (std::holds_alternative<int64_t>(v)) return std::get<int64_t>(v) > 0;
                if (std::holds_alternative<int>(v)) return std::get<int>(v) > 0;
                return false;
            },
            "must be a positive integer"));
        
        addRule(ConfigValidationRule("neuron_count",
            [](const ConfigValue& v) {
                if (std::holds_alternative<int64_t>(v)) return std::get<int64_t>(v) > 0;
                if (std::holds_alternative<int>(v)) return std::get<int>(v) > 0 && std::get<int>(v) <= 100000;
                return false;
            },
            "must be a positive integer between 1 and 100000"));
        
        addRule(ConfigValidationRule("region_count",
            [](const ConfigValue& v) {
                if (std::holds_alternative<int64_t>(v)) return std::get<int64_t>(v) > 0 && std::get<int64_t>(v) <= 1000;
                return false;
            },
            "must be a positive integer between 1 and 1000"));
        
        addRule(ConfigValidationRule("simulation_timestep",
            [](const ConfigValue& v) {
                if (std::holds_alternative<double>(v)) return std::get<double>(v) > 0.0 && std::get<double>(v) <= 1.0;
                return false;
            },
            "must be a positive double between 0.0001 and 1.0"));
        
        // Floating point parameters that should be within specific ranges
        addRule(ConfigValidationRule("connection_probability",
            [](const ConfigValue& v) {
                if (std::holds_alternative<double>(v)) {
                    double val = std::get<double>(v);
                    return val >= 0.0 && val <= 1.0;
                }
                if (std::holds_alternative<float>(v)) {
                    float val = std::get<float>(v);
                    return val >= 0.0f && val <= 1.0f;
                }
                return false;
            },
            "must be a float between 0.0 and 1.0"));
        
        addRule(ConfigValidationRule("stdp_ltp_weight",
            [](const ConfigValue& v) {
                if (std::holds_alternative<double>(v)) return std::get<double>(v) >= 0.0;
                if (std::holds_alternative<float>(v)) return std::get<float>(v) >= 0.0f;
                return false;
            },
            "must be a non-negative float"));
        
        addRule(ConfigValidationRule("stdp_ltd_weight",
            [](const ConfigValue& v) {
                if (std::holds_alternative<double>(v)) return std::get<double>(v) >= 0.0;
                if (std::holds_alternative<float>(v)) return std::get<float>(v) >= 0.0f;
                return false;
            },
            "must be a non-negative float"));
        
        addRule(ConfigValidationRule("stdp_tau",
            [](const ConfigValue& v) {
                if (std::holds_alternative<double>(v)) return std::get<double>(v) > 0.0 && std::get<double>(v) <= 100.0;
                if (std::holds_alternative<float>(v)) return std::get<float>(v) > 0.0f && std::get<float>(v) <= 100.0f;
                return false;
            },
            "must be a positive float between 0.1 and 100.0"));
        
        addRule(ConfigValidationRule("synaptogenesis_rate",
            [](const ConfigValue& v) {
                if (std::holds_alternative<double>(v)) return std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0;
                if (std::holds_alternative<float>(v)) return std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f;
                return false;
            },
            "must be a float between 0.0 and 1.0"));
        
        addRule(ConfigValidationRule("pruning_rate",
            [](const ConfigValue& v) {
                if (std::holds_alternative<double>(v)) return std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0;
                if (std::holds_alternative<float>(v)) return std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f;
                return false;
            },
            "must be a float between 0.0 and 1.0"));
    }
    
private:
    std::vector<ConfigValidationRule> rules_;
};

// Global validator instance
ConfigValidator& getValidator() {
    return ConfigValidator::instance();
}

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
            
            if (!set(key, value, ConfigSource::File)) {
                return false;
            }
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
                if (!set(key, value, ConfigSource::CommandLine)) {
                    return false;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            if (!set(key, value, ConfigSource::CommandLine)) {
                return false;
            }
        }
    }
    
    // Validate configuration after loading
    pImpl->validationErrors = validate();
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Convert ConfigValue to string representation
        std::string valueStr;
        std::visit([&valueStr](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStr = "\"" + arg + "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                valueStr = "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) valueStr += ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>) {
                        valueStr += "\"" + arg[i] + "\"";
                    } else {
                        valueStr += std::to_string(arg[i]);
                    }
                }
                valueStr += "]";
            } else {
                valueStr = std::to_string(arg);
            }
        }, entry.value);
        
        file << entry.key << " = " << valueStr << "\n";
    }
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate the value before setting it
    if (!validateParameter(key, value)) {
        return; // Silently ignore invalid values for now
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    // Clear validation errors since we just set a valid value
    pImpl->validationErrors.clear();
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

bool Config::validateParameter(const std::string& key, const ConfigValue& value) const {
    auto it = std::find_if(rules_.begin(), rules_.end(),
                          [&key](const ConfigValidationRule& rule) {
                              return rule.paramName == key;
                          });
    if (it == rules_.end()) {
        return true; // No validation rule for this parameter
    }
    return it->validate(value);
}

std::vector<std::string> Config::validate() const {
    std::vector<std::string> errors;
    
    // Validate parameter values
    for (const auto& entry : pImpl->entries) {
        if (!validateParameter(entry.key, entry.value)) {
            errors.push_back("Invalid value for parameter '" + entry.key + "'");
        }
    }
    
    return errors;
}

bool Config::isValid() const {
    return pImpl->validationErrors.empty();
}

const std::vector<std::string>& Config::getValidationErrors() const {
    return pImpl->validationErrors;
}

bool Config::hasRequiredParams(const std::set<std::string>& requiredParams) const {
    return getValidator().validateRequired(pImpl->entries, requiredParams);
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
