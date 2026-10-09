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

// Load from file (JSON/YAML format with fallback to simple format)
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Could not open config file: " + filepath);
        return false;
    }
    
    // Try to parse as JSON first
    file.clear();
    file.seekg(0, std::ios::beg);
    std::string firstLine;
    std::getline(file, firstLine);
    
    if (firstLine.find('{') != std::string::npos || firstLine.find('[') != std::string::npos) {
        // Try JSON parsing
        file.clear();
        file.seekg(0, std::ios::beg);
        
        // Simple JSON parsing - in a real implementation, use a proper JSON library
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        // Basic JSON parsing - extract key=value pairs
        // This is a simplified implementation - production code should use a JSON library
        size_t pos = 0;
        while ((pos = content.find('"', pos)) != std::string::npos) {
            size_t keyStart = pos + 1;
            pos = content.find('"', keyStart);
            if (pos == std::string::npos) break;
            
            std::string key = content.substr(keyStart, pos - keyStart);
            
            // Find value after :
            size_t colonPos = content.find(':', pos);
            if (colonPos == std::string::npos) break;
            
            size_t valueStart = colonPos + 1;
            // Skip whitespace
            while (valueStart < content.size() && (content[valueStart] == ' ' || content[valueStart] == '\t' || content[valueStart] == '\n')) {
                valueStart++;
            }
            
            if (content[valueStart] == '"') {
                size_t valueEnd = content.find('"', valueStart + 1);
                if (valueEnd != std::string::npos) {
                    std::string value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
                    set(key, value, ConfigSource::File);
                    NLM_LOG_DEBUG("Parsed JSON config: " + key + " = " + value);
                }
            } else if (content[valueStart] == '{' || content[valueStart] == '[') {
                // Skip complex JSON values
                continue;
            } else {
                // Try to parse as number or boolean
                size_t valueEnd = content.find_first_of(",}", valueStart);
                if (valueEnd != std::string::npos) {
                    std::string valueStr = content.substr(valueStart, valueEnd - valueStart);
                    // Try to convert to double first
                    try {
                        double doubleVal = std::stod(valueStr);
                        set(key, doubleVal, ConfigSource::File);
                    } catch (...) {
                        // Try boolean
                        if (valueStr == "true" || valueStr == "false") {
                            bool boolVal = (valueStr == "true");
                            set(key, boolVal, ConfigSource::File);
                        } else {
                            set(key, valueStr, ConfigSource::File);
                        }
                    }
                    NLM_LOG_DEBUG("Parsed JSON config: " + key + " = " + valueStr);
                }
            }
            
            pos = valueStart + 1;
        }
        
        if (!pImpl->entries.empty()) {
            NLM_LOG_INFO("Successfully parsed JSON configuration from: " + filepath);
            return true;
        }
    }
    
    // Fallback to simple key=value format
    NLM_LOG_DEBUG("Falling back to simple key=value format for: " + filepath);
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
            NLM_LOG_DEBUG("Parsed simple config: " + key + " = " + value);
        }
    }
    
    NLM_LOG_INFO("Successfully parsed configuration from: " + filepath + " (" + 
                 std::to_string(pImpl->entries.size()) + " entries)");
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
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Convert ConfigValue to string representation
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else {
                file << arg;  // int, int64_t, double, etc.
            }
        }, entry.value);
        file << "\n";
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
    // Validate key
    if (key.empty()) {
        return;  // Reject empty keys
    }
    
    // Validate that key doesn't contain path separators
    if (key.find('/') != std::string::npos || key.find('\\') != std::string::npos) {
        return;  // Reject keys with path separators
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

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        return;  // Reject empty keys
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        return;  // Reject empty keys
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        return;  // Reject empty keys
    }
    
    // Validate reasonable value ranges for common parameters
    if (key == "random_seed" && (value < 0.0 || value > 1000000.0)) {
        return;  // Reject unreasonable seeds
    }
    if (key == "simulation_timestep" && (value <= 0.0 || value > 1.0)) {
        return;  // Reject unreasonable timesteps
    }
    if (key == "neuron_count" && (value < 1.0 || value > 1000000.0)) {
        return;  // Reject unreasonable neuron counts
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        return;  // Reject empty keys
    }
    
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
