#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cassert>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Add logging for error reporting
    std::vector<std::string> errorMessages;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Validate file path
    if (filepath.empty()) {
        throw std::invalid_argument("Config::loadFromFile: Filepath cannot be empty");
    }
    
    // Check if file exists and is readable
    if (!std::filesystem::exists(filepath)) {
        throw std::runtime_error("Config::loadFromFile: File does not exist: " + filepath);
    }
    
    if (!std::filesystem::is_regular_file(filepath)) {
        throw std::runtime_error("Config::loadFromFile: Path is not a regular file: " + filepath);
    }
    
    if (!std::filesystem::is_readable(filepath)) {
        throw std::runtime_error("Config::loadFromFile: File is not readable: " + filepath);
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Config::loadFromFile: Failed to open file: " + filepath);
    }
    
    // Check file size to catch potential corruption
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    if (fileSize > 10 * 1024 * 1024) { // 10MB limit
        throw std::runtime_error("Config::loadFromFile: File too large: " + filepath);
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with validation
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            // Log warning but continue processing other lines
            pImpl->errorMessages.push_back("Config::loadFromFile: Invalid syntax at line " + std::to_string(lineNumber) + ": missing '='");
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty()) {
            pImpl->errorMessages.push_back("Config::loadFromFile: Empty key at line " + std::to_string(lineNumber));
            continue;
        }
        
        if (key.size() > 100) {
            pImpl->errorMessages.push_back("Config::loadFromFile: Key too long at line " + std::to_string(lineNumber));
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Validate value length
        if (value.size() > 1024) {
            pImpl->errorMessages.push_back("Config::loadFromFile: Value too long at line " + std::to_string(lineNumber) + ": " + key);
        }
        
        set(key, value, ConfigSource::File);
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (!argv) {
        throw std::invalid_argument("Config::loadFromArgs: argv cannot be null");
    }
    
    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) {
            pImpl->errorMessages.push_back("Config::loadFromArgs: Null argument at position " + std::to_string(i));
            continue;
        }
        
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key format
                if (key.empty()) {
                    pImpl->errorMessages.push_back("Config::loadFromArgs: Empty key in argument: " + arg);
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                pImpl->errorMessages.push_back("Config::loadFromArgs: Invalid --key=value format: " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (key.empty()) {
                pImpl->errorMessages.push_back("Config::loadFromArgs: Empty key in argument: " + arg);
                continue;
            }
            
            set(key, value, ConfigSource::CommandLine);
        } else {
            pImpl->errorMessages.push_back("Config::loadFromArgs: Invalid argument format: " + arg);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    if (filepath.empty()) {
        throw std::invalid_argument("Config::saveToFile: Filepath cannot be empty");
    }
    
    // Create parent directories if they don't exist
    std::filesystem::path path(filepath);
    std::filesystem::path parentDir = path.parent_path();
    if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
        if (!std::filesystem::create_directories(parentDir)) {
            throw std::runtime_error("Config::saveToFile: Failed to create directories: " + parentDir.string());
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Config::saveToFile: Failed to open file for writing: " + filepath);
    }
    
    // Set file permissions
    file << std::ifstream::binary;
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        // Convert variant to string properly
        file << entry.key << " = ";
        
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
        
        file << "\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (key.empty()) {
        throw std::invalid_argument("Config::get: Key cannot be empty");
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        pImpl->errorMessages.push_back("Config::get: Type mismatch for key: " + key);
        return std::nullopt;
    } catch (...) {
        pImpl->errorMessages.push_back("Config::get: Unexpected error for key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    if (key.empty()) {
        throw std::invalid_argument("Config::getOr: Key cannot be empty");
    }
    
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (key.empty()) {
        throw std::invalid_argument("Config::set: Key cannot be empty");
    }
    
    // Validate source
    if (source == ConfigSource::Default && !value.empty()) {
        pImpl->errorMessages.push_back("Config::set: Cannot set non-default value for key: " + key);
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
        return;
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
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    // Add error messages to summary
    if (!pImpl->errorMessages.empty()) {
        oss << "\nError Messages:";
        for (const auto& error : pImpl->errorMessages) {
            oss << "\n  ERROR: " << error;
        }
    }
    
    return oss.str();
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
