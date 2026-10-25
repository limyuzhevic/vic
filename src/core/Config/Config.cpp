#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <json/json.h>
#include <unordered_map>
#include <ctime>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> metadata;
    bool autoSaveEnabled;
    std::string autoSavePath;
    std::string lastError;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->autoSaveEnabled = false;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open file: " + filepath;
        return false;
    }
    
    // Try to parse as JSON first
    if (loadFromJson(filepath)) {
        pImpl->lastError = "";
        return true;
    }
    
    // Fall back to simple key=value format
    if (loadFromSimpleFormat(filepath)) {
        pImpl->lastError = "";
        return true;
    }
    
    pImpl->lastError = "Failed to load configuration from: " + filepath;
    return false;
}

bool Config::loadFromJson(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        Json::CharReaderBuilder readerBuilder;
        readerBuilder["collectComments"] = false;
        
        Json::Value root;
        std::string errors;
        
        if (!Json::parseFromStream(readerBuilder, file, &root, &errors)) {
            NLM_LOG_ERROR("JSON parsing error: " + errors);
            return false;
        }
        
        // Parse JSON configuration
        clear();  // Clear existing entries
        
        if (root.isMember("config")) {
            const Json::Value& config = root["config"];
            parseJsonConfig(config);
        } else {
            parseJsonConfig(root);
        }
        
        // Load metadata if present
        if (root.isMember("metadata")) {
            const Json::Value& metadata = root["metadata"];
            if (metadata.isMember("created")) {
                pImpl->metadata["created"] = metadata["created"].asString();
            }
            if (metadata.isMember("description")) {
                pImpl->metadata["description"] = metadata["description"].asString();
            }
            if (metadata.isMember("version")) {
                pImpl->metadata["version"] = metadata["version"].asString();
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing exception: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    clear();  // Clear existing entries
    
    std::string line;
    int lineNumber = 0;
    int parsedLines = 0;
    
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
            parsedLines++;
        } else {
            NLM_LOG_WARNING("Invalid line " + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    NLM_LOG_INFO("Loaded " + std::to_string(parsedLines) + " configuration entries from " + filepath);
    return true;
}

void Config::parseJsonConfig(const Json::Value& config) {
    if (!config.isObject()) {
        return;
    }
    
    for (const auto& key : config.getMemberNames()) {
        const Json::Value& value = config[key];
        
        if (value.isInt()) {
            set(key, value.asInt(), ConfigSource::File);
        } else if (value.isUInt64()) {
            set(key, static_cast<int64_t>(value.asUInt64()), ConfigSource::File);
        } else if (value.isDouble()) {
            set(key, value.asDouble(), ConfigSource::File);
        } else if (value.isBool()) {
            set(key, value.asBool(), ConfigSource::File);
        } else if (value.isString()) {
            set(key, value.asString(), ConfigSource::File);
        } else if (value.isArray()) {
            std::vector<double> doubleArray;
            for (const auto& item : value) {
                if (item.isDouble()) {
                    doubleArray.push_back(item.asDouble());
                } else if (item.isInt()) {
                    doubleArray.push_back(static_cast<double>(item.asInt()));
                }
            }
            if (!doubleArray.empty()) {
                set(key, doubleArray, ConfigSource::File);
            }
        }
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool parsedArgs = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
                parsedArgs = true;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
            parsedArgs = true;
        }
        // Handle --help
        else if (arg == "--help" || arg == "-h") {
            NLM_LOG_INFO("Use --help for more information about configuration options");
        }
        // Handle --version
        else if (arg == "--version") {
            NLM_LOG_INFO("NLM Configuration System v1.0");
        }
    }
    
    if (parsedArgs) {
        NLM_LOG_INFO("Loaded configuration from command line arguments");
    }
    return true;
}

void Config::enableAutoSave(const std::string& path) {
    pImpl->autoSaveEnabled = true;
    pImpl->autoSavePath = path;
}

void Config::disableAutoSave() {
    pImpl->autoSaveEnabled = false;
    pImpl->autoSavePath = "";
}

bool Config::shouldAutoSave() const {
    return pImpl->autoSaveEnabled && !pImpl->autoSavePath.empty();
}

std::string Config::getAutoSavePath() const {
    return pImpl->autoSavePath;
}

void Config::setMetadata(const std::string& key, const std::string& value) {
    pImpl->metadata[key] = value;
}

std::string Config::getMetadata(const std::string& key) const {
    auto it = pImpl->metadata.find(key);
    return it != pImpl->metadata.end() ? it->second : "";
}

std::vector<std::string> Config::getMetadataKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->metadata.size());
    for (const auto& meta : pImpl->metadata) {
        keys.push_back(meta.first);
    }
    return keys;
}

std::string Config::getLastError() const {
    return pImpl->lastError;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open file for writing: " + filepath;
        return false;
    }
    
    // Try to save as JSON first
    if (saveToJson(filepath)) {
        pImpl->lastError = "";
        return true;
    }
    
    // Fall back to simple key=value format
    if (saveToSimpleFormat(filepath)) {
        pImpl->lastError = "";
        return true;
    }
    
    pImpl->lastError = "Failed to save configuration to: " + filepath;
    return false;
}

bool Config::saveToJson(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        Json::Value root;
        Json::Value config;
        
        // Create config object
        for (const auto& entry : pImpl->entries) {
            std::visit([&config, &entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    config[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, int>) {
                    config[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    config[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    config[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    config[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    Json::Value array;
                    for (const auto& item : arg) {
                        array.append(item);
                    }
                    config[entry.key] = array;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    Json::Value array;
                    for (const auto& item : arg) {
                        array.append(item);
                    }
                    config[entry.key] = array;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    Json::Value array;
                    for (const auto& item : arg) {
                        array.append(item);
                    }
                    config[entry.key] = array;
                }
            }, entry.value);
        }
        
        root["config"] = config;
        
        // Add metadata
        Json::Value metadata;
        for (const auto& meta : pImpl->metadata) {
            metadata[meta.first] = meta.second;
        }
        
        if (!metadata.empty()) {
            root["metadata"] = metadata;
        }
        
        // Write JSON to file with pretty formatting
        Json::StreamWriterBuilder writerBuilder;
        writerBuilder["indentation"] = "  ";
        writerBuilder["enableYAMLCompatibility"] = true;
        
        std::unique_ptr<Json::StreamWriter> writer(writerBuilder.newStreamWriter(&file));
        writer->write(root);
        writer->flush();
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON writing exception: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToSimpleFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Add header comment
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM configuration system\n";
    file << "# Format: key = value\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
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
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

void Config::backupCurrentConfig(const std::string& backupDir) {
    if (!std::filesystem::exists(backupDir)) {
        std::filesystem::create_directories(backupDir);
    }
    
    // Create backup filename with timestamp
    std::time_t now = std::time(nullptr);
    char timestamp[100];
    std::strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", std::localtime(&now));
    
    std::string backupPath = backupDir + "/nlm_config_" + std::string(timestamp) + ".json";
    
    if (saveToFile(backupPath)) {
        NLM_LOG_INFO("Configuration backed up to: " + backupPath);
    } else {
        NLM_LOG_ERROR("Failed to create backup: " + backupPath);
    }
}

bool Config::mergeWithFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Parse the incoming file
    Config tempConfig;
    if (!tempConfig.loadFromFile(filepath)) {
        return false;
    }
    
    // Merge entries from temp config
    for (const auto& entry : tempConfig.pImpl->entries) {
        // Check if this key already exists
        if (has(entry.key)) {
            // Update with higher priority (File > Runtime > CommandLine)
            ConfigSource existingSource = getConfigEntry(entry.key).source;
            ConfigSource newSource = entry.source;
            
            // CommandLine has highest priority, then File, then Runtime
            if (newSource > existingSource) {
                set(entry.key, entry.value, newSource);
            }
        } else {
            // Add new entry
            set(entry.key, entry.value, entry.source);
        }
    }
    
    // Merge metadata
    for (const auto& meta : tempConfig.pImpl->metadata) {
        pImpl->metadata[meta.first] = meta.second;
    }
    
    return true;
}

ConfigEntry Config::getConfigEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n
    if (it != pImpl->entries.end()) {
        return *it;
    }
    
    return ConfigEntry(key, ConfigValue(), ConfigSource::Default, "");
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
    pImpl->metadata.clear();
    pImpl->autoSaveEnabled = false;
    pImpl->autoSavePath = "";
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
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    if (!pImpl->metadata.empty()) {
        oss << "\nMetadata:\n";
        for (const auto& meta : pImpl->metadata) {
            oss << "  " << meta.first << ": " << meta.second << "\n";
        }
    }
    if (pImpl->autoSaveEnabled) {
        oss << "\nAuto-save: enabled (" << pImpl->autoSavePath << ")\n";
    }
    if (!pImpl->lastError.empty()) {
        oss << "\nLast error: " << pImpl->lastError << "\n";
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
