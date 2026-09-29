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
    NLM_LOG_INFO("Loading configuration from file: " + filepath);
    
    // Check if file exists and is accessible before attempting to open
    std::error_code ec;
    if (!std::filesystem::exists(filepath, ec)) {
        NLM_LOG_ERROR("Configuration file does not exist: " + filepath);
        return false;
    }
    
    if (!std::filesystem::is_regular_file(filepath, ec)) {
        NLM_LOG_ERROR("Configuration path is not a regular file: " + filepath);
        return false;
    }
    
    // Try to open the file with proper error handling
    std::ifstream file(filepath, std::ios::in);
    if (!file.is_open()) {
        // Try to get more specific error information
        std::string errorMsg = "Failed to open configuration file: " + filepath;
        
        // Check file permissions
        std::error_code perm_ec;
        auto perms = std::filesystem::status(filepath, perm_ec).permissions();
        if (!(perms & std::filesystem::perms::owner_read)) {
            errorMsg += " - Permission denied (read access)";
        } else if (!(perms & std::filesystem::perms::owner_write)) {
            errorMsg += " - Permission denied (write access for backup)";
        } else {
            errorMsg += " - File may be corrupted or in use";
        }
        
        NLM_LOG_ERROR(errorMsg);
        return false;
    }
    
    // Check file size to avoid loading extremely large files
    std::uintmax_t fileSize = std::filesystem::file_size(filepath, ec);
    if (ec) {
        NLM_LOG_WARNING("Could not determine file size: " + ec.message());
    } else if (fileSize > 10 * 1024 * 1024) { // 10MB limit
        NLM_LOG_ERROR("Configuration file too large (" + std::to_string(fileSize) + " bytes). Max allowed: 10MB");
        file.close();
        return false;
    }
    
    // Create backup before modifying original file
    std::string backupPath = filepath + ".backup";
    if (!backupConfig(*this, backupPath)) {
        NLM_LOG_WARNING("Could not create backup configuration, but continuing...");
    }
    
    std::string line;
    size_t lineNumber = 0;
    size_t validLines = 0;
    
    try {
        while (std::getline(file, line)) {
            lineNumber++;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs with validation
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                NLM_LOG_WARNING("Invalid configuration line " + std::to_string(lineNumber) + 
                              ": missing '=' operator: " + line);
                continue;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Validate key format
            if (key.empty()) {
                NLM_LOG_WARNING("Invalid configuration line " + std::to_string(lineNumber) + ": empty key");
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
                validLines++;
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error parsing configuration line " + std::to_string(lineNumber) +
                            " (key=" + key + ", value=" + value + "): " + std::string(e.what()));
                continue;
            }
        }
        
        NLM_LOG_INFO("Configuration loaded successfully: " + std::to_string(validLines) + 
                    " valid entries from " + std::to_string(lineNumber) + " lines");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception while reading configuration file: " + std::string(e.what()));
        return false;
    }
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
    NLM_LOG_INFO("Saving configuration to file: " + filepath);
    
    // Check if filepath is valid
    if (filepath.empty()) {
        NLM_LOG_ERROR("Invalid filepath: empty string");
        return false;
    }
    
    // Check if the path exists and is writable
    std::error_code ec;
    if (std::filesystem::exists(filepath, ec)) {
        // File exists, check if it's writable
        auto perms = std::filesystem::status(filepath, ec).permissions();
        if (!(perms & std::filesystem::perms::owner_write)) {
            NLM_LOG_ERROR("Configuration file exists but is not writable: " + filepath);
            return false;
        }
    } else {
        // File doesn't exist, check if parent directory is writable
        std::filesystem::path parentPath = std::filesystem::path(filepath).parent_path();
        if (!parentPath.empty() && !std::filesystem::exists(parentPath, ec)) {
            NLM_LOG_ERROR("Parent directory does not exist: " + parentPath.string());
            return false;
        }
        if (!std::filesystem::is_directory(parentPath, ec)) {
            NLM_LOG_ERROR("Parent path is not a directory: " + parentPath.string());
            return false;
        }
        auto perms = std::filesystem::status(parentPath, ec).permissions();
        if (!(perms & (std::filesystem::perms::owner_write | std::filesystem::perms::group_write | std::filesystem::perms::others_write))) {
            NLM_LOG_ERROR("Parent directory is not writable: " + parentPath.string());
            return false;
        }
    }
    
    // Try to open the file for writing
    std::ofstream file(filepath, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        std::string errorMsg = "Failed to open configuration file for writing: " + filepath;
        
        // Check specific error conditions
        std::error_code perm_ec;
        auto perms = std::filesystem::status(filepath, perm_ec).permissions();
        if (!(perms & std::filesystem::perms::owner_write)) {
            errorMsg += " - Permission denied (write access)";
        } else if (std::filesystem::space(filepath, perm_ec).available < 1024) { // Less than 1KB available
            errorMsg += " - No disk space available";
        } else {
            errorMsg += " - File may be locked or corrupted";
        }
        
        NLM_LOG_ERROR(errorMsg);
        return false;
    }
    
    try {
        // Write configuration entries to file
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            
            // Write key=value pair
            file << entry.key << " = ";
            
            // Handle different value types
            if (std::holds_alternative<int>(entry.value)) {
                file << std::get<int>(entry.value);
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                file << std::get<int64_t>(entry.value);
            } else if (std::holds_alternative<double>(entry.value)) {
                file << std::get<double>(entry.value);
            } else if (std::holds_alternative<bool>(entry.value)) {
                file << (std::get<bool>(entry.value) ? "true" : "false");
            } else if (std::holds_alternative<std::string>(entry.value)) {
                // Quote strings that contain spaces or special characters
                std::string strValue = std::get<std::string>(entry.value);
                if (strValue.find(' ') != std::string::npos || 
                    strValue.find('#') != std::string::npos ||
                    strValue.find('=') != std::string::npos) {
                    file << "\"" << strValue << "\"";
                } else {
                    file << strValue;
                }
            } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
                const auto& vec = std::get<std::vector<int>>(entry.value);
                file << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << vec[i];
                }
                file << "]";
            } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
                const auto& vec = std::get<std::vector<double>>(entry.value);
                file << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << vec[i];
                }
                file << "]";
            } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                const auto& vec = std::get<std::vector<std::string>>(entry.value);
                file << "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << vec[i] << "\"";
                }
                file << "]";
            }
            
            file << "\n\n";
        }
        
        // Flush to ensure all data is written to disk
        file.flush();
        
        // Verify the file was actually written to by trying to read it back
        if (!file.good()) {
            NLM_LOG_ERROR("Configuration file write operation failed (bad stream)");
            file.close();
            return false;
        }
        
        NLM_LOG_INFO("Configuration saved successfully to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception while saving configuration file: " + std::string(e.what()));
        file.close();
        return false;
    }
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
