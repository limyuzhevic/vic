#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <ctime>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    mutable std::string lastError;
    
    Impl() : entries() {
        // Initialize with default configuration if needed
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Validate that initialization succeeded
    if (!pImpl) {
        throw std::runtime_error("Failed to allocate Config implementation");
    }
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Validate filepath
    if (filepath.empty()) {
        pImpl->lastError = "Filepath cannot be empty";
        return false;
    }
    
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        pImpl->lastError = "Configuration file does not exist: " + filepath;
        return false;
    }
    
    if (!std::filesystem::is_regular_file(filepath)) {
        pImpl->lastError = "Configuration path is not a file: " + filepath;
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Failed to open configuration file: " + filepath;
        return false;
    }
    
    // Get file modification time for logging
    auto fileTime = std::filesystem::last_write_time(filepath);
    
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            pImpl->lastError = "Invalid configuration line " + std::to_string(lineNumber) + ": missing '='";
            continue;  // Skip invalid lines but continue processing
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty()) {
            pImpl->lastError = "Invalid configuration line " + std::to_string(lineNumber) + ": empty key";
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        try {
            // Try to set the value (will attempt to parse type)
            set(key, value, ConfigSource::File);
        } catch (const std::exception& e) {
            pImpl->lastError = "Error parsing configuration line " + std::to_string(lineNumber) + ": " + std::string(e.what());
            // Continue processing other lines
        }
    }
    
    if (!pImpl->lastError.empty()) {
        // Log the error but don't fail the entire operation
        // File loading is tolerant of malformed lines
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (!argv || argc <= 1) {
        return true;  // No arguments to process
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.empty()) {
            continue;
        }
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (key.empty()) {
                    pImpl->lastError = "Command line argument has empty key: " + arg;
                    continue;
                }
                
                try {
                    set(key, value, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    pImpl->lastError = "Error processing command line argument " + arg + ": " + std::string(e.what());
                    // Continue processing other arguments
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (key.empty()) {
                pImpl->lastError = "Command line argument has empty key: " + arg;
                continue;
            }
            
            try {
                set(key, value, ConfigSource::CommandLine);
            } catch (const std::exception& e) {
                pImpl->lastError = "Error processing command line argument " + arg + " " + value + ": " + std::string(e.what());
                // Continue processing other arguments
            }
        } else {
            pImpl->lastError = "Invalid command line argument format: " + arg;
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    if (filepath.empty()) {
        pImpl->lastError = "Filepath cannot be empty";
        return false;
    }
    
    // Create parent directories if they don't exist
    std::filesystem::path path(filepath);
    if (!path.parent_path().empty() && !std::filesystem::exists(path.parent_path())) {
        if (!std::filesystem::create_directories(path.parent_path())) {
            pImpl->lastError = "Failed to create directory structure for: " + filepath;
            return false;
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Failed to open configuration file for writing: " + filepath;
        return false;
    }
    
    // Set good file bits
    file << std::left;
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Visit variant to get proper output format
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    // Check if file was written successfully
    if (!file.good()) {
        pImpl->lastError = "Failed to write configuration to file: " + filepath;
        return false;
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (key.empty()) {
        pImpl->lastError = "Key cannot be empty";
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
        pImpl->lastError = "Type mismatch for key: " + key;
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    }
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (key.empty()) {
        throw std::invalid_argument("Key cannot be empty");
    }
    
    // Validate key format
    if (!isValidKey(key)) {
        throw std::invalid_argument("Invalid key format: " + key);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        // Validate the new entry before adding
        validateConfigEntry(key, value);
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
        pImpl->lastError = "Key cannot be empty";
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
        oss << "  " << entry.key << " = ";
        
        // Visit variant to get proper output format
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
        
        oss << " (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    if (str.empty()) {
        return str;
    }
    
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
    if (key.empty()) return false;
    if (key[0] == '-' || key[0] == ' ') return false;
    if (key.find('=') != std::string::npos) return false;
    if (key.find(' ') != std::string::npos) return false;
    return true;
}

void Config::validateConfigEntry(const std::string& key, const ConfigValue& value) {
    // Validate based on value type
    std::visit([this, &key](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            if (arg < 0) throw std::invalid_argument("Integer value cannot be negative: " + key);
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::isnan(arg) || std::isinf(arg)) {
                throw std::invalid_argument("Double value must be finite: " + key);
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (arg.empty()) {
                throw std::invalid_argument("String value cannot be empty: " + key);
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            if (arg.empty()) {
                throw std::invalid_argument("Vector value cannot be empty: " + key);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (arg.empty()) {
                throw std::invalid_argument("Vector value cannot be empty: " + key);
            }
            for (const auto& item : arg) {
                if (std::isnan(item) || std::isinf(item)) {
                    throw std::invalid_argument("Vector contains invalid double value: " + key);
                }
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (arg.empty()) {
                throw std::invalid_argument("Vector value cannot be empty: " + key);
            }
        }
    }, value);
}

std::string Config::getLastError() const {
    return pImpl->lastError;
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
