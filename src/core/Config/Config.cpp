#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <unordered_set>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Add validation cache for common operations
    mutable std::unordered_set<std::string> keyCache;
    mutable bool cacheDirty = false;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

namespace {
    // Helper function to validate key names
    bool isValidKey(const std::string& key) {
        if (key.empty()) return false;
        if (key.length() > 256) return false;  // Reasonable key length limit
        // Keys should not contain control characters
        for (char c : key) {
            if (std::iscntrl(static_cast<unsigned char>(c))) {
                return false;
            }
        }
        return true;
    }
    
    // Helper function to validate file path
    bool isValidFilePath(const std::string& filepath) {
        if (filepath.empty()) return false;
        // Check for path traversal attempts
        if (filepath.find("..") != std::string::npos) {
            return false;
        }
        // Try to create a path object to validate basic format
        try {
            std::filesystem::path path(filepath);
            // For write operations, ensure parent directory exists
            if (std::filesystem::exists(path.parent_path())) {
                return true;
            }
            // If parent doesn't exist, we'll try to create it
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    // Helper function to trim whitespace
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    // Helper function to convert to lowercase
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    // Helper function to parse boolean values
    bool parseBool(const std::string& str) {
        std::string lower = toLower(str);
        if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
            return true;
        } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
            return false;
        } else {
            throw std::invalid_argument("Invalid boolean value: " + str);
        }
    }
    
    // Helper function to parse numeric values
    template<typename T>
    T parseNumeric(const std::string& str) {
        std::istringstream iss(str);
        T value;
        if (!(iss >> value)) {
            throw std::invalid_argument("Invalid numeric value: " + str);
        }
        return value;
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    if (!isValidFilePath(filepath)) {
        NLM_LOG_ERROR("Invalid file path: " + filepath);
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for reading: " + filepath);
        return false;
    }
    
    // Get file size for potential validation
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Reasonable file size limit (e.g., 1MB)
    if (fileSize > 1024 * 1024) {
        NLM_LOG_ERROR("File too large: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    
    try {
        while (std::getline(file, line)) {
            lineNumber++;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse simple key=value pairs
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                NLM_LOG_WARNING("Skipping invalid line " + std::to_string(lineNumber) + 
                              " in file " + filepath + ": no '=' found");
                continue;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Validate key
            if (!isValidKey(key)) {
                NLM_LOG_WARNING("Skipping invalid key at line " + std::to_string(lineNumber) +
                              " in file " + filepath + ": " + key);
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
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Error parsing line " + std::to_string(lineNumber) +
                              " in file " + filepath + ": " + std::string(e.what()));
                continue;
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error reading file " + filepath + ": " + std::string(e.what()));
        return false;
    }
    
    pImpl->cacheDirty = true;
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 0 || argv == nullptr) {
        NLM_LOG_WARNING("Invalid command line arguments: argc=" + std::to_string(argc));
        return false;
    }
    
    try {
        for (int i = 1; i < argc; ++i) {
            if (argv[i] == nullptr) {
                NLM_LOG_WARNING("Skipping null argument at position " + std::to_string(i));
                continue;
            }
            
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    // Validate key
                    if (!isValidKey(key)) {
                        NLM_LOG_WARNING("Invalid key in command line argument: " + key);
                        continue;
                    }
                    
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    NLM_LOG_WARNING("Invalid command line argument format: " + arg);
                }
            }
            // Handle -key value format
            else if (arg[0] == '-') {
                std::string key = arg.substr(1);
                
                // Validate key
                if (!isValidKey(key)) {
                    NLM_LOG_WARNING("Invalid key in command line argument: " + key);
                    continue;
                }
                
                if (i + 1 < argc && argv[i + 1] != nullptr) {
                    std::string value = argv[++i];
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    NLM_LOG_WARNING("Missing value for key: " + key);
                }
            } else {
                NLM_LOG_WARNING("Skipping unrecognized command line argument: " + arg);
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing command line arguments: " + std::string(e.what()));
        return false;
    }
    
    pImpl->cacheDirty = true;
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    if (!isValidFilePath(filepath)) {
        NLM_LOG_ERROR("Invalid file path: " + filepath);
        return false;
    }
    
    // Ensure parent directory exists
    std::filesystem::path path(filepath);
    if (!std::filesystem::exists(path.parent_path())) {
        try {
            std::filesystem::create_directories(path.parent_path());
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Cannot create directory " + path.parent_path().string() + ": " + std::string(e.what()));
            return false;
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for writing: " + filepath);
        return false;
    }
    
    // Set good bit to check for write errors
    if (!file.good()) {
        NLM_LOG_ERROR("File stream not in good state for writing: " + filepath);
        return false;
    }
    
    try {
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            // Format the value properly
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
            
            if (!file.good()) {
                NLM_LOG_ERROR("Write error occurred while saving to file: " + filepath);
                return false;
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error writing to file " + filepath + ": " + std::string(e.what()));
        return false;
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (!isValidKey(key)) {
        NLM_LOG_WARNING("Invalid key requested: " + key);
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
        NLM_LOG_WARNING("Type mismatch for key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (!isValidKey(key)) {
        throw std::invalid_argument("Invalid key: " + key);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    pImpl->cacheDirty = true;
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    try {
        set(key, ConfigValue(value), source);
    } catch (const std::invalid_argument& e) {
        throw;
    } catch (const std::exception& e) {
        throw std::runtime_error("Error setting string key \"" + key + "\": " + std::string(e.what()));
    }
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
    if (!isValidKey(key)) {
        return false;
    }
    
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    if (!isValidKey(key)) {
        NLM_LOG_WARNING("Attempt to remove invalid key: " + key);
        return;
    }
    
    size_t removed = std::erase_if(pImpl->entries, [&key](const ConfigEntry& e) {
        return e.key == key;
    });
    
    if (removed > 0) {
        pImpl->cacheDirty = true;
        NLM_LOG_INFO("Removed " + std::to_string(removed) + " config entries for key: " + key);
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
    size_t oldSize = pImpl->entries.size();
    pImpl->entries.clear();
    pImpl->cacheDirty = true;
    
    if (oldSize > 0) {
        NLM_LOG_INFO("Cleared " + std::to_string(oldSize) + " configuration entries");
    }
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
    return oss.str();
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
