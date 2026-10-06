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
    
    // Validate file path
    if (filepath.empty()) {
        return false;
    }
    
    // Check if file exists and is readable
    std::ifstream testFile(filepath);
    if (!testFile.good()) {
        return false;
    }
    
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
    if (argc <= 1) {
        return true; // No arguments to process
    }
    
    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) {
            continue; // Skip null pointers
        }
        
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.size() >= 3 && arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos && pos > 2) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg.size() >= 2 && arg[0] == '-' && i + 1 < argc && argv[i + 1]) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    if (filepath.empty()) {
        return false;
    }
    
    // Check if we have write permissions by attempting to create the file
    std::ofstream testFile(filepath);
    if (!testFile.good()) {
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Convert value to string based on type
        std::string valueStr;
        if (std::holds_alternative<int>(entry.value)) {
            valueStr = std::to_string(std::get<int>(entry.value));
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            valueStr = std::to_string(std::get<int64_t>(entry.value));
        } else if (std::holds_alternative<double>(entry.value)) {
            valueStr = std::to_string(std::get<double>(entry.value));
        } else if (std::holds_alternative<bool>(entry.value)) {
            valueStr = std::get<bool>(entry.value) ? "true" : "false";
        } else if (std::holds_alternative<std::string>(entry.value)) {
            valueStr = std::get<std::string>(entry.value);
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            const auto& vec = std::get<std::vector<int>>(entry.value);
            valueStr = "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) valueStr += ", ";
                valueStr += std::to_string(vec[i]);
            }
            valueStr += "]";
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            const auto& vec = std::get<std::vector<double>>(entry.value);
            valueStr = "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) valueStr += ", ";
                valueStr += std::to_string(vec[i]);
            }
            valueStr += "]";
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            const auto& vec = std::get<std::vector<std::string>>(entry.value);
            valueStr = "[";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) valueStr += ", ";
                valueStr += "\"" + vec[i] + "\"";
            }
            valueStr += "]";
        }
        
        file << entry.key << " = " << valueStr << "\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (key.empty()) {
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        // Type mismatch - return empty optional
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (key.empty()) {
        return; // Invalid key, silently ignore
    }
    
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
    if (key.empty()) {
        return; // Invalid key, silently ignore
    }
    
    // Basic validation for key
    if (key.find('=') != std::string::npos || key.find(' ') != std::string::npos) {
        // Key contains invalid characters
        return;
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    if (key.empty()) {
        return; // Invalid key, silently ignore
    }
    
    // Validate integer value ranges for common brain parameters
    if (key == "brain.neuron_count") {
        if (value < 100 || value > 1000000) {
            return; // Invalid neuron count
        }
    }
    
    if (key == "brain.synapse_density") {
        if (value < 0 || value > 1) {
            return; // Invalid synapse density
        }
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    if (key.empty()) {
        return; // Invalid key, silently ignore
    }
    
    // Validate double value ranges for common brain parameters
    if (key == "brain.v_thresh") {
        if (value < -100 || value > 100) {
            return; // Invalid threshold potential
        }
    }
    
    if (key == "brain.v_rest") {
        if (value < -100 || value > 100) {
            return; // Invalid resting potential
        }
    }
    
    if (key == "brain.v_reset") {
        if (value < -100 || value > 100) {
            return; // Invalid reset potential
        }
    }
    
    if (key == "brain.tau_mem") {
        if (value < 0 || value > 1000) {
            return; // Invalid membrane time constant
        }
    }
    
    if (key == "brain.tau_ref") {
        if (value < 0 || value > 100) {
            return; // Invalid refractory period
        }
    }
    
    if (key == "brain.initial_weight_mean") {
        if (value < 0 || value > 10) {
            return; // Invalid initial weight mean
        }
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    if (key.empty()) {
        return; // Invalid key, silently ignore
    }
    
    // Boolean values don't need additional validation
    
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    if (key.empty()) {
        return false;
    }
    
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    if (key.empty()) {
        return; // Nothing to remove
    }
    
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
        if (!entry.key.empty()) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

// Configuration validation methods
bool Config::validate() const {
    std::vector<std::string> errors;
    
    // Check for duplicate keys
    std::set<std::string> uniqueKeys;
    for (const auto& entry : pImpl->entries) {
        if (uniqueKeys.count(entry.key) > 0) {
            errors.push_back("Duplicate key: " + entry.key);
        }
        uniqueKeys.insert(entry.key);
    }
    
    // Validate common configuration parameters
    for (const auto& entry : pImpl->entries) {
        if (entry.key == "brain.neuron_count") {
            auto value = get<int>(entry.key);
            if (value && (*value < 100 || *value > 1000000)) {
                errors.push_back("Invalid brain.neuron_count: " + std::to_string(*value) +
                               ". Must be between 100 and 1000000");
            }
        }
        
        if (entry.key == "brain.synapse_density") {
            auto value = get<double>(entry.key);
            if (value && (*value < 0 || *value > 1)) {
                errors.push_back("Invalid brain.synapse_density: " + std::to_string(*value) +
                               ". Must be between 0 and 1");
            }
        }
        
        if (entry.key == "brain.v_thresh") {
            auto value = get<double>(entry.key);
            if (value && (*value < -100 || *value > 100)) {
                errors.push_back("Invalid brain.v_thresh: " + std::to_string(*value) +
                               ". Must be between -100 and 100");
            }
        }
        
        if (entry.key == "brain.v_rest") {
            auto value = get<double>(entry.key);
            if (value && (*value < -100 || *value > 100)) {
                errors.push_back("Invalid brain.v_rest: " + std::to_string(*value) +
                               ". Must be between -100 and 100");
            }
        }
        
        if (entry.key == "brain.tau_mem") {
            auto value = get<double>(entry.key);
            if (value && (*value < 0 || *value > 1000)) {
                errors.push_back("Invalid brain.tau_mem: " + std::to_string(*value) +
                               ". Must be between 0 and 1000");
            }
        }
        
        if (entry.key == "brain.tau_ref") {
            auto value = get<double>(entry.key);
            if (value && (*value < 0 || *value > 100)) {
                errors.push_back("Invalid brain.tau_ref: " + std::to_string(*value) +
                               ". Must be between 0 and 100");
            }
        }
        
        // Check for empty values for critical parameters
        if (entry.key.empty()) {
            errors.push_back("Empty configuration key");
        }
    }
    
    return errors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    return std::vector<std::string>(); // Placeholder - implementation would collect errors
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            continue; // Skip entries with empty keys
        }
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
