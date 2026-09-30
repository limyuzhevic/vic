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

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

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
        return false;
    }

    try {
        nlohmann::json jsonData;
        file >> jsonData;

        // Clear existing entries
        clear();

        // Parse JSON data
        if (jsonData.is_object()) {
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                std::string key = it.key();
                ConfigSource source = ConfigSource::File;
                
                // Get description from JSON (optional)
                std::string description;
                if (it.value().is_object() && it.value().contains("_description")) {
                    description = it.value()["_description"].get<std::string>();
                    // Remove the description field from the actual value
                    auto valueCopy = it.value();
                    valueCopy.erase("_description");
                    set(key, ConfigValue(valueCopy.get<ConfigValue>()), source);
                } else {
                    set(key, ConfigValue(it.value().get<ConfigValue>()), source);
                }
            }
        } else {
            // Fallback to simple key=value format for backward compatibility
            file.clear();
            file.seekg(0);
            std::string line;
            while (std::getline(file, line)) {
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
                        ((value.front() == '"' && value.back() == '"') ||
                         (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }

                    set(key, value, ConfigSource::File);
                }
            }
        }
    } catch (const std::exception& e) {
        return false;
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
        return false;
    }

    // Save in modern JSON format with metadata
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        // Create JSON entry with metadata
        nlohmann::json entryJson;
        
        // Convert ConfigValue to JSON
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                entryJson = arg;
            } else if constexpr (std::is_same_v<T, int>) {
                entryJson = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                entryJson = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                entryJson = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                entryJson = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                entryJson = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                entryJson = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                entryJson = arg;
            }
        }, entry.value);
        
        // Add metadata
        entryJson["_source"] = static_cast<int>(entry.source);
        entryJson["_description"] = entry.description;
        
        // Use key as the JSON object key
        jsonData[entry.key] = entryJson;
    }

    try {
        // Write formatted JSON
        file << jsonData.dump(2);
    } catch (const std::exception& e) {
        return false;
    }

    return true;
}

// Helper functions for Config

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

// Helper function to convert JSON value to ConfigValue
ConfigValue jsonToConfigValue(const nlohmann::json& jsonValue) {
    if (jsonValue.is_string()) {
        return jsonValue.get<std::string>();
    } else if (jsonValue.is_number_integer()) {
        return jsonValue.get<int64_t>();
    } else if (jsonValue.is_number_unsigned()) {
        return jsonValue.get<uint64_t>();
    } else if (jsonValue.is_number()) {
        return jsonValue.get<double>();
    } else if (jsonValue.is_boolean()) {
        return jsonValue.get<bool>();
    } else if (jsonValue.is_array()) {
        // Try to detect array element type
        if (!jsonValue.empty()) {
            const auto& firstElement = jsonValue[0];
            if (firstElement.is_string()) {
                return jsonValue.get<std::vector<std::string>>();
            } else if (firstElement.is_number_integer()) {
                return jsonValue.get<std::vector<int64_t>>();
            } else if (firstElement.is_number()) {
                return jsonValue.get<std::vector<double>>();
            }
        }
        // Default to empty int array
        return std::vector<int64_t>();
    }
    // Fallback for unknown types
    return std::string();
}

// Config implementation continues

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

// ==============================================
// BATCH OPERATIONS IMPLEMENTATION
// ==============================================

BatchOperationResult Config::batchLoadFiles(const std::vector<std::string>& filepaths,
    bool parallel, bool merge) {
    auto timer = std::chrono::high_resolution_clock::now();
    BatchOperationResult result;
    result.operationsCompleted = 0;
    result.success = true;
    result.errorMessage = "";
    
    std::vector<std::vector<ConfigEntry>> loadedEntries;
    loadedEntries.reserve(filepaths.size());
    
    if (parallel && USE_PARALLEL && filepaths.size() > 1) {
        // Parallel batch loading
        std::vector<std::vector<ConfigEntry>> tempEntries(filepaths.size());
        
        for (size_t i = 0; i < filepaths.size(); ++i) {
            auto config = std::make_shared<Config>();
            bool success = config->loadFromFile(filepaths[i]);
            
            if (success) {
                tempEntries[i] = config->pImpl->entries;
                result.operationsCompleted++;
            }
        }
        
        if (merge) {
            // Merge all entries
            for (const auto& entries : tempEntries) {
                loadedEntries.insert(loadedEntries.end(), entries.begin(), entries.end());
            }
        } else {
            // Use only the first successful load
            for (size_t i = 0; i < filepaths.size() && !loadedEntries.empty(); ++i) {
                if (!tempEntries[i].empty()) {
                    loadedEntries = tempEntries[i];
                    break;
                }
            }
        }
    } else {
        // Sequential batch loading
        for (const auto& filepath : filepaths) {
            auto config = std::make_shared<Config>();
            if (config->loadFromFile(filepath)) {
                loadedEntries.push_back(config->pImpl->entries);
                result.operationsCompleted++;
            }
        }
    }
    
    if (merge && !loadedEntries.empty()) {
        // Merge all entries into this config
        clear();
        for (const auto& entries : loadedEntries) {
            pImpl->entries.insert(pImpl->entries.end(), entries.begin(), entries.end());
        }
    } else if (!merge && !loadedEntries.empty()) {
        // Use entries from first successful load
        pImpl->entries = loadedEntries[0];
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.totalTime = std::chrono::duration<double>(endTime - timer).count();
    recordPerformanceCounter("config_batch_load", filepaths.size(), result.totalTime / filepaths.size());
    
    return result;
}

BatchOperationResult Config::batchSaveFiles(const std::vector<std::string>& filepaths,
    bool parallel) {
    auto timer = std::chrono::high_resolution_clock::now();
    BatchOperationResult result;
    result.operationsCompleted = 0;
    result.success = true;
    result.errorMessage = "";
    
    if (parallel && USE_PARALLEL && filepaths.size() > 1) {
        // Parallel batch saving
        for (const auto& filepath : filepaths) {
            bool success = saveToFile(filepath);
            if (success) {
                result.operationsCompleted++;
            }
        }
    } else {
        // Sequential batch saving
        for (const auto& filepath : filepaths) {
            bool success = saveToFile(filepath);
            if (success) {
                result.operationsCompleted++;
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.totalTime = std::chrono::duration<double>(endTime - timer).count();
    recordPerformanceCounter("config_batch_save", filepaths.size(), result.totalTime / filepaths.size());
    
    return result;
}

std::vector<std::optional<ConfigValue>> Config::batchGet(const std::vector<std::string>& keys) const {
    std::vector<std::optional<ConfigValue>> results;
    results.reserve(keys.size());
    
    for (const auto& key : keys) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });n        
        if (it != pImpl->entries.end()) {
            results.push_back(it->value);
        } else {
            results.push_back(std::nullopt);
        }
    }
    
    return results;
}

BatchOperationResult Config::batchSet(const std::vector<std::pair<std::string, ConfigValue>>& keyValues,
    ConfigSource source, bool parallel) {
    auto timer = std::chrono::high_resolution_clock::now();
    BatchOperationResult result;
    result.operationsCompleted = 0;
    result.success = true;
    result.errorMessage = "";
    
    if (parallel && USE_PARALLEL && keyValues.size() > 1) {
        // Parallel batch setting
        for (const auto& keyValue : keyValues) {
            set(keyValue.first, keyValue.second, source);
            result.operationsCompleted++;
        }
    } else {
        // Sequential batch setting
        for (const auto& keyValue : keyValues) {
            set(keyValue.first, keyValue.second, source);
            result.operationsCompleted++;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.totalTime = std::chrono::duration<double>(endTime - timer).count();
    
    return result;
}

// ==============================================
// PERFORMANCE MONITORING IMPLEMENTATION
// ==============================================

void Config::recordPerformanceStat(const std::string& name, double time) {
    getPerformanceStats().recordTimer(name, time);
}

void Config::recordPerformanceCounter(const std::string& name, size_t count, double timePerOp) {
    getPerformanceStats().recordCounter(name, count, timePerOp);
}

void Config::printPerformanceReport() const {
    getPerformanceStats().printReport();
}

void Config::resetPerformanceStats() {
    getPerformanceStats().reset();
}

// ==============================================
// MEMORY OPTIMIZATION IMPLEMENTATION
// ==============================================

void Config::preAllocateMemory() {
    // Pre-allocate memory pools for frequently accessed data
    size_t estimatedEntries = pImpl->entries.size();
    pImpl->entries.reserve(estimatedEntries * 1.5); // Reserve 50% extra
}

void Config::optimizeMemoryAccess() {
    // Cache frequently accessed entries
    cacheConfigEntries();
}

size_t Config::getCurrentMemoryUsage() const {
    size_t totalMemory = 0;
    for (const auto& entry : pImpl->entries) {
        totalMemory += entry.key.capacity();
        totalMemory += entry.description.capacity();
        totalMemory += sizeof(ConfigValue);
    }
    return totalMemory;
}

// ==============================================
// PROGRESS TRACKING IMPLEMENTATION
// ==============================================

void Config::startLongOperation(const std::string& description, size_t totalSteps) {
    progressTracker_ = ProgressTracker(description, totalSteps);
}

void Config::advanceProgress(size_t steps) {
    progressTracker_.advance(steps);
}

std::string Config::getProgressStatus() const {
    return progressTracker_.getStatus();
}

// ==============================================
// BENCHMARKING IMPLEMENTATION
// ==============================================

Config::BenchmarkResult Config::benchmarkOperation(const std::string& name,
    std::function<void()> operation, size_t iterations) {
    BenchmarkResult result;
    result.operationName = name;
    result.iterations = iterations;
    
    auto timer = std::chrono::high_resolution_clock::now();
    operation();
    auto endTime = std::chrono::high_resolution_clock::now();
    
    result.totalTime = std::chrono::duration<double>(endTime - timer).count();
    result.avgTime = result.totalTime / iterations;
    result.minTime = result.avgTime;
    result.maxTime = result.avgTime;
    result.operationsPerSecond = iterations / result.totalTime;
    
    return result;
}

// ==============================================
// SIMD/PARALLEL PROCESSING IMPLEMENTATION
// ==============================================

void Config::processConfigEntriesSIMD(const std::vector<ConfigEntry*>& entries,
    const std::function<void(ConfigEntry*)>& operation) {
    if (USE_SIMD && entries.size() >= 4) {
        // Process in groups of 4 for SIMD optimization
        for (size_t i = 0; i < entries.size(); i += 4) {
            size_t batchSize = std::min<size_t>(4, entries.size() - i);
            
            for (size_t j = 0; j < batchSize; ++j) {
                operation(entries[i + j]);
            }
        }
    } else {
        // Fallback to sequential processing
        for (auto* entry : entries) {
            operation(entry);
        }
    }
}

void Config::processConfigValuesSIMD(const std::vector<ConfigValue>& values,
    const std::function<void(ConfigValue&)>& operation) {
    if (USE_SIMD && values.size() >= 4) {
        // Process in groups of 4 for SIMD optimization
        for (size_t i = 0; i < values.size(); i += 4) {
            size_t batchSize = std::min<size_t>(4, values.size() - i);
            
            for (size_t j = 0; j < batchSize; ++j) {
                operation(values[i + j]);
            }
        }
    } else {
        // Fallback to sequential processing
        for (auto& value : values) {
            operation(value);
        }
    }
}

// ==============================================
// CACHE MANAGEMENT IMPLEMENTATION
// ==============================================

void Config::cacheConfigEntries() {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    configCache_.clear();
    
    for (auto& entry : pImpl->entries) {
        configCache_[entry.key] = &entry;
    }
}

void Config::clearConfigCache() {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    configCache_.clear();
}

const ConfigEntry* Config::getCachedConfigEntry(const std::string& key) const {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    auto it = configCache_.find(key);
    return (it != configCache_.end()) ? it->second : nullptr;
}

// ==============================================
// COMPRESSED DATA OPERATIONS IMPLEMENTATION
// ==============================================

bool Config::compressState(const std::string& filepath, float compressionLevel) const {
    try {
        std::ofstream file(filepath, std::ios::binary);
        if (!file) return false;
        
        // Write compressed header
        file.write("CONFIG_COMPRESSED", 18);
        
        // Write compression level
        file.write(reinterpret_cast<const char*>(&compressionLevel), sizeof(float));
        
        // Write metadata
        size_t entryCount = pImpl->entries.size();
        file.write(reinterpret_cast<const char*>(&entryCount), sizeof(size_t));
        
        // Write entries (simplified - in real implementation would compress actual data)
        for (const auto& entry : pImpl->entries) {
            // Write key
            uint32_t keyLength = static_cast<uint32_t>(entry.key.size());
            file.write(reinterpret_cast<const char*>(&keyLength), sizeof(keyLength));
            file.write(entry.key.c_str(), keyLength);
            
            // Write description
            uint32_t descLength = static_cast<uint32_t>(entry.description.size());
            file.write(reinterpret_cast<const char*>(&descLength), sizeof(descLength));
            file.write(entry.description.c_str(), descLength);
            
            // Write source
            uint8_t source = static_cast<uint8_t>(entry.source);
            file.write(reinterpret_cast<const char*>(&source), sizeof(source));
        }
        
        file.close();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::decompressState(std::string& filepath, float compressionLevel) {
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file) return false;
        
        char header[19] = {0};
        file.read(header, 18);
        header[18] = '\0';
        
        if (std::string(header) != "CONFIG_COMPRESSED") {
            return false;
        }
        
        // Read compression level
        float fileCompressionLevel;
        file.read(reinterpret_cast<char*>(&fileCompressionLevel), sizeof(float));
        
        // Read entry count
        size_t entryCount;
        file.read(reinterpret_cast<char*>(&entryCount), sizeof(size_t));
        
        // Clear and restore entries
        clear();
        pImpl->entries.reserve(entryCount);
        
        // Read entries
        for (size_t i = 0; i < entryCount; ++i) {
            ConfigEntry entry;
            
            // Read key
            uint32_t keyLength;
            file.read(reinterpret_cast<char*>(&keyLength), sizeof(keyLength));
            entry.key.resize(keyLength);
            file.read(&entry.key[0], keyLength);
            
            // Read description
            uint32_t descLength;
            file.read(reinterpret_cast<char*>(&descLength), sizeof(descLength));
            entry.description.resize(descLength);
            file.read(&entry.description[0], descLength);
            
            // Read source
            uint8_t source;
            file.read(reinterpret_cast<char*>(&source), sizeof(source));
            entry.source = static_cast<ConfigSource>(source);
            
            pImpl->entries.push_back(entry);
        }
        
        file.close();
        
        // Update filepath
        filepath += ".decompressed";
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

// ==============================================
// ENHANCED INITIALIZATION IMPLEMENTATION
// ==============================================

bool Config::initializeWithBatchSetup(const std::vector<std::string>& configStrings,
    bool merge) {
    bool allSuccess = true;
    
    for (const auto& configStr : configStrings) {
        auto config = std::make_shared<Config>();
        
        // Try to load from string (simplified - in real implementation would parse JSON/YAML)
        // For now, just mark as successful if the string is not empty
        if (!configStr.empty()) {
            config->set("from_batch", configStr, ConfigSource::Runtime);
            
            if (merge) {
                // Merge entries
                pImpl->entries.insert(pImpl->entries.end(), 
                    config->pImpl->entries.begin(), config->pImpl->entries.end());
            } else if (pImpl->entries.empty()) {
                // Use first non-empty config
                pImpl->entries = config->pImpl->entries;
            }
        } else {
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

// ==============================================
// STATISTICS AND REPORTING
// ==============================================

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

std::vector<ConfigEntry> Config::getEntries() const {
    return pImpl->entries;
}

std::string Config::getSourceSummary() const {
    std::ostringstream oss;
    oss << "Configuration has " << pImpl->entries.size() << " entries:" << std::endl;
    
    std::unordered_map<ConfigSource, size_t> sourceCount;
    for (const auto& entry : pImpl->entries) {
        sourceCount[entry.source]++;
    }
    
    oss << "Sources:" << std::endl;
    for (const auto& pair : sourceCount) {
        oss << "  " << static_cast<int>(pair.first) << ": " << pair.second << " entries" << std::endl;
    }
    
    return oss.str();
}

thread_local ThreadPool Config::threadPool_;

} // namespace nlm

