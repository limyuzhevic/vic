#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string lastError;
    FileFormat fileFormat;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->fileFormat = FileFormat::Auto;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        setError("Cannot open file: " + filepath);
        return false;
    }
    
    // Clear existing entries
    clear();
    
    // Detect file format
    FileFormat format = detectFileFormat(filepath);
    setFileFormat(format);
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        try {
            if (pImpl->fileFormat == FileFormat::JSON) {
                // For JSON files, we'd need to parse the entire structure
                // This is a simplified implementation - proper JSON parsing would be more complex
                setError("JSON file format not yet fully implemented");
                return false;
            } else {
                // Parse key=value pairs for INI/CFG format
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    
                    // Remove surrounding quotes
                    if (value.size() >= 2 && 
                        ((value.front() == '"' && value.back() == '"') ||
                         (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    set(key, value, ConfigSource::File);
                } else {
                    // Try to handle boolean values without equals sign
                    if (line == "true" || line == "false") {
                        set(line, (line == "true"), ConfigSource::File);
                    }
                }
            }
        } catch (const std::exception& e) {
            setError("Error parsing line " + std::to_string(lineNumber) + ": " + e.what());
            return false;
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    clearError();
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            } else {
                setError("Invalid command line argument format: " + arg);
                return false;
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        } else {
            setError("Invalid command line argument: " + arg);
            return false;
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        setError("Cannot open file for writing: " + filepath);
        return false;
    }
    
    switch (pImpl->fileFormat) {
        case FileFormat::JSON:
            // Export in JSON format
            file << exportToJSON();
            break;
        case FileFormat::YAML:
            // YAML export would require a YAML library
            file << "# YAML format not yet implemented\n";
            for (const auto& entry : pImpl->entries) {
                file << entry.key << ": " << "PLACEHOLDER_VALUE\n";
            }
            break;
        case FileFormat::INI:
            // INI format
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                file << entry.key << " = ";
                
                try {
                    if (std::holds_alternative<int>(entry.value)) {
                        file << std::get<int>(entry.value);
                    } else if (std::holds_alternative<int64_t>(entry.value)) {
                        file << std::get<int64_t>(entry.value);
                    } else if (std::holds_alternative<double>(entry.value)) {
                        file << std::get<double>(entry.value);
                    } else if (std::holds_alternative<bool>(entry.value)) {
                        file << (std::get<bool>(entry.value) ? "true" : "false");
                    } else if (std::holds_alternative<std::string>(entry.value)) {
                        file << "\"" << std::get<std::string>(entry.value) << "\"";
                    } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
                        const auto& vec = std::get<std::vector<int>>(entry.value);
                        file << "[";
                        for (size_t i = 0; i < vec.size(); ++i) {
                            file << vec[i];
                            if (i < vec.size() - 1) file << ", ";
                        }
                        file << "]";
                    } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
                        const auto& vec = std::get<std::vector<double>>(entry.value);
                        file << "[";
                        for (size_t i = 0; i < vec.size(); ++i) {
                            file << vec[i];
                            if (i < vec.size() - 1) file << ", ";
                        }
                        file << "]";
                    } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                        const auto& vec = std::get<std::vector<std::string>>(entry.value);
                        file << "[";
                        for (size_t i = 0; i < vec.size(); ++i) {
                            file << "\"" << vec[i] << "\"";
                            if (i < vec.size() - 1) file << ", ";
                        }
                        file << "]";
                    }
                } catch (const std::bad_variant_access&) {
                    file << "\"UNKNOWN_TYPE\"";
                }
                
                file << "\n\n";
            }
            break;
        case FileFormat::CFG:
        case FileFormat::Auto:
        default:
            // CFG format (key=value)
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                file << entry.key << " = ";
                
                try {
                    if (std::holds_alternative<int>(entry.value)) {
                        file << std::get<int>(entry.value);
                    } else if (std::holds_alternative<int64_t>(entry.value)) {
                        file << std::get<int64_t>(entry.value);
                    } else if (std::holds_alternative<double>(entry.value)) {
                        file << std::get<double>(entry.value);
                    } else if (std::holds_alternative<bool>(entry.value)) {
                        file << (std::get<bool>(entry.value) ? "true" : "false");
                    } else if (std::holds_alternative<std::string>(entry.value)) {
                        file << "\"" << std::get<std::string>(entry.value) << "\"";
                    } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
                        const auto& vec = std::get<std::vector<int>>(entry.value);
                        file << "[";
                        for (size_t i = 0; i < vec.size(); ++i) {
                            file << vec[i];
                            if (i < vec.size() - 1) file << ", ";
                        }
                        file << "]";
                    } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
                        const auto& vec = std::get<std::vector<double>>(entry.value);
                        file << "[";
                        for (size_t i = 0; i < vec.size(); ++i) {
                            file << vec[i];
                            if (i < vec.size() - 1) file << ", ";
                        }
                        file << "]";
                    } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                        const auto& vec = std::get<std::vector<std::string>>(entry.value);
                        file << "[";
                        for (size_t i = 0; i < vec.size(); ++i) {
                            file << "\"" << vec[i] << "\"";
                            if (i < vec.size() - 1) file << ", ";
                        }
                        file << "]";
                    }
                } catch (const std::bad_variant_access&) {
                    file << "\"UNKNOWN_TYPE\"";
                }
                
                file << "\n\n";
            }
            break;
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
        setError("Type mismatch for key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    clearError();
    
    if (!validateConfig(key, value)) {
        return;
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
    clearError();
    
    // Validate key and value
    if (key.empty() || key.find('=') != std::string::npos) {
        setError("Invalid key: " + key);
        return;
    }
    
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

bool Config::validateConfig(const std::string& key, const ConfigValue& value) const {
    if (key.empty()) {
        setError("Key cannot be empty");
        return false;
    }
    
    if (key.find('=') != std::string::npos || key.find(' ') != std::string::npos) {
        setError("Key cannot contain '=' or spaces: " + key);
        return false;
    }
    
    // Additional validation can be added here
    return true;
}

bool Config::isValidKey(const std::string& key) const {
    return !key.empty() && key.find('=') == std::string::npos && key.find(' ') == std::string::npos;
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    clearError();
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

std::vector<std::string> Config::getKeysBySource(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::clear() {
    clearError();
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    oss << "File format: " << static_cast<int>(pImpl->fileFormat) << "\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
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
                    oss << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) oss << ", ";
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
        oss << "\n";
    }
    return oss.str();
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    
    setError("Key not found: " + key);
    return ConfigSource::Default;
}

std::string Config::getDescription(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->description;
    }
    
    setError("Key not found: " + key);
    return "";
}

bool Config::merge(const Config& other) {
    clearError();
    
    for (const auto& otherEntry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&otherEntry](const ConfigEntry& e) { return e.key == otherEntry.key; });
        
        if (it != pImpl->entries.end()) {
            // Update existing entry
            it->value = otherEntry.value;
            // Keep current source if it's not Default
            if (it->source == ConfigSource::Default) {
                it->source = otherEntry.source;
            }
        } else {
            // Add new entry
            pImpl->entries.push_back(otherEntry);
        }
    }
    
    return true;
}

bool Config::importFromJSON(const std::string& jsonString) {
    clearError();
    
    try {
        auto json = nlohmann::json::parse(jsonString);
        
        // Clear existing entries
        clear();
        
        // Import entries from JSON
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            
            if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_number_integer()) {
                value = it->get<int64_t>();
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_array()) {
                // Try to determine array element type
                if (!it->empty()) {
                    if (it->front().is_number_integer()) {
                        value = it->get<std::vector<int64_t>>();
                    } else if (it->front().is_number_float()) {
                        value = it->get<std::vector<double>>();
                    } else if (it->front().is_string()) {
                        value = it->get<std::vector<std::string>>();
                    }
                }
            }
            
            set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        setError("JSON parsing error: " + std::string(e.what()));
        return false;
    }
}

std::string Config::exportToJSON() const {
    nlohmann::json json;
    
    for (const auto& entry : pImpl->entries) {
        json[entry.key] = configValueToJSON(entry.value);
    }
    
    return json.dump(2); // Pretty print with 2-space indentation
}

void Config::setFileFormat(FileFormat format) {
    pImpl->fileFormat = format;
}

FileFormat Config::getFileFormat() const {
    return pImpl->fileFormat;
}

void Config::setError(const std::string& error) {
    pImpl->lastError = error;
}

std::string Config::getLastError() const {
    return pImpl->lastError;
}

void Config::clearError() {
    pImpl->lastError.clear();
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

// Helper function implementations
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

bool Config::isNumber(const std::string& str) {
    if (str.empty()) return false;
    
    size_t posDecimal = str.find('.');
    size_t posExponent = str.find_first_of("eE");
    
    // Check if string contains only digits, optional decimal point, and optional exponent
    for (char c : str) {
        if (!(std::isdigit(c) || c == '.' || c == 'e' || c == 'E' || c == '+' || c == '-')) {
            return false;
        }
    }
    
    return true;
}

Config::FileFormat Config::detectFileFormat(const std::string& filepath) {
    std::string extension = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "json") {
        return FileFormat::JSON;
    } else if (extension == "yaml" || extension == "yml") {
        return FileFormat::YAML;
    } else if (extension == "ini") {
        return FileFormat::INI;
    } else {
        return FileFormat::CFG;
    }
}

nlohmann::json Config::configValueToJSON(const ConfigValue& value) {
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

std::optional<Config::ConfigValue> Config::jsonToConfigValue(const nlohmann::json& j) {
    std::optional<ConfigValue> result;
    
    if (j.is_string()) {
        result = j.get<std::string>();
    } else if (j.is_number_integer()) {
        result = j.get<int64_t>();
    } else if (j.is_number_float()) {
        result = j.get<double>();
    } else if (j.is_boolean()) {
        result = j.get<bool>();
    } else if (j.is_array()) {
        if (!j.empty()) {
            if (j.front().is_number_integer()) {
                result = j.get<std::vector<int64_t>>();
            } else if (j.front().is_number_float()) {
                result = j.get<std::vector<double>>();
            } else if (j.front().is_string()) {
                result = j.get<std::vector<std::string>>();
            }
        }
    }
    
    return result;
}

// Configuration preset implementations
std::shared_ptr<Config> Config::createDefaultConfig() {
    auto config = std::make_shared<Config>();
    
    // Basic brain settings
    config->set("brain.neuron_count", 1000);
    config->set("brain.synapse_density", 0.1);
    config->set("brain.connection_probability", 0.05);
    config->set("brain.initial_weight_mean", 0.5);
    config->set("brain.initial_weight_std", 0.1);
    config->set("brain.v_thresh", -50.0);
    config->set("brain.v_rest", -70.0);
    config->set("brain.v_reset", -75.0);
    config->set("brain.tau_mem", 20.0);
    config->set("brain.tau_ref", 2.0);
    
    // Plasticity settings
    config->set("plasticity.stdp.enable", true);
    config->set("plasticity.stdp.learning_rate", 0.001);
    config->set("plasticity.stdp.tau_plus", 20.0);
    config->set("plasticity.stdp.tau_minus", 20.0);
    config->set("plasticity.hebbian.enable", true);
    config->set("plasticity.structural.enable", true);
    
    // Neuromodulation settings
    config->set("neuromod.dopamine.scale", 1.0);
    config->set("neuromod.curiosity.enable", true);
    config->set("neuromod.novelty.enable", true);
    
    // Development settings
    config->set("development.synaptogenesis_rate", 0.001);
    config->set("development.pruning_rate", 0.0001);
    
    // Simulation settings
    config->set("simulation_timestep", 0.001);
    config->set("random_seed", 42);
    
    return config;
}

std::shared_ptr<Config> Config::createMinimalConfig() {
    auto config = std::make_shared<Config>();
    
    // Minimal settings for basic operation
    config->set("brain.neuron_count", 100);
    config->set("brain.synapse_density", 0.1);
    config->set("simulation_timestep", 0.001);
    config->set("random_seed", 42);
    
    return config;
}

std::shared_ptr<Config> Config::createResearchConfig() {
    auto config = std::make_shared<Config>();
    
    // Research-focused settings with advanced features
    config->set("brain.neuron_count", 5000);
    config->set("brain.synapse_density", 0.15);
    config->set("plasticity.stdp.learning_rate", 0.002);
    config->set("plasticity.structural.enable", true);
    config->set("neuromod.curiosity.enable", true);
    config->set("neuromod.novelty.enable", true);
    config->set("brain.enable_development", true);
    config->set("development.synaptogenesis_rate", 0.002);
    config->set("development.pruning_rate", 0.0002);
    config->set("simulation_timestep", 0.0005);
    config->set("random_seed", 12345);
    
    return config;
}

bool Config::validateAllConfigs(const Config& config) {
    // Basic validation - can be extended with more specific checks
    if (config.getEntryCount() == 0) {
        return false;
    }
    
    // Check for essential keys
    auto essentialKeys = config.getKeys();
    bool hasEssential = false;
    for (const auto& key : essentialKeys) {
        if (key.find("neuron_count") != std::string::npos ||
            key.find("simulation_timestep") != std::string::npos) {
            hasEssential = true;
            break;
        }
    }
    
    return hasEssential;
}

std::vector<std::string> Config::getValidationErrors(const Config& config) {
    std::vector<std::string> errors;
    
    if (config.getEntryCount() == 0) {
        errors.push_back("Configuration is empty");
    }
    
    // Check for negative values where not allowed
    for (const auto& entry : config.pImpl->entries) {
        try {
            if (std::holds_alternative<double>(entry.value)) {
                double value = std::get<double>(entry.value);
                if (value < 0.0) {
                    errors.push_back("Negative value for key: " + entry.key);
                }
            } else if (std::holds_alternative<int>(entry.value)) {
                int value = std::get<int>(entry.value);
                if (value < 0) {
                    errors.push_back("Negative value for key: " + entry.key);
                }
            }
        } catch (const std::bad_variant_access&) {
            // Skip if type is not handled
        }
    }
    
    return errors;
}

} // namespace nlm

// Note: The nlohmann/json library needs to be added to CMakeLists.txt
// Add the following to CMakeLists.txt:
// find_package(nlohmann_json REQUIRED)
// target_link_libraries(pynlm_core nlohmann_json::nlohmann_json)
