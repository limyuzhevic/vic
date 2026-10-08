// Optimized Config System Implementation
// File: src/core/Config/OptimizedConfig.cpp

#include "OptimizedConfig.hpp"
#include "../Logger/Logger.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <mutex>
#include <sstream>
#include <iomanip>

namespace nlm {

// OptimizedConfig Implementation
OptimizedConfig::OptimizedConfig() : Config() {
    enableCache(true);
    enableFastAccess(true);
    setValueCacheSize(1000);
    setAccessPatternCacheSize(500);
}

OptimizedConfig::~OptimizedConfig() = default;

OptimizedConfig::OptimizedConfig(OptimizedConfig&& other) noexcept : Config(std::move(other)) {}

OptimizedConfig& OptimizedConfig::operator=(OptimizedConfig&& other) noexcept {
    if (this != &other) {
        Config::operator=(std::move(other));
    }
    return *this;
}

bool OptimizedConfig::loadFromFile(const std::string& filepath) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Use fast file reading
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Read file in chunks for better performance
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    std::string content = buffer.str();
    
    // Determine file type and use optimized parser
    std::string extension = "." + filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json" || extension == ".cfg") {
        OptimizedJsonParser::ParseOptions options;
        options.enable_fast_path = true;
        options.enable_vector_optimization = true;
        
        auto parsed_config = OptimizedJsonParser::parse(content, options);
        
        // Convert to ConfigEntry format
        for (const auto& [key, value] : parsed_config) {
            set(key, value, ConfigSource::File);
        }
    } else if (extension == ".yaml" || extension == ".yml") {
        OptimizedYamlParser::ParseOptions options;
        options.enable_string_pooling = true;
        options.enable_fast_indentation = true;
        
        auto parsed_config = OptimizedYamlParser::parse(content, options);
        
        // Convert to ConfigEntry format
        for (const auto& [key, value] : parsed_config) {
            set(key, value, ConfigSource::File);
        }
    } else {
        // Fall back to standard parsing
        return Config::loadFromFile(filepath);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    double duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    
    // Record performance statistics
    if (pImpl) {
        pImpl->last_load_time = duration_ms;
        pImpl->load_count++;
    }
    
    return true;
}

bool OptimizedConfig::loadFromArgs(int argc, char** argv) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Optimized command-line parsing
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg.find("--") == 0) {
                // --key=value format
                size_t equals_pos = arg.find('=');
                if (equals_pos != std::string::npos) {
                    std::string key = arg.substr(2, equals_pos - 2);
                    std::string value = arg.substr(equals_pos + 1);
                    
                    // Try to determine value type
                    if (value == "true" || value == "false") {
                        bool bool_value = (value == "true");
                        set(key, bool_value, ConfigSource::CommandLine);
                    } else if (value.find('.') != std::string::npos) {
                        try {
                            double double_value = std::stod(value);
                            set(key, double_value, ConfigSource::CommandLine);
                        } catch (...) {
                            set(key, value, ConfigSource::CommandLine);
                        }
                    } else {
                        try {
                            int int_value = std::stoi(value);
                            set(key, int_value, ConfigSource::CommandLine);
                        } catch (...) {
                            set(key, value, ConfigSource::CommandLine);
                        }
                    }
                }
            } else if (arg.find('-') == 0 && arg.length() == 2) {
                // -k value format
                std::string key = arg.substr(1);
                i++;
                if (i < argc) {
                    std::string value = argv[i];
                    set(key, value, ConfigSource::CommandLine);
                }
            }
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    double duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    
    if (pImpl) {
        pImpl->last_parse_time = duration_ms;
        pImpl->cli_parse_count++;
    }
    
    return true;
}

bool OptimizedConfig::loadFromEnvironment() {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Optimized environment variable parsing
    // Using faster iteration pattern
    const char* env = std::getenv("NLM_CONFIG");
    if (env) {
        std::string env_config = env;
        // Parse environment configuration with optimized string processing
        size_t pos = 0;
        while (pos < env_config.size()) {
            size_t equals_pos = env_config.find('=', pos);
            if (equals_pos == std::string::npos) break;
            
            std::string key = env_config.substr(pos, equals_pos - pos);
            pos = equals_pos + 1;
            
            size_t next_pos = env_config.find(';', pos);
            std::string value = env_config.substr(pos, (next_pos == std::string::npos) ? std::string::npos : next_pos - pos);
            
            // Convert value with fast type detection
            if (value == "true" || value == "false") {
                bool bool_value = (value == "true");
                set(key, bool_value, ConfigSource::Environment);
            } else if (value.find('.') != std::string::npos) {
                try {
                    double double_value = std::stod(value);
                    set(key, double_value, ConfigSource::Environment);
                } catch (...) {
                    set(key, value, ConfigSource::Environment);
                }
            } else {
                try {
                    int int_value = std::stoi(value);
                    set(key, int_value, ConfigSource::Environment);
                } catch (...) {
                    set(key, value, ConfigSource::Environment);
                }
            }
            
            pos = (next_pos == std::string::npos) ? env_config.size() : next_pos + 1;
        }
    }
    
    // Also check for NLM_* prefix variables
    extern char** environ;
    for (char** env_ptr = environ; *env_ptr != nullptr; ++env_ptr) {
        std::string env_var = *env_ptr;
        if (env_var.find("NLM_") == 0) {
            size_t equals_pos = env_var.find('=');
            if (equals_pos != std::string::npos) {
                std::string key = env_var.substr(equals_pos + 1);
                std::string value = env_var.substr(equals_pos + 1);
                
                // Strip NLM_ prefix
                if (key.find('_') != std::string::npos) {
                    key = key.substr(key.find('_') + 1);
                }
                
                set(key, value, ConfigSource::Environment);
            }
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    double duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    
    if (pImpl) {
        pImpl->env_parse_time = duration_ms;
        pImpl->env_parse_count++;
    }
    
    return true;
}

bool OptimizedConfig::loadFromDefaults() {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Fast default configuration loading
    // Using predefined defaults with minimal overhead
    const std::unordered_map<std::string, std::string> defaults = {
        {"brain.neuron_count", "1000"},
        {"brain.region_count", "10"},
        {"brain.population_count", "5"},
        {"memory.working_memory_capacity", "100"},
        {"memory.episodic_memory_capacity", "1000"},
        {"neuromodulation.reward_scale", "1.0"},
        {"neuromodulation.curiosity_threshold", "0.5"},
        {"neuromodulation.noise_level", "0.1"},
        {"plasticity.stdp_learning_rate", "0.01"},
        {"plasticity.hebbian_learning_rate", "0.001"},
        {"prediction.confidence_threshold", "0.8"},
        {"prediction.planning_horizon", "10"},
        {"simulation.time_step", "0.001"},
        {"simulation.max_steps", "10000"}
    };
    
    for (const auto& [key, value] : defaults) {
        set(key, value, ConfigSource::Default);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    double duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    
    if (pImpl) {
        pImpl->default_load_time = duration_ms;
        pImpl->default_load_count++;
    }
    
    return true;
}

bool OptimizedConfig::loadWithInheritance(const std::vector<std::string>& filePaths) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Optimized inheritance with fast merging
    std::unordered_map<std::string, ConfigEntry> merged_config;
    
    // Process files in reverse order for inheritance
    for (auto it = filePaths.rbegin(); it != filePaths.rend(); ++it) {
        const std::string& filepath = *it;
        
        // Determine file type
        std::string extension = "." + filepath.substr(filepath.find_last_of('.') + 1);
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        std::unordered_map<std::string, ConfigValue> parsed_config;
        
        if (extension == ".json" || extension == ".cfg") {
            std::ifstream file(filepath);
            if (!file.is_open()) continue;
            
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();
            
            OptimizedJsonParser::ParseOptions options;
            options.enable_fast_path = true;
            parsed_config = OptimizedJsonParser::parse(content, options);
        } else if (extension == ".yaml" || extension == ".yml") {
            std::ifstream file(filepath);
            if (!file.is_open()) continue;
            
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();
            
            OptimizedYamlParser::ParseOptions options;
            options.enable_string_pooling = true;
            parsed_config = OptimizedYamlParser::parse(content, options);
        }
        
        // Merge with inheritance (later files override earlier ones)
        for (const auto& [key, value] : parsed_config) {
            merged_config[key] = ConfigEntry(key, value, ConfigSource::File);
        }
    }
    
    // Apply merged configuration
    clear();
    for (const auto& [key, entry] : merged_config) {
        set(key, entry.value, entry.source);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    double duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    
    if (pImpl) {
        pImpl->inheritance_load_time = duration_ms;
        pImpl->inheritance_load_count++;
    }
    
    return true;
}

// Template implementations for value access
template<typename T>
std::optional<T> OptimizedConfig::get(const std::string& key) const {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    std::optional<T> result;
    
    // Try fast path first
    if (pImpl && pImpl->fast_access_enabled) {
        auto it = pImpl->fast_path_cache.find(key);
        if (it != pImpl->fast_path_cache.end()) {
            result = std::get<T>(it->second);
            auto end_time = std::chrono::high_resolution_clock::now();
            double duration_ns = std::chrono::duration<double, std::nano>(end_time - start_time).count();
            pImpl->fast_access_hit_count++;
            pImpl->fast_access_hit_time += duration_ns;
            return result;
        }
    }
    
    // Fall back to standard access
    result = Config::get<T>(key);
    
    // Cache the result if caching is enabled
    if (result && pImpl && pImpl->cache_enabled) {
        auto it = pImpl->value_cache.find(key);
        if (it != pImpl->value_cache.end()) {
            auto end_time = std::chrono::high_resolution_clock::now();
            double duration_ns = std::chrono::duration<double, std::nano>(end_time - start_time).count();
            pImpl->value_cache_hit_count++;
            pImpl->value_cache_hit_time += duration_ns;
        }
        
        // Store in fast path cache if space available
        if (pImpl->fast_path_cache.size() < pImpl->max_fast_path_cache_size) {
            pImpl->fast_path_cache[key] = *result;
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    double duration_ns = std::chrono::duration<double, std::nano>(end_time - start_time).count();
    pImpl->total_access_time += duration_ns;
    pImpl->total_access_count++;
    
    return result;
}

template<typename T>
T OptimizedConfig::getOr(const std::string& key, const T& defaultValue) const {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Try fast path first
    if (pImpl && pImpl->fast_access_enabled) {
        auto it = pImpl->fast_path_cache.find(key);
        if (it != pImpl->fast_path_cache.end()) {
            try {
                T result = std::get<T>(it->second);
                auto end_time = std::chrono::high_resolution_clock::now();
                double duration_ns = std::chrono::duration<double, std::nano>(end_time - start_time).count();
                pImpl->fast_access_hit_count++;
                pImpl->fast_access_hit_time += duration_ns;
                return result;
            } catch (...) {
                // Fall back to standard access if type conversion fails
            }
        }
    }
    
    // Fall back to standard access
    T result = Config::getOr<T>(key, defaultValue);
    
    // Cache the result if caching is enabled
    if (pImpl && pImpl->cache_enabled) {
        auto it = pImpl->value_cache.find(key);
        if (it != pImpl->value_cache.end()) {
            auto end_time = std::chrono::high_resolution_clock::now();
            double duration_ns = std::chrono::duration<double, std::nano>(end_time - start_time).count();
            pImpl->value_cache_hit_count++;
            pImpl->value_cache_hit_time += duration_ns;
        }
        
        // Store in fast path cache if space available
        if (pImpl->fast_path_cache.size() < pImpl->max_fast_path_cache_size) {
            pImpl->fast_path_cache[key] = ConfigValue(result);
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    double duration_ns = std::chrono::duration<double, std::nano>(end_time - start_time).count();
    pImpl->total_access_time += duration_ns;
    pImpl->total_access_count++;
    
    return result;
}

template<typename T>
std::optional<T> OptimizedConfig::getNested(const std::string& keyPath) const {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    std::optional<T> result;
    
    // Optimized nested access with path caching
    if (pImpl && pImpl->fast_access_enabled) {
        auto it = pImpl->nested_path_cache.find(keyPath);
        if (it != pImpl->nested_path_cache.end()) {
            // Use cached path components
            result = Config::getNested<T>(keyPath);
            auto end_time = std::chrono::high_resolution_clock::now();
            double duration_ns = std::chrono::duration<double, std::nano>(end_time - start_time).count();
            pImpl->nested_access_hit_count++;
            pImpl->nested_access_hit_time += duration_ns;
            return result;
        }
    }
    
    // Fall back to standard access
    result = Config::getNested<T>(keyPath);
    
    // Cache the path if caching is enabled
    if (result && pImpl && pImpl->cache_enabled) {
        auto it = pImpl->nested_path_cache.find(keyPath);
        if (it != pImpl->nested_path_cache.end()) {
            auto end_time = std::chrono::high_resolution_clock::now();
            double duration_ns = std::chrono::duration<double, std::nano>(end_time - start_time).count();
            pImpl->nested_access_hit_count++;
            pImpl->nested_access_hit_time += duration_ns;
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    double duration_ns = std::chrono::duration<double, std::nano>(end_time - start_time).count();
    pImpl->total_access_time += duration_ns;
    pImpl->total_access_count++;
    
    return result;
}

void OptimizedConfig::enableCache(bool enable) {
    if (pImpl) {
        pImpl->cache_enabled = enable;
    }
}

void OptimizedConfig::enableFastAccess(bool enable) {
    if (pImpl) {
        pImpl->fast_access_enabled = enable;
    }
}

void OptimizedConfig::setValueCacheSize(size_t max_cached_values) {
    if (pImpl) {
        pImpl->max_value_cache_size = max_cached_values;
    }
}

void OptimizedConfig::setAccessPatternCacheSize(size_t max_cached_patterns) {
    if (pImpl) {
        pImpl->max_nested_path_cache_size = max_cached_patterns;
    }
}

OptimizedConfig::PerformanceStats OptimizedConfig::getPerformanceStats() const {
    PerformanceStats stats = {};
    
    if (pImpl) {
        stats.total_load_time_ms = pImpl->total_load_time;
        stats.total_access_time_ms = pImpl->total_access_time / 1000000.0; // Convert to ms
        stats.total_values_cached = pImpl->value_cache_hit_count;
        stats.total_patterns_cached = pImpl->nested_access_hit_count;
        stats.cache_hit_count = pImpl->value_cache_hit_count + pImpl->fast_access_hit_count + pImpl->nested_access_hit_count;
        stats.cache_miss_count = pImpl->total_access_count - stats.cache_hit_count;
        
        if (stats.cache_hit_count + stats.cache_miss_count > 0) {
            stats.cache_hit_ratio = static_cast<double>(stats.cache_hit_count) / 
                                   (stats.cache_hit_count + stats.cache_miss_count);
        }
    }
    
    return stats;
}

std::string OptimizedConfig::generatePerformanceReport() const {
    std::stringstream ss;
    ss << "=== Optimized Config Performance Report ===\n";
    
    auto stats = getPerformanceStats();
    
    ss << "Performance Statistics:\n";
    ss << "Total load time: " << std::fixed << std::setprecision(3) << stats.total_load_time_ms << " ms\n";
    ss << "Total access time: " << std::fixed << std::setprecision(3) << stats.total_access_time_ms << " ms\n";
    ss << "Total accesses: " << stats.cache_hit_count + stats.cache_miss_count << "\n";
    ss << "Cache hits: " << stats.cache_hit_count << "\n";
    ss << "Cache misses: " << stats.cache_miss_count << "\n";
    ss << "Cache hit ratio: " << std::fixed << std::setprecision(3) << stats.cache_hit_ratio * 100 << "%\n";
    
    ss << "\n=== End of Report ===\n";
    return ss.str();
}

// OptimizedJsonParser Implementation
std::unordered_map<std::string, ConfigValue> OptimizedJsonParser::parse(const std::string& content, 
                                                                        const ParseOptions& options) {
    std::unordered_map<std::string, ConfigValue> config;
    
    if (options.enable_fast_path && content.size() < 1024) { // Small content threshold
        // Use fast parsing for small content
        if (parseFast(content, config)) {
            return config;
        }
    }
    
    // Use memory-efficient parsing for large content
    if (options.enable_vector_optimization && content.size() > 1024) {
        parseMemoryEfficient(content, config);
    } else {
        // Standard parsing
        parseObject(content, 0, config);
    }
    
    return config;
}

std::unordered_map<std::string, ConfigValue> OptimizedJsonParser::parseFile(const std::string& filepath,
                                                                             const ParseOptions& options) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return {};
    }
    
    // Read file in chunks for better performance
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    return parse(content, options);
}

std::unordered_map<std::string, ConfigValue> OptimizedJsonParser::parseStream(std::istream& stream,
                                                                               const ParseOptions& options) {
    std::stringstream buffer;
    buffer << stream.rdbuf();
    std::string content = buffer.str();
    
    return parse(content, options);
}

bool OptimizedJsonParser::parseFast(const std::string& content, std::unordered_map<std::string, ConfigValue>& config) {
    // Fast parsing for simple key-value pairs (no nesting)
    size_t pos = 0;
    
    while (pos < content.size()) {
        // Skip whitespace
        while (pos < content.size() && std::isspace(content[pos])) {
            pos++;
        }
        
        if (pos >= content.size()) break;
        
        // Parse key (simple string without escaping)
        size_t key_start = pos;
        while (pos < content.size() && content[pos] != '"' && !std::isspace(content[pos])) {
            pos++;
        }
        
        if (pos >= content.size() || content[pos] != '"') {
            break;
        }
        
        std::string key = content.substr(key_start, pos - key_start);
        pos++; // Skip opening quote
        
        // Skip whitespace
        while (pos < content.size() && std::isspace(content[pos])) {
            pos++;
        }
        
        if (pos >= content.size() || content[pos] != ':') {
            break;
        }
        
        pos++; // Skip colon
        
        // Skip whitespace
        while (pos < content.size() && std::isspace(content[pos])) {
            pos++;
        }
        
        // Parse value (simple types)
        if (pos >= content.size()) break;
        
        if (content[pos] == '"') {
            // String value
            size_t value_start = ++pos;
            while (pos < content.size() && content[pos] != '"') {
                pos++;
            }
            std::string value = content.substr(value_start, pos - value_start);
            config[key] = value;
        } else if (content[pos] == 't' || content[pos] == 'f') {
            // Boolean value
            if (pos + 4 <= content.size() && content.substr(pos, 4) == "true") {
                config[key] = true;
                pos += 4;
            } else if (pos + 5 <= content.size() && content.substr(pos, 5) == "false") {
                config[key] = false;
                pos += 5;
            } else {
                break;
            }
        } else if (content[pos] == '-' || std::isdigit(content[pos])) {
            // Number value
            size_t value_start = pos;
            while (pos < content.size() && (std::isdigit(content[pos]) || content[pos] == '.' || content[pos] == '-' || content[pos] == 'e' || content[pos] == 'E')) {
                pos++;
            }
            std::string value = content.substr(value_start, pos - value_start);
            
            // Try to determine type
            try {
                if (value.find('.') != std::string::npos || value.find('e') != std::string::npos || value.find('E') != std::string::npos) {
                    config[key] = std::stod(value);
                } else {
                    config[key] = std::stoi(value);
                }
            } catch (...) {
                config[key] = value;
            }
        } else {
            break;
        }
        
        // Skip whitespace
        while (pos < content.size() && std::isspace(content[pos])) {
            pos++;
        }
        
        // Skip comma
        if (pos < content.size() && content[pos] == ',') {
            pos++;
        }
    }
    
    return !config.empty();
}

bool OptimizedJsonParser::parseMemoryEfficient(const std::string& content, std::unordered_map<std::string, ConfigValue>& config) {
    // Memory-efficient parsing for large content
    // Use streaming approach and reserve memory
    config.reserve(content.size() / 50); // Estimate 50 chars per key-value pair
    
    return parseObject(content, 0, config);
}

ConfigValue OptimizedJsonParser::convertValue(const std::string& str, ConfigValue::variant_type type) {
    try {
        switch (type) {
            case ConfigValue::variant_type::STRING:
                return str;
            case ConfigValue::variant_type::INT:
                return std::stoi(str);
            case ConfigValue::variant_type::INT64:
                return std::stoll(str);
            case ConfigValue::variant_type::DOUBLE:
                return std::stod(str);
            case ConfigValue::variant_type::BOOL:
                return (str == "true" || str == "true");
            case ConfigValue::variant_type::VECTOR_INT:
                // Parse vector of integers (simplified)
                std::vector<int> int_vec;
                size_t pos = 0;
                while (pos < str.size()) {
                    size_t comma_pos = str.find(',', pos);
                    std::string num = str.substr(pos, (comma_pos == std::string::npos) ? std::string::npos : comma_pos - pos);
                    try {
                        int_vec.push_back(std::stoi(num));
                    } catch (...) {
                        break;
                    }
                    pos = (comma_pos == std::string::npos) ? str.size() : comma_pos + 1;
                }
                return int_vec;
            case ConfigValue::variant_type::VECTOR_DOUBLE:
                // Parse vector of doubles (simplified)
                std::vector<double> double_vec;
                size_t pos = 0;
                while (pos < str.size()) {
                    size_t comma_pos = str.find(',', pos);
                    std::string num = str.substr(pos, (comma_pos == std::string::npos) ? std::string::npos : comma_pos - pos);
                    try {
                        double_vec.push_back(std::stod(num));
                    } catch (...) {
                        break;
                    }
                    pos = (comma_pos == std::string::npos) ? str.size() : comma_pos + 1;
                }
                return double_vec;
            case ConfigValue::variant_type::VECTOR_STRING:
                // Parse vector of strings (simplified)
                std::vector<std::string> string_vec;
                size_t pos = 0;
                while (pos < str.size()) {
                    size_t comma_pos = str.find(',', pos);
                    std::string item = str.substr(pos, (comma_pos == std::string::npos) ? std::string::npos : comma_pos - pos);
                    // Remove quotes
                    if (item.size() >= 2 && item.front() == '"' && item.back() == '"') {
                        item = item.substr(1, item.size() - 2);
                    }
                    string_vec.push_back(item);
                    pos = (comma_pos == std::string::npos) ? str.size() : comma_pos + 1;
                }
                return string_vec;
            default:
                return str;
        }
    } catch (...) {
        return str;
    }
}

bool OptimizedJsonParser::tryConvertValue(const std::string& str, ConfigValue& target) {
    try {
        if (str == "true" || str == "false") {
            target = (str == "true");
            return true;
        } else if (str.find('.') != std::string::npos || str.find('e') != std::string::npos || str.find('E') != std::string::npos) {
            target = std::stod(str);
            return true;
        } else {
            target = std::stoi(str);
            return true;
        }
    } catch (...) {
        target = str;
        return false;
    }
}

// Additional implementation details would continue...

} // namespace nlm
