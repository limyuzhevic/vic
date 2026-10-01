#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <mutex>

namespace nlm {

// Configuration validation constants
namespace ConfigValidation {
    const size_t MIN_NEURON_COUNT = 10;
    const size_t MAX_NEURON_COUNT = 10000;
    const size_t MIN_REGION_COUNT = 1;
    const size_t MAX_REGION_COUNT = 100;
    const double MIN_SIMULATION_TIMESTEP = 0.0001;
    const double MAX_SIMULATION_TIMESTEP = 1.0;
    const float MIN_CONNECTION_PROBABILITY = 0.0f;
    const float MAX_CONNECTION_PROBABILITY = 1.0f;
    const int64_t MIN_RANDOM_SEED = 1;
    const int64_t MAX_RANDOM_SEED = 0x7FFFFFFFFFFFFFFF; // INT64_MAX
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    mutable std::unordered_map<std::string, size_t> keyIndex;  // For optimized lookups
    mutable std::mutex mutex;  // Protect access to entries and keyIndex
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    size_t validEntries = 0;
    size_t invalidEntries = 0;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Set the value (this will convert string to appropriate type)
            set(key, value, ConfigSource::File);
            
            // Validate the set value
            if (has(key)) {
                auto val = get<ConfigValue>(key);
                if (val.has_value() && !validateValue(key, val.value())) {
                    logValidationWarning(key, val.value());
                    invalidEntries++;
                } else {
                    validEntries++;
                }
            }
        } else {
            NLM_LOG_WARNING("Invalid config format in " + filepath + ": line " + 
                           std::to_string(lineNumber) + " - missing '='");
            invalidEntries++;
        }
    }
    
    NLM_LOG_INFO("Configuration file loaded: " + filepath + 
                " (valid entries: " + std::to_string(validEntries) + ", " +
                "invalid entries: " + std::to_string(invalidEntries) + ")");
    return true;
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) {
    // Validate configuration keys have reasonable values
    if (key == "neuron_count") {
        if (const auto* val = std::get_if<size_t>(&value)) {
            return (*val >= ConfigValidation::MIN_NEURON_COUNT && 
                    *val <= ConfigValidation::MAX_NEURON_COUNT);
        }
    } else if (key == "region_count") {
        if (const auto* val = std::get_if<size_t>(&value)) {
            return (*val >= ConfigValidation::MIN_REGION_COUNT && 
                    *val <= ConfigValidation::MAX_REGION_COUNT);
        }
    } else if (key == "simulation_timestep") {
        if (const auto* val = std::get_if<double>(&value)) {
            return (*val >= ConfigValidation::MIN_SIMULATION_TIMESTEP && 
                    *val <= ConfigValidation::MAX_SIMULATION_TIMESTEP);
        }
    } else if (key == "connection_probability") {
        if (const auto* val = std::get_if<float>(&value)) {
            return (*val >= ConfigValidation::MIN_CONNECTION_PROBABILITY && 
                    *val <= ConfigValidation::MAX_CONNECTION_PROBABILITY);
        }
    } else if (key == "random_seed") {
        if (const auto* val = std::get_if<int64_t>(&value)) {
            return (*val >= ConfigValidation::MIN_RANDOM_SEED && 
                    *val <= ConfigValidation::MAX_RANDOM_SEED);
        }
    }
    // For unknown keys or non-numeric types, accept any value
    return true;
}

void Config::logValidationWarning(const std::string& key, const ConfigValue& value) {
    std::string valueStr;
    std::visit([&valueStr](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            valueStr = "\"" + arg + "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            valueStr = "[" + std::to_string(arg.size()) + " integers]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            valueStr = "[" + std::to_string(arg.size()) + " doubles]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            valueStr = "[" + std::to_string(arg.size()) + " strings]";
        } else {
            valueStr = std::to_string(arg);
        }
    }, value);
    
    NLM_LOG_WARNING("Configuration value validation warning for key: " + key + " = " + valueStr);
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated on " << std::chrono::system_clock::now() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Format the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, int>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, double>) {
                file << entry.key << " = " << std::setprecision(6) << arg << "\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << std::setprecision(6) << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]\n";
            } else {
                file << entry.key << " = " << "UNKNOWN_TYPE\n";
            }
        }, entry.value);
        
        file << "# Source: " << static_cast<int>(entry.source) << "\n\n";
    }
    
    file.flush();
    if (!file.good()) {
        NLM_LOG_ERROR("Error writing to configuration file: " + filepath);
        return false;
    }
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
    return true;
}

// Optimized lookup using index
void Config::updateIndex() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->keyIndex.clear();
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        pImpl->keyIndex[pImpl->entries[i].key] = i;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Optimized lookup using index
    auto it = pImpl->keyIndex.find(key);
    if (it != pImpl->keyIndex.end()) {
        size_t index = it->second;
        if (index < pImpl->entries.size()) {
            const ConfigEntry& entry = pImpl->entries[index];
            try {
                return std::get<T>(entry.value);
            } catch (const std::bad_variant_access&) {
                return std::nullopt;
            }
        }
    }
    
    // Fallback to linear search if index is out of sync
    auto endIt = pImpl->entries.end();
    auto foundIt = std::find_if(pImpl->entries.begin(), endIt,
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (foundIt == endIt) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(foundIt->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Validate the value before setting
    if (!validateValue(key, value)) {
        logValidationWarning(key, value);
    }
    
    auto it = pImpl->keyIndex.find(key);
    if (it != pImpl->keyIndex.end()) {
        size_t index = it->second;
        if (index < pImpl->entries.size() && pImpl->entries[index].key == key) {
            pImpl->entries[index].value = value;
            pImpl->entries[index].source = source;
            return;
        }
    }
    
    pImpl->entries.emplace_back(key, value, source);
    // Invalidate index since we added a new entry
    pImpl->keyIndex.clear();
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    // Use index for fast lookup
    return pImpl->keyIndex.find(key) != pImpl->keyIndex.end();
}

void Config::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->keyIndex.find(key);
    if (it != pImpl->keyIndex.end()) {
        size_t index = it->second;
        if (index < pImpl->entries.size()) {
            // Remove from entries vector
            pImpl->entries.erase(pImpl->entries.begin() + index);
            // Reindex remaining entries
            updateIndex();
            return;
        }
    }
    
    // Fallback to linear search
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    // Invalidate index since we removed entries
    pImpl->keyIndex.clear();
}

std::vector<std::string> Config::getKeys() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->entries.clear();
    pImpl->keyIndex.clear();
}

std::string Config::summary() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    // Ensure index is up to date
    if (pImpl->keyIndex.empty() && !pImpl->entries.empty()) {
        updateIndex();
    }
    
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
// get methods
// Note: The actual instantiations are already provided in the header file
// The template implementations below ensure they are compiled

// The actual template instantiations from the header file:
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

// getOr methods
template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm