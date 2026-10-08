#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <system_error>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Track memory allocation status for better error reporting
    bool memoryAllocationFailed = false;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with empty entries vector
    pImpl->entries.reserve(256); // Pre-allocate for typical usage
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    if (filepath.empty()) {
        return false;
    }
    
    // Check if file exists and is accessible
    std::error_code ec;
    bool exists = std::filesystem::exists(filepath, ec);
    if (ec) {
        // File system error
        return false;
    }
    
    if (!exists) {
        // File doesn't exist - this is not an error, just return false
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        // File exists but cannot be opened (permissions, etc.)
        return false;
    }
    
    // Check file size to prevent loading extremely large files
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    const size_t MAX_CONFIG_FILE_SIZE = 10 * 1024 * 1024; // 10MB limit
    if (fileSize > MAX_CONFIG_FILE_SIZE) {
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with validation
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            // Invalid line format - skip with warning (but don't fail)
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty() || key.find('=') != std::string::npos) {
            // Invalid key - skip
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        try {
            set(key, value, ConfigSource::File);
        } catch (const std::exception&) {
            // If set() fails, skip this line but continue processing
            // This prevents one bad line from stopping the entire file load
            continue;
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 1 || argv == nullptr) {
        return true; // No arguments - not an error
    }
    
    for (int i = 1; i < argc; ++i) {
        if (argv[i] == nullptr) {
            continue; // Skip null pointer
        }
        
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.size() >= 2 && arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key and value
                if (!key.empty() && !value.empty()) {
                    set(key, value, ConfigSource::CommandLine);
                }
            }
        }
        // Handle -key value format
        else if (arg.size() >= 1 && arg[0] == '-' && i + 1 < argc && argv[i + 1] != nullptr) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key and value
            if (!key.empty() && !value.empty()) {
                set(key, value, ConfigSource::CommandLine);
            }
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    if (filepath.empty()) {
        return false;
    }
    
    // Create parent directories if they don't exist
    std::error_code ec;
    std::filesystem::path path(filepath);
    std::filesystem::create_directories(path.parent_path(), ec);
    if (ec) {
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Set good precision for floating point numbers
    file.precision(6);
    file << std::fixed;
    
    for (const auto& entry : pImpl->entries) {
        try {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            // Use std::visit to handle different value types
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
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
            
            file << "\n\n"; // Two newlines between entries
            
        } catch (const std::exception&) {
            // Skip problematic entries but continue with others
            continue;
        }
    }
    
    // Ensure file is written to disk
    if (!file.flush()) {
        return false;
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (key.empty()) {
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
        // Type mismatch - key exists but has wrong type
        return std::nullopt;
    } catch (const std::exception&) {
        // Other error accessing variant
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (key.empty()) {
        return; // Silently ignore empty keys
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Update existing entry
        it->value = value;
        it->source = source;
    } else {
        // Add new entry
        try {
            pImpl->entries.emplace_back(key, value, source);
        } catch (const std::bad_alloc&) {
            pImpl->memoryAllocationFailed = true;
            // Re-throw as runtime_error for consistency
            throw std::runtime_error("Memory allocation failed while setting configuration");
        }
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    if (key.empty() || value.empty()) {
        return;
    }
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
    pImpl->memoryAllocationFailed = false;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    if (pImpl->memoryAllocationFailed) {
        oss << "  WARNING: Memory allocation failures occurred during configuration operations\n";
    }
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        
        try {
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    oss << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) oss << ", ";
                        oss << arg[i];
                    }
                    oss << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    oss << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) oss << ", ";
                        oss << arg[i];
                    }
                    oss << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    oss << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) oss << ", ";
                        oss << "\"" << arg[i] << "\"";
                    }
                    oss << "]";
                } else {
                    oss << arg;
                }
            }, entry.value);
        } catch (const std::exception&) {
            oss << "[ERROR accessing value]";
        }
        
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    if (str.empty()) {
        return "";
    }
    
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return ""; // String is all whitespace
    }
    
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    if (str.empty()) {
        return "";
    }
    
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