#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<std::string> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Improved loadFromFile with better error handling
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    bool hasErrors = false;
    
    while (std::getline(file, line)) {
        lineNum++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs with enhanced error handling
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid configuration line " + std::to_string(lineNum) + ": " + line);
            hasErrors = true;
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (key.empty()) {
            NLM_LOG_WARNING("Empty key in configuration line " + std::to_string(lineNum));
            hasErrors = true;
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '\"' && value.back() == '\"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        try {
            set(key, value, ConfigSource::File);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error parsing configuration line " + std::to_string(lineNum) + ": " + 
                         std::string(e.what()) + " - Line: " + line);
            hasErrors = true;
        }
    }
    
    if (hasErrors) {
        NLM_LOG_WARNING("Configuration file " + filepath + " loaded with errors");
        // Continue with partial loading - don't fail entirely
    } else {
        NLM_LOG_INFO("Successfully loaded configuration from: " + filepath);
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
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated on " << getCurrentTimestamp() << std::endl;
    file << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        if (!entry.description.empty()) {
            file << "# " << entry.description << std::endl;
        }
        
        // Format the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << std::setprecision(6) << arg[i];
                }
                file << "]";
            } else {
                file << entry.key << " = " << arg;
            }
        }, entry.value);
        
        file << "  # " << static_cast<int>(entry.source) << std::endl;
        file << std::endl;
    }
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->timestamp = std::chrono::system_clock::now();
        it->validated = false; // Invalidate previous validation
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    // Validate the new value
    validateValue(key, value);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
    pImpl->validationErrors.clear(); // Clear errors when key is removed
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
    pImpl->validationErrors.clear();
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

bool Config::validate() const {
    pImpl->validationErrors.clear();
    
    for (const auto& entry : pImpl->entries) {
        validateValue(entry.key, entry.value);
    }
    
    if (!validateDependencies()) {
        return false;
    }
    
    // Mark all entries as validated
    for (auto& entry : pImpl->entries) {
        entry.validated = true;
    }
    
    return pImpl->validationErrors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    // Ensure validation is up to date
    if (!validate()) {
        std::vector<std::string> errors;
        for (const auto& error : pImpl->validationErrors) {
            errors.push_back(error.key + ": " + error.message);
        }
        return errors;
    }
    return {};
}

void Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Check for type-specific validation
    std::visit([this, &key](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, int> || 
                     std::is_same_v<T, int64_t> || 
                     std::is_same_v<T, double>) {
            double val = static_cast<double>(arg);
            if (val < -1e6 || val > 1e6) {
                pImpl->validationErrors.emplace_back(
                    key, ValidationErrorType::OutOfRange,
                    "Value " + std::to_string(val) + " is out of reasonable range [-1e6, 1e6]"
                );
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            // Check for empty string for required fields
            if (arg.empty() && key.find("name") != std::string::npos) {
                pImpl->validationErrors.emplace_back(
                    key, ValidationErrorType::InvalidValue,
                    "Name field cannot be empty"
                );
            }
            
            // Basic pattern validation for specific keys
            if (key == "email") {
                static const std::regex emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$");
                if (!std::regex_match(arg, emailRegex)) {
                    pImpl->validationErrors.emplace_back(
                        key, ValidationErrorType::InvalidValue,
                        "Invalid email format",
                        "Use format: username@domain.com"
                    );
                }
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                             std::is_same_v<T, std::vector<double>> ||
                             std::is_same_v<T, std::vector<std::string>>) {
            if (arg.size() > 1000) {
                pImpl->validationErrors.emplace_back(
                    key, ValidationErrorType::OutOfRange,
                    "Vector size " + std::to_string(arg.size()) + " exceeds maximum of 1000"
                );
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            // Boolean values are generally fine, but log them
        }
    }, value);
}

bool Config::validateDependencies() const {
    // Check that all required dependencies exist
    for (const auto& entry : pImpl->entries) {
        // Example dependency validation - could be expanded
        if (entry.key == "neuron_count" || entry.key == "region_count") {
            if (entry.value.index() == static_cast<size_t>(ConfigValueIndex::double)) {
                double val = std::get<double>(entry.value);
                if (val < 1.0) {
                    pImpl->validationErrors.emplace_back(
                        entry.key, ValidationErrorType::InvalidValue,
                        "Value must be greater than 0"
                    );
                } else if (val > 100000.0) {
                    pImpl->validationErrors.emplace_back(
                        entry.key, ValidationErrorType::OutOfRange,
                        "Value must be less than 100000"
                    );
                }
            }
        }
    }
    
    return pImpl->validationErrors.empty();
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm