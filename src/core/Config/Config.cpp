#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <variant>
#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>
#include <algorithm>

// Forward declaration for nlohmann JSON support
namespace nlohmann {
    template<typename T>
    class basic_json;
    using json = basic_json<nullptr_t>;
}

namespace nlm {

// Helper function to convert string to appropriate type
static ConfigValue stringToConfigValue(const std::string& str) {
    // Try to parse as integer
    try {
        size_t pos;
        long long intVal = std::stoll(str, &pos);
        if (pos == str.size()) {
            return intVal;
        }
    } catch (...) {}
    
    // Try to parse as floating point
    try {
        size_t pos;
        double floatVal = std::stod(str, &pos);
        if (pos == str.size()) {
            return floatVal;
        }
    } catch (...) {}
    
    // Try to parse as boolean
    std::string lowerStr = str;
    for (char& c : lowerStr) {
        c = std::tolower(c);
    }
    if (lowerStr == "true" || lowerStr == "false") {
        return lowerStr == "true";
    }
    
    // Return as string
    return str;
}

// Helper struct for hash-based pair lookup
struct PairHash {
    std::size_t operator()(const std::pair<uint64_t, uint64_t>& p) const {
        return std::hash<uint64_t>()(p.first) ^ std::hash<uint64_t>()(p.second);
    }
};

// Helper function to parse JSON-like format (simplified version)
static bool parseJSONLike(const std::string& content, std::vector<ConfigEntry>& entries) {
    // Simple JSON parser for configuration files
    // Supports: {"key1": "value1", "key2": 42, "key3": true, "key4": [1, 2, 3]}
    
    size_t pos = 0;
    
    // Skip whitespace and opening brace
    while (pos < content.size() && std::isspace(content[pos])) pos++;
    if (pos >= content.size() || content[pos] != '{') {
        return false; // Not a JSON object
    }
    pos++;
    
    while (pos < content.size()) {
        // Skip whitespace
        while (pos < content.size() && std::isspace(content[pos])) pos++;
        
        // Check for closing brace
        if (pos >= content.size() || content[pos] == '}') {
            break;
        }
        
        // Parse key
        std::string key;
        if (content[pos] == '"') {
            pos++;
            while (pos < content.size() && content[pos] != '"') {
                if (content[pos] == '\\' && pos + 1 < content.size()) {
                    pos++; // Skip escaped character
                }
                key += content[pos];
                pos++;
            }
            if (pos < content.size() && content[pos] == '"') {
                pos++;
            }
        } else {
            // Unquoted key (simple parser)
            while (pos < content.size() && content[pos] != ':' && !std::isspace(content[pos])) {
                key += content[pos];
                pos++;
            }
        }
        
        if (key.empty()) {
            break;
        }
        
        // Skip whitespace and colon
        while (pos < content.size() && std::isspace(content[pos])) pos++;
        if (pos >= content.size() || content[pos] != ':') {
            break;
        }
        pos++;
        
        // Skip whitespace
        while (pos < content.size() && std::isspace(content[pos])) pos++;
        
        // Parse value
        ConfigValue value;
        if (pos < content.size() && content[pos] == '"') {
            // String value
            pos++;
            std::string strValue;
            while (pos < content.size() && content[pos] != '"') {
                if (content[pos] == '\\' && pos + 1 < content.size()) {
                    pos++; // Skip escaped character
                }
                strValue += content[pos];
                pos++;
            }
            if (pos < content.size() && content[pos] == '"') {
                pos++;
            }
            value = strValue;
        } else if (pos < content.size() && (std::isdigit(content[pos]) || content[pos] == '-')) {
            // Number value
            std::string numStr;
            while (pos < content.size() && (std::isdigit(content[pos]) || content[pos] == '.' || content[pos] == '-')) {
                numStr += content[pos];
                pos++;
            }
            value = stringToConfigValue(numStr);
        } else if (pos < content.size() && content[pos] == 't' && 
                   pos + 3 < content.size() && content.substr(pos, 4) == "true") {
            // Boolean true
            value = true;
            pos += 4;
        } else if (pos < content.size() && content[pos] == 'f' && 
                   pos + 4 < content.size() && content.substr(pos, 5) == "false") {
            // Boolean false
            value = false;
            pos += 5;
        } else if (pos < content.size() && content[pos] == '[') {
            // Array value (simplified)
            pos++; // Skip '['
            while (pos < content.size() && content[pos] != ']') {
                // Skip whitespace
                while (pos < content.size() && std::isspace(content[pos])) pos++;
                
                if (pos < content.size() && content[pos] == '"') {
                    // String element
                    pos++;
                    std::string strElement;
                    while (pos < content.size() && content[pos] != '"') {
                        if (content[pos] == '\\' && pos + 1 < content.size()) {
                            pos++; // Skip escaped character
                        }
                        strElement += content[pos];
                        pos++;
                    }
                    if (pos < content.size() && content[pos] == '"') {
                        pos++;
                    }
                } else if (pos < content.size() && (std::isdigit(content[pos]) || content[pos] == '-')) {
                    // Number element
                    std::string numElement;
                    while (pos < content.size() && (std::isdigit(content[pos]) || content[pos] == '.' || content[pos] == '-')) {
                        numElement += content[pos];
                        pos++;
                    }
                } else {
                    // Skip unknown element
                    while (pos < content.size() && content[pos] != ',' && content[pos] != ']') {
                        pos++;
                    }
                }
                
                // Skip comma
                while (pos < content.size() && std::isspace(content[pos])) pos++;
                if (pos < content.size() && content[pos] == ',') {
                    pos++;
                }
            }
            if (pos < content.size() && content[pos] == ']') {
                pos++;
            }
            // For now, we'll just parse the JSON, not store arrays
            // A full implementation would create a vector type
            value = std::string("[...]"); // Placeholder for array
        } else {
            // Unknown value type, skip
            while (pos < content.size() && content[pos] != ',' && content[pos] != '}') {
                pos++;
            }
        }
        
        // Add entry if we successfully parsed
        if (!key.empty()) {
            entries.emplace_back(key, value, ConfigSource::File);
        }
        
        // Skip comma and whitespace
        while (pos < content.size() && std::isspace(content[pos])) pos++;
        if (pos < content.size() && content[pos] == ',') {
            pos++;
        }
    }
    
    return true;
}

bool Config::loadFromFile(const std::string& filepath) {
    NLM_LOG_INFO("Loading configuration from: " + filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Configuration file not found: " + filepath + ". Using default configuration.");
        return false;
    }
    
    // Read the entire file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Try to parse as JSON first
    pImpl->entries.clear();
    bool parsedAsJSON = false;
    
    // Check if content looks like JSON (starts with { and contains :)
    if (content.find('{') != std::string::npos && content.find(':') != std::string::npos) {
        if (parseJSONLike(content, pImpl->entries)) {
            parsedAsJSON = true;
            NLM_LOG_INFO("Configuration parsed as JSON format.");
        }
    }
    
    // If not JSON, try the original key=value format
    if (!parsedAsJSON) {
        NLM_LOG_INFO("Configuration appears to be in key=value format. Parsing with legacy parser.");
        
        file.clear();
        file.seekg(0, std::ios::beg);
        
        std::string line;
        size_t lineNumber = 0;
        while (std::getline(file, line)) {
            lineNumber++;
            
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
                NLM_LOG_INFO("Loaded config entry: " + key + " = " + value);
            } else {
                NLM_LOG_WARNING("Skipping malformed config line " + std::to_string(lineNumber) + ": " + line);
            }
        }
    }
    
    NLM_LOG_INFO("Configuration loaded successfully. " + std::to_string(pImpl->entries.size()) + " entries.");
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save configuration to file: " + filepath);
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
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

// Add default runtime configuration options for debug and instrumentation
void Config::addRuntimeDefaults() {
    // Debug and instrumentation options
    set("debug_enabled", false, ConfigSource::Default);
    set("debug_step_logging", false, ConfigSource::Default);
    set("debug_memory_tracking", false, ConfigSource::Default);
    set("debug_performance_monitoring", false, ConfigSource::Default);
    set("debug_neuron_profiling", false, ConfigSource::Default);
    set("debug_plasticity_tracking", false, ConfigSource::Default);
    set("debug_checkpoint_validation", true, ConfigSource::Default);
    
    // Performance monitoring options
    set("performance_metrics_enabled", false, ConfigSource::Default);
    set("performance_sampling_rate", 0.01f, ConfigSource::Default); // Sample 1% of events
    set("performance_track_neuron_states", false, ConfigSource::Default);
    set("performance_track_synapse_states", false, ConfigSource::Default);
    set("performance_track_memory_usage", false, ConfigSource::Default);
    set("performance_track_cpu_usage", false, ConfigSource::Default);
    
    // Visualization and output options
    set("visualization_enabled", false, ConfigSource::Default);
    set("visualization_update_rate", 30.240f, ConfigSource::Default); // Hz
    set("visualization_color_scheme", std::string("default"), ConfigSource::Default);
    set("visualization_show_neurons", true, ConfigSource::Default);
    set("visualization_show_synapses", true, ConfigSource::Default);
    set("visualization_show_plasticity", false, ConfigSource::Default);
    
    // Logging and output options
    set("log_level", std::string("INFO"), ConfigSource::Default);
    set("log_to_file", false, ConfigSource::Default);
    set("log_filename", std::string("nlm.log"), ConfigSource::Default);
    set("log_rotation_enabled", true, ConfigSource::Default);
    set("log_rotation_max_size_mb", 100.0f, ConfigSource::Default);
    set("log_structured", true, ConfigSource::Default); // Structured logging with context
    
    // Recording and data collection options
    set("recorder_enabled", false, ConfigSource::Default);
    set("recorder_output_path", std::string("./recordings"), ConfigSource::Default);
    set("recorder_max_recordings", 100, ConfigSource::Default);
    set("recorder_compress_recordings", true, ConfigSource::Default);
    set("recorder_record_brain_state", true, ConfigSource::Default);
    set("recorder_record_sensor_data", false, ConfigSource::Default);
    set("recorder_record_action_data", true, ConfigSource::Default);
    
    // Development and testing options
    set("development_mode", false, ConfigSource::Default);
    set("skip_validation", false, ConfigSource::Default);
    set("allow_experimental_features", false, ConfigSource::Default);
    set("enable_debug_networks", false, ConfigSource::Default); // Use smaller networks for faster debugging
    set("debug_network_size", 100, ConfigSource::Default); // For debug networks
    set("stress_test_mode", false, ConfigSource::Default);
    set("enable_time_measurement", true, ConfigSource::Default);
    
    // Memory and resource management options
    set("memory_pool_enabled", true, ConfigSource::Default);
    set("memory_pool_size_neuron_states", 10000, ConfigSource::Default);
    set("memory_pool_size_synapse_states", 50000, ConfigSource::Default);
    set("memory_pool_size_working_memory", 1000, ConfigSource::Default);
    set("memory_pool_size_episodic_memory", 10000, ConfigSource::Default);
    set("gc_enable_automatic", true, ConfigSource::Default);
    set("gc_collect_frequency_steps", 1000, ConfigSource::Default);
    
    // Advanced plasticity options
    set("plasticity_stdp_enabled", true, ConfigSource::Default);
    set("plasticity_hebbian_enabled", true, ConfigSource::Default);
    set("plasticity_reward_modulated_enabled", false, ConfigSource::Default);
    set("plasticity_structural_enabled", true, ConfigSource::Default);
    set("plasticity_structural_synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("plasticity_structural_pruning_rate", 0.00001f, ConfigSource::Default);
    set("plasticity_time_constant", 20.0f, ConfigSource::Default); // For STDP
    
    // Novelty and curiosity options
    set("novelty_enabled", true, ConfigSource::Default);
    set("curiosity_enabled", true, ConfigSource::Default);
    set("prediction_error_enabled", true, ConfigSource::Default);
    set("novelty_threshold", 0.5f, ConfigSource::Default);
    set("curiosity_strength", 1.0f, ConfigSource::Default);
    
    // Attention and cognitive options
    set("attention_enabled", true, ConfigSource::Default);
    set("concept_formation_enabled", true, ConfigSource::Default);
    set("neural_planner_enabled", true, ConfigSource::Default);
    set("attention_inhibition_strength", 0.5f, ConfigSource::Default);
    set("attention_excitation_strength", 1.5f, ConfigSource::Default);
    
    // Development and maturation options
    set("development_enabled", true, ConfigSource::Default);
    set("development_maturation_rate", 0.01f, ConfigSource::Default);
    set("development_critical_period_length", 100, ConfigSource::Default);
    set("development_plasticity_decay", 0.1f, ConfigSource::Default);
    
    // Checkpoint and persistence options
    set("checkpoint_enabled", true, ConfigSource::Default);
    set("checkpoint_interval_steps", 1000, ConfigSource::Default);
    set("checkpoint_max_size_mb", 1000.0f, ConfigSource::Default);
    set("checkpoint_compression_enabled", true, ConfigSource::Default);
    set("checkpoint_validation_enabled", true, ConfigSource::Default);
    set("checkpoint_auto_cleanup", true, ConfigSource::Default);
    
    // Sensor and motor options
    set("sensor_data_enabled", true, ConfigSource::Default);
    set("motor_output_enabled", true, ConfigSource::Default);
    set("action_selection_algorithm", std::string("simple"), ConfigSource::Default);
    set("reward_processing_enabled", true, ConfigSource::Default);
    
    // Advanced debugging options
    set("debug_break_on_error", false, ConfigSource::Default);
    set("debug_validate_connectivity", true, ConfigSource::Default);
    set("debug_validate_plasticity", true, ConfigSource::Default);
    set("debug_validate_memory_systems", true, ConfigSource::Default);
    set("debug_export_debug_data", false, ConfigSource::Default);
    set("debug_export_debug_path", std::string("./debug_output"), ConfigSource::Default);
    
    // Performance optimization options
    set("optimization_enable_simd", true, ConfigSource::Default);
    set("optimization_enable_threads", false, ConfigSource::Default);
    set("optimization_thread_count", 0, ConfigSource::Default); // 0 = auto-detect
    set("optimization_enable_vectorization", true, ConfigSource::Default);
    set("optimization_enable_cache_optimization", true, ConfigSource::Default);
    
    // Testing and simulation options
    set("simulation_step_limit", 0, ConfigSource::Default); // 0 = unlimited
    set("simulation_time_limit", 0.0f, ConfigSource::Default); // 0.0 = unlimited
    set("enable_hardware_profiling", false, ConfigSource::Default);
    set("enable_profiling", false, ConfigSource::Default);
    set("profiling_output_file", std::string("profile.txt"), ConfigSource::Default);
    set("profiling_sampling_rate", 0.1f, ConfigSource::Default); // Sample 10% of events
}
}} // namespace nlm