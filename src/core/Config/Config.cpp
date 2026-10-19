#include "Config.hpp"
#include "Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cerrno>
#include <system_error>

namespace nlm {

namespace {
    constexpr const char* ERROR_CONFIG_FILE_NOT_FOUND = "Configuration file not found or cannot be opened: ";
    constexpr const char* ERROR_CONFIG_FILE_READ = "Error reading configuration file: ";
    constexpr const char* ERROR_CONFIG_INVALID_FORMAT = "Invalid configuration format at line: ";
    constexpr const char* ERROR_CONFIG_MALFORMED_VALUE = "Malformed configuration value for key: ";
    constexpr const char* ERROR_CONFIG_SAVE_FAILED = "Failed to save configuration to file: ";
}

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
    
    NLM_LOG_INFO("Attempting to load configuration from: " + filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR(std::string(ERROR_CONFIG_FILE_NOT_FOUND) + filepath);
        NLM_LOG_ERROR("Error details: " + std::string(std::strerror(errno)));
        return false;
    }
    
    size_t lineNumber = 0;
    std::string line;
    while (std::getline(file, line)) {
        ++lineNumber;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING(std::string(ERROR_CONFIG_INVALID_FORMAT) + 
                          std::to_string(lineNumber) + ": Missing '=' operator");
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty()) {
            NLM_LOG_WARNING(std::string(ERROR_CONFIG_INVALID_FORMAT) + 
                          std::to_string(lineNumber) + ": Empty key name");
            continue;
        }
        
        // Validate value
        if (value.empty()) {
            NLM_LOG_WARNING(std::string(ERROR_CONFIG_INVALID_FORMAT) + 
                          std::to_string(lineNumber) + ": Empty value for key: " + key);
            continue;
        }
        
        // Remove quotes if present
        bool hadQuotes = false;
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            hadQuotes = true;
            value = value.substr(1, value.size() - 2);
        }
        
        // Validate quoted value removal
        if (hadQuotes && value.empty()) {
            NLM_LOG_WARNING(std::string(ERROR_CONFIG_MALFORMED_VALUE) + key);
            continue;
        }
        
        set(key, value, ConfigSource::File);
    }
    
    if (file.bad()) {
        NLM_LOG_ERROR(std::string(ERROR_CONFIG_FILE_READ) + filepath);
        NLM_LOG_ERROR("Error details: Stream error during reading");
        return false;
    }
    
    NLM_LOG_INFO("Successfully loaded configuration from: " + filepath);
    NLM_LOG_INFO("Loaded " + std::to_string(pImpl->entries.size()) + " configuration entries");
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    NLM_LOG_INFO("Parsing command-line arguments for configuration");
    
    if (argc < 1) {
        NLM_LOG_WARNING("No command-line arguments provided");
        return true;
    }
    
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
                    NLM_LOG_WARNING("Empty key name in argument: " + arg);
                    continue;
                }
                
                // Validate value
                if (value.empty()) {
                    NLM_LOG_WARNING("Empty value for key: " + key + " in argument: " + arg);
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
                NLM_LOG_INFO("Parsed argument: --" + key + "=" + value);
            } else {
                NLM_LOG_WARNING("Malformed argument (missing '='): " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-') {
            if (i + 1 >= argc) {
                NLM_LOG_ERROR("Missing value for argument key: " + arg);
                continue;
            }
            
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key
            if (key.empty()) {
                NLM_LOG_WARNING("Empty key name in argument: " + arg);
                continue;
            }
            
            set(key, value, ConfigSource::CommandLine);
            NLM_LOG_INFO("Parsed argument: -" + key + " " + value);
        } else {
            NLM_LOG_WARNING("Skipping unrecognized argument: " + arg);
        }
    }
    
    NLM_LOG_INFO("Command-line argument parsing complete");
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    NLM_LOG_INFO("Attempting to save configuration to: " + filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR(std::string(ERROR_CONFIG_SAVE_FAILED) + filepath);
        NLM_LOG_ERROR("Error details: " + std::string(std::strerror(errno)));
        return false;
    }
    
    // Check if stream is writable
    if (!file.good()) {
        NLM_LOG_ERROR(std::string(ERROR_CONFIG_SAVE_FAILED) + filepath);
        NLM_LOG_ERROR("Error details: Stream not in good state");
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Convert ConfigValue to string representation
        std::ostringstream valueStream;
        std::visit([&valueStream](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStream << "\"" << arg << "\"";
            } else {
                valueStream << arg;
            }
        }, entry.value);
        
        file << entry.key << " = " << valueStream.str() << "\n";
        
        // Check for write errors
        if (file.fail() || file.bad()) {
            NLM_LOG_ERROR(std::string(ERROR_CONFIG_SAVE_FAILED) + filepath);
            NLM_LOG_ERROR("Error details: Failed to write entry: " + entry.key);
            return false;
        }
    }
    
    file.flush();
    
    // Verify file was closed properly
    if (file.bad()) {
        NLM_LOG_ERROR(std::string(ERROR_CONFIG_SAVE_FAILED) + filepath);
        NLM_LOG_ERROR("Error details: Stream error during finalization");
        return false;
    }
    
    NLM_LOG_INFO("Configuration saved successfully to: " + filepath);
    NLM_LOG_INFO("Saved " + std::to_string(pImpl->entries.size()) + " configuration entries");
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

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    
    if (pImpl->entries.empty()) {
        oss << "  [Empty configuration]";
    } else {
        oss << "\nConfiguration Entries:";
        for (const auto& entry : pImpl->entries) {
            oss << "\n  " << entry.key << " = [";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, entry.value);
            oss << "] (Source: " << static_cast<int>(entry.source) << ")";
        }
        
        oss << "\n\nSource Priority Summary:";
        size_t defaultCount = 0, fileCount = 0, cliCount = 0, runtimeCount = 0;
        for (const auto& entry : pImpl->entries) {
            switch (entry.source) {
                case ConfigSource::Default: defaultCount++; break;
                case ConfigSource::File: fileCount++; break;
                case ConfigSource::CommandLine: cliCount++; break;
                case ConfigSource::Runtime: runtimeCount++; break;
            }
        }
        oss << "\n  Default:   " << defaultCount << " entries";
        oss << "\n  File:      " << fileCount << " entries";
        oss << "\n  CommandLine:" << cliCount << " entries";
        oss << "\n  Runtime:   " << runtimeCount << " entries";
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
