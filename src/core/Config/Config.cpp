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

std::vector<std::string> Config::validate() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        std::string error = validateKeyAndValue(entry.key, entry.value);
        if (!error.empty()) {
            errors.push_back(error);
        }
    }
    
    return errors;
}

std::string Config::validateKeyAndValue(const std::string& key, const ConfigValue& value) const {
    // Validate key format
    if (key.empty()) {
        return "Empty key is not allowed";
    }
    
    if (key.find('.') == std::string::npos) {
        return "Key must contain a dot separator (e.g., 'section.key')";
    }
    
    // Check for invalid characters in key
    for (char c : key) {
        if (!(std::isalnum(c) || c == '.' || c == '_' || c == '-')) {
            return "Key contains invalid characters: " + std::string(1, c);
        }
    }
    
    // Validate value based on key type
    return validateValueByKey(key, value);
}

std::string Config::validateValueByKey(const std::string& key, const ConfigValue& value) const {
    std::string section = key.substr(0, key.find('.'));
    std::string field = key.substr(key.find('.') + 1);
    
    try {
        if (section == "brain") {
            return validateBrainField(field, value);
        } else if (section == "plasticity") {
            return validatePlasticityField(field, value);
        } else if (section == "neuromod") {
            return validateNeuromodField(field, value);
        } else if (section == "development") {
            return validateDevelopmentField(field, value);
        } else if (section == "prediction") {
            return validatePredictionField(field, value);
        }
    } catch (const std::bad_variant_access&) {
        return "Invalid value type for key: " + key;
    }
    
    return "";
}

std::string Config::validateBrainField(const std::string& field, const ConfigValue& value) const {
    try {
        if (field == "neuron_count") {
            int64_t count = std::get<int64_t>(value);
            if (count <= 0 || count > 1000000) {
                return "brain.neuron_count must be between 1 and 1000000";
            }
        } else if (field == "synapse_density") {
            float density = std::get<float>(value);
            if (density < 0.0f || density > 1.0f) {
                return "brain.synapse_density must be between 0.0 and 1.0";
            }
        } else if (field == "connection_probability") {
            float prob = std::get<float>(value);
            if (prob < 0.0f || prob > 1.0f) {
                return "brain.connection_probability must be between 0.0 and 1.0";
            }
        } else if (field == "v_thresh") {
            float vthresh = std::get<float>(value);
            if (vthresh < -100.0f || vthresh > 100.0f) {
                return "brain.v_thresh must be between -100.0 and 100.0";
            }
        } else if (field == "v_rest") {
            float vrest = std::get<float>(value);
            if (vrest < -100.0f || vrest > 100.0f) {
                return "brain.v_rest must be between -100.0 and 100.0";
            }
        } else if (field == "v_reset") {
            float vreset = std::get<float>(value);
            if (vreset < -100.0f || vreset > 100.0f) {
                return "brain.v_reset must be between -100.0 and 100.0";
            }
        } else if (field == "tau_mem") {
            float tau = std::get<float>(value);
            if (tau <= 0.0f || tau > 1000.0f) {
                return "brain.tau_mem must be between 0.0 and 1000.0";
            }
        } else if (field == "tau_ref") {
            float tau = std::get<float>(value);
            if (tau < 0.0f || tau > 100.0f) {
                return "brain.tau_ref must be between 0.0 and 100.0";
            }
        }
    } catch (const std::bad_variant_access&) {
        // Ignore type errors - they'll be caught by the caller
    }
    
    return "";
}

std::string Config::validatePlasticityField(const std::string& field, const ConfigValue& value) const {
    try {
        if (field == "stdp.enable") {
            bool enable = std::get<bool>(value);
            if (!enable) {
                return "plasticity.stdp.enable should be true for Phase 2";
            }
        } else if (field == "stdp.learning_rate") {
            float rate = std::get<float>(value);
            if (rate <= 0.0f || rate > 1.0f) {
                return "plasticity.stdp.learning_rate must be between 0.0 and 1.0";
            }
        } else if (field == "stdp.tau_plus") {
            float tau = std::get<float>(value);
            if (tau <= 0.0f) {
                return "plasticity.stdp.tau_plus must be positive";
            }
        } else if (field == "stdp.tau_minus") {
            float tau = std::get<float>(value);
            if (tau <= 0.0f) {
                return "plasticity.stdp.tau_minus must be positive";
            }
        } else if (field == "hebbian.enable") {
            bool enable = std::get<bool>(value);
            if (!enable) {
                return "plasticity.hebbian.enable should be true for Phase 2";
            }
        } else if (field == "structural.enable") {
            bool enable = std::get<bool>(value);
            if (!enable) {
                return "plasticity.structural.enable should be true for Phase 2";
            }
        }
    } catch (const std::bad_variant_access&) {
        // Ignore type errors - they'll be caught by the caller
    }
    
    return "";
}

std::string Config::validateNeuromodField(const std::string& field, const ConfigValue& value) const {
    try {
        if (field == "dopamine.scale") {
            float scale = std::get<float>(value);
            if (scale < 0.0f || scale > 10.0f) {
                return "neuromod.dopamine.scale must be between 0.0 and 10.0";
            }
        } else if (field == "curiosity.enable") {
            bool enable = std::get<bool>(value);
            if (!enable) {
                return "neuromod.curiosity.enable should be true for Phase 2";
            }
        } else if (field == "novelty.enable") {
            bool enable = std::get<bool>(value);
            if (!enable) {
                return "neuromod.novelty.enable should be true for Phase 2";
            }
        }
    } catch (const std::bad_variant_access&) {
        // Ignore type errors - they'll be caught by the caller
    }
    
    return "";
}

std::string Config::validateDevelopmentField(const std::string& field, const ConfigValue& value) const {
    try {
        if (field == "enable") {
            bool enable = std::get<bool>(value);
            if (!enable) {
                return "development.enable should be true for Phase 2";
            }
        }
    } catch (const std::bad_variant_access&) {
        // Ignore type errors - they'll be caught by the caller
    }
    
    return "";
}

std::string Config::validatePredictionField(const std::string& field, const ConfigValue& value) const {
    try {
        if (field == "enable") {
            bool enable = std::get<bool>(value);
            if (!enable) {
                return "prediction.enable should be true for Phase 2";
            }
        }
    } catch (const std::bad_variant_access&) {
        // Ignore type errors - they'll be caught by the caller
    }
    
    return "";
}

std::string Config::validateKey(const std::string& key) const {
    // Validate key format
    if (key.empty()) {
        return "Empty key is not allowed";
    }
    
    if (key.find('.') == std::string::npos) {
        return "Key must contain a dot separator (e.g., 'section.key')";
    }
    
    // Check for invalid characters in key
    for (char c : key) {
        if (!(std::isalnum(c) || c == '.' || c == '_' || c == '-')) {
            return "Key contains invalid characters: " + std::string(1, c);
        }
    }
    
    return "";
}

std::string Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // This is a helper method for backward compatibility
    return validateKeyAndValue(key, value);
}
