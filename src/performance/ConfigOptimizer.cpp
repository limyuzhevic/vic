#include "ConfigOptimizer.hpp"

namespace nlm {

ConfigOptimizer::ConfigOptimizer(bool enableCaching,
                                bool enableCompression,
                                bool enableLazyLoading,
                                size_t cacheSize,
                                int compressionLevel)
    : initialized_(false)
    , cachingEnabled_(enableCaching)
    , compressionEnabled_(enableCompression)
    , lazyLoadingEnabled_(enableLazyLoading)
    , cacheSize_(cacheSize)
    , compressionLevel_(compressionLevel) {
}

ConfigOptimizer::~ConfigOptimizer() {
    if (initialized_) {
        finalize();
    }
}

bool ConfigOptimizer::initialize() {
    if (initialized_) {
        return true;
    }
    
    try {
        // Initialize cache if enabled
        if (cachingEnabled_) {
            configCache_.clear();
        }
        
        // Reset statistics
        loadingStats_ = LoadingStats();
        compressionStats_ = CompressionStats();
        
        // Load default schemas
        loadDefaultSchemas();
        
        initialized_ = true;
        return true;
    } catch (...) {
        initialized_ = false;
        return false;
    }
}

void ConfigOptimizer::finalize() {
    if (!initialized_) {
        return;
    }
    
    // Clear cache
    clearConfigCache();
    
    // Clear lazy loading configuration
    {
        std::lock_guard<std::mutex> lock(lazyLoadMutex_);
        lazyLoadSections_.clear();
    }
    
    // Clear compiled schemas
    compiledSchemas_.clear();
    
    initialized_ = false;
}

bool ConfigOptimizer::loadConfig(const std::string& filepath, std::string& configData) {
    auto startTime = std::chrono::steady_clock::now();
    
    bool result = false;
    
    // Check cache first
    if (cachingEnabled_) {
        std::lock_guard<std::mutex> lock(cacheMutex_);
        if (getCachedConfig(filepath, configData)) {
            loadingStats_.totalLoads++;
            loadingStats_.successfulLoads++;
            loadingStats_.lastLoadTime = std::chrono::steady_clock::now();
            return true;
        }
    }
    
    // Load from file
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
        // Read file content
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        configData.resize(fileSize);
        if (file.read(&configData[0], fileSize)) {
            // Validate JSON structure
            std::vector<ConfigValidationError> errors;
            if (validateJSONConfig(configData, errors)) {
                // Cache the configuration
                if (cachingEnabled_) {
                    cacheConfig(filepath, configData);
                }
                
                result = true;
            }
        }
        
        file.close();
    }
    
    // Update statistics
    {
        std::lock_guard<std::mutex> lock(statsMutex_);
        loadingStats_.totalLoads++;
        if (result) {
            loadingStats_.successfulLoads++;
        } else {
            loadingStats_.failedLoads++;
        }
        
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        if (loadingStats_.totalLoads > 0) {
            loadingStats_.averageLoadTime = static_cast<double>(loadingStats_.averageLoadTime * 
                (loadingStats_.totalLoads - 1) + duration.count()) / loadingStats_.totalLoads;
        } else {
            loadingStats_.averageLoadTime = static_cast<double>(duration.count());
        }
        
        loadingStats_.lastLoadTime = endTime;
    }
    
    return result;
}

bool ConfigOptimizer::loadConfigFromString(const std::string& configString, std::string& configData) {
    auto startTime = std::chrono::steady_clock::now();
    
    // Validate JSON structure
    std::vector<ConfigValidationError> errors;
    if (!validateJSONConfig(configString, errors)) {
        return false;
    }
    
    configData = configString;
    
    // Update statistics
    {
        std::lock_guard<std::mutex> lock(statsMutex_);
        loadingStats_.totalLoads++;
        loadingStats_.successfulLoads++;
        
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::milliseconds>(endTime - startTime);
        
        if (loadingStats_.totalLoads > 0) {
            loadingStats_.averageLoadTime = static_cast<double>(loadingStats_.averageLoadTime * 
                (loadingStats_.totalLoads - 1) + duration.count()) / loadingStats_.totalLoads;
        } else {
            loadingStats_.averageLoadTime = static_cast<double>(duration.count());
        }
        
        loadingStats_.lastLoadTime = endTime;
    }
    
    return true;
}

bool ConfigOptimizer::validateConfig(const std::string& configData, const std::string& schema,
                                    std::vector<ConfigValidationError>& errors) {
    errors.clear();
    
    // Convert to JSON if needed (simplified implementation)
    std::string jsonConfig = configData;
    if (!jsonConfig.empty() && (jsonConfig.front() != '{' || jsonConfig.back() != '}')) {
        // Try to convert from other formats to JSON
        // This is a placeholder - real implementation would parse the format
        jsonConfig = "{\"content\": \"" + jsonConfig + "\"}";
    }
    
    return validateJSONConfig(jsonConfig, errors);
}

bool ConfigOptimizer::validateJSONConfig(const std::string& configData,
                                        std::vector<ConfigValidationError>& errors) {
    errors.clear();
    
    if (configData.empty()) {
        ConfigValidationError error;
        error.path = "$root";
        error.message = "Configuration is empty";
        error.expectedType = "non-empty string";
        error.actualType = "empty";
        error.isFatal = true;
        errors.push_back(error);
        return false;
    }
    
    // Minify JSON for efficient parsing
    std::string minified = minifyJSON(configData);
    
    // Basic JSON validation (check braces and commas)
    int braceCount = 0;
    int bracketCount = 0;
    bool inString = false;
    char prevChar = '\0';
    
    for (size_t i = 0; i < minified.size(); ++i) {
        char c = minified[i];
        
        if (c == '"' && prevChar != '\\') {
            inString = !inString;
        } else if (!inString) {
            if (c == '{') {
                braceCount++;
            } else if (c == '}') {
                braceCount--;
                if (braceCount < 0) {
                    ConfigValidationError error;
                    error.path = std::to_string(i);
                    error.message = "Extra closing brace";
                    error.expectedType = "valid JSON structure";
                    error.actualType = "invalid structure";
                    error.isFatal = true;
                    errors.push_back(error);
                    return false;
                }
            } else if (c == '[') {
                bracketCount++;
            } else if (c == ']') {
                bracketCount--;
                if (bracketCount < 0) {
                    ConfigValidationError error;
                    error.path = std::to_string(i);
                    error.message = "Extra closing bracket";
                    error.expectedType = "valid JSON structure";
                    error.actualType = "invalid structure";
                    error.isFatal = true;
                    errors.push_back(error);
                    return false;
                }
            }
        }
        
        prevChar = c;
    }
    
    if (braceCount != 0 || bracketCount != 0) {
        ConfigValidationError error;
        error.path = "$root";
        error.message = "Unbalanced braces or brackets";
        error.expectedType = "balanced JSON structure";
        error.actualType = "unbalanced structure";
        error.isFatal = true;
        errors.push_back(error);
        return false;
    }
    
    // Validate against compiled schemas
    for (const auto& schema : compiledSchemas_) {
        if (validateRequiredFields(minified, schema, errors)) {
            return errors.empty();
        }
    }
    
    return true;
}

CompressionStats ConfigOptimizer::compressConfig(const std::string& configData,
                                                std::string& compressedData) {
    CompressionStats stats;
    stats.originalSize = configData.size();
    
    if (!compressionEnabled_ || configData.empty()) {
        compressedData = configData;
        stats.compressedSize = stats.originalSize;
        stats.compressionRatio = 1.0;
    } else {
        compressedData = simpleCompress(configData);
        stats.compressedSize = compressedData.size();
        
        if (stats.originalSize > 0) {
            stats.compressionRatio = static_cast<double>(stats.compressedSize) / stats.originalSize;
        } else {
            stats.compressionRatio = 1.0;
        }
    }
    
    stats.compressionTime = std::chrono::steady_clock::now();
    compressionStats_ = stats;
    
    return stats;
}

bool ConfigOptimizer::decompressConfig(const std::string& compressedData, std::string& configData) {
    if (compressedData == configData) {
        // Already decompressed or not compressed
        configData = compressedData;
        return true;
    }
    
    try {
        configData = simpleDecompress(compressedData);
        return true;
    } catch (...) {
        return false;
    }
}

bool ConfigOptimizer::getCachedConfig(const std::string& key, std::string& configData) {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    
    auto it = configCache_.find(key);
    if (it != configCache_.end()) {
        configData = it->second.value;
        it->second.accessCount++;
        it->second.timestamp = std::chrono::steady_clock::now();
        return true;
    }
    
    return false;
}

void ConfigOptimizer::cacheConfig(const std::string& key, const std::string& configData) {
    if (!cachingEnabled_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(cacheMutex_);
    
    // Evict if cache is full
    cacheEvictionPolicy();
    
    ConfigCacheEntry entry;
    entry.key = key;
    entry.value = configData;
    entry.timestamp = std::chrono::steady_clock::now();
    entry.accessCount = 1;
    entry.memorySize = configData.size();
    
    configCache_[key] = std::move(entry);
}

void ConfigOptimizer::clearConfigCache() {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    configCache_.clear();
}

void ConfigOptimizer::enableLazyLoading(const std::string& section) {
    std::lock_guard<std::mutex> lock(lazyLoadMutex_);
    lazyLoadSections_.insert(section);
}

void ConfigOptimizer::disableLazyLoading(const std::string& section) {
    std::lock_guard<std::mutex> lock(lazyLoadMutex_);
    lazyLoadSections_.erase(section);
}

bool ConfigOptimizer::isLazyLoadingEnabled(const std::string& section) const {
    std::lock_guard<std::mutex> lock(lazyLoadMutex_);
    return lazyLoadSections_.find(section) != lazyLoadSections_.end();
}

LoadingStats ConfigOptimizer::getLoadingStats() const {
    std::lock_guard<std::mutex> lock(statsMutex_);
    return loadingStats_;
}

CompressionStats ConfigOptimizer::getCompressionStats() const {
    return compressionStats_;
}

std::string ConfigOptimizer::generateReport() const {
    std::ostringstream oss;
    
    oss << "=== Configuration Optimization Report ===\n";
    oss << "Caching Enabled: " << (cachingEnabled_ ? "Yes" : "No") << "\n";
    oss << "Compression Enabled: " << (compressionEnabled_ ? "Yes" : "No") << "\n";
    oss << "Lazy Loading Enabled: " << (lazyLoadingEnabled_ ? "Yes" : "No") << "\n";
    oss << "Cache Size: " << configCache_.size() << " / " << cacheSize_ << "\n";
    oss << "Compression Level: " << compressionLevel_ << "\n";
    
    oss << "\n=== Loading Statistics ===\n";
    oss << "Total Loads: " << loadingStats_.totalLoads << "\n";
    oss << "Successful Loads: " << loadingStats_.successfulLoads << "\n";
    oss << "Failed Loads: " << loadingStats_.failedLoads << "\n";
    oss << "Success Rate: " << std::fixed << std::setprecision(2)
        << (loadingStats_.totalLoads > 0 ? 
            (static_cast<double>(loadingStats_.successfulLoads) / loadingStats_.totalLoads * 100) : 0)
        << "%\n";
    oss << "Average Load Time: " << std::fixed << std::setprecision(3)
        << loadingStats_.averageLoadTime << " ms\n";
    oss << "Last Load Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - loadingStats_.lastLoadTime).count() << " ms ago\n";
    
    oss << "\n=== Compression Statistics ===\n";
    oss << "Original Size: " << compressionStats_.originalSize << " bytes\n";
    oss << "Compressed Size: " << compressionStats_.compressedSize << " bytes\n";
    oss << "Compression Ratio: " << std::fixed << std::setprecision(3)
        << (compressionStats_.originalSize > 0 ? 
            (compressionStats_.compressedSize / compressionStats_.originalSize * 100) : 0)
        << "%\n";
    
    oss << "\n=== Lazy Loading Sections ===\n";
    {
        std::lock_guard<std::mutex> lock(lazyLoadMutex_);
        oss << "Enabled Sections: " << lazyLoadSections_.size() << "\n";
        for (const auto& section : lazyLoadSections_) {
            oss << "  - " << section << "\n";
        }
    }
    
    oss << "\n=== Compiled Schemas ===\n";
    oss << "Loaded Schemas: " << compiledSchemas_.size() << "\n";
    
    return oss.str();
}

int ConfigOptimizer::getOptimalCompressionLevel(size_t configSize) {
    if (configSize < 1024) {
        return 0; // No compression for small configs
    } else if (configSize < 10 * 1024) {
        return 1;
    } else if (configSize < 100 * 1024) {
        return 6;
    } else {
        return 9; // Maximum compression for large configs
    }
}

double ConfigOptimizer::profileLoadPerformance(const std::string& filepath, size_t iterations) {
    if (iterations == 0) {
        return 0.0;
    }
    
    std::vector<double> times;
    times.reserve(iterations);
    
    for (size_t i = 0; i < iterations; ++i) {
        std::string configData;
        auto startTime = std::chrono::steady_clock::now();
        bool result = loadConfig(filepath, configData);
        auto endTime = std::chrono::steady_clock::now();
        
        if (result) {
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            times.push_back(static_cast<double>(duration.count()));
        }
    }
    
    if (times.empty()) {
        return 0.0;
    }
    
    double sum = std::accumulate(times.begin(), times.end(), 0.0);
    return sum / times.size();
}

double ConfigOptimizer::profileValidationPerformance(const std::string& configData, size_t iterations) {
    if (iterations == 0 || configData.empty()) {
        return 0.0;
    }
    
    std::vector<double> times;
    times.reserve(iterations);
    
    std::vector<ConfigValidationError> errors;
    
    for (size_t i = 0; i < iterations; ++i) {
        auto startTime = std::chrono::steady_clock::now();
        bool result = validateJSONConfig(configData, errors);
        auto endTime = std::chrono::steady_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        times.push_back(static_cast<double>(duration.count()));
    }
    
    if (times.empty()) {
        return 0.0;
    }
    
    double sum = std::accumulate(times.begin(), times.end(), 0.0);
    return sum / times.size();
}

void ConfigOptimizer::loadDefaultSchemas() {
    // Load basic neural network configuration schemas
    ConfigSchema neuronSchema;
    neuronSchema.name = "Neuron";
    neuronSchema.fields = {
        {"id", ConfigFieldType::INTEGER},
        {"type", ConfigFieldType::STRING},
        {"position", ConfigFieldType::VECTOR},
        {"weight", ConfigFieldType::FLOAT}
    };
    neuronSchema.description = "Neuron configuration schema";
    neuronSchema.isValid = true;
    compiledSchemas_.push_back(neuronSchema);
    
    ConfigSchema synapseSchema;
    synapseSchema.name = "Synapse";
    synapseSchema.fields = {
        {"source", ConfigFieldType::INTEGER},
        {"target", ConfigFieldType::INTEGER},
        {"weight", ConfigFieldType::FLOAT},
        {"delay", ConfigFieldType::INTEGER},
        {"type", ConfigFieldType::STRING}
    };
    synapseSchema.description = "Synapse configuration schema";
    synapseSchema.isValid = true;
    compiledSchemas_.push_back(synapseSchema);
}

bool ConfigOptimizer::validateRequiredFields(const std::string& json, const ConfigSchema& schema,
                                           std::vector<ConfigValidationError>& errors) const {
    // This is a simplified implementation
    // In a real implementation, would parse JSON and check fields
    
    // For now, just accept if JSON is non-empty
    if (!json.empty() && json.front() == '{' && json.back() == '}') {
        return true;
    }
    
    ConfigValidationError error;
    error.path = "$root";
    error.message = "Invalid configuration format";
    error.expectedType = "valid JSON object";
    error.actualType = "invalid format";
    error.isFatal = false;
    errors.push_back(error);
    
    return false;
}

void ConfigOptimizer::cacheEvictionPolicy() {
    if (configCache_.size() >= cacheSize_) {
        // Simple LRU-style eviction: remove oldest entries
        std::vector<std::string> keysToRemove;
        
        for (const auto& entry : configCache_) {
            keysToRemove.push_back(entry.first);
            if (keysToRemove.size() >= cacheSize_ / 2) {
                break; // Remove half the cache
            }
        }
        
        for (const auto& key : keysToRemove) {
            configCache_.erase(key);
        }
    }
}

std::string ConfigOptimizer::minifyJSON(const std::string& json) {
    std::string result;
    bool inString = false;
    char prevChar = '\0';
    
    for (char c : json) {
        if (c == '"' && prevChar != '\\') {
            inString = !inString;
            result.push_back(c);
        } else if (!inString) {
            if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
                // Skip whitespace
                continue;
            }
            result.push_back(c);
        } else {
            result.push_back(c);
        }
        
        prevChar = c;
    }
    
    return result;
}

std::string ConfigOptimizer::extractJSONField(const std::string& json, const std::string& field) {
    // Simplified JSON field extraction
    // In real implementation, would use proper JSON parser
    
    size_t pos = json.find('"' + field + '"');
    if (pos == std::string::npos) {
        return "";
    }
    
    size_t colonPos = json.find(':', pos);
    if (colonPos == std::string::npos) {
        return "";
    }
    
    size_t startValue = json.find_first_not_of(' ', colonPos + 1);
    if (startValue == std::string::npos) {
        return "";
    }
    
    // Find end of value (next comma or closing brace)
    size_t endValue = json.find_first_of(",}", startValue);
    if (endValue == std::string::npos) {
        endValue = json.size();
    }
    
    return json.substr(startValue, endValue - startValue);
}

std::string ConfigOptimizer::simpleCompress(const std::string& data) {
    // Very simple compression for demonstration
    // In real implementation, would use zlib, lz4, etc.
    
    // Simple run-length encoding for repeated characters
    std::string result;
    size_t i = 0;
    
    while (i < data.size()) {
        char current = data[i];
        size_t count = 1;
        
        // Count consecutive identical characters
        while (i + count < data.size() && data[i + count] == current) {
            count++;
        }
        
        if (count >= 3) {
            // Encode as count + character
            result += std::to_string(count) + current;
        } else {
            // Just encode the character
            for (size_t j = 0; j < count; ++j) {
                result += data[i + j];
            }
        }
        
        i += count;
    }
    
    // If compression didn't help, return original
    if (result.size() >= data.size()) {
        return data;
    }
    
    return result;
}

std::string ConfigOptimizer::simpleDecompress(const std::string& data) {
    std::string result;
    
    size_t i = 0;
    while (i < data.size()) {
        // Check if we have a number followed by a character (compressed format)
        if (i + 2 < data.size() && std::isdigit(data[i])) {
            // Find end of number
            size_t numEnd = i;
            while (numEnd < data.size() && std::isdigit(data[numEnd])) {
                numEnd++;
            }
            
            if (numEnd < data.size()) {
                // Parse number
                int count = std::stoi(data.substr(i, numEnd - i));
                char repeatChar = data[numEnd];
                
                // Add repeated character
                for (int j = 0; j < count; ++j) {
                    result.push_back(repeatChar);
                }
                
                i = numEnd + 1;
                continue;
            }
        }
        
        // Regular character
        result.push_back(data[i]);
        i++;
    }
    
    return result;
}

bool ConfigOptimizer::loadLazyConfig(const std::string& section, std::string& configData) {
    // In real implementation, would load only the specified section
    // For now, return empty
    return false;
}

} // namespace nlm