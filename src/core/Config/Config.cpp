#include "Config.hpp"
#include "Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <optional>
#include <system_error>

namespace nlm {
static void ensureDirectoryExists(const std::string& filepath) {
    size_t pos = filepath.find_last_of("/\\");
    if (pos != std::string::npos) {
        std::string dir = filepath.substr(0, pos);
        if (!dir.empty()) {
            std::error_code ec;
            std::filesystem::create_directories(dir, ec);
            if (ec) {
                NLM_LOG_ERROR("Failed to create config directory " + dir + ": " + ec.message());
            }
        }
    }
}

static void validateValue(const ConfigValue& value, const std::string& key) {
    std::visit([key](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, std::string>) {
            if (arg.empty()) {
                NLM_LOG_WARNING("Config key " + key + " has empty string value");
            }
            // Validate string doesn't contain newlines
            if (arg.find('\n') != std::string::npos || arg.find('\r') != std::string::npos) {
                throw std::invalid_argument("Config value for key " + key + " contains newlines");
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            if (arg.size() > 1000) {
                throw std::invalid_argument("Config vector for key " + key + " is too large (" + 
                                           std::to_string(arg.size()) + " elements)");
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (arg.size() > 1000) {
                throw std::invalid_argument("Config vector for key " + key + " is too large (" + 
                                           std::to_string(arg.size()) + " elements)");
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (arg.size() > 1000) {
                throw std::invalid_argument("Config vector for key " + key + " is too large (" + 
                                           std::to_string(arg.size()) + " elements)");
            }
            for (const auto& str : arg) {
                if (str.empty()) {
                    throw std::invalid_argument("Config vector contains empty string for key " + key);
                }
            }
        }
    }, value);
}
namespace nlm {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check if file exists and is readable
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Config file not found or cannot be opened: " + filepath);
        return false;
    }
    
    // Check file size to avoid parsing extremely large files
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    if (fileSize > 10 * 1024 * 1024) {  // 10MB limit
        NLM_LOG_ERROR("Config file too large: " + filepath + " (" + 
                     std::to_string(fileSize) + " bytes)");
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    size_t currentLineCount = 0;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        ++currentLineCount;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Enforce reasonable line length to prevent malformed lines
        if (line.length() > 1024) {
            NLM_LOG_ERROR("Config line too long in " + filepath + ": line " + 
                         std::to_string(lineNumber) + " (" + 
                         std::to_string(line.length()) + " characters)");
            continue;  // Skip but continue processing
        }
        
        // Parse simple key=value pairs with better validation
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid config line in " + filepath + ": line " + 
                           std::to_string(lineNumber) + " (no '=' found)");
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty()) {
            NLM_LOG_WARNING("Empty config key in " + filepath + ": line " + 
                           std::to_string(lineNumber));
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Set the configuration value
        try {
            set(key, value, ConfigSource::File);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error parsing config line in " + filepath + ": line " + 
                         std::to_string(lineNumber) + " - " + e.what());
        }
    }
    
    if (currentLineCount > 1000) {
        NLM_LOG_WARNING("Config file " + filepath + " has many lines (" + 
                       std::to_string(currentLineCount) + ". Consider optimizing");
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
    // Validate filepath
    if (filepath.empty()) {
        NLM_LOG_ERROR("Cannot save config to empty filepath");
        return false;
    }
    
    // Ensure directory exists
    ensureDirectoryExists(filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    // Set good formatting
    file << std::fixed << std::setprecision(6);
    
    for (const auto& entry : pImpl->entries) {
        // Write comment if present
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        
        // Write the key-value pair
        file << entry.key << " = ";
        
        // Write the value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    file.flush();
    
    if (!file.good()) {
        NLM_LOG_ERROR("Failed to write complete config to file: " + filepath);
        return false;
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
    // Validate key
    if (key.empty()) {
        throw std::invalid_argument("Config key cannot be empty");
    }
    
    if (key.length() > 255) {
        throw std::invalid_argument("Config key too long: " + key);
    }
    
    // Check for invalid characters in key
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-' && c != '.') {
            throw std::invalid_argument("Invalid character in config key: " + key);
        }
    }
    
    // Validate value based on type
    validateValue(value, key);
    
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

// Helper function implementations
void Config::validateValue(const ConfigValue& value, const std::string& key) {
    std::visit([key](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, std::string>) {
            if (arg.empty()) {
                NLM_LOG_WARNING("Config key " + key + " has empty string value");
            }
            // Validate string doesn't contain newlines
            if (arg.find('\n') != std::string::npos || arg.find('\r') != std::string::npos) {
                throw std::invalid_argument("Config value for key " + key + " contains newlines");
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            if (arg.size() > 1000) {
                throw std::invalid_argument("Config vector for key " + key + " is too large (" + 
                                           std::to_string(arg.size()) + " elements)");
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (arg.size() > 1000) {
                throw std::invalid_argument("Config vector for key " + key + " is too large (" + 
                                           std::to_string(arg.size()) + " elements)");
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (arg.size() > 1000) {
                throw std::invalid_argument("Config vector for key " + key + " is too large (" + 
                                           std::to_string(arg.size()) + " elements)");
            }
            for (const auto& str : arg) {
                if (str.empty()) {
                    throw std::invalid_argument("Config vector contains empty string for key " + key);
                }
            }
        }
    }, value);
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

} // namespace nlm
