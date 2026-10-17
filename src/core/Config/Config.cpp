#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp> // Assuming nlohmann::json is available

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::string> environmentPrefixes;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON format first
    std::ifstream jsonFile(filepath);
    if (jsonFile.is_open()) {
        // Try to parse as JSON
        try {
            nlohmann::json jsonData;
            jsonFile >> jsonData;
            std::string jsonString = jsonData.dump();
            return loadFromJSON(jsonString);
        } catch (const nlohmann::json::parse_error&) {
            jsonFile.close();
            // Fall back to simple format
        }
    }
    
    // Try simple key=value format
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

bool Config::loadFromJSON(const std::string& jsonString) {
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonString);
        
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            
            if (it->is_string()) {
                set(key, it->get<std::string>(), ConfigSource::File);
            } else if (it->is_number_integer()) {
                set(key, it->get<int64_t>(), ConfigSource::File);
            } else if (it->is_number_unsigned()) {
                set(key, static_cast<int64_t>(it->get<uint64_t>()), ConfigSource::File);
            } else if (it->is_number_float()) {
                set(key, it->get<double>(), ConfigSource::File);
            } else if (it->is_boolean()) {
                set(key, it->get<bool>(), ConfigSource::File);
            } else if (it->is_array()) {
                // For arrays, create a comma-separated string for now
                // Better to preserve as actual array type
                std::vector<std::string> stringArray;
                for (const auto& item : *it) {
                    stringArray.push_back(item.dump());
                }
                set(key, stringArray, ConfigSource::File);
            } else {
                NLM_LOG_WARNING("Skipping JSON key '" + key + "' with unsupported type");
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error: " + std::string(e.what()));
        return false;
    }
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

void Config::loadFromEnvironment(const std::string& prefix, bool prefixRequired) {
    for (const auto& envPair : std::filesystem::environment()) {
        std::string key = envPair.first;
        std::string value = envPair.second;
        
        // Check if key has the prefix
        if (!prefix.empty()) {
            if (prefixRequired && key.find(prefix) != 0) {
                continue;
            }
            
            if (!prefixRequired && key.find(prefix) == std::string::npos) {
                continue;
            }
            
            // Strip prefix
            if (key.find(prefix) == 0) {
                key = key.substr(prefix.size());
            }
        }
        
        // Parse value based on type (simple heuristic)
        if (value == "true" || value == "false") {
            set(key, value == "true", ConfigSource::File);
        } else {
            try {
                // Try integer
                size_t pos;
                int64_t intVal = std::stoll(value, &pos);
                if (pos == value.size()) {
                    set(key, intVal, ConfigSource::File);
                    continue;
                }
            } catch (...) {}
            
            try {
                // Try double
                size_t pos;
                double doubleVal = std::stod(value, &pos);
                if (pos == value.size()) {
                    set(key, doubleVal, ConfigSource::File);
                    continue;
                }
            } catch (...) {}
            
            // Default to string
            set(key, value, ConfigSource::File);
        }
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{" << std::endl;
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": " << toJSON(entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",";
        }
        file << std::endl;
    }
    file << "}" << std::endl;
    
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

ConfigValue Config::getRaw(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->value;
    }
    
    throw ConfigValidationError("Key not found: " + key);
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    validateAndSet(key, value, source);
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

std::vector<ConfigEntry> Config::getAllEntries() const {
    return pImpl->entries;
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
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    oss << "\"" << arg[j] << "\"";
                    if (j < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    oss << arg[j];
                    if (j < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    oss << arg[j];
                    if (j < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        if (!entry.validate()) {
            oss << " [VALIDATION FAILED]";
        }
        if (i < pImpl->entries.size() - 1) {
            oss << "\n";
        }
    }
    return oss.str();
}

bool Config::applySchema(const ConfigSchema& schema) {
    bool allValid = true;
    
    for (const auto& entry : pImpl->entries) {
        if (!schema.validateField(entry.key, entry.value)) {
            std::cerr << "Schema validation error for field '" << entry.key << "'\n";
            allValid = false;
        }
    }
    
    return allValid;
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.getAllEntries()) {
        // Only override if the source is higher priority
        bool shouldOverride = true;
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it != pImpl->entries.end()) {
            // Check source priority (Higher number = higher priority)
            static const std::map<ConfigSource, int> sourcePriority = {
                {ConfigSource::Runtime, 4},
                {ConfigSource::Override, 3},
                {ConfigSource::CommandLine, 2},
                {ConfigSource::File, 1},
                {ConfigSource::Default, 0}
            };
            
            if (sourcePriority.at(entry.source) > sourcePriority.at(it->source)) {
                it->value = entry.value;
                it->source = entry.source;
            }
        } else {
            pImpl->entries.push_back(entry);
        }
    }
}

void Config::validateAndSet(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // First, try to apply schema validation if available
    // For now, we'll just basic validation
    
    // Check if key already exists
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
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

bool Config::parseJSONValue(const std::string& value, ConfigValue& result) {
    // Simple parsing - in production, use proper JSON parsing
    if (value.empty()) {
        result = std::string();
        return true;
    }
    
    // Check if it's a number
    try {
        if (value.find('.') != std::string::npos) {
            double val = std::stod(value);
            result = val;
            return true;
        } else {
            int64_t val = std::stoll(value);
            result = val;
            return true;
        }
    } catch (...) {}
    
    // Check if it's a boolean
    if (value == "true" || value == "false") {
        result = (value == "true");
        return true;
    }
    
    // Default to string
    result = value;
    return true;
}

std::string Config::toJSON(const ConfigValue& value) {
    std::ostringstream oss;
    
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << std::setprecision(6) << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << "\"" << arg[i] << "\"";
                if (i < arg.size() - 1) oss << ", ";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << arg[i];
                if (i < arg.size() - 1) oss << ", ";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << std::setprecision(6) << arg[i];
                if (i < arg.size() - 1) oss << ", ";
            }
            oss << "]";
        }
    }, value);
    
    return oss.str();
}

ConfigSchema getDefaultConfigSchema() {
    ConfigSchema schema;
    
    // Basic simulation parameters
    schema.addField("random_seed", int64_t(42),
                   "Random seed for reproducible simulations",
                   false);
    
    schema.addField("simulation_timestep", double(0.001),
                   "Simulation timestep in seconds",
                   false);
    
    schema.addField("neuron_count", int64_t(500),
                   "Number of neurons in the simulation",
                   false);
    
    schema.addField("region_count", int64_t(1),
                   "Number of neural regions",
                   false);
    
    // Connection parameters
    schema.addField("connection_probability", double(0.15f),
                   "Probability of forming connections between neurons",
                   false);
    
    // STDP parameters
    schema.addField("stdp_ltp_weight", double(0.02f),
                   "STDP Long-Term Potentiation weight",
                   false);
    
    schema.addField("stdp_ltd_weight", double(0.015f),
                   "STDP Long-Term Depression weight",
                   false);
    
    schema.addField("stdp_tau", double(20.0f),
                   "STDP time constant",
                   false);
    
    // Structural plasticity parameters
    schema.addField("synaptogenesis_rate", double(0.0001f),
                   "Rate of new synapse formation",
                   false);
    
    schema.addField("pruning_rate", double(0.00001f),
                   "Rate of synapse elimination",
                   false);
    
    // Environment parameters
    schema.addField("environment_width", int64_t(20),
                   "Width of the simulated environment",
                   false);
    
    schema.addField("environment_height", int64_t(20),
                   "Height of the simulated environment",
                   false);
    
    // Vision parameters
    schema.addField("vision_width", int64_t(16),
                   "Width of visual field in pixels",
                   false);
    
    schema.addField("vision_height", int64_t(16),
                   "Height of visual field in pixels",
                   false);
    
    // Add validation rules
    // Positive values
    schema.addField("random_seed", int64_t(0), "", false)
         .addValidator(ConfigHelpers::nonNegativeValidator());
    
    schema.addField("neuron_count", int64_t(1), "", false)
         .addValidator(ConfigHelpers::positiveValidator());
    
    schema.addField("region_count", int64_t(1), "", false)
         .addValidator(ConfigHelpers::positiveValidator());
    
    schema.addField("simulation_timestep", double(0.0), "", false)
         .addValidator(ConfigHelpers::positiveValidator());
    
    // Range validations
    schema.addField("connection_probability", double(0.0), "", false)
         .addValidator(ConfigHelpers::rangeValidator(0.0, 1.0));
    
    // Growth rate validations
    schema.addField("synaptogenesis_rate", double(0.0), "", false)
         .addValidator(ConfigHelpers::nonNegativeValidator());
    
    schema.addField("pruning_rate", double(0.0), "", false)
         .addValidator(ConfigHelpers::nonNegativeValidator());
    
    return schema;
}

// Validation helper implementations
ValidationRule ConfigHelpers::rangeValidator(double min, double max, const std::string& typeName) {
    return [min, max, typeName](const ConfigValue& value) {
        try {
            if (std::holds_alternative<double>(value)) {
                return std::get<double>(value) >= min && std::get<double>(value) <= max;
            } else if (std::holds_alternative<int64_t>(value)) {
                double val = static_cast<double>(std::get<int64_t>(value));
                return val >= min && val <= max;
            } else if (std::holds_alternative<float>(value)) {
                return std::get<float>(value) >= min && std::get<float>(value) <= max;
            }
        } catch (...) {}
        return false;
    };
}

ValidationRule ConfigHelpers::rangeValidator(int64_t min, int64_t max, const std::string& typeName) {
    return [min, max](const ConfigValue& value) {
        try {
            if (std::holds_alternative<int64_t>(value)) {
                return std::get<int64_t>(value) >= min && std::get<int64_t>(value) <= max;
            } else if (std::holds_alternative<int>(value)) {
                int64_t val = static_cast<int64_t>(std::get<int>(value));
                return val >= min && val <= max;
            }
        } catch (...) {}
        return false;
    };
}

ValidationRule ConfigHelpers::positiveValidator() {
    return [](const ConfigValue& value) {
        try {
            if (std::holds_alternative<int64_t>(value)) {
                return std::get<int64_t>(value) > 0;
            } else if (std::holds_alternative<int>(value)) {
                return std::get<int>(value) > 0;
            } else if (std::holds_alternative<double>(value)) {
                return std::get<double>(value) > 0.0;
            } else if (std::holds_alternative<float>(value)) {
                return std::get<float>(value) > 0.0f;
            }
        } catch (...) {}
        return false;
    };
}

ValidationRule ConfigHelpers::nonNegativeValidator() {
    return [](const ConfigValue& value) {
        try {
            if (std::holds_alternative<int64_t>(value)) {
                return std::get<int64_t>(value) >= 0;
            } else if (std::holds_alternative<int>(value)) {
                return std::get<int>(value) >= 0;
            } else if (std::holds_alternative<double>(value)) {
                return std::get<double>(value) >= 0.0;
            } else if (std::holds_alternative<float>(value)) {
                return std::get<float>(value) >= 0.0f;
            }
        } catch (...) {}
        return false;
    };
}

ValidationRule ConfigHelpers::booleanValidator() {
    return [](const ConfigValue& value) {
        return std::holds_alternative<bool>(value);
    };
}

ValidationRule ConfigHelpers::stringNotEmptyValidator() {
    return [](const ConfigValue& value) {
        if (!std::holds_alternative<std::string>(value)) return false;
        const auto& str = std::get<std::string>(value);
        return !str.empty();
    };
}

ValidationRule ConfigHelpers::stringLengthValidator(size_t min, size_t max) {
    return [min, max](const ConfigValue& value) {
        if (!std::holds_alternative<std::string>(value)) return false;
        const auto& str = std::get<std::string>(value);
        return str.size() >= min && str.size() <= max;
    };
}

ValidationRule ConfigHelpers::getConstantValidator(const std::vector<std::string>& validValues) {
    return [validValues](const ConfigValue& value) {
        if (!std::holds_alternative<std::string>(value)) return false;
        const auto& str = std::get<std::string>(value);
        return std::find(validValues.begin(), validValues.end(), str) != validValues.end();
    };
}

ValidationRule ConfigHelpers::getConstantValidator(const std::vector<int>& validValues) {
    return [validValues](const ConfigValue& value) {
        try {
            if (std::holds_alternative<int>(value)) {
                int val = std::get<int>(value);
                return std::find(validValues.begin(), validValues.end(), val) != validValues.end();
            } else if (std::holds_alternative<int64_t>(value)) {
                int64_t val = std::get<int64_t>(value);
                return std::find(validValues.begin(), validValues.end(), static_cast<int>(val)) != validValues.end();
            }
        } catch (...) {}
        return false;
    };
}

ValidationRule ConfigHelpers::fileExistsValidator(const std::string& pathKey) {
    // This validator would need access to the config to get the path
    // For now, return a placeholder
    return [](const ConfigValue& value) {
        return true; // Assume valid for now
    };
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