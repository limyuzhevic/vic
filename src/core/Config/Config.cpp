// nlm - Neural Learning Machine
// Core Configuration System
// Phase 2: Enhanced Configuration Management
//
// This phase enhances the configuration system with:
// - Better error handling and validation
// - Support for environment variables
// - Configuration inheritance
// - Enhanced serialization (JSON/YAML support)
// - Validation rules for different value types
// - Better documentation and help system

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <cctype>
#include <regex>
#include <memory>
#include <optional>

using namespace nlm;

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> descriptions;
    bool validateOnSet;
    bool allowUnknownKeys;
    
    Impl() : validateOnSet(true), allowUnknownKeys(false) {}
    
    // Helper to normalize key names
    std::string normalizeKey(const std::string& key) const {
        std::string result = key;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        // Replace hyphens with underscores
        std::replace(result.begin(), result.end(), '-', '_');
        return result;
    }
    
    // Helper to convert string to appropriate type
    ConfigValue stringToValue(const std::string& value, const std::string& type) const {
        if (type == "string") return value;
        else if (type == "int") return std::stoi(value);
        else if (type == "int64") return std::stoll(value);
        else if (type == "double") return std::stod(value);
        else if (type == "float") return std::stof(value);
        else if (type == "bool") {
            std::string lower = value;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            return (lower == "true" || lower == "1" || lower == "yes");
        }
        else if (type == "string_list") {
            std::vector<std::string> result;
            std::stringstream ss(value);
            std::string item;
            while (std::getline(ss, item, ',')) {
                item = trim(item);
                if (!item.empty()) result.push_back(item);
            }
            return result;
        }
        else if (type == "float_list") {
            std::vector<double> result;
            std::stringstream ss(value);
            std::string item;
            while (std::getline(ss, item, ',')) {
                item = trim(item);
                if (!item.empty()) result.push_back(std::stod(item));
            }
            return result;
        }
        return value; // Default to string
    }
    
    // Helper to validate configuration entry
    bool validateEntry(const ConfigEntry& entry) const {
        if (!validateOnSet) return true;
        
        // Check if key exists in descriptions
        auto descIt = descriptions.find(normalizeKey(entry.key));
        if (descIt == descriptions.end()) {
            return allowUnknownKeys;
        }
        
        const auto& desc = descIt->second;
        
        // Check if value type matches expected type
        bool typeMatches = false;
        if (desc.type == "string" && std::holds_alternative<std::string>(entry.value)) typeMatches = true;
        else if (desc.type == "int" && std::holds_alternative<int>(entry.value)) typeMatches = true;
        else if (desc.type == "int64" && std::holds_alternative<int64_t>(entry.value)) typeMatches = true;
        else if (desc.type == "double" && std::holds_alternative<double>(entry.value)) typeMatches = true;
        else if (desc.type == "float" && std::holds_alternative<double>(entry.value)) typeMatches = true;
        else if (desc.type == "bool" && std::holds_alternative<bool>(entry.value)) typeMatches = true;
        else if (desc.type == "string_list" && std::holds_alternative<std::vector<std::string>>(entry.value)) typeMatches = true;
        else if (desc.type == "float_list" && std::holds_alternative<std::vector<double>>(entry.value)) typeMatches = true;
        
        if (!typeMatches) return false;
        
        return true;
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default configuration descriptions
    initializeDescriptions();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Enhanced loadFromFile with JSON support
bool Config::loadFromFile(const std::string& filepath) {
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    // Try to detect file format
    std::string extension = "." + std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        return loadFromJSON(filepath);
    } else {
        // Fall back to simple key=value format
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        file >> j;
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            if (it->is_string()) {
                configValue = it->get<std::string>();
            } else if (it->is_number_integer()) {
                configValue = it->get<int64_t>();
            } else if (it->is_number_float()) {
                configValue = it->get<double>();
            } else if (it->is_boolean()) {
                configValue = it->get<bool>();
            } else if (it->is_array()) {
                // Check if array contains strings or numbers
                bool isStringArray = true;
                bool isNumberArray = true;
                
                for (const auto& item : *it) {
                    if (!item.is_string()) isStringArray = false;
                    if (!item.is_number()) isNumberArray = false;
                }
                
                if (isStringArray) {
                    std::vector<std::string> strArray;
                    for (const auto& item : *it) {
                        strArray.push_back(item.get<std::string>());
                    }
                    configValue = strArray;
                } else if (isNumberArray) {
                    std::vector<double> numArray;
                    for (const auto& item : *it) {
                        numArray.push_back(item.get<double>());
                    }
                    configValue = numArray;
                } else {
                    // Default to string representation
                    configValue = it->dump();
                }
            } else {
                configValue = it->dump();
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
        
    } catch (const json::parse_error& e) {
        NLM_LOG_ERROR("Failed to parse JSON config file " + filepath + ": " + std::string(e.what()));
        return false;
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
            
            // Check for type annotation
            size_t colonPos = key.find(':');
            std::string type = "string";
            if (colonPos != std::string::npos) {
                type = trim(key.substr(colonPos + 1));
                key = trim(key.substr(0, colonPos));
            }
            
            // Convert value to appropriate type
            ConfigValue configValue = pImpl->stringToValue(value, type);
            set(key, configValue, ConfigSource::File);
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
    
    // Get file extension
    std::string extension = "." + std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        saveToJSON(filepath);
    } else {
        saveToSimpleFormat(filepath);
    }
    
    return true;
}

void Config::saveToJSON(const std::string& filepath) const {
    json j;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&j, &entry](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                j[entry.key] = val;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                j[entry.key] = val;
            } else if constexpr (std::is_same_v<T, double>) {
                j[entry.key] = val;
            } else if constexpr (std::is_same_v<T, bool>) {
                j[entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                j[entry.key] = val;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                j[entry.key] = val;
            }
        }, entry.value);
    }
    
    std::ofstream file(filepath);
    file << j.dump(2); // Pretty print with 2-space indentation
}

void Config::saveToSimpleFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << entry.key << " = ";
        
        std::visit([&file](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, int64_t> ||
                                 std::is_same_v<T, double> ||
                                 std::is_same_v<T, bool>) {
                file << val;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i) file << ", ";
                    file << "\"" << val[i] << "\"";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i) file << ", ";
                    file << val[i];
                }
                file << "]";
            } else {
                file << "UNKNOWN";
            }
        }, entry.value);
        
        file << "\n";
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Normalize key name
    std::string normalizedKey = pImpl->normalizeKey(key);
    
    // Validate the value before setting
    if (pImpl->validateEntry(ConfigEntry(key, value, source))) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&normalizedKey](const ConfigEntry& e) { return pImpl->normalizeKey(e.key) == normalizedKey; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
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
    std::string normalizedKey = pImpl->normalizeKey(key);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalizedKey](const ConfigEntry& e) { return pImpl->normalizeKey(e.key) == normalizedKey; });
}

void Config::remove(const std::string& key) {
    std::string normalizedKey = pImpl->normalizeKey(key);
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&normalizedKey](const ConfigEntry& e) { return pImpl->normalizeKey(e.key) == normalizedKey; }),
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
    oss << "Enhanced Configuration (" << pImpl->entries.size() << " entries):\n";
    
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

// Helper to initialize configuration descriptions
void Config::initializeDescriptions() {
    // Register common configuration keys with descriptions
    
    // Neural network parameters
    pImpl->descriptions["neuron_count"] = ConfigDescription{
        "neuron_count", "int64", "Number of neurons in the network",
        ConfigValue(static_cast<int64_t>(100)), true, {}, {}
    };
    
    pImpl->descriptions["region_count"] = ConfigDescription{
        "region_count", "int64", "Number of neural regions",
        ConfigValue(static_cast<int64_t>(1)), true, {}, {}
    };
    
    pImpl->descriptions["connection_probability"] = ConfigDescription{
        "connection_probability", "double", "Probability of connection between neurons",
        ConfigValue(0.15), true, {}, {}
    };
    
    // STDP parameters
    pImpl->descriptions["stdp_ltp_weight"] = ConfigDescription{
        "stdp_ltp_weight", "double", "STDP Long-Term Potentiation weight",
        ConfigValue(0.02), true, {}, {}
    };
    
    pImpl->descriptions["stdp_ltd_weight"] = ConfigDescription{
        "stdp_ltd_weight", "double", "STDP Long-Term Depression weight",
        ConfigValue(0.015), true, {}, {}
    };
    
    pImpl->descriptions["stdp_tau"] = ConfigDescription{
        "stdp_tau", "double", "STDP time constant (ms)",
        ConfigValue(20.0), true, {}, {}
    };
    
    // Structural plasticity parameters
    pImpl->descriptions["synaptogenesis_rate"] = ConfigDescription{
        "synaptogenesis_rate", "double", "Rate of synaptogenesis (per step)",
        ConfigValue(0.0001), true, {}, {}
    };
    
    pImpl->descriptions["pruning_rate"] = ConfigDescription{
        "pruning_rate", "double", "Rate of pruning (per step)",
        ConfigValue(0.00001), true, {}, {}
    };
    
    // Simulation parameters
    pImpl->descriptions["simulation_timestep"] = ConfigDescription{
        "simulation_timestep", "double", "Simulation time step (seconds)",
        ConfigValue(0.001), true, {}, {}
    };
    
    pImpl->descriptions["random_seed"] = ConfigDescription{
        "random_seed", "int64", "Random seed for reproducibility",
        ConfigValue(static_cast<int64_t>(42)), true, {}, {}
    };
}

const ConfigDescription& Config::getDescription(const std::string& key) const {
    static const ConfigDescription emptyDesc {"", "string", "", ConfigValue(), false, {}, {}};
    
    auto it = pImpl->descriptions.find(pImpl->normalizeKey(key));
    return (it != pImpl->descriptions.end()) ? it->second : emptyDesc;
}

// Template instantiations
namespace {
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
}

} // namespace nlm