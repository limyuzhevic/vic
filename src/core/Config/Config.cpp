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
    // Log the start of file loading operation
    NLM_LOG_INFO("Loading configuration from: " + filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        NLM_LOG_ERROR("File may not exist or may have insufficient permissions");
        return false;
    }
    
    // Track file parsing errors
    std::vector<std::string> errors;
    size_t lineNumber = 0;
    
    std::string line;
    while (std::getline(file, line)) {
        ++lineNumber;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        try {
            // Parse simple key=value pairs with validation
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                errors.push_back("Line " + std::to_string(lineNumber) + ": Missing '=' separator");
                continue;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Validate key format
            if (key.empty()) {
                errors.push_back("Line " + std::to_string(lineNumber) + ": Empty key");
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Basic validation of value format
            if (value.empty() && !key.empty()) {
                // Empty values are allowed for boolean false, etc.
                // But we'll log a warning for debugging
                NLM_LOG_WARN("Empty value for key: " + key);
            }
            
            set(key, value, ConfigSource::File);
            
        } catch (const std::exception& e) {
            errors.push_back("Line " + std::to_string(lineNumber) + ": " + std::string(e.what()));
            continue;
        }
    }
    
    // Log any parsing errors
    if (!errors.empty()) {
        NLM_LOG_WARN("Configuration file parsing completed with " + std::to_string(errors.size()) + " errors:");
        for (const auto& error : errors) {
            NLM_LOG_WARN("  " + error);
        }
    }
    
    NLM_LOG_INFO("Configuration loaded: " + std::to_string(pImpl->entries.size()) + " entries");
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    NLM_LOG_INFO("Loading configuration from command line arguments");
    
    // Track argument parsing errors
    std::vector<std::string> errors;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key format
                if (key.empty()) {
                    errors.push_back("Argument '" + arg + "': Empty key after '--'");
                    continue;
                }
                
                // Remove leading/trailing spaces from key
                key = trim(key);
                if (key.empty()) {
                    errors.push_back("Argument '" + arg + "': Empty key after '--'");
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
                NLM_LOG_INFO("  Command line: " + key + " = " + (value.length() > 50 ? value.substr(0, 47) + "..." : value));
                
            } else {
                errors.push_back("Argument '" + arg + "': Missing '=' separator");
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key format
            key = trim(key);
            if (key.empty()) {
                errors.push_back("Argument '" + arg + "': Empty key after '-'");
                continue;
            }
            
            set(key, value, ConfigSource::CommandLine);
            NLM_LOG_INFO("  Command line: " + key + " = " + (value.length() > 50 ? value.substr(0, 47) + "..." : value));
            
        } else if (arg[0] == '-') {
            errors.push_back("Argument '" + arg + "': Missing value for flag");
        }
    }
    
    // Log any parsing errors
    if (!errors.empty()) {
        NLM_LOG_WARN("Command line argument parsing completed with " + std::to_string(errors.size()) + " errors:");
        for (const auto& error : errors) {
            NLM_LOG_WARN("  " + error);
        }
    }
    
    NLM_LOG_INFO("Command line configuration loaded: " + std::to_string(pImpl->entries.size()) + " entries");
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    // Write configuration in a simple key=value format
    // This format is easy to parse and human-readable
    for (const auto& entry : pImpl->entries) {
        // Write description as comment
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        
        // Write key=value pair
        file << entry.key << " = ";
        
        // Handle different value types
        std::visit([&file](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                // Quote strings to handle special characters
                file << "\"" << value << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (value ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << value[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << value[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << value[i] << "\"";
                }
                file << "]";
            } else {
                // Handle basic types (int, int64_t, double, bool)
                file << value;
            }
        }, entry.value);
        
        file << "\n\n";
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
    // Validate key format
    if (key.empty()) {
        NLM_LOG_ERROR("Config key cannot be empty");
        return;
    }
    
    // Remove leading/trailing spaces from key
    std::string trimmedKey = trim(key);
    if (trimmedKey.empty()) {
        NLM_LOG_ERROR("Config key cannot be empty after trimming: '" + key + "'");
        return;
    }
    
    // Log configuration change
    NLM_LOG_INFO("Setting configuration: " + trimmedKey + " = " + 
                 (std::holds_alternative<std::string>(value) && std::get<std::string>(value).length() > 50 ? 
                  std::get<std::string>(value).substr(0, 47) + "..." : 
                  std::visit([](const auto& v) { return std::to_string(v); }, value)) +
                 " (source: " + std::to_string(static_cast<int>(source)) + ")");
    
    // Update or add the configuration entry
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&trimmedKey](const ConfigEntry& e) { return e.key == trimmedKey; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_INFO("  Updated existing configuration entry");
    } else {
        pImpl->entries.emplace_back(trimmedKey, value, source);
        NLM_LOG_INFO("  Added new configuration entry");
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
    std::string trimmedKey = trim(key);
    if (trimmedKey.empty()) {
        NLM_LOG_WARN("Empty key provided to has()");
        return false;
    }
    
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&trimmedKey](const ConfigEntry& e) { return e.key == trimmedKey; });
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
