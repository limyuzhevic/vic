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
    // ============================================================================
    // Enhanced configuration file loading with comprehensive error handling
    // ============================================================================
    
    if (filepath.empty()) {
        NLM_LOG_ERROR("Config file loading failed: empty file path");
        return false;
    }
    
    // Check if file exists and is readable
    if (!std::filesystem::exists(filepath)) {
        NLM_LOG_ERROR("Config file loading failed: file does not exist: " + filepath);
        return false;
    }
    
    if (!std::filesystem::is_regular_file(filepath)) {
        NLM_LOG_ERROR("Config file loading failed: path is not a file: " + filepath);
        return false;
    }
    
    if (!std::filesystem::is_readable(filepath)) {
        NLM_LOG_ERROR("Config file loading failed: file is not readable: " + filepath);
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Config file loading failed: could not open file: " + filepath);
        return false;
    }
    
    // Get file size for logging
    std::filesystem::path pathObj(filepath);
    size_t fileSize = std::filesystem::file_size(pathObj);
    NLM_LOG_INFO("Loading configuration from " + filepath + " (" + std::to_string(fileSize) + " bytes)");
    
    std::string line;
    size_t lineNumber = 0;
    size_t validEntries = 0;
    size_t skippedEntries = 0;
    size_t errorEntries = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            skippedEntries++;
            continue;
        }
        
        // Parse key=value pairs with validation
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Config file " + filepath + ": line " + std::to_string(lineNumber) + 
                          " skipped - invalid format (no '=' found)");
            errorEntries++;
            continue;
        }
        
        // Validate key format
        std::string key = trim(line.substr(0, pos));
        if (key.empty()) {
            NLM_LOG_WARNING("Config file " + filepath + ": line " + std::to_string(lineNumber) + 
                          " skipped - empty key");
            errorEntries++;
            continue;
        }
        
        // Check for key validity (alphanumeric and underscores)
        bool keyValid = true;
        for (char c : key) {
            if (!std::isalnum(c) && c != '_' && c != '-') {
                keyValid = false;
                break;
            }
        }
        
        if (!keyValid) {
            NLM_LOG_WARNING("Config file " + filepath + ": line " + std::to_string(lineNumber) + 
                          " skipped - invalid key format: " + key);
            errorEntries++;
            continue;
        }
        
        // Parse value
        std::string value = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Validate value is not empty
        if (value.empty()) {
            NLM_LOG_WARNING("Config file " + filepath + ": line " + std::to_string(lineNumber) + 
                          " skipped - empty value for key: " + key);
            errorEntries++;
            continue;
        }
        
        // Set the configuration value
        try {
            set(key, value, ConfigSource::File);
            validEntries++;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Config file " + filepath + ": line " + std::to_string(lineNumber) + 
                        " failed to set key-value pair: key='" + key + "', value='" + value + 
                        "', error: " + std::string(e.what()));
            errorEntries++;
            continue;
        }
    }
    
    file.close();
    
    NLM_LOG_INFO("Configuration file loaded successfully: " + std::to_string(validEntries) + " entries loaded, " +
                 std::to_string(skippedEntries) + " skipped (comments/empty), " + std::to_string(errorEntries) + " errors");
    
    if (errorEntries > 0) {
        NLM_LOG_WARNING("Configuration file loaded with errors. Please check the file format.");
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    // ============================================================================
    // Enhanced command line argument parsing with comprehensive error handling
    // ============================================================================
    
    if (argc <= 0 || argv == nullptr) {
        NLM_LOG_ERROR("Command line argument parsing failed: invalid argc or argv");
        return false;
    }
    
    NLM_LOG_INFO("Parsing command line arguments (argc=" + std::to_string(argc) + ")");
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.empty()) {
            NLM_LOG_WARNING("Command line: skipping empty argument at position " + std::to_string(i));
            continue;
        }
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                // Validate key format
                std::string key = arg.substr(2, pos - 2);
                if (key.empty()) {
                    NLM_LOG_ERROR("Command line: invalid argument format at position " + std::to_string(i) +
                                ": key is empty");
                    return false;
                }
                
                // Validate key (alphanumeric and underscores)
                bool keyValid = true;
                for (char c : key) {
                    if (!std::isalnum(c) && c != '_' && c != '-') {
                        keyValid = false;
                        break;
                    }
                }
                
                if (!keyValid) {
                    NLM_LOG_ERROR("Command line: invalid key format at position " + std::to_string(i) +
                                ": '" + key + "' (keys must contain only alphanumeric characters, underscores, or hyphens)");
                    return false;
                }
                
                std::string value = arg.substr(pos + 1);
                if (value.empty()) {
                    NLM_LOG_WARNING("Command line: argument " + arg + " has empty value");
                }
                
                try {
                    set(key, value, ConfigSource::CommandLine);
                    NLM_LOG_INFO("Command line: set key='" + key + "', value='" + value + "'");
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Command line: failed to set key='" + key + "', value='" + value + 
                                "' at position " + std::to_string(i) + ": " + std::string(e.what()));
                    return false;
                }
            } else {
                NLM_LOG_ERROR("Command line: invalid argument format at position " + std::to_string(i) +
                            ": expected --key=value format, got '" + arg + "'");
                return false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            
            // Validate key format
            if (key.empty()) {
                NLM_LOG_ERROR("Command line: invalid argument format at position " + std::to_string(i) +
                            ": key is empty");
                return false;
            }
            
            bool keyValid = true;
            for (char c : key) {
                if (!std::isalnum(c) && c != '_' && c != '-') {
                    keyValid = false;
                    break;
                }
            }
            
            if (!keyValid) {
                NLM_LOG_ERROR("Command line: invalid key format at position " + std::to_string(i) +
                            ": '" + key + "' (keys must contain only alphanumeric characters, underscores, or hyphens)");
                return false;
            }
            
            std::string value = argv[++i];
            if (value.empty()) {
                NLM_LOG_WARNING("Command line: argument -" + key + " has empty value");
            }
            
            try {
                set(key, value, ConfigSource::CommandLine);
                NLM_LOG_INFO("Command line: set key='" + key + "', value='" + value + "'");
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Command line: failed to set key='" + key + "', value='" + value + 
                            "' at position " + std::to_string(i) + ": " + std::string(e.what()));
                return false;
            }
        } else {
            NLM_LOG_WARNING("Command line: unrecognized argument at position " + std::to_string(i) +
                          ": '" + arg + "' (ignoring)");
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // ============================================================================
    // Enhanced configuration file saving with comprehensive error handling
    // ============================================================================
    
    if (filepath.empty()) {
        NLM_LOG_ERROR("Configuration save failed: empty file path");
        return false;
    }
    
    // Check if directory exists and is writable
    std::filesystem::path pathObj(filepath);
    std::filesystem::path parentDir = pathObj.parent_path();
    
    if (!parentDir.empty()) {
        if (!std::filesystem::exists(parentDir)) {
            NLM_LOG_ERROR("Configuration save failed: directory does not exist: " + parentDir.string());
            return false;
        }
        
        if (!std::filesystem::is_directory(parentDir)) {
            NLM_LOG_ERROR("Configuration save failed: path is not a directory: " + parentDir.string());
            return false;
        }
        
        if (!std::filesystem::is_writable(parentDir)) {
            NLM_LOG_ERROR("Configuration save failed: directory is not writable: " + parentDir.string());
            return false;
        }
    }
    
    // Check if file already exists and is writable
    if (std::filesystem::exists(pathObj)) {
        if (!std::filesystem::is_regular_file(pathObj)) {
            NLM_LOG_ERROR("Configuration save failed: path exists but is not a file: " + filepath);
            return false;
        }
        
        if (!std::filesystem::is_writable(pathObj)) {
            NLM_LOG_ERROR("Configuration save failed: file is not writable: " + filepath);
            return false;
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Configuration save failed: could not open file for writing: " + filepath);
        return false;
    }
    
    NLM_LOG_INFO("Saving configuration to " + filepath + " (" + std::to_string(pImpl->entries.size()) + " entries)");
    
    // Write configuration header
    file << "# NLM Configuration File\n";
    file << "# Generated on " << std::chrono::system_clock::now() << "\n";
    file << "# WARNING: This file is auto-generated. Manual edits may be overwritten.\n\n";
    
    // Write entries
    size_t successfulWrites = 0;
    size_t failedWrites = 0;
    
    for (const auto& entry : pImpl->entries) {
        try {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << "[VALUE]" << "  # [" << static_cast<int>(entry.source) << "]\n\n";
            successfulWrites++;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to write configuration entry: key='" + entry.key + "', error: " + std::string(e.what()));
            failedWrites++;
        }
    }
    
    file.close();
    
    if (failedWrites > 0) {
        NLM_LOG_WARNING("Configuration saved with " + std::to_string(failedWrites) + " failed entries");
    }
    
    NLM_LOG_INFO("Configuration saved successfully: " + std::to_string(successfulWrites) + " entries written");
    
    return successfulWrites > 0;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // ============================================================================
    // Enhanced configuration value retrieval with comprehensive error handling
    // ============================================================================
    
    if (key.empty()) {
        NLM_LOG_ERROR("Configuration get failed: empty key");
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_WARNING("Configuration get: key not found: '" + key + "'");
        return std::nullopt;
    }
    
    try {
        T value = std::get<T>(it->value);
        NLM_LOG_INFO("Configuration get: retrieved key='" + key + "', value type='" + getTypeName(T()) + "'");
        return value;
    } catch (const std::bad_variant_access&) {
        NLM_LOG_ERROR("Configuration get: type mismatch for key='" + key + 
                    "', expected type: '" + getTypeName(T()) + "', actual type: '" + getVariantTypeName(it->value) + "'");
        return std::nullopt;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Configuration get: unexpected error retrieving key='" + key + "': " + std::string(e.what()));
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    // ============================================================================
    // Enhanced configuration value retrieval with default fallback
    // ============================================================================
    
    auto val = get<T>(key);
    if (val.has_value()) {
        NLM_LOG_INFO("Configuration getOr: retrieved key='" + key + "' with value: " + std::to_string(val.value()));
        return val.value();
    } else {
        NLM_LOG_INFO("Configuration getOr: key='" + key + "' not found, using default value: " + std::to_string(defaultValue));
        return defaultValue;
    }
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
