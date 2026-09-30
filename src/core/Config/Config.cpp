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
    // Use filesystem for better cross-platform compatibility
    if (!std::filesystem::exists(filepath)) {
        NLM_LOG_WARNING("Config file does not exist: " + filepath);
        return false;
    }
    
    // Check if file is readable
    if (!std::filesystem::is_regular_file(filepath) || !std::filesystem::status(filepath).permissions() & std::filesystem::perms::owner_read) {
        NLM_LOG_ERROR("Cannot read config file (permissions or not a regular file): " + filepath);
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Check file size to prevent loading extremely large files
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Limit file size to 1MB for configuration files
    const size_t maxFileSize = 1024 * 1024;
    if (fileSize > maxFileSize) {
        NLM_LOG_ERROR("Config file too large: " + std::to_string(fileSize) + " bytes (max " + std::to_string(maxFileSize) + ")");
        return false;
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
        
        // Parse simple key=value pairs with better error handling
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": no '=' found: " + line.substr(0, 50));
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty()) {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": empty key");
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Validate value
        if (value.empty()) {
            NLM_LOG_WARNING("Empty value for key: " + key);
        }
        
        // Set configuration with better error handling
        try {
            set(key, value, ConfigSource::File);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error setting config key " + key + " from file " + filepath + ": " + e.what());
        }
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
    // Create parent directories if they don't exist (platform-independent)
    std::filesystem::path filePath(filepath);
    std::error_code ec;
    if (!filePath.parent_path().empty()) {
        if (!std::filesystem::exists(filePath.parent_path())) {
            if (!std::filesystem::create_directories(filePath.parent_path(), ec)) {
                NLM_LOG_ERROR("Cannot create config directory " + filePath.parent_path().string() + ": " + ec.message());
                return false;
            }
        }
    }
    
    // Try to open file for writing
    std::ofstream file(filepath, std::ios::trunc);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file for writing: " + filepath);
        return false;
    }
    
    // Set file permissions (readable by user, group, others on Unix)
    if (!filepath.empty() && !std::filesystem::exists(filepath)) {
        file.close();
        if (!std::filesystem::create_directory(filepath, ec)) {
            NLM_LOG_ERROR("Cannot create config file: " + filepath + ": " + ec.message());
            return false;
        }
        file.open(filepath, std::ios::trunc);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot reopen config file after creation: " + filepath);
            return false;
        }
    }
    
    try {
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << "[VALUE]" << "\n";  // Actual value would be written in Phase 2
            if (!file.good()) {
                NLM_LOG_ERROR("Error writing to config file: " + filepath);
                return false;
            }
        }
        
        file.flush();
        
        // Verify file was written successfully
        if (!file.good()) {
            NLM_LOG_ERROR("Error flushing config file: " + filepath);
            return false;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception writing config file " + filepath + ": " + e.what());
        return false;
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // Validate input
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot get value with empty key");
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_DEBUG("Config key not found: " + key);
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_ERROR("Type mismatch for config key " + key + " (expected " + 
                     (std::is_same_v<T, int> ? "int" :
                      std::is_same_v<T, int64_t> ? "int64_t" :
                      std::is_same_v<T, double> ? "double" :
                      std::is_same_v<T, bool> ? "bool" :
                      std::is_same_v<T, std::string> ? "string" :
                      "unknown") + ")");
        return std::nullopt;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception accessing config key " + key + ": " + e.what());
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
        try {
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, entry.value);
        } catch (const std::exception& e) {
            oss << "[ERROR: " << e.what() << "]";
        }
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
