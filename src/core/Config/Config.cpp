#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()), expertOptions_() {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)), expertOptions_(std::move(other.expertOptions_)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        expertOptions_ = std::move(other.expertOptions_);
        validationErrors_ = std::move(other.validationErrors_);
    }
    return *this;
}

ConfigValue Config::getAnyType(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::string(); // Empty string as default for unknown types
    }
    
    return it->value;
}

bool Config::loadFromFile(const std::string& filepath) {
    // Check file extension for format
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    bool success = false;
    if (ext == "json") {
        success = loadFromJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        success = loadFromYAML(filepath);
    } else if (ext == "toml") {
        success = loadFromTOML(filepath);
    } else {
        // Default to simple key=value format
        success = loadFromSimpleFile(filepath);
    }
    
    return success;
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
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

bool Config::loadFromJSON(const std::string& filepath) {
    // Simplified JSON parser - placeholder for full implementation
    // TODO: Implement proper JSON parsing with error handling
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // For now, delegate to simple parser but add expert options
    bool success = loadFromSimpleFile(filepath);
    
    // Set some default expert options for JSON configs
    if (success) {
        expertOptions_.enableDebugLogging = true;
        expertOptions_.enableMetrics = true;
    }
    
    return success;
}

bool Config::loadFromYAML(const std::string& filepath) {
    // Simplified YAML parser - placeholder for full implementation
    // TODO: Implement proper YAML parsing
    return loadFromSimpleFile(filepath);
}

bool Config::loadFromTOML(const std::string& filepath) {
    // Simplified TOML parser - placeholder for full implementation
    // TODO: Implement proper TOML parsing
    return loadFromSimpleFile(filepath);
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
        // Handle expert mode flags
        else if (arg == "--expert" || arg == "-x") {
            expertOptions_.enableDebugLogging = true;
            expertOptions_.enableMetrics = true;
            set("expert_mode", true, ConfigSource::CommandLine, "Enable expert mode features", "Expert mode enables advanced debugging and metrics");
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath, bool prettyPrint) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (prettyPrint) {
        // Format as JSON-like structure for human readability
        file << "{\n";
        bool first = true;
        for (const auto& entry : pImpl->entries) {
            if (!first) file << ",\n";
            first = false;
            
            file << "  \"" << entry.key << "\": ";
            
            // Format value based on type
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    file << arg;
                } else {
                    file << "\"" << "unknown" << "\"";
                }
            }, entry.value);
            
            // Add description as comment
            if (!entry.description.empty()) {
                file << "  // " << entry.description;
            }
        }
        file << "\n}\n";
    } else {
        // Simple key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            // Format value as string
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
    }
    
    return true;
}

std::string Config::summary(bool includeExpert, bool includeValidation) const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        if (includeExpert || !entry.expertOnly) {
            oss << "  " << entry.key << " = ";
            
            // Format value based on type
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, entry.value);
            
            oss << " (" << static_cast<int>(entry.source) << ")";
            if (!entry.description.empty()) {
                oss << " - " << entry.description;
            }
            oss << "\n";
        }
    }
    
    if (includeValidation && !validationErrors_.empty()) {
        oss << "\nValidation Errors:\n";
        for (const auto& error : validationErrors_) {
            oss << "  ERROR: " << error << "\n";
        }
    }
    
    if (includeExpert) {
        oss << "\nExpert Options:\n";
        oss << "  enableSIMD: " << (expertOptions_.enableSIMD ? "true" : "false") << "\n";
        oss << "  enableParallel: " << (expertOptions_.enableParallel ? "true" : "false") << "\n";
        oss << "  threadCount: " << expertOptions_.threadCount << "\n";
        oss << "  enableDebugLogging: " << (expertOptions_.enableDebugLogging ? "true" : "false") << "\n";
        oss << "  enableMetrics: " << (expertOptions_.enableMetrics ? "true" : "false") << "\n";
    }
    
    return oss.str();
}

std::string Config::getUserFriendlySummary() const {
    std::ostringstream oss;
    oss << "User Configuration:\n";
    oss << "  Key performance settings are optimized for typical use.\n";
    oss << "  Enable expert mode with --expert flag for advanced options.\n";
    oss << "\nCurrent settings:\n";
    
    for (const auto& entry : pImpl->entries) {
        if (!entry.expertOnly) {
            oss << "    " << entry.key << ": ";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << arg;
                } else {
                    oss << arg;
                }
            }, entry.value);
            oss << "\n";
        }
    }
    
    return oss.str();
}

std::string Config::getExpertSummary() const {
    std::ostringstream oss;
    oss << "Expert Configuration:\n";
    oss << summary(true, true) << "\n";
    
    oss << "\nPerformance Recommendations:\n";
    if (expertOptions_.enableSIMD && expertOptions_.enableParallel) {
        oss << "  SIMD and parallel processing enabled - expect significant speed improvements.\n";
    }
    
    if (expertOptions_.enableDebugLogging) {
        oss << "  Debug logging enabled - expect verbose output.\n";
    }
    
    oss << "\nAdvanced Options Available:\n";
    oss << "  - Enable adaptive time stepping for variable simulation speed\n";
    oss << "  - Configure custom neuromodulation parameters\n";
    oss << "  - Set up advanced memory management strategies\n";
    
    return oss.str();
}

bool Config::executeScript(const std::string& scriptContent, const ExpertConfigOptions& expertOpts) {
    return executeScriptImpl(scriptContent, expertOpts);
}

bool Config::executeScriptImpl(const std::string& scriptContent, const ExpertConfigOptions& expertOpts) {
    // Placeholder for script execution - parse and apply configuration changes
    // This would use a simple scripting language for expert users
    
    // For now, just set expert options and validate
    setExpertOptions(expertOpts);
    
    // Clear any existing validation errors
    validationErrors_.clear();
    
    // Parse script (simplified - real implementation would use proper scripting engine)
    std::istringstream stream(scriptContent);
    std::string line;
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Very simple script parsing: key = value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            set(key, value, ConfigSource::Script);
        }
    }
    
    return true;
}

bool Config::validateKey(const std::string& key) const {
    // Basic validation: key should not be empty and should only contain valid characters
    if (key.empty()) {
        validationErrors_.push_back("Empty key not allowed");
        return false;
    }
    
    // Check for valid characters (alphanumeric, underscore, hyphen)
    std::regex validKeyPattern("^[a-zA-Z0-9_-]+$");
    if (!std::regex_match(key, validKeyPattern)) {
        validationErrors_.push_back("Key '" + key + "' contains invalid characters. Only alphanumeric, underscore, and hyphen allowed.");
        return false;
    }
    
    return true;
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Validation depends on key type
    if (key == "neuron_count") {
        if (std::holds_alternative<int64_t>(value)) {
            int64_t val = std::get<int64_t>(value);
            if (val <= 0) {
                validationErrors_.push_back("neuron_count must be positive");
                return false;
            }
        } else if (std::holds_alternative<int>(value)) {
            int val = std::get<int>(value);
            if (val <= 0) {
                validationErrors_.push_back("neuron_count must be positive");
                return false;
            }
        }
    } else if (key == "connection_probability") {
        if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (val < 0.0 || val > 1.0) {
                validationErrors_.push_back("connection_probability must be between 0.0 and 1.0");
                return false;
            }
        }
    }
    
    return true;
}

std::vector<std::string> Config::getValidationErrors() const {
    return validationErrors_;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source,
                 const std::string& description, const std::string& expertNote, bool expert) {
    // Validate key first
    if (!validateKey(key)) {
        return;
    }
    
    // Validate value if key exists
    auto existing = getAnyType(key);
    if (existing.has_value()) {
        if (!validateValue(key, value)) {
            return;
        }
    }
    
    ConfigEntry entry;
    entry.key = key;
    entry.value = value;
    entry.source = source;
    entry.description = description;
    entry.expertNote = expertNote;
    entry.expertOnly = expert;
    
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                      [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    pImpl->entries.push_back(entry);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source,
                 const std::string& description, const std::string& expertNote, bool expert) {
    set(key, ConfigValue(value), source, description, expertNote, expert);
}

void Config::set(const std::string& key, int value, ConfigSource source,
                 const std::string& description, const std::string& expertNote, bool expert) {
    set(key, ConfigValue(value), source, description, expertNote, expert);
}

void Config::set(const std::string& key, double value, ConfigSource source,
                 const std::string& description, const std::string& expertNote, bool expert) {
    set(key, ConfigValue(value), source, description, expertNote, expert);
}

void Config::set(const std::string& key, bool value, ConfigSource source,
                 const std::string& description, const std::string& expertNote, bool expert) {
    set(key, ConfigValue(value), source, description, expertNote, expert);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

bool Config::isExpertOnly(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->expertOnly;
    }
    return false;
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                      [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys(bool includeExpertOnly) const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        if (includeExpertOnly || !entry.expertOnly) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::vector<std::string> Config::getExpertKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.expertOnly) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

bool Config::merge(const Config& other, ConfigSource source) {
    bool changed = false;
    for (const auto& entry : other.pImpl->entries) {
        bool exists = false;
        for (auto& existing : pImpl->entries) {
            if (existing.key == entry.key) {
                existing.value = entry.value;
                existing.source = source; // Override with new source
                changed = true;
                exists = true;
                break;
            }
        }
        if (!exists) {
            pImpl->entries.push_back(entry);
            changed = true;
        }
    }
    return changed;
}

bool Config::isSubset(const Config& other) const {
    for (const auto& entry : pImpl->entries) {
        bool found = false;
        for (const auto& otherEntry : other.pImpl->entries) {
            if (entry.key == otherEntry.key) {
                // For simple comparison, just check if key exists
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    return true;
}

void Config::clear() {
    pImpl->entries.clear();
    validationErrors_.clear();
}
