#include "Config.hpp"
#include "../Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_DEBUG("Config::Config(): Created empty configuration");
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Parse configuration value string to ConfigValue
    static ConfigValue parseConfigValue(const std::string& str);
    
    NLM_LOG_DEBUG("Config::loadFromFile(): Attempting to load configuration from " + filepath);
    
    // Validate filepath
    if (filepath.empty()) {
        NLM_LOG_ERROR("Config::loadFromFile(): Empty filepath provided");
        return false;
    }
    
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        NLM_LOG_ERROR("Config::loadFromFile(): File does not exist: " + filepath);
        return false;
    }
    
    if (!std::filesystem::is_regular_file(filepath)) {
        NLM_LOG_ERROR("Config::loadFromFile(): Path is not a regular file: " + filepath);
        return false;
    }
    
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Config::loadFromFile(): Failed to open file for reading: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    size_t successfulEntries = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
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
            
            // Validate key
            if (key.empty()) {
                NLM_LOG_WARNING("Config::loadFromFile(): Empty key found at line " + std::to_string(lineNumber));
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
            ++successfulEntries;
        } else {
            NLM_LOG_WARNING("Config::loadFromFile(): Invalid format at line " + std::to_string(lineNumber) +
                           ": expected key=value format");
        }
    }
    
    NLM_LOG_INFO("Config::loadFromFile(): Loaded " + std::to_string(successfulEntries) + " entries from " + 
                 filepath + " (" + std::to_string(lineNumber) + " lines processed)");
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    NLM_LOG_DEBUG("Config::loadFromArgs(): Parsing command line arguments");
    
    // Validate arguments
    if (!argv) {
        NLM_LOG_ERROR("Config::loadFromArgs(): Null argv pointer");
        return false;
    }
    
    if (argc <= 0) {
        NLM_LOG_WARNING("Config::loadFromArgs(): No arguments provided");
        return true;  // Not an error, just nothing to do
    }
    
    size_t successfulEntries = 0;
    for (int i = 1; i < argc; ++i) {
        // Validate argv[i]
        if (!argv[i]) {
            NLM_LOG_WARNING("Config::loadFromArgs(): Null argument at index " + std::to_string(i));
            continue;
        }
        
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key
                if (key.empty()) {
                    NLM_LOG_WARNING("Config::loadFromArgs(): Empty key in argument " + arg);
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
                ++successfulEntries;
            } else {
                NLM_LOG_WARNING("Config::loadFromArgs(): Invalid --key=value format: " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            
            // Validate key
            if (key.empty()) {
                NLM_LOG_WARNING("Config::loadFromArgs(): Empty key in argument " + arg);
                continue;
            }
            
            // Validate next argument exists
            if (!argv[i + 1]) {
                NLM_LOG_WARNING("Config::loadFromArgs(): Null value for key " + key);
                continue;
            }
            
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
            ++successfulEntries;
        } else {
            NLM_LOG_WARNING("Config::loadFromArgs(): Unrecognized argument format: " + arg);
        }
    }
    
    NLM_LOG_INFO("Config::loadFromArgs(): Processed " + std::to_string(successfulEntries) + " configuration entries from command line");
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    NLM_LOG_DEBUG("Config::saveToFile(): Attempting to save configuration to " + filepath);
    
    // Validate filepath
    if (filepath.empty()) {
        NLM_LOG_ERROR("Config::saveToFile(): Empty filepath provided");
        return false;
    }
    
    // Check if parent directory exists and is writable
    std::filesystem::path path(filepath);
    std::filesystem::path parentDir = path.parent_path();
    
    if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
        NLM_LOG_WARNING("Config::saveToFile(): Parent directory does not exist: " + parentDir.string() +
                       ". Attempting to create it.");
        if (!std::filesystem::create_directories(parentDir)) {
            NLM_LOG_ERROR("Config::saveToFile(): Failed to create parent directory: " + parentDir.string());
            return false;
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Config::saveToFile(): Failed to open file for writing: " + filepath);
        return false;
    }
    
    // Write configuration in a readable format
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Convert value to string based on type
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
        
        file << "\n\n";
    }
    
    file.flush();
    if (!file.good()) {
        NLM_LOG_ERROR("Config::saveToFile(): Error occurred while writing to file: " + filepath);
        return false;
    }
    
    NLM_LOG_INFO("Config::saveToFile(): Successfully saved " + std::to_string(pImpl->entries.size()) + 
                 " entries to " + filepath);
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (key.empty()) {
        NLM_LOG_ERROR("Config::get(): Empty key provided");
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_DEBUG("Config::get(): Key not found: " + key);
        return std::nullopt;
    }
    
    try {
        NLM_LOG_DEBUG("Config::get(): Successfully retrieved value for key: " + key);
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_WARNING("Config::get(): Type mismatch for key " + key + ". Requested type doesn't match stored value");
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    if (key.empty()) {
        NLM_LOG_ERROR("Config::getOr(): Empty key provided");
        return defaultValue;
    }
    
    auto val = get<T>(key);
    if (val.has_value()) {
        NLM_LOG_DEBUG("Config::getOr(): Retrieved value for key " + key + ": " + std::to_string(val.value()));
        return val.value();
    } else {
        NLM_LOG_DEBUG("Config::getOr(): Key not found " + key + ", using default value: " + std::to_string(defaultValue));
        return defaultValue;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate inputs
    if (key.empty()) {
        NLM_LOG_ERROR("Config::set(): Empty key provided");
        return;
    }
    
    NLM_LOG_DEBUG("Config::set(): Setting key " + key + " = [value] (source: " + 
                 (source == ConfigSource::Default ? "Default" :
                  source == ConfigSource::File ? "File" :
                  source == ConfigSource::CommandLine ? "CommandLine" : "Runtime") + ")");
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        NLM_LOG_DEBUG("Config::set(): Key already exists, updating value");
        it->value = value;
        it->source = source;
    } else {
        NLM_LOG_DEBUG("Config::set(): Adding new key-value pair");
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    if (key.empty()) {
        NLM_LOG_ERROR("Config::set(string): Empty key provided");
        return;
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    if (key.empty()) {
        NLM_LOG_ERROR("Config::set(int): Empty key provided");
        return;
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    if (key.empty()) {
        NLM_LOG_ERROR("Config::set(double): Empty key provided");
        return;
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    if (key.empty()) {
        NLM_LOG_ERROR("Config::set(bool): Empty key provided");
        return;
    }
    
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    if (key.empty()) {
        NLM_LOG_ERROR("Config::has(): Empty key provided");
        return false;
    }
    
    bool result = std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (result) {
        NLM_LOG_DEBUG("Config::has(): Key found: " + key);
    } else {
        NLM_LOG_DEBUG("Config::has(): Key not found: " + key);
    }
    
    return result;
}

void Config::remove(const std::string& key) {
    if (key.empty()) {
        NLM_LOG_ERROR("Config::remove(): Empty key provided");
        return;
    }
    
    NLM_LOG_DEBUG("Config::remove(): Attempting to remove key: " + key);
    
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    NLM_LOG_DEBUG("Config::remove(): Key " + key + " removed (or was not present)");
}

std::vector<std::string> Config::getKeys() const {
    NLM_LOG_DEBUG("Config::getKeys(): Retrieving all keys");
    
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    
    NLM_LOG_DEBUG("Config::getKeys(): Retrieved " + std::to_string(keys.size()) + " keys");
    return keys;
}

void Config::clear() {
    NLM_LOG_DEBUG("Config::clear(): Clearing all configuration entries");
    
    size_t count = pImpl->entries.size();
    pImpl->entries.clear();
    
    NLM_LOG_INFO("Config::clear(): Cleared " + std::to_string(count) + " configuration entries");
}

std::string Config::summary() const {
    NLM_LOG_DEBUG("Config::summary(): Generating configuration summary");
    
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
    
    std::string result = oss.str();
    NLM_LOG_DEBUG("Config::summary(): Generated summary with " + std::to_string(pImpl->entries.size()) + " entries");
    return result;
}

std::string Config::trim(const std::string& str) {
    if (str.empty()) return "";
    
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    if (str.empty()) return "";
    
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
