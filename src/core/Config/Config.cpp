#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <set>
#include <cmath>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::vector<std::string>> validationRules;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize default validation rules
    pImpl->validationRules = {
        {"neuron_count", {"positive_int"}},
        {"region_count", {"positive_int"}},
        {"simulation_timestep", {"positive_float", "min_0.0001"}},
        {"connection_probability", {"range_0_to_1"}},
        {"plasticity_learning_rate", {"positive_float"}},
        {"stdp_ltp_weight", {"positive_float"}},
        {"stdp_ltd_weight", {"positive_float"}},
        {"stdp_time_constant", {"positive_float"}},
        {"synaptogenesis_rate", {"positive_float"}},
        {"pruning_rate", {"positive_float"}},
        {"dopamine_baseline", {"range_0_to_1"}},
        {"reward_discount_factor", {"range_0_to_1"}},
        {"environment_width", {"positive_int", "max_1000"}},
        {"environment_height", {"positive_int", "max_1000"}},
        {"max_simulation_steps", {"positive_int", "max_1000000"}},
        {"visualization_update_rate", {"positive_float"}}
    };
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Validation helper functions
namespace {
    bool validatePositiveInt(const ConfigValue& value, const std::string& key) {
        if (const auto* val = std::get_if<int64_t>(&value)) {
            return *val > 0;
        } else if (const auto* val = std::get_if<int>(&value)) {
            return *val > 0;
        }
        return false;
    }
    
    bool validatePositiveFloat(const ConfigValue& value, const std::string& key) {
        if (const auto* val = std::get_if<double>(&value)) {
            return *val > 0.0;
        }
        return false;
    }
    
    bool validateRange0To1(const ConfigValue& value, const std::string& key) {
        if (const auto* val = std::get_if<double>(&value)) {
            return *val >= 0.0 && *val <= 1.0;
        } else if (const auto* val = std::get_if<float>(&value)) {
            return *val >= 0.0f && *val <= 1.0f;
        }
        return false;
    }
    
    bool validateMin(const ConfigValue& value, const std::string& key, double minVal) {
        if (const auto* val = std::get_if<double>(&value)) {
            return *val >= minVal;
        } else if (const auto* val = std::get_if<float>(&value)) {
            return *val >= minVal;
        }
        return false;
    }
    
    bool validateMax(const ConfigValue& value, const std::string& key, int64_t maxVal) {
        if (const auto* val = std::get_if<int64_t>(&value)) {
            return *val <= maxVal;
        } else if (const auto* val = std::get_if<int>(&value)) {
            return *val <= maxVal;
        }
        return false;
    }
    
    bool applyValidationRule(const ConfigValue& value, const std::string& rule, const std::string& key) {
        if (rule == "positive_int") {
            return validatePositiveInt(value, key);
        } else if (rule == "positive_float") {
            return validatePositiveFloat(value, key);
        } else if (rule == "range_0_to_1") {
            return validateRange0To1(value, key);
        } else if (rule.find("min_") == 0) {
            double minVal = std::stod(rule.substr(4));
            return validateMin(value, key, minVal);
        } else if (rule.find("max_") == 0) {
            int64_t maxVal = std::stoll(rule.substr(4));
            return validateMax(value, key, maxVal);
        }
        return true; // Unknown rule, skip validation
    }
}

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
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else {
                file << arg;
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

bool Config::validate(const std::unordered_map<std::string, std::vector<std::string>>& customRules) const {
    std::unordered_map<std::string, std::vector<std::string>> allRules = pImpl->validationRules;
    
    // Merge custom rules
    for (const auto& custom : customRules) {
        allRules[custom.first] = custom.second;
    }
    
    bool isValid = true;
    
    for (const auto& entry : pImpl->entries) {
        auto it = allRules.find(entry.key);
        if (it != allRules.end()) {
            for (const auto& rule : it->second) {
                if (!applyValidationRule(entry.value, rule, entry.key)) {
                    std::cerr << "ERROR: Configuration key '" << entry.key << "' failed validation: " << rule << std::endl;
                    std::cerr << "  Value: ";
                    std::visit([](auto&& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, std::string>) {
                            std::cerr << "\"" << arg << "\"";
                        } else {
                            std::cerr << arg;
                        }
                    }, entry.value);
                    std::cerr << std::endl;
                    isValid = false;
                }
            }
        }
    }
    
    return isValid;
}

bool Config::validate(const std::string& schemaFile) const {
    // TODO PHASE 3: Implement schema validation
    // For now, use default validation rules
    return validate({});
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
