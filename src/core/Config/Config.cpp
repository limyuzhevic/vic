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

bool Config::loadFromFile(const std::string& filepath) {
    // Check file extension for format
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json" || ext == ".jsonc") {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAML(filepath);
    } else {
        // Default to simple key=value format for backward compatibility
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Use Python for JSON parsing if available (lightweight)
        // For now, implement a simple JSON parser
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        return parseJSON(content);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse JSON file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    // YAML parsing - can be implemented later
    NLM_LOG_WARNING("YAML parsing not yet implemented, falling back to simple format");
    return loadFromSimpleFormat(filepath);
}

bool Config::parseJSON(const std::string& json) {
    // Simple JSON parser for configuration objects
    // Supports: {"key1": "value1", "key2": 123, "key3": true, "key4": [1, 2, 3]}
    
    // Remove whitespace for simpler parsing
    std::string cleaned;
    for (char c : json) {
        if (!std::isspace(c)) {
            cleaned += c;
        }
    }
    
    if (cleaned.empty() || cleaned[0] != '{' || cleaned.back() != '}') {
        NLM_LOG_ERROR("Invalid JSON format: must be an object");
        return false;
    }
    
    // Extract content between braces
    std::string content = cleaned.substr(1, cleaned.size() - 2);
    
    // Simple key-value pair parsing (assumes no nested objects for now)
    size_t pos = 0;
    while (pos < content.size()) {
        // Find next key
        size_t keyStart = content.find('"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        // Find value after colon
        size_t colonPos = content.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        size_t valueStart = content.find_first_not_of(' ', colonPos + 1);
        if (valueStart == std::string::npos) break;
        
        // Find end of value (either comma or closing brace)
        size_t valueEnd = content.find_first_of(',}', valueStart);
        if (valueEnd == std::string::npos) valueEnd = content.size();
        
        std::string valueStr = content.substr(valueStart, valueEnd - valueStart);
        
        // Parse value based on type
        ConfigValue configValue;
        if (valueStr == "true") {
            configValue = true;
        } else if (valueStr == "false") {
            configValue = false;
        } else if (valueStr[0] == '"' && valueStr.back() == '"') {
            // Remove quotes
            configValue = valueStr.substr(1, valueStr.size() - 2);
        } else if (std::isdigit(valueStr[0]) || (valueStr[0] == '-' && std::isdigit(valueStr[1]))) {
            // Try to parse as number
            try {
                if (valueStr.find('.') != std::string::npos) {
                    configValue = std::stod(valueStr);
                } else {
                    configValue = std::stoll(valueStr);
                }
            } catch (...) {
                configValue = valueStr; // Fall back to string
            }
        } else {
            configValue = valueStr;
        }
        
        set(key, configValue, ConfigSource::File);
        
        // Move to next pair
        pos = valueEnd;
        if (pos < content.size() && content[pos] == ',') {
            pos++;
        }
    }
    
    return true;
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
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
    
    // Save in JSON format
    file << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        
        // Save value based on type
        std::visit([&file](const auto& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << v;
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                file << v;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (v ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << v << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < v.size(); ++i) {
                    if (i) file << ", ";
                    file << v[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < v.size(); ++i) {
                    if (i) file << ", ";
                    file << v[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < v.size(); ++i) {
                    if (i) file << ", ";
                    file << "\"" << v[i] << "\"";
                }
                file << "]";
            }
        }, entry.value);
        
        file << "\n";
    }
    
    file << "}\n";
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key before setting
    if (!validateKey(key)) {
        NLM_LOG_WARNING("Invalid configuration key: " + key);
        return;
    }
    
    // Validate value for known keys
    if (!validateValue(key, value)) {
        NLM_LOG_WARNING("Invalid value for configuration key: " + key);
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

bool Config::validateKey(const std::string& key) const {
    if (key.empty()) return false;
    if (key.find_first_of(" \"\\`')[]{}<>!@#$%^&*()+=;:,.?/\\|") != std::string::npos) return false;
    
    // Check for reserved prefixes
    std::string lowerKey = toLower(key);
    if (lowerKey.find("reserved_") == 0) return false;
    
    return true;
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    std::string lowerKey = toLower(key);
    
    if (lowerKey == "brain.neuron_count" || lowerKey == "neuron_count") {
        if (!std::holds_alternative<int64_t>(value) && !std::holds_alternative<int>(value)) return false;
        int64_t neurons = std::holds_alternative<int64_t>(value) ? 
            std::get<int64_t>(value) : std::get<int>(value);
        return neurons > 0 && neurons <= 1000000;
    }
    
    if (lowerKey == "brain.synapse_density" || lowerKey == "synapse_density") {
        if (!std::holds_alternative<double>(value) && !std::holds_alternative<float>(value)) return false;
        double density = std::holds_alternative<double>(value) ? 
            std::get<double>(value) : std::get<float>(value);
        return density >= 0.0 && density <= 1.0;
    }
    
    if (lowerKey == "brain.region_count" || lowerKey == "region_count") {
        if (!std::holds_alternative<int64_t>(value) && !std::holds_alternative<int>(value)) return false;
        int64_t regions = std::holds_alternative<int64_t>(value) ? 
            std::get<int64_t>(value) : std::get<int>(value);
        return regions > 0 && regions <= 100;
    }
    
    if (lowerKey == "brain.connection_probability" || lowerKey == "connection_probability") {
        if (!std::holds_alternative<double>(value) && !std::holds_alternative<float>(value)) return false;
        double prob = std::holds_alternative<double>(value) ? 
            std::get<double>(value) : std::get<float>(value);
        return prob >= 0.0 && prob <= 1.0;
    }
    
    if (lowerKey == "plasticity.stdp.learning_rate") {
        if (!std::holds_alternative<double>(value) && !std::holds_alternative<float>(value)) return false;
        double rate = std::holds_alternative<double>(value) ? 
            std::get<double>(value) : std::get<float>(value);
        return rate >= 0.0 && rate <= 1.0;
    }
    
    if (lowerKey == "simulation.timestep") {
        if (!std::holds_alternative<double>(value) && !std::holds_alternative<float>(value)) return false;
        double timestep = std::holds_alternative<double>(value) ? 
            std::get<double>(value) : std::get<float>(value);
        return timestep > 0.0 && timestep <= 1.0;
    }
    
    return true; // Accept any value for unknown keys
}

std::string Config::generateConfigTemplate(const std::string& category) const {
    std::ostringstream oss;
    oss << "# NLM Configuration Template - " << category << std::endl;
    oss << "# Copy this to config.json and modify values as needed" << std::endl;
    oss << "#" << std::endl;
    
    if (category == "simple" || category.empty()) {
        oss << "{\n";
        oss << "  \"brain.neuron_count\": 1000,\n";
        oss << "  \"brain.synapse_density\": 0.1,\n";
        oss << "  \"brain.region_count\": 2,\n";
        oss << "  \"brain.connection_probability\": 0.05,\n";
        oss << "  \"plasticity.stdp.enable\": true,\n";
        oss << "  \"plasticity.stdp.learning_rate\": 0.001,\n";
        oss << "  \"neuromod.curiosity.enable\": true,\n";
        oss << "  \"neuromod.novelty.enable\": true,\n";
        oss << "  \"development.enable\": true,\n";
        oss << "  \"simulation.timestep\": 0.001,\n";
        oss << "  \"simulation.max_steps\": 1000\n";
        oss << "}" << std::endl;
    } else if (category == "fast") {
        oss << "{\n";
        oss << "  \"brain.neuron_count\": 500,\n";
        oss << "  \"brain.synapse_density\": 0.05,\n";
        oss << "  \"brain.region_count\": 1,\n";
        oss << "  \"plasticity.stdp.learning_rate\": 0.0001,\n";
        oss << "  \"simulation.timestep\": 0.001,\n";
        oss << "  \"simulation.max_steps\": 500\n";
        oss << "}" << std::endl;
    } else if (category == "learning") {
        oss << "{\n";
        oss << "  \"brain.neuron_count\": 2000,\n";
        oss << "  \"brain.synapse_density\": 0.2,\n";
        oss << "  \"brain.region_count\": 3,\n";
        oss << "  \"plasticity.stdp.learning_rate\": 0.01,\n";
        oss << "  \"plasticity.hebbian.enable\": true,\n";
        oss << "  \"neuromod.curiosity.enable\": true,\n";
        oss << "  \"neuromod.novelty.enable\": true,\n";
        oss << "  \"development.enable\": true,\n";
        oss << "  \"development.critical_period_length\": 200,\n";
        oss << "  \"memory.working.enable\": true,\n";
        oss << "  \"memory.episodic.enable\": true,\n";
        oss << "  \"simulation.timestep\": 0.001,\n";
        oss << "  \"simulation.max_steps\": 5000\n";
        oss << "}" << std::endl;
    }
    
    return oss.str();
}

void Config::exportConfigForExport(const std::string& format, const std::string& filename) const {
    if (format == "json") {
        saveToFile(filename + ".json");
    } else if (format == "yaml" || format == "yml") {
        // YAML export not implemented yet
        NLM_LOG_WARNING("YAML export not yet implemented");
    } else if (format == "ini" || format == "cfg") {
        saveToSimpleFormat(filename + ".ini");
    } else {
        NLM_LOG_ERROR("Unsupported export format: " + format);
    }
}

void Config::saveToSimpleFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for simple format export: " + filepath);
        return;
    }
    
    file << "# NLM Configuration File (INI format)" << std::endl;
    file << "# Generated by NLM Config" << std::endl;
    file << "#" << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << std::endl;
        std::visit([&file](const auto& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << v << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < v.size(); ++i) {
                    if (i) file << ", ";
                    file << "\"" << v[i] << "\"";
                }
                file << "]";
            } else {
                file << entry.key << " = " << v;
            }
        }, entry.value);
        file << std::endl << std::endl;
    }
    
    file.close();
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

} // namespace nlm
