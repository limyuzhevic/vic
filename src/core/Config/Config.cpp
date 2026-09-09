#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <stdexcept>

namespace nlm {

// Static member initialization
std::string Config::lastError = "";

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    if (!isValidPath(filepath)) {
        lastError = "Invalid file path: " + filepath;
        return false;
    }
    
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        lastError = "File not found: " + filepath;
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        lastError = "Failed to open file: " + filepath;
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            lastError = "Invalid line format at line " + std::to_string(lineNum) + ": missing '='";
            return false;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (key.empty()) {
            lastError = "Empty key at line " + std::to_string(lineNum);
            return false;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        try {
            set(key, value, ConfigSource::File);
        } catch (const ConfigValidationError& e) {
            lastError = "Validation error at line " + std::to_string(lineNum) + ": " + e.what();
            return false;
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 1) {
        return true; // No arguments, nothing to load
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (key.empty()) {
                    lastError = "Empty key in command line argument: " + arg;
                    return false;
                }
                
                try {
                    set(key, value, ConfigSource::CommandLine);
                } catch (const ConfigValidationError& e) {
                    lastError = "Validation error in command line argument " + arg + ": " + e.what();
                    return false;
                }
            } else {
                lastError = "Invalid command line argument format (missing '='): " + arg;
                return false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (key.empty()) {
                lastError = "Empty key in command line argument: " + arg;
                return false;
            }
            
            try {
                set(key, value, ConfigSource::CommandLine);
            } catch (const ConfigValidationError& e) {
                lastError = "Validation error in command line argument " + arg + ": " + e.what();
                return false;
            }
        } else {
            lastError = "Invalid command line argument format: " + arg;
            return false;
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    if (!isValidPath(filepath)) {
        lastError = "Invalid file path for saving: " + filepath;
        throw ConfigSaveError(lastError);
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        lastError = "Failed to open file for writing: " + filepath;
        throw ConfigSaveError(lastError);
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Convert value to string based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double>) {
                file << doubleToString(arg);
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << intToString(arg);
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << intToString(arg[i]);
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << doubleToString(arg[i]);
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
        T value = std::get<T>(it->value);
        return value;
    } catch (const std::bad_variant_access&) {
        lastError = "Type mismatch for key '" + key + "': expected " + typeid(T).name();
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return *val;
    }
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::string normalizedKey = validateAndNormalizeKey(key);
    
    // Check if key is valid
    if (!isValidKey(normalizedKey)) {
        lastError = "Invalid key name: '" + key + "'. Key must be non-empty and contain only alphanumeric characters and underscores.";
        throw ConfigValidationError(lastError);
    }
    
    // Validate value based on type
    validateValue(value);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(normalizedKey, value, source);
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

void Config::addInt(const std::string& key, int value, const std::string& description) {
    if (value < 0) {
        lastError = "Invalid value for key '" + key + "': negative values are not allowed";
        throw ConfigValidationError(lastError);
    }
    set(key, value, ConfigSource::Runtime);
}

void Config::addDouble(const std::string& key, double value, const std::string& description) {
    if (value < 0.0) {
        lastError = "Invalid value for key '" + key + "': negative values are not allowed";
        throw ConfigValidationError(lastError);
    }
    set(key, value, ConfigSource::Runtime);
}

void Config::addBool(const std::string& key, bool value, const std::string& description) {
    set(key, value, ConfigSource::Runtime);
}

void Config::addString(const std::string& key, const std::string& value, const std::string& description) {
    if (value.empty()) {
        lastError = "Invalid value for key '" + key + "': empty strings are not allowed";
        throw ConfigValidationError(lastError);
    }
    set(key, value, ConfigSource::Runtime);
}

int Config::getInt(const std::string& key, int defaultValue, const std::string& description) const {
    auto val = get<int>(key);
    if (val.has_value()) {
        return *val;
    }
    return defaultValue;
}

double Config::getDouble(const std::string& key, double defaultValue, const std::string& description) const {
    auto val = get<double>(key);
    if (val.has_value()) {
        return *val;
    }
    return defaultValue;
}

bool Config::getBool(const std::string& key, bool defaultValue, const std::string& description) const {
    auto val = get<bool>(key);
    if (val.has_value()) {
        return *val;
    }
    return defaultValue;
}

std::string Config::getString(const std::string& key, const std::string& defaultValue, const std::string& description) const {
    auto val = get<std::string>(key);
    if (val.has_value()) {
        return *val;
    }
    return defaultValue;
}

bool Config::has(const std::string& key) const {
    std::string normalizedKey = validateAndNormalizeKey(key);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; });
}

void Config::remove(const std::string& key) {
    std::string normalizedKey = validateAndNormalizeKey(key);
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; }),
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

bool Config::isValidKey(const std::string& key) {
    if (key.empty()) {
        return false;
    }
    return std::all_of(key.begin(), key.end(), isAlphaNumericOrUnderscore);
}

bool Config::isValidPath(const std::string& path) {
    if (path.empty()) {
        return false;
    }
    return !path.contains('\0') && !path.contains('/') && !path.contains('\\');
}

std::string Config::validateAndNormalizeKey(const std::string& key) {
    std::string result = trim(key);
    
    // Remove any non-alphanumeric/underscore characters
    std::string clean;
    for (char c : result) {
        if (isAlphaNumericOrUnderscore(c)) {
            clean += c;
        }
    }
    
    if (clean.empty()) {
        lastError = "Key cannot be reduced to empty string: '" + key + "'";
        throw ConfigValidationError(lastError);
    }
    
    // Convert to lowercase
    return toLower(clean);
}

std::string Config::getLastError() {
    return lastError;
}

void Config::validateValue(const ConfigValue& value) {
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            if (arg < 0) {
                throw ConfigValidationError("Negative integer values are not allowed");
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (arg < 0.0) {
                throw ConfigValidationError("Negative double values are not allowed");
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (arg.empty()) {
                throw ConfigValidationError("Empty string values are not allowed");
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            if (arg.empty()) {
                throw ConfigValidationError("Empty integer vector values are not allowed");
            }
            for (const auto& item : arg) {
                if (item < 0) {
                    throw ConfigValidationError("Negative values in integer vector are not allowed");
                }
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (arg.empty()) {
                throw ConfigValidationError("Empty double vector values are not allowed");
            }
            for (const auto& item : arg) {
                if (item < 0.0) {
                    throw ConfigValidationError("Negative values in double vector are not allowed");
                }
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (arg.empty()) {
                throw ConfigValidationError("Empty string vector values are not allowed");
            }
            for (const auto& item : arg) {
                if (item.empty()) {
                    throw ConfigValidationError("Empty string values in vector are not allowed");
                }
            }
        }
    }, value);
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

bool Config::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

bool Config::isAlphaNumericOrUnderscore(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

std::string Config::intToString(int value) {
    return std::to_string(value);
}

std::string Config::doubleToString(double value) {
    std::ostringstream oss;
    oss.precision(10);
    oss << std::fixed << value;
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