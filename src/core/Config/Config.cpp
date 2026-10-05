#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <ctime>
#include <iomanip>

// JSON and YAML support
namespace {

// Helper function to convert string to lower case
std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Check if file extension is JSON
bool isJSONFile(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string ext = path.extension().string();
    if (ext.empty()) return false;
    ext = toLower(ext.substr(1)); // Remove dot and convert to lower
    return ext == "json";
}

// Check if file extension is YAML
bool isYAMLFile(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string ext = path.extension().string();
    if (ext.empty()) return false;
    ext = toLower(ext.substr(1)); // Remove dot and convert to lower
    return ext == "yaml" || ext == "yml";
}

// Check if file extension is CFG
bool isCFGFile(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string ext = path.extension().string();
    if (ext.empty()) return false;
    ext = toLower(ext.substr(1)); // Remove dot and convert to lower
    return ext == "cfg" || ext == "config";
}

// Parse simple key=value format (existing format)
bool parseSimpleFormat(const std::string& content, Config* config) {
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        line = Config::trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = Config::trim(line.substr(0, pos));
            std::string value = Config::trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            config->set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

// Parse JSON-like content (simplified)
bool parseJSONFormat(const std::string& content, Config* config) {
    // This is a simplified JSON parser for config files
    // Real implementation would use nlohmann::json or similar
    
    std::istringstream stream(content);
    std::string line;
    std::string currentKey;
    
    while (std::getline(stream, line)) {
        line = Config::trim(line);
        
        if (line.empty() || line[0] == '/' || line[0] == '#') {
            continue;
        }
        
        // Look for key-value pairs like "key": "value"
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos && colonPos > 0) {
            currentKey = Config::trim(line.substr(0, colonPos));
            
            std::string valuePart = Config::trim(line.substr(colonPos + 1));
            
            // Remove trailing comma if present
            if (!valuePart.empty() && valuePart.back() == ',') {
                valuePart.pop_back();
            }
            
            // Remove quotes from value
            if ((valuePart.front() == '"' && valuePart.back() == '"') ||
                (valuePart.front() == '\'' && valuePart.back() == '\'')) {
                valuePart = valuePart.substr(1, valuePart.size() - 2);
            }
            
            // Try to parse as different types
            try {
                // Try boolean
                if (valuePart == "true" || valuePart == "false") {
                    bool boolVal = (valuePart == "true");
                    config->set(currentKey, boolVal, ConfigSource::File);
                }
                // Try integer
                else if (valuePart.find('.') == std::string::npos && 
                        std::all_of(valuePart.begin(), valuePart.end(), ::isdigit)) {
                    int intVal = std::stoi(valuePart);
                    config->set(currentKey, intVal, ConfigSource::File);
                }
                // Try double
                else {
                    double doubleVal = std::stod(valuePart);
                    config->set(currentKey, doubleVal, ConfigSource::File);
                }
            } catch (...) {
                // If parsing fails, treat as string
                config->set(currentKey, valuePart, ConfigSource::File);
            }
        }
    }
    
    return true;
}

// Generate unique filename with timestamp
std::string generateTimestampedFilename(const std::string& basePath) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << basePath;
    
    // Add timestamp to filename
    tm tm_buf;
    gmtime_r(&time, &tm_buf);
    ss << "_" << std::put_time(&tm_buf, "%Y%m%d_%H%M%S");
    
    return ss.str();
}

} // namespace anonymous

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string lastLoadedFile;
    bool autoBackupEnabled;
    
    Impl() : autoBackupEnabled(true) {
        // Initialize with common defaults
        entries.emplace_back("simulation_timestep", 0.001, ConfigSource::Default);
        entries.emplace_back("neuron_count", 500, ConfigSource::Default);
        entries.emplace_back("region_count", 1, ConfigSource::Default);
        entries.emplace_back("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        entries.emplace_back("max_neurons_per_region", 1000, ConfigSource::Default);
        entries.emplace_back("max_synapses_per_neuron", 100, ConfigSource::Default);
        entries.emplace_back("connection_probability", 0.15f, ConfigSource::Default);
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        // Log error (in real implementation)
        return false;
    }
    
    // Determine file format
    bool isJSON = isJSONFile(filepath);
    bool isYAML = isYAMLFile(filepath);
    bool isCFG = isCFGFile(filepath);
    
    // Read file content
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();
    
    // Clear existing entries (except defaults)
    clear();
    
    bool success = false;
    
    if (isJSON) {
        success = parseJSONFormat(content, this);
    } else if (isYAML) {
        // For now, treat YAML the same as JSON
        // Real implementation would use proper YAML parser
        success = parseJSONFormat(content, this);
    } else {
        // Use simple format for .cfg and other files
        success = parseSimpleFormat(content, this);
    }
    
    if (success) {
        pImpl->lastLoadedFile = filepath;
        
        // Create backup if auto-backup is enabled
        if (pImpl->autoBackupEnabled) {
            std::string backupFile = generateTimestampedFilename(filepath);
            saveToFile(backupFile);
        }
    }
    
    return success;
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
    // Create directory if it doesn't exist
    std::filesystem::path path(filepath);
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Save with human-readable format
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated by NLM on " << getCurrentTimeString() << std::endl;
    file << "# Source: " << (pImpl->lastLoadedFile.empty() ? "default" : pImpl->lastLoadedFile) << std::endl;
    file << "" << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.source == ConfigSource::Default) {
            continue; // Skip default entries
        }
        
        file << "# " << entry.description << "\n";
        file << "# Source: " << static_cast<int>(entry.source) << "\n";
        
        // Format based on value type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, double> || 
                                std::is_same_v<T, float>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, int>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_convertible_v<T, std::string>) {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        
        file << "" << std::endl;
    }
    
    return true;
}

std::string Config::getCurrentTimeString() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    tm tm_buf;
    gmtime_r(&time, &tm_buf);
    ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    
    return ss.str();
}

// Rest of the implementation remains the same...

// Implement the missing functions
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
    // Keep default entries
    pImpl->entries.emplace_back("simulation_timestep", 0.001, ConfigSource::Default);
    pImpl->entries.emplace_back("neuron_count", 500, ConfigSource::Default);
    pImpl->entries.emplace_back("region_count", 1, ConfigSource::Default);
    pImpl->entries.emplace_back("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    
    // Separate default from user entries
    oss << "Default entries: " << getUserEntryCount(ConfigSource::Default) << "\n";
    oss << "File entries: " << getUserEntryCount(ConfigSource::File) << "\n";
    oss << "Command line entries: " << getUserEntryCount(ConfigSource::CommandLine) << "\n";
    oss << "Runtime entries: " << getUserEntryCount(ConfigSource::Runtime) << "\n";
    oss << "\n";
    
    oss << "Loaded from: " << (pImpl->lastLoadedFile.empty() ? "default" : pImpl->lastLoadedFile) << "\n";
    oss << "Auto backup: " << (pImpl->autoBackupEnabled ? "enabled" : "disabled") << "\n";
    oss << "\n";
    
    // Show most recent user entries
    oss << "Most recent entries:";
    const size_t maxShow = 5;
    size_t shown = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.source != ConfigSource::Default && shown < maxShow) {
            oss << "\n  " << entry.key << " = [";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, entry.value);
            oss << "] (" << static_cast<int>(entry.source) << ")";
            shown++;
        }
    }
    
    return oss.str();
}

// Helper function to count entries by source
size_t Config::getUserEntryCount(ConfigSource source) const {
    return std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [source](const ConfigEntry& e) { return e.source == source; });
}

// Implement trim function
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

} // namespace nlm
