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
    
    if (filepath.empty()) {
        NLM_LOG_ERROR("Config file path cannot be empty");
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " << filepath);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
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
                NLM_LOG_WARNING("Empty key found at line " << lineNumber);
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        } else {
            NLM_LOG_WARNING("Invalid config line " << lineNumber << ": " << line);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 0 || argv == nullptr) {
        NLM_LOG_ERROR("Invalid arguments to loadFromArgs");
        return false;
    }
    
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key
                if (key.empty()) {
                    NLM_LOG_WARNING("Empty key found in command line arguments");
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Invalid command line argument format: " << arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key
            if (key.empty()) {
                NLM_LOG_WARNING("Empty key found in command line arguments");
                continue;
            }
            
            set(key, value, ConfigSource::CommandLine);
        } else {
            NLM_LOG_WARNING("Ignoring unrecognized command line argument: " << arg);
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    if (filepath.empty()) {
        NLM_LOG_ERROR("Config file path cannot be empty");
        return false;
    }
    
    // Check if directory exists and is writable
    std::filesystem::path path(filepath);
    if (path.has_parent_path()) {
        std::filesystem::path parent = path.parent_path();
        if (!std::filesystem::exists(parent)) {
            NLM_LOG_ERROR("Config directory does not exist: " << parent.string());
            return false;
        }
        if (!std::filesystem::is_directory(parent)) {
            NLM_LOG_ERROR("Config path is not a directory: " << parent.string());
            return false;
        }
        if (!std::filesystem::is_writable(parent)) {
            NLM_LOG_ERROR("Config directory is not writable: " << parent.string());
            return false;
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " << filepath);
        return false;
    }
    
    // Set good formatting
    file << std::fixed << std::setprecision(6);
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Format value based on type
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
        NLM_LOG_ERROR("Failed to write complete config to file: " << filepath);
        return false;
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (key.empty()) {
        NLM_LOG_ERROR("Empty key provided to get()");
        return std::nullopt;
    }
    
    // Trim whitespace from key
    std::string trimmedKey = trim(key);
    if (trimmedKey.empty()) {
        NLM_LOG_ERROR("Config key with only whitespace provided to get()");
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&trimmedKey](const ConfigEntry& e) { return e.key == trimmedKey; });
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_INFO("Config key not found: " << trimmedKey);
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_ERROR("Config key " << trimmedKey << " has incorrect type for requested access");
        return std::nullopt;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error accessing config key " << trimmedKey << ": " << e.what());
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    if (key.empty()) {
        NLM_LOG_WARNING("Empty key provided to getOr()");
        return defaultValue;
    }
    
    auto val = get<T>(key);
    if (!val.has_value()) {
        NLM_LOG_INFO("Config key not found, using default: " << key);
        return defaultValue;
    }
    
    return val.value();
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Config key cannot be empty");
        return;
    }
    
    // Trim whitespace from key
    std::string trimmedKey = trim(key);
    if (trimmedKey.empty()) {
        NLM_LOG_ERROR("Config key contains only whitespace");
        return;
    }
    
    // Remove the original entry if it exists
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&trimmedKey](const ConfigEntry& e) { return e.key == trimmedKey; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(trimmedKey, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Config key cannot be empty");
        return;
    }
    
    // Trim whitespace from key
    std::string trimmedKey = trim(key);
    if (trimmedKey.empty()) {
        NLM_LOG_ERROR("Config key contains only whitespace");
        return;
    }
    
    set(trimmedKey, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Config key cannot be empty");
        return;
    }
    
    // Trim whitespace from key
    std::string trimmedKey = trim(key);
    if (trimmedKey.empty()) {
        NLM_LOG_ERROR("Config key contains only whitespace");
        return;
    }
    
    set(trimmedKey, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Config key cannot be empty");
        return;
    }
    
    // Trim whitespace from key
    std::string trimmedKey = trim(key);
    if (trimmedKey.empty()) {
        NLM_LOG_ERROR("Config key contains only whitespace");
        return;
    }
    
    set(trimmedKey, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Config key cannot be empty");
        return;
    }
    
    // Trim whitespace from key
    std::string trimmedKey = trim(key);
    if (trimmedKey.empty()) {
        NLM_LOG_ERROR("Config key contains only whitespace");
        return;
    }
    
    set(trimmedKey, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    if (key.empty()) {
        NLM_LOG_WARNING("Empty key provided to has()");
        return false;
    }
    
    // Trim whitespace from key
    std::string trimmedKey = trim(key);
    if (trimmedKey.empty()) {
        NLM_LOG_WARNING("Config key with only whitespace provided to has()");
        return false;
    }
    
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&trimmedKey](const ConfigEntry& e) { return e.key == trimmedKey; });
}

void Config::remove(const std::string& key) {
    if (key.empty()) {
        NLM_LOG_WARNING("Attempted to remove empty config key");
        return;
    }
    
    // Trim whitespace from key
    std::string trimmedKey = trim(key);
    if (trimmedKey.empty()) {
        NLM_LOG_WARNING("Attempted to remove config key with only whitespace");
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&trimmedKey](const ConfigEntry& e) { return e.key == trimmedKey; });
    
    if (it != pImpl->entries.end()) {
        pImpl->entries.erase(it);
        NLM_LOG_INFO("Removed config key: " << trimmedKey);
    } else {
        NLM_LOG_WARNING("Config key not found for removal: " << trimmedKey);
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
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    if (pImpl->entries.empty()) {
        oss << "  (empty)";
        return oss.str();
    }
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = ";
        
        // Format value based on type with error handling
        try {
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (arg ? "true" : "false");
                } else {
                    oss << arg;
                }
            }, entry.value);
        } catch (const std::exception& e) {
            oss << "[ERROR: " << e.what() << "]";
        }
        
        oss << " (" << static_cast<int>(entry.source) << ")\n";
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
