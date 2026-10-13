#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <optional>
#include <cctype>
#include <vector>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON format first
    if (loadFromJSON(filepath)) {
        return true;
    }
    
    // Try YAML format (simplified: key=value format but with better parsing)
    if (loadFromYAML(filepath)) {
        return true;
    }
    
    // Fallback to legacy simple key=value format for backward compatibility
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
    // Choose format based on file extension
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        saveToJSON(filepath);
        return true;
    } else if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".yaml") {
        saveToYAML(filepath);
        return true;
    } else if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".cfg") {
        // Legacy config format for backward compatibility
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# NLM Configuration File\n";
        file << "# Generated automatically\n\n";
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << entry.key << " = \"" << arg << "\"\n";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << entry.key << " = " << (arg ? "true" : "false") << "\n";
                } else {
                    file << entry.key << " = " << arg << "\n";
                }
            }, entry.value);
            
            file << "\n";
        }
        
        return true;
    } else {
        // Default to JSON format
        saveToJSON(filepath + ".json");
        return true;
    }
}

// Helper functions for JSON/YAML parsing

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file content
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    // File is automatically closed when ifstream object goes out of scope
    
    // Simple JSON parser - looks for objects { ... }
    if (content.empty() || content.find('{') == std::string::npos) {
        return false; // Not a JSON object
    }
    
    try {
        // Strip whitespace
        std::string jsonStr = trim(content);
        if (jsonStr.front() != '{' || jsonStr.back() != '}') {
            return false;
        }
        
        // Extract content between braces
        std::string inner = jsonStr.substr(1, jsonStr.size() - 2);
        
        // Parse key-value pairs (simplified)
        std::regex pairPattern(R"("([^"=]+)"\s*:\s*([^,}]+))");
        auto begin = std::sregex_iterator(inner.begin(), inner.end(), pairPattern);
        auto end = std::sregex_iterator();
        
        bool hasEntries = false;
        for (std::sregex_iterator i = begin; i != end; ++i) {
            std::smatch match = *i;
            if (match.size() >= 3) {
                std::string key = trim(match[1].str());
                std::string valueStr = trim(match[2].str());
                
                // Try to parse the value based on its type
                set(key, valueStr, ConfigSource::File);
                hasEntries = true;
            }
        }
        
        return hasEntries;
    } catch (const std::exception&) {
        // If JSON parsing fails, continue with other formats
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
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
        
        // Parse YAML key: value format
        size_t pos = line.find(':');
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

void Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << (i == pImpl->entries.size() - 1 ? "\n" : ",\n");
    }
    file << "}\n";
    // File is automatically closed when ofstream object goes out of scope
}

void Config::saveToYAML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << ": \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << ": " << (arg ? "true" : "false") << "\n";
            } else {
                file << entry.key << ": " << arg << "\n";
            }
        }, entry.value);
        
        file << "\n";
    }
}

// Validation helper functions
bool Config::validateRange(const std::string& key, double value, double min, double max) {
    return value >= min && value <= max;
}

bool Config::validateRange(const std::string& key, int value, int min, int max) {
    return value >= min && value <= max;
}

bool Config::validateProbability(const std::string& key, double value) {
    return value >= 0.0 && value <= 1.0;
}

bool Config::validatePositive(const std::string& key, double value) {
    return value > 0.0;
}

bool Config::validateNonNegative(const std::string& key, int value) {
    return value >= 0;
}

// Configuration validation implementation
bool Config::validate() const {
    std::string errors = getValidationErrors();
    return errors.empty();
}

std::string Config::getValidationErrors() const {
    std::ostringstream oss;
    
    for (const auto& entry : pImpl->entries) {
        bool isValid = isValidValue(entry.key, entry.value);
        if (!isValid) {
            oss << "Invalid value for key \"" << entry.key << "\": ";
            oss << "value ";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, double>) {
                    oss << arg;
                } else if constexpr (std::is_same_v<T, int>) {
                    oss << arg;
                } else {
                    oss << "\"" << arg << "\"";
                }
            }, entry.value);
            oss << " (expected different type/range)\n";
        }
    }
    
    return oss.str();
}

std::string Config::getDefaultValue(const std::string& key) {
    // Return default value for known configuration keys
    if (key == "random_seed") return "42";
    if (key == "simulation_timestep") return "0.001";
    if (key == "neuron_count") return "1000";
    if (key == "region_count") return "2";
    if (key == "connection_probability") return "0.1";
    if (key == "plasticity_learning_rate") return "0.01";
    if (key == "stdp_ltp_weight") return "0.01";
    if (key == "stdp_ltd_weight") return "0.012";
    if (key == "stdp_time_constant") return "20.0";
    if (key == "development_synaptogenesis_rate") return "0.001";
    if (key == "development_pruning_rate") return "0.0001";
    if (key == "dopamine_baseline") return "0.1";
    if (key == "reward_discount_factor") return "0.99";
    if (key == "environment_width") return "10";
    if (key == "environment_height") return "10";
    if (key == "max_simulation_steps") return "10000";
    if (key == "visualization_update_rate") return "30.0";
    
    return "";
}

bool Config::isValidKey(const std::string& key) {
    // List of valid configuration keys
    static const std::vector<std::string> validKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count",
        "connection_probability",
        "plasticity_learning_rate",
        "stdp_ltp_weight",
        "stdp_ltd_weight",
        "stdp_time_constant",
        "development_synaptogenesis_rate",
        "development_pruning_rate",
        "dopamine_baseline",
        "reward_discount_factor",
        "environment_name",
        "environment_width",
        "environment_height",
        "log_level",
        "log_to_file",
        "log_filename",
        "max_simulation_steps",
        "simulation_time_limit",
        "visualization_enabled",
        "visualization_update_rate"
    };
    
    return std::find(validKeys.begin(), validKeys.end(), toLower(key)) != validKeys.end();
}

bool Config::isValidValue(const std::string& key, const ConfigValue& value) {
    std::string lowerKey = toLower(key);
    
    try {
        if (lowerKey == "random_seed") {
            int val = std::get<int>(value);
            return validateNonNegative("random_seed", val);
        } else if (lowerKey == "simulation_timestep") {
            double val = std::get<double>(value);
            return validatePositive("simulation_timestep", val);
        } else if (lowerKey == "neuron_count") {
            int val = std::get<int>(value);
            return validateNonNegative("neuron_count", val);
        } else if (lowerKey == "region_count") {
            int val = std::get<int>(value);
            return validateNonNegative("region_count", val);
        } else if (lowerKey == "connection_probability") {
            double val = std::get<double>(value);
            return validateProbability("connection_probability", val);
        } else if (lowerKey == "plasticity_learning_rate") {
            double val = std::get<double>(value);
            return validateProbability("plasticity_learning_rate", val);
        } else if (lowerKey == "stdp_ltp_weight") {
            double val = std::get<double>(value);
            return validateNonNegative("stdp_ltp_weight", val);
        } else if (lowerKey == "stdp_ltd_weight") {
            double val = std::get<double>(value);
            return validateNonNegative("stdp_ltd_weight", val);
        } else if (lowerKey == "stdp_time_constant") {
            double val = std::get<double>(value);
            return validatePositive("stdp_time_constant", val);
        } else if (lowerKey == "development_synaptogenesis_rate") {
            double val = std::get<double>(value);
            return validateNonNegative("development_synaptogenesis_rate", val);
        } else if (lowerKey == "development_pruning_rate") {
            double val = std::get<double>(value);
            return validateNonNegative("development_pruning_rate", val);
        } else if (lowerKey == "dopamine_baseline") {
            double val = std::get<double>(value);
            return validateNonNegative("dopamine_baseline", val);
        } else if (lowerKey == "reward_discount_factor") {
            double val = std::get<double>(value);
            return validateProbability("reward_discount_factor", val);
        } else if (lowerKey == "environment_width") {
            int val = std::get<int>(value);
            return validatePositive("environment_width", val);
        } else if (lowerKey == "environment_height") {
            int val = std::get<int>(value);
            return validatePositive("environment_height", val);
        } else if (lowerKey == "max_simulation_steps") {
            int val = std::get<int>(value);
            return validateNonNegative("max_simulation_steps", val);
        } else if (lowerKey == "visualization_update_rate") {
            double val = std::get<double>(value);
            return validatePositive("visualization_update_rate", val);
        } else if (lowerKey == "log_to_file") {
            bool val = std::get<bool>(value);
            return true; // Any boolean is valid
        }
    } catch (const std::bad_variant_access&) {
        return false; // Wrong type
    }
    
    return true; // Unknown key or string/bool type
}

// Template method implementations
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
