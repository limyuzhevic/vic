#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <chrono>

namespace nlm {

// Config implementation constants
namespace ConfigConstants {
    constexpr size_t MAX_FILESIZE = 10 * 1024 * 1024;  // 10MB
    constexpr size_t MAX_LINE_LENGTH = 1024;
    constexpr size_t MAX_KEY_LENGTH = 256;
    constexpr size_t MAX_VALUE_LENGTH = 4096;
    constexpr size_t MAX_ENTRIES = 10000;
    
    // Valid configuration sections
    const std::vector<std::string> VALID_SECTIONS = {
        "brain", "simulation", "memory", "plasticity", 
        "neuromodulation", "cognition", "development", "world"
    };
    
    // Required configuration keys
    const std::vector<std::string> REQUIRED_KEYS = {
        "simulation_timestep", "random_seed"
    };
    
    // Default configuration values
    const std::map<std::string, ConfigValue> DEFAULT_VALUES = {
        {"simulation_timestep", 0.001},
        {"random_seed", static_cast<int64_t>(42)},
        {"neuron_count", static_cast<int64_t>(1000)},
        {"region_count", static_cast<int64_t>(1)},
        {"connection_probability", 0.1},
        {"stdp_ltp_weight", 0.01f},
        {"stdp_ltd_weight", 0.012f},
        {"stdp_tau", 20.0f},
        {"synaptogenesis_rate", 0.0001f},
        {"pruning_rate", 0.00001f},
        {"replay_interval", static_cast<size_t>(100)},
        {"consolidation_interval", static_cast<size_t>(1000)}
    };
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::chrono::system_clock::time_point lastModified;
    size_t maxEntries;
    
    Impl() : maxEntries(ConfigConstants::MAX_ENTRIES), lastModified(std::chrono::system_clock::now()) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    initializeDefaults();
}

Config::~Config() = default;

void Config::initializeDefaults() {
    for (const auto& pair : ConfigConstants::DEFAULT_VALUES) {
        set(pair.first, pair.second, ConfigSource::Default);
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    if (filepath.empty()) {
        NLM_LOG_ERROR("Config::loadFromFile: Empty filepath provided");
        return false;
    }
    
    // Check file existence and size
    std::error_code ec;
    if (!std::filesystem::exists(filepath, ec)) {
        NLM_LOG_WARNING("Config::loadFromFile: File not found: " + filepath);
        return false;
    }
    
    auto fileSize = std::filesystem::file_size(filepath, ec);
    if (ec) {
        NLM_LOG_ERROR("Config::loadFromFile: Failed to get file size for " + filepath + ": " + ec.message());
        return false;
    }
    
    if (fileSize > ConfigConstants::MAX_FILESIZE) {
        NLM_LOG_ERROR("Config::loadFromFile: File too large: " + filepath + " (" + 
                     std::to_string(fileSize) + " bytes > " + 
                     std::to_string(ConfigConstants::MAX_FILESIZE) + " bytes)");
        return false;
    }
    
    NLM_LOG_INFO("Config::loadFromFile: Loading configuration from " + filepath + " (" + 
                 std::to_string(fileSize) + " bytes)");
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Config::loadFromFile: Failed to open file: " + filepath);
        return false;
    }
    
    size_t lineCount = 0;
    size_t validEntries = 0;
    size_t errorCount = 0;
    
    std::string line;
    while (std::getline(file, line) && lineCount < ConfigConstants::MAX_LINE_LENGTH) {
        lineCount++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Config::loadFromFile: Invalid line format in " + filepath + ": " + line);
            errorCount++;
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty() || key.length() > ConfigConstants::MAX_KEY_LENGTH) {
            NLM_LOG_WARNING("Config::loadFromFile: Invalid key in " + filepath + ": " + key);
            errorCount++;
            continue;
        }
        
        // Validate value
        if (value.length() > ConfigConstants::MAX_VALUE_LENGTH) {
            NLM_LOG_WARNING("Config::loadFromFile: Value too long for key " + key + " in " + filepath);
            errorCount++;
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        try {
            set(key, value, ConfigSource::File);
            validEntries++;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Config::loadFromFile: Failed to set key " + key + ": " + e.what());
            errorCount++;
        }
    }
    
    pImpl->lastModified = std::chrono::system_clock::now();
    
    NLM_LOG_INFO("Config::loadFromFile: Loaded " + filepath + " - " + 
                 std::to_string(validEntries) + " valid entries, " + 
                 std::to_string(errorCount) + " errors, " + 
                 std::to_string(lineCount) + " lines processed");
    
    // Validate required keys
    std::vector<std::string> missingKeys;
    for (const auto& requiredKey : ConfigConstants::REQUIRED_KEYS) {
        if (!has(requiredKey)) {
            missingKeys.push_back(requiredKey);
        }
    }
    
    if (!missingKeys.empty()) {
        NLM_LOG_WARNING("Config::loadFromFile: Missing required keys after loading " + filepath + ": " + 
                        joinStrings(missingKeys, ", "));
    }
    
    return !errorCount || lineCount > 0;  // Allow some errors if file is not completely empty
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (!argv || argc <= 1) {
        NLM_LOG_WARNING("Config::loadFromArgs: No command line arguments to parse");
        return true;
    }
    
    NLM_LOG_INFO("Config::loadFromArgs: Parsing " + std::to_string(argc - 1) + " command line arguments");
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                try {
                    set(key, value, ConfigSource::CommandLine);
                    NLM_LOG_INFO("Config::loadFromArgs: Set key " + key + " = " + value);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Config::loadFromArgs: Failed to set key " + key + ": " + e.what());
                }
            } else {
                NLM_LOG_WARNING("Config::loadFromArgs: Malformed argument --" + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            try {
                set(key, value, ConfigSource::CommandLine);
                NLM_LOG_INFO("Config::loadFromArgs: Set key " + key + " = " + value);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Config::loadFromArgs: Failed to set key " + key + ": " + e.what());
            }
        } else {
            NLM_LOG_WARNING("Config::loadFromArgs: Unrecognized argument: " + arg);
        }
    }
    
    pImpl->lastModified = std::chrono::system_clock::now();
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    if (filepath.empty()) {
        NLM_LOG_ERROR("Config::saveToFile: Empty filepath provided");
        return false;
    }
    
    // Create backup if file exists
    std::error_code ec;
    if (std::filesystem::exists(filepath, ec)) {
        std::string backupPath = filepath + ".backup";
        if (!std::filesystem::copy_file(filepath, backupPath, std::filesystem::copy_options::overwrite_existing, ec)) {
            NLM_LOG_WARNING("Config::saveToFile: Failed to create backup file " + backupPath + ": " + ec.message());
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Config::saveToFile: Failed to open file for writing: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated at: " << formatTimestamp(pImpl->lastModified) << "\n";
    file << "# This file contains configuration for the Neural Learning Machine\n\n";
    
    // Write configuration sections
    std::vector<std::string> writtenKeys;
    for (const auto& section : ConfigConstants::VALID_SECTIONS) {
        bool hasSectionKeys = false;
        for (const auto& entry : pImpl->entries) {
            if (startsWith(entry.key, section + ".")) {
                hasSectionKeys = true;
                break;
            }
        }
        
        if (hasSectionKeys) {
            file << "# " << section << " section\n";
            for (const auto& entry : pImpl->entries) {
                if (startsWith(entry.key, section + ".")) {
                    std::string key = entry.key.substr(section.length() + 1);
                    file << section << "." << key << " = " << formatValue(entry.value) << "\n";
                    writtenKeys.push_back(entry.key);
                }
            }
            file << "\n";
        }
    }
    
    // Write remaining keys
    for (const auto& entry : pImpl->entries) {
        bool alreadyWritten = false;
        for (const auto& writtenKey : writtenKeys) {
            if (entry.key == writtenKey) {
                alreadyWritten = true;
                break;
            }
        }
        
        if (!alreadyWritten) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << formatValue(entry.value) << "\n\n";
        }
    }
    
    file << "# End of configuration file\n";
    
    if (!file.good()) {
        NLM_LOG_ERROR("Config::saveToFile: Error writing to file: " + filepath);
        return false;
    }
    
    pImpl->lastModified = std::chrono::system_clock::now();
    
    NLM_LOG_INFO("Config::saveToFile: Saved configuration to " + filepath + 
                 " (" + std::to_string(pImpl->entries.size()) + " entries)");
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (key.empty()) {
        NLM_LOG_ERROR("Config::get: Empty key provided");
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n
    if (it == pImpl->entries.end()) {
        NLM_LOG_DEBUG("Config::get: Key not found: " + key);
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_ERROR("Config::get: Type mismatch for key " + key);
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
        throw std::invalid_argument("Config::set: Empty key provided");
    }
    
    if (key.length() > ConfigConstants::MAX_KEY_LENGTH) {
        throw std::invalid_argument("Config::set: Key too long: " + key);
    }
    
    // Validate key format (allow alphanumeric, dots, underscores)
    if (!isValidKey(key)) {
        throw std::invalid_argument("Config::set: Invalid key format: " + key);
    }
    
    if (pImpl->entries.size() >= pImpl->maxEntries) {
        throw std::runtime_error("Config::set: Maximum number of entries reached");
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_DEBUG("Config::set: Updated key " + key + " with source " + 
                     std::to_string(static_cast<int>(source)));
    } else {
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_DEBUG("Config::set: Added new key " + key);
    }
    
    pImpl->lastModified = std::chrono::system_clock::now();
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
    if (key.empty()) {
        return false;
    }
    
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    if (key.empty()) {
        NLM_LOG_WARNING("Config::remove: Attempted to remove empty key");
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        NLM_LOG_INFO("Config::remove: Removed key " + key);
        pImpl->entries.erase(it);
        pImpl->lastModified = std::chrono::system_clock::now();
    }
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
    if (!pImpl->entries.empty()) {
        NLM_LOG_INFO("Config::clear: Clearing " + std::to_string(pImpl->entries.size()) + " configuration entries");
    }
    pImpl->entries.clear();
    pImpl->lastModified = std::chrono::system_clock::now();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << "/" << pImpl->maxEntries << " entries):\n";
    oss << "Last modified: " << formatTimestamp(pImpl->lastModified) << "\n\n";
    
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

bool Config::isValidKey(const std::string& key) {
    if (key.empty() || key.length() > MAX_KEY_LENGTH) {
        return false;
    }
    
    // Allow alphanumeric, dots, underscores
    for (char c : key) {
        if (!std::isalnum(c) && c != '.' && c != '_' && c != '-') {
            return false;
        }
    }
    
    // Check for valid sections
    for (const auto& section : VALID_SECTIONS) {
        if (key.find(section + ".") == 0) {
            return true;
        }
    }
    
    // Top-level keys are also valid
    return true;
}

std::string Config::formatValue(const ConfigValue& value) {
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

std::string Config::joinStrings(const std::vector<std::string>& strings, const std::string& separator) {
    if (strings.empty()) return "";
    
    std::ostringstream oss;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) oss << separator;
        oss << strings[i];
    }
    
    return oss.str();
}

std::string Config::formatTimestamp(const std::chrono::system_clock::time_point& timestamp) {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::tm tm = *std::localtime(&time);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

bool Config::startsWith(const std::string& str, const std::string& prefix) {
    return str.rfind(prefix, 0) == 0;
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
