#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::vector<std::string>> validationRules;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize validation rules for common keys
    initializeValidationRules();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeValidationRules() {
    // Define validation rules for configuration parameters
    validationRules["random_seed"] = {"positive_integer", "max_1000000"};
    validationRules["simulation_timestep"] = {"positive_float", "max_1.0"};
    validationRules["neuron_count"] = {"positive_integer", "max_100000"};
    validationRules["region_count"] = {"positive_integer", "max_100"};
    validationRules["connection_probability"] = {"float_range", "0.0-1.0"};
    validationRules["stdp_ltp_weight"] = {"positive_float", "max_1.0"};
    validationRules["stdp_ltd_weight"] = {"positive_float", "max_1.0"};
    validationRules["stdp_tau"] = {"positive_integer", "max_1000"};
    validationRules["synaptogenesis_rate"] = {"positive_float", "max_1.0"};
    validationRules["pruning_rate"] = {"positive_float", "max_1.0"};
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    auto it = pImpl->validationRules.find(key);
    if (it == pImpl->validationRules.end()) {
        return true; // No validation rules defined
    }

    for (const auto& rule : it->second) {
        if (!validateValueAgainstRule(key, value, rule)) {
            return false;
        }
    }

    return true;
}

bool Config::validateValueAgainstRule(const std::string& key, const ConfigValue& value, const std::string& rule) const {
    try {
        if (rule == "positive_integer") {
            if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value)) {
                int64_t val = std::holds_alternative<int>(value) ? 
                    static_cast<int64_t>(std::get<int>(value)) : 
                    std::get<int64_t>(value);
                return val > 0;
            }
            return false;
        } else if (rule == "positive_float") {
            if (std::holds_alternative<double>(value)) {
                double val = std::get<double>(value);
                return val > 0.0;
            }
            return false;
        } else if (rule == "float_range") {
            if (std::holds_alternative<double>(value)) {
                double val = std::get<double>(value);
                return val >= 0.0 && val <= 1.0;
            }
            return false;
        } else if (rule.find("max_") != std::string::npos) {
            // Handle max_X rules
            size_t pos = rule.find("max_");
            if (pos != std::string::npos) {
                std::string numStr = rule.substr(pos + 4);
                try {
                    double maxVal = std::stod(numStr);
                    if (std::holds_alternative<int>(value)) {
                        return static_cast<double>(std::get<int>(value)) <= maxVal;
                    } else if (std::holds_alternative<int64_t>(value)) {
                        return static_cast<double>(std::get<int64_t>(value)) <= maxVal;
                    } else if (std::holds_alternative<double>(value)) {
                        return std::get<double>(value) <= maxVal;
                    }
                } catch (...) {
                    return false;
                }
            }
        }
    } catch (...) {
        return false;
    }
    return true; // Unknown rule, assume valid
}

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file type based on extension
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    try {
        if (ext == ".json" || ext == ".cfg") {
            return loadFromJSON(filepath);
        } else if (ext == ".yaml" || ext == ".yml") {
            return loadFromYAML(filepath);
        } else {
            // Fallback to simple key=value format
            return loadFromSimpleFormat(filepath);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load config from " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    try {
        nlohmann::json j;
        file >> j;

        // Clear existing entries
        clear();

        // Parse JSON
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;

            if (it->is_number_integer()) {
                value = it->get<int64_t>();
            } else if (it->is_number_unsigned()) {
                value = static_cast<int64_t>(it->get<uint64_t>());
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_array()) {
                // Handle arrays
                std::vector<int> intArray;
                std::vector<double> doubleArray;
                std::vector<std::string> stringArray;
                
                for (const auto& item : *it) {
                    if (item.is_number_integer()) {
                        intArray.push_back(item.get<int>());
                    } else if (item.is_number_float()) {
                        doubleArray.push_back(item.get<double>());
                    } else if (item.is_string()) {
                        stringArray.push_back(item.get<std::string>());
                    }
                }
                
                // Prefer integer arrays, then double, then string
                if (!intArray.empty()) {
                    value = intArray;
                } else if (!doubleArray.empty()) {
                    value = doubleArray;
                } else if (!stringArray.empty()) {
                    value = stringArray;
                } else {
                    continue; // Skip empty arrays
                }
            }

            // Validate the value before setting
            if (validateValue(key, value)) {
                set(key, value, ConfigSource::File);
            } else {
                NLM_LOG_WARNING("Invalid value for key '" + key + "' in JSON file");
            }
        }

        return true;
    } catch (const nlohmann::json::exception& e) {
        NLM_LOG_ERROR("JSON parsing error in " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    try {
        YAML::Node doc = YAML::LoadFile(filepath);

        // Clear existing entries
        clear();

        // Parse YAML recursively
        parseYAMLNode(doc, "root");

        return true;
    } catch (const YAML::Exception& e) {
        NLM_LOG_ERROR("YAML parsing error in " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

void Config::parseYAMLNode(const YAML::Node& node, const std::string& prefix) {
    if (node.IsScalar()) {
        std::string key = prefix.empty() ? "value" : prefix;
        ConfigValue value;

        if (node.IsInteger()) {
            value = node.as<int64_t>();
        } else if (node.IsFloat()) {
            value = node.as<double>();
        } else if (node.IsBoolean()) {
            value = node.as<bool>();
        } else {
            value = node.as<std::string>();
        }

        if (validateValue(key, value)) {
            set(key, value, ConfigSource::File);
        }
    } else if (node.IsSequence()) {
        // Convert to array format
        std::vector<double> arrayValues;
        for (const auto& item : node) {
            if (item.IsInteger()) {
                arrayValues.push_back(static_cast<double>(item.as<int64_t>()));
            } else if (item.IsFloat()) {
                arrayValues.push_back(item.as<double>());
            } else if (item.IsScalar()) {
                try {
                    arrayValues.push_back(std::stod(item.as<std::string>()));
                } catch (...) {
                    arrayValues.push_back(0.0);
                }
            }
        }
        if (!arrayValues.empty()) {
            std::string key = prefix.empty() ? "array" : prefix;
            if (validateValue(key, arrayValues)) {
                set(key, arrayValues, ConfigSource::File);
            }
        }
    } else if (node.IsMap()) {
        for (const auto& pair : node) {
            std::string key = prefix.empty() ? pair.first.as<std::string>() : prefix + "." + pair.first.as<std::string>();
            parseYAMLNode(pair.second, key);
        }
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
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

            // Convert string value to appropriate type
            ConfigValue configValue;
            try {
                // Try to detect type
                if (value.find('.') != std::string::npos) {
                    // Try double first
                    size_t pos;
                    double doubleVal = std::stod(value, &pos);
                    if (pos == value.size()) {
                        configValue = doubleVal;
                    } else {
                        // Try integer if no decimal point
                        size_t pos2;
                        int64_t intVal = std::stoll(value, &pos2);
                        if (pos2 == value.size()) {
                            configValue = intVal;
                        } else {
                            configValue = value; // Keep as string
                        }
                    }
                } else {
                    // Try integer
                    size_t pos;
                    int64_t intVal = std::stoll(value, &pos);
                    if (pos == value.size()) {
                        configValue = intVal;
                    } else {
                        configValue = value; // Keep as string
                    }
                }

                // Validate the value before setting
                if (validateValue(key, configValue)) {
                    set(key, configValue, ConfigSource::File);
                } else {
                    NLM_LOG_WARNING("Invalid value for key '" + key + "' in config file");
                }
            } catch (...) {
                // Keep as string if conversion fails
                configValue = value;
                set(key, configValue, ConfigSource::File);
            }
        }
    }

    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    // Determine format based on extension
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    try {
        if (ext == ".json") {
            saveToJSON(file);
        } else if (ext == ".yaml" || ext == ".yml") {
            saveToYAML(file);
        } else {
            // Default to simple format
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                file << entry.key << " = " << getValueString(entry.value) << "\n";
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save config to " + filepath + ": " + std::string(e.what()));
        return false;
    }

    return true;
}

void Config::saveToJSON(std::ostream& stream) const {
    nlohmann::json j;

    for (const auto& entry : pImpl->entries) {
        j[entry.key] = getValueFromConfigValue(entry.value);
    }

    stream << j.dump(2); // Pretty print with 2-space indentation
}

void Config::saveToYAML(std::ostream& stream) const {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;

    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        emitter << YAML::Value << getValueFromConfigValue(entry.value);
    }

    emitter << YAML::EndMap;
    stream << emitter.c_str();
}

std::string Config::getValueString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

nlohmann::json Config::getValueFromConfigValue(const ConfigValue& value) const {
    nlohmann::json j;

    std::visit([&j](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j = arg;
        }
    }, value);

    return j;
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
