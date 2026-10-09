#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
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
        
        // Parse key=value pairs with validation
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_ERROR("Invalid config format at line " + std::to_string(lineNumber) + ": missing '='");
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        if (key.empty()) {
            NLM_LOG_ERROR("Invalid config format at line " + std::to_string(lineNumber) + ": empty key");
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Validate and parse value
        ConfigEntry entry(key, ConfigValue(value), ConfigSource::File);
        
        // Enhanced validation
        validateConfigEntry(entry, filepath, lineNumber);
        
        set(key, ConfigValue(value), ConfigSource::File);
    }
    
    NLM_LOG_INFO("Loaded configuration from: " + filepath + " (" + std::to_string(pImpl->entries.size()) + " entries)");
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
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    // Write header
    file << "# NLM Configuration File\n";
    file << "# Saved at runtime: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        // Skip entries with placeholder values
        if (std::holds_alternative<std::string>(entry.value) && 
            std::get<std::string>(entry.value) == "PLACEHOLDER_VALUE") {
            continue;
        }
        
        // Write entry with description if available
        file << "# " << entry.description << "\n";
        
        // Format based on type
        if (std::holds_alternative<int>(entry.value)) {
            file << entry.key << " = " << std::get<int>(entry.value) << "\n";
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            file << entry.key << " = " << std::get<int64_t>(entry.value) << "\n";
        } else if (std::holds_alternative<double>(entry.value)) {
            // Format doubles to reasonable precision
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(6) << std::get<double>(entry.value);
            file << entry.key << " = " << oss.str() << "\n";
        } else if (std::holds_alternative<bool>(entry.value)) {
            file << entry.key << " = " << (std::get<bool>(entry.value) ? "true" : "false") << "\n";
        } else if (std::holds_alternative<std::string>(entry.value)) {
            file << entry.key << " = \"" << std::get<std::string>(entry.value) << "\"\n";
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            // Handle vector types
            const auto& vec = std::get<std::vector<int>>(entry.value);
            file << entry.key << " = [";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) file << ", ";
                file << vec[i];
            }
            file << "]\n";
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            const auto& vec = std::get<std::vector<double>>(entry.value);
            file << entry.key << " = [";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) file << ", ";
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(6) << vec[i];
                file << oss.str();
            }
            file << "]\n";
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            const auto& vec = std::get<std::vector<std::string>>(entry.value);
            file << entry.key << " = [";
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) file << ", ";
                file << "\"" << vec[i] << "\"";
            }
            file << "]\n";
        }
        
        file << "\n";
    }
    
    NLM_LOG_INFO("Configuration saved to: " + filepath + " (" + std::to_string(pImpl->entries.size()) + " entries)");
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

// Validation helper functions
void Config::validateConfigEntry(const ConfigEntry& entry, const std::string& filepath, size_t lineNumber) {
    // Check for duplicate keys
    for (const auto& existing : pImpl->entries) {
        if (existing.key == entry.key) {
            NLM_LOG_WARNING("Duplicate config key '" + entry.key + "' at line " + std::to_string(lineNumber) + ". Previous value at line " + std::to_string(findLineNumber(existing.key, filepath)));
        }
    }
    
    // Validate value based on key type patterns
    validateValueForKey(entry.key, entry.value, filepath, lineNumber);
}

void Config::validateValueForKey(const std::string& key, const ConfigValue& value, const std::string& filepath, size_t lineNumber) {
    // Key-based validation patterns
    std::string lowerKey = toLower(key);
    
    if (lowerKey.find("rate") != std::string::npos) {
        // Rate values should typically be between 0.0 and 10.0
        if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (val < 0.0 || val > 10.0) {
                NLM_LOG_WARNING("Rate parameter '" + key + "' has extreme value " + std::to_string(val) + " at line " + std::to_string(lineNumber));
            }
        }
    }
    else if (lowerKey.find("count") != std::string::npos || lowerKey.find("neuron") != std::string::npos) {
        // Count values should be positive
        if (std::holds_alternative<int>(value)) {
            int val = std::get<int>(value);
            if (val <= 0) {
                NLM_LOG_ERROR("Count parameter '" + key + "' has non-positive value " + std::to_string(val) + " at line " + std::to_string(lineNumber));
            }
        } else if (std::holds_alternative<int64_t>(value)) {
            int64_t val = std::get<int64_t>(value);
            if (val <= 0) {
                NLM_LOG_ERROR("Count parameter '" + key + "' has non-positive value " + std::to_string(val) + " at line " + std::to_string(lineNumber));
            }
        }
    }
    else if (lowerKey.find("probability") != std::string::npos) {
        // Probability values should be between 0.0 and 1.0
        if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (val < 0.0 || val > 1.0) {
                NLM_LOG_WARNING("Probability parameter '" + key + "' has value " + std::to_string(val) + " (should be 0.0-1.0) at line " + std::to_string(lineNumber));
            }
        }
    }
    else if (lowerKey.find("threshold") != std::string::npos || lowerKey.find("potential") != std::string::npos) {
        // Threshold/potential values should be reasonable neuron membrane potentials
        if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (val < -100.0 || val > 100.0) {
                NLM_LOG_WARNING("Membrane potential/threshold parameter '" + key + "' has extreme value " + std::to_string(val) + " at line " + std::to_string(lineNumber));
            }
        }
    }
}

size_t Config::findLineNumber(const std::string& key, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return 0;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string currentKey = trim(line.substr(0, pos));
            if (toLower(currentKey) == toLower(key)) {
                return lineNumber;
            }
        }
    }
    return 0;
}

// Explicit template instantiations

} // namespace nlm
