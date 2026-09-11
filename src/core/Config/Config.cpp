#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <thread>
#include <chrono>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    mutable std::mutex mutex; // For thread safety
    mutable std::shared_mutex rwMutex; // For reentrant read/write locks
    std::unordered_map<std::string, size_t> keyIndex; // For O(1) lookups
    
    // Validation and optimization state
    std::vector<std::string> validationErrors;
    std::string lastComparison;
    std::string lastOptimization;
    double lastMemoryUsage;
    std::string lastPerformanceStats;
    std::chrono::steady_clock::time_point lastStatsUpdate;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default values
    pImpl->lastMemoryUsage = 0.0;
    pImpl->lastStatsUpdate = std::chrono::steady_clock::now();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

nlohmann::json Config::toJson(const ConfigValue& value) const {
    nlohmann::json j;
    std::visit([&j](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j = arg;
        }
    }, value);
    return j;
}

ConfigValue Config::fromJson(const nlohmann::json& j) const {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_number_integer()) {
        return j.get<int>();
    } else if (j.is_number_unsigned()) {
        return j.get<int64_t>();
    } else if (j.is_number()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_array()) {
        // Determine array type from first element
        if (j.empty()) {
            return std::vector<int>();
        }
        
        const auto& first = j[0];
        if (first.is_number_integer()) {
            std::vector<int> arr;
            arr.reserve(j.size());
            for (const auto& item : j) {
                arr.push_back(item.get<int>());
            }
            return arr;
        } else if (first.is_number()) {
            std::vector<double> arr;
            arr.reserve(j.size());
            for (const auto& item : j) {
                arr.push_back(item.get<double>());
            }
            return arr;
        } else if (first.is_string()) {
            std::vector<std::string> arr;
            arr.reserve(j.size());
            for (const auto& item : j) {
                arr.push_back(item.get<std::string>());
            }
            return arr;
        } else {
            throw std::runtime_error("Unsupported array element type in JSON");
        }
    } else {
        throw std::runtime_error("Unsupported JSON value type");
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json j;
        file >> j;
        
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Clear existing entries
        pImpl->entries.clear();
        pImpl->keyIndex.clear();
        
        // Parse JSON object
        if (j.is_object()) {
            for (auto it = j.begin(); it != j.end(); ++it) {
                try {
                    ConfigValue value = fromJson(it.value());
                    set(it.key(), value, ConfigSource::File);
                } catch (const std::exception& e) {
                    // Skip invalid entries with a warning
                    continue;
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
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

bool Config::batchLoadFromFiles(const std::vector<std::string>& filepaths) {
    bool success = true;
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& filepath : filepaths) {
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                pImpl->validationErrors.push_back("Failed to open file: " + filepath);
                success = false;
                continue;
            }
            
            nlohmann::json j;
            file >> j;
            
            if (j.is_object()) {
                for (auto it = j.begin(); it != j.end(); ++it) {
                    try {
                        ConfigValue value = fromJson(it.value());
                        set(it.key(), value, ConfigSource::File);
                    } catch (const std::exception& e) {
                        pImpl->validationErrors.push_back("Invalid entry in " + filepath + ": " + it.key());
                        success = false;
                    }
                }
            } else {
                pImpl->validationErrors.push_back("Invalid JSON format in " + filepath);
                success = false;
            }
        } catch (const std::exception& e) {
            pImpl->validationErrors.push_back("Error loading " + filepath + ": " + std::string(e.what()));
            success = false;
        }
    }
    
    // Clear previous errors
    pImpl->validationErrors.clear();
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::shared_lock<std::shared_mutex> lock(pImpl->rwMutex);
    
    nlohmann::json j;
    
    for (const auto& entry : pImpl->entries) {
        nlohmann::json entryJson;
        entryJson["key"] = entry.key;
        entryJson["value"] = toJson(entry.value);
        entryJson["source"] = static_cast<int>(entry.source);
        
        if (!entry.description.empty()) {
            entryJson["description"] = entry.description;
        }
        
        j.push_back(entryJson);
    }
    
    file << j.dump(2);
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->keyIndex.find(key);
    if (it != pImpl->keyIndex.end()) {
        // Update existing entry
        size_t index = it->second;
        if (index < pImpl->entries.size()) {
            pImpl->entries[index].value = value;
            pImpl->entries[index].source = source;
            pImpl->entries[index].position = index;
        }
    } else {
        // Add new entry
        ConfigEntry entry;
        entry.key = key;
        entry.value = value;
        entry.source = source;
        entry.position = pImpl->entries.size();
        
        pImpl->entries.push_back(entry);
        pImpl->keyIndex[key] = entry.position;
    }
}

// Backward compatibility overloads - redirect to single set method
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->rwMutex);
    
    auto it = pImpl->keyIndex.find(key);
    if (it == pImpl->keyIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = it->second;
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    const auto& entry = pImpl->entries[index];
    
    try {
        return std::get<T>(entry.value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
std::optional<T> Config::get(size_t index) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->rwMutex);
    
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    const auto& entry = pImpl->entries[index];
    
    try {
        return std::get<T>(entry.value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

std::optional<int> Config::get(int index) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->rwMutex);
    
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    const auto& entry = pImpl->entries[index];
    
    try {
        return std::get<int>(entry.value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

std::optional<int64_t> Config::get(int64_t index) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->rwMutex);
    
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    const auto& entry = pImpl->entries[index];
    
    try {
        return std::get<int64_t>(entry.value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

std::optional<double> Config::get(double index) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->rwMutex);
    
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    const auto& entry = pImpl->entries[index];
    
    try {
        return std::get<double>(entry.value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

std::optional<bool> Config::get(bool index) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->rwMutex);
    
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    const auto& entry = pImpl->entries[index];
    
    try {
        return std::get<bool>(entry.value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

std::optional<std::string> Config::get(const std::string& key) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->rwMutex);
    
    auto it = pImpl->keyIndex.find(key);
    if (it == pImpl->keyIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = it->second;
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    const auto& entry = pImpl->entries[index];
    
    try {
        return std::get<std::string>(entry.value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

std::optional<std::vector<int>> Config::get(const std::vector<int>& key) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->rwMutex);
    
    auto it = pImpl->keyIndex.find(key);
    if (it == pImpl->keyIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = it->second;
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    const auto& entry = pImpl->entries[index];
    
    try {
        return std::get<std::vector<int>>(entry.value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

std::optional<std::vector<double>> Config::get(const std::vector<double>& key) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->rwMutex);
    
    auto it = pImpl->keyIndex.find(key);
    if (it == pImpl->keyIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = it->second;
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    const auto& entry = pImpl->entries[index];
    
    try {
        return std::get<std::vector<double>>(entry.value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

std::optional<std::vector<std::string>> Config::get(const std::vector<std::string>& key) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->rwMutex);
    
    auto it = pImpl->keyIndex.find(key);
    if (it == pImpl->keyIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = it->second;
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    const auto& entry = pImpl->entries[index];
    
    try {
        return std::get<std::vector<std::string>>(entry.value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

bool Config::has(const std::string& key) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->rwMutex);
    return pImpl->keyIndex.find(key) != pImpl->keyIndex.end();
}

void Config::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->keyIndex.find(key);
    if (it != pImpl->keyIndex.end()) {
        size_t index = it->second;
        
        // Update remaining entries' positions
        for (auto& entry : pImpl->entries) {
            if (entry.position > index) {
                entry.position--;
            }
        }
        
        // Remove from entries vector
        pImpl->entries.erase(pImpl->entries.begin() + index);
        
        // Remove from index
        pImpl->keyIndex.erase(it);
        
        // Rebuild index
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            pImpl->entries[i].position = i;
            pImpl->keyIndex[pImpl->entries[i].key] = i;
        }
    }
}

std::vector<std::string> Config::getKeys() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->rwMutex);
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
    std::shared_lock<std::shared_mutex> lock(pImpl->rwMutex);
    
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

size_t Config::size() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->rwMutex);
    return pImpl->entries.size();
}

bool Config::validateSchema(const std::vector<ConfigSchemaEntry>& schema) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->validationErrors.clear();
    
    bool isValid = true;
    
    for (const auto& schemaEntry : schema) {
        // Check if key exists
        if (!has(schemaEntry.key)) {
            if (schemaEntry.required) {
                pImpl->validationErrors.push_back("Missing required key: " + schemaEntry.key);
                isValid = false;
            }
            continue;
        }
        
        // Get value type
        auto valueOpt = getFromIndex(schemaEntry.key);
        if (!valueOpt) {
            pImpl->validationErrors.push_back("Cannot get value for key: " + schemaEntry.key);
            isValid = false;
            continue;
        }
        
        // Type checking (simplified - would need proper type deduction)
        // For now, just basic validation
        std::string typeName = getTypeName(schemaEntry.type);
        std::string valueTypeName = getVariantTypeName(*valueOpt);
        
        // Basic validation (would need more sophisticated type checking)
        // This is a placeholder for actual type validation
    }
    
    return isValid;
}

std::string Config::getValidationErrors() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->rwMutex);
    std::ostringstream oss;
    for (const auto& error : pImpl->validationErrors) {
        oss << "Error: " << error << "\n";
    }
    return oss.str();
}

std::string Config::compareTo(const Config& other) const {
    std::shared_lock<std::shared_mutex> lock1(pImpl->rwMutex);
    std::shared_lock<std::shared_mutex> lock2(other.pImpl->rwMutex, std::defer_lock);
    std::lock_guard<std::mutex> guard(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "Configuration Comparison:\n";
    oss << "This config has " << pImpl->entries.size() << " entries\n";
    oss << "Other config has " << other.pImpl->entries.size() << " entries\n";
    
    // Find common keys
    std::set<std::string> commonKeys;
    for (const auto& entry : pImpl->entries) {
        if (other.has(entry.key)) {
            commonKeys.insert(entry.key);
        }
    }
    
    oss << "Common keys: " << commonKeys.size() << "\n";
    
    // Compare common keys
    for (const auto& key : commonKeys) {
        auto val1 = getFromIndex(key);
        auto val2 = other.getFromIndex(key);
        
        if (val1 && val2) {
            // Compare values
            if (compareValues(*val1, *val2)) {
                oss << "  " << key << ": Same\n";
            } else {
                oss << "  " << key << ": Different\n";
            }
        }
    }
    
    // Find unique keys
    std::set<std::string> onlyInThis, onlyInOther;
    for (const auto& entry : pImpl->entries) {
        if (!other.has(entry.key)) {
            onlyInThis.insert(entry.key);
        }
    }
    
    for (const auto& entry : other.pImpl->entries) {
        if (!has(entry.key)) {
            onlyInOther.insert(entry.key);
        }
    }
    
    oss << "Only in this config: " << onlyInThis.size() << " keys\n";
    oss << "Only in other config: " << onlyInOther.size() << " keys\n";
    
    pImpl->lastComparison = oss.str();
    return oss.str();
}

std::string Config::optimize(const std::string& optimizationType) const {
    std::shared_lock<std::shared_mutex> lock(pImpl->rwMutex);
    std::ostringstream oss;
    oss << "Configuration Optimization Results:\n";
    oss << "Optimization type: " << optimizationType << "\n";
    oss << "Current configuration size: " << pImpl->entries.size() << " entries\n";
    
    // Generate optimization suggestions
    if (optimizationType == "performance") {
        oss << "\nPerformance optimization suggestions:\n";
        
        // Find slow-to-access keys
        oss << "  Consider moving frequently accessed keys to front of configuration\n";
        oss << "  Group related keys together for better cache locality\n";
        oss << "  Remove unused keys to reduce memory footprint\n";
        
        // Calculate estimated optimization gain
        double estimatedGain = pImpl->entries.size() * 0.05; // 5% gain estimate
        oss << "  Estimated performance gain: " << estimatedGain * 100.0f << "%\n";
    }
    
    else if (optimizationType == "memory") {
        oss << "\nMemory optimization suggestions:\n";
        
        // Estimate memory usage
        size_t currentMemory = estimateMemoryUsage();
        oss << "  Current memory usage: " << currentMemory << " bytes\n";
        oss << "  Estimated memory after optimization: " << currentMemory * 0.9 << " bytes\n";
        oss << "  Memory savings: " << currentMemory * 0.1 << " bytes (10%)\n";
        
        // Suggest compression
        oss << "  Consider compressing string values\n";
        oss << "  Use smaller data types where possible\n";
    }
    
    else if (optimizationType == "access") {
        oss << "\nAccess optimization suggestions:\n";
        
        // Find hot keys
        oss << "  Identify frequently accessed keys\n";
        oss << "  Create index for hot keys\n";
        oss << "  Consider pre-fetching commonly used values\n";
    }
    
    pImpl->lastOptimization = oss.str();
    return oss.str();
}

double Config::getMemoryUsage() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->rwMutex);
    
    // Update cache if older than 1 second
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->lastStatsUpdate).count();
    
    if (elapsed >= 1) {
        pImpl->lastMemoryUsage = estimateMemoryUsage();
        pImpl->lastStatsUpdate = now;
    }
    
    return pImpl->lastMemoryUsage;
}

std::string Config::getPerformanceStats() const {
    std::shared_lock<std::shared_mutex> lock(pImpl->rwMutex);
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->lastStatsUpdate).count();
    
    std::ostringstream oss;
    oss << "=== Performance Statistics ===\n";
    oss << "Last updated: " << elapsed << " seconds ago\n";
    oss << "Configuration size: " << pImpl->entries.size() << " entries\n";
    oss << "Memory usage: " << getMemoryUsage() << " bytes\n";
    oss << "Keys indexed: " << pImpl->keyIndex.size() << "\n";
    
    // Access time estimation
    double avgAccessTime = pImpl->entries.empty() ? 0.0 : 
        1000.0 / static_cast<double>(pImpl->entries.size()); // microseconds
    oss << "Average access time: " << avgAccessTime << " microseconds\n";
    
    // Throughput estimation
    oss << "Estimated throughput: " << (pImpl->keyIndex.size() > 0 ? 
        1000000.0 / static_cast<double>(pImpl->keyIndex.size()) : 0.0) << " operations/sec\n";
    
    pImpl->lastPerformanceStats = oss.str();
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

// Helper function implementations
auto Config::getFromIndex(const std::string& key) const -> std::optional<ConfigValue> {
    auto it = pImpl->keyIndex.find(key);
    if (it == pImpl->keyIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = it->second;
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    return pImpl->entries[index].value;
}

std::string Config::getTypeName(const std::type_index& type) const {
    // Simple type name mapping
    if (type == typeid(int)) return "int";
    if (type == typeid(int64_t)) return "int64_t";
    if (type == typeid(double)) return "double";
    if (type == typeid(bool)) return "bool";
    if (type == typeid(std::string)) return "string";
    if (type == typeid(std::vector<int>)) return "vector<int>";
    if (type == typeid(std::vector<double>)) return "vector<double>";
    if (type == typeid(std::vector<std::string>)) return "vector<string>";
    return "unknown";
}

std::string Config::getVariantTypeName(const ConfigValue& value) const {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) return "int";
        if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
        if constexpr (std::is_same_v<T, double>) return "double";
        if constexpr (std::is_same_v<T, bool>) return "bool";
        if constexpr (std::is_same_v<T, std::string>) return "string";
        if constexpr (std::is_same_v<T, std::vector<int>>) return "vector<int>";
        if constexpr (std::is_same_v<T, std::vector<double>>) return "vector<double>";
        if constexpr (std::is_same_v<T, std::vector<std::string>>) return "vector<string>";
        return "unknown";
    }, value);
}

size_t Config::estimateMemoryUsage() const {
    size_t total = 0;
    for (const auto& entry : pImpl->entries) {
        // Estimate size of each entry
        total += sizeof(entry.key);
        total += sizeof(entry.source);
        total += sizeof(entry.description);
        
        // Estimate variant size (worst case)
        total += 64; // Rough estimate
    }
    
    return total;
}

bool Config::compareValues(const ConfigValue& a, const ConfigValue& b) const {
    return std::visit([](auto&& arg1, auto&& arg2) -> bool {
        using T1 = std::decay_t<decltype(arg1)>;
        using T2 = std::decay_t<decltype(arg2)>;
        
        if constexpr (std::is_same_v<T1, T2>) {
            return arg1 == arg2;
        } else {
            return false; // Different types are considered different
        }
    }, a, b);
}

// Explicit template instantiations for get
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::vector<int>&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::vector<double>&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::vector<std::string>&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm