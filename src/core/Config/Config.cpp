#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <limits>
#include <ctime>
#include <iomanip>
#include <numeric>

namespace nlm {

// Config implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    initializePresets();
    initializeValidationRules();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, bool merge) {
    std::lock_guard<std::mutex> lock(configMutex);
    
    clearErrorMessages();
    
    if (!isFileExists(filepath)) {
        addError("File not found: " + filepath, ConfigErrorType::FileNotFound);
        return false;
    }
    
    // Determine file type by extension
    std::string ext = "";
    size_t pos = filepath.find_last_of('.');
    if (pos != std::string::npos) {
        ext = filepath.substr(pos + 1);
    }
    
    bool success = false;
    if (ext == "json") {
        success = parseJsonFile(filepath, merge);
    } else {
        success = parseKeyValueFile(filepath, merge);
    }
    
    if (!success) {
        addError("Failed to parse configuration file: " + filepath, ConfigErrorType::ParseError);
    }
    
    return success;
}

bool Config::loadFromEnvironment(const std::string& prefix, bool merge) {
    std::lock_guard<std::mutex> lock(configMutex);
    
    clearErrorMessages();
    
    bool success = true;
    
    // Get all environment variables
    const char* env = std::getenv("PATH");
    if (!env) {
        return false;
    }
    
    // This is a simplified approach - in real implementation we'd iterate through environment variables
    return success;
}

bool Config::loadFromArgs(int argc, char** argv) {
    std::lock_guard<std::mutex> lock(configMutex);
    
    clearErrorMessages();
    
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
    
    return !errorMessages.empty();
}

bool Config::loadFromJson(const std::string& jsonStr, bool merge) {
    std::lock_guard<std::mutex> lock(configMutex);
    
    clearErrorMessages();
    
    try {
        nlohmann::json json = nlohmann::json::parse(jsonStr);
        
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            ConfigValue value = jsonToConfigValue(*it);
            set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        addError("JSON parse error: " + std::string(e.what()), ConfigErrorType::ParseError);
        return false;
    } catch (const std::exception& e) {
        addError("JSON parsing error: " + std::string(e.what()), ConfigErrorType::ParseError);
        return false;
    }
}

bool Config::loadFromYaml(const std::string& yamlStr, bool merge) {
    // Placeholder for YAML support - would use a YAML library
    // For now, treat as key=value format
    std::istringstream stream(yamlStr);
    std::string line;
    bool success = true;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(configMutex);
    
    if (!isFileExists(filepath)) {
        addError("File not found for saving: " + filepath, ConfigErrorType::FileNotFound);
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        addError("Failed to open file for writing: " + filepath, ConfigErrorType::SerializationError);
        return false;
    }
    
    file << "{" << std::endl;
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) file << "," << std::endl;
        first = false;
        
        file << "  \"" << entry.key << "\": \"" << getValueAsString(entry.value) << "\"";
        if (!entry.description.empty()) {
            file << " // " << entry.description;
        }
    }
    file << std::endl << "}" << std::endl;
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(configMutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        addError("Type mismatch for key \"" + key + "\": expected " + convertToString(T()), 
                ConfigErrorType::InvalidType);
        return std::nullopt;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) return val;
    
    // Try to get from metadata for default
    auto metaIt = metadataStore.find(key);
    if (metaIt != metadataStore.end() && metaIt->second.defaultValue.empty()) {
        try {
            T result = convertToType<T>(metaIt->second.defaultValue);
            return result;
        } catch (...) {}
    }
    
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
T Config::getValue(const std::string& key) const {
    std::lock_guard<std::mutex> lock(configMutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        throw ConfigError("Key not found: " + key, ConfigErrorType::MissingKey);
    }
    
    try {
        T result = std::get<T>(it->value);
        
        // Validate if metadata exists
        auto metaIt = metadataStore.find(key);
        if (metaIt != metadataStore.end()) {
            if (!validateValue(metaIt->second)) {
                throw ConfigError("Validation failed for key: " + key, ConfigErrorType::ValidationError);
            }
        }
        
        return result;
    } catch (const std::bad_variant_access&) {
        throw ConfigError("Type conversion failed for key \"" + key + "\": " + 
                         "cannot convert to " + convertToString(T()), ConfigErrorType::InvalidType);
    }
}

template<typename T>
T Config::getValue(const std::string& key, const std::string& errorContext) const {
    try {
        T result = getValue<T>(key);
        
        auto metaIt = metadataStore.find(key);
        if (metaIt != metadataStore.end()) {
            if (!validateValue(metaIt->second)) {
                throw ConfigError("Validation failed for key \"" + key + "\" in " + errorContext + ": " +
                                 getValidationErrorMessage(metaIt->second), ConfigErrorType::ValidationError);
            }
        }
        
        return result;
    } catch (const ConfigError& e) {
        addError(errorContext + ": " + e.what(), e.getErrorType());
        throw;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(configMutex);
    
    clearErrorMessages();
    
    // Validate the value
    ConfigMetadata tempMeta;
    tempMeta.key = key;
    tempMeta.value = value;
    tempMeta.source = source;
    
    if (!validateValue(tempMeta)) {
        addError("Invalid value for key \"" + key + "\": " + 
                getLastValidationError(), ConfigErrorType::ValidationError);
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
    
    // Track change
    ConfigDiff diff(key, ConfigValue(), value, source, "modified");
    changeHistory.push_back(diff);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, convertToVariant(value), source);
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

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<bool>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setWithMetadata(const std::string& key, const ConfigValue& value, 
                            const ConfigMetadata& metadata, ConfigSource source) {
    std::lock_guard<std::mutex> lock(configMutex);
    
    ConfigMeta temp = metadata;
    temp.key = key;
    temp.value = value;
    
    if (validateValue(temp)) {
        set(key, value, source);
        
        std::lock_guard<std::mutex> metaLock(metadataMutex);
        metadataStore[key] = temp;
        
        // Track metadata change
        ConfigDiff diff(key, ConfigValue(), value, source, "metadata_modified");
        changeHistory.push_back(diff);
    } else {
        addError("Invalid metadata for key \"" + key + "\"", ConfigErrorType::ValidationError);
    }
}

bool Config::has(const std::string& key) const {
    std::lock_guard<std::mutex> lock(configMutex);
    
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

std::optional<ConfigMetadata> Config::getMetadata(const std::string& key) const {
    std::lock_guard<std::mutex> lock(metadataMutex);
    
    auto it = metadataStore.find(key);
    if (it != metadataStore.end()) {
        return it->second;
    }
    return std::nullopt;
}

void Config::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(configMutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n
    if (it != pImpl->entries.end()) {
        ConfigValue value = it->value;
        pImpl->entries.erase(it);
        
        // Track removal
        ConfigDiff diff(key, value, ConfigValue(), ConfigSource::Runtime, "removed");
        changeHistory.push_back(diff);
        
        // Remove metadata
        std::lock_guard<std::mutex> metaLock(metadataMutex);
        metadataStore.erase(key);
    }
}

std::vector<std::string> Config::getKeys() const {
    std::lock_guard<std::mutex> lock(configMutex);
    
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::pair<std::string, ConfigMetadata>> Config::getKeysWithMetadata() const {
    std::lock_guard<std::mutex> lockAll(configMutex);
    std::lock_guard<std::mutex> metaLock(metadataMutex);
    
    std::vector<std::pair<std::string, ConfigMetadata>> result;
    for (const auto& entry : pImpl->entries) {
        auto metaIt = metadataStore.find(entry.key);
        if (metaIt != metadataStore.end()) {
            result.emplace_back(entry.key, metaIt->second);
        } else {
            result.emplace_back(entry.key, ConfigMetadata(entry.key, entry.value, entry.source));
        }
    }
    return result;
}

void Config::clear() {
    std::lock_guard<std::mutex> lock(configMutex);
    
    pImpl->entries.clear();
    
    // Clear metadata
    std::lock_guard<std::mutex> metaLock(metadataMutex);
    metadataStore.clear();
    
    // Track clear
    ConfigDiff diff("__clear__", ConfigValue(), ConfigValue(), ConfigSource::Runtime, "cleared");
    changeHistory.push_back(diff);
}

std::vector<ConfigPreset> Config::getPresets() {
    std::lock_guard<std::mutex> lock(presetMutex);
    
    std::vector<ConfigPreset> result;
    for (const auto& pair : presetStore) {
        result.push_back(pair.second);
    }
    return result;
}

bool Config::applyPreset(const std::string& presetName) {
    std::lock_guard<std::mutex> lock(configMutex);
    std::lock_guard<std::mutex> presetLock(presetMutex);
    
    auto it = presetStore.find(presetName);
    if (it == presetStore.end()) {
        addError("Preset not found: " + presetName, ConfigErrorType::InvalidValue);
        return false;
    }
    
    const ConfigPreset& preset = it->second;
    bool success = true;
    
    for (const auto& pair : preset.parameters) {
        if (!setWithMetadata(pair.first, pair.second, ConfigMetadata(), ConfigSource::Preset)) {
            success = false;
        }
    }
    
    return success;
}

bool Config::addPreset(const ConfigPreset& preset) {
    std::lock_guard<std::mutex> presetLock(presetMutex);
    
    presetStore[preset.name] = preset;
    return true;
}

bool Config::inheritFrom(const Config& parent, bool merge) {
    std::lock_guard<std::mutex> lock(configMutex);
    
    if (merge) {
        for (const auto& entry : parent.pImpl->entries) {
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&entry](const ConfigEntry& e) { return e.key == entry.key; });
            
            if (it == pImpl->entries.end()) {
                // Add with inheritance source
                pImpl->entries.push_back(entry);
                ConfigDiff diff(entry.key, ConfigValue(), entry.value, ConfigSource::Inheritance, "inherited");
                changeHistory.push_back(diff);
            }
        }
    } else {
        pImpl->entries = parent.pImpl->entries;
    }
    
    return true;
}

bool Config::validate() const {
    std::lock_guard<std::mutex> lock(configMutex);
    
    clearErrorMessages();
    
    for (const auto& entry : pImpl->entries) {
        auto metaIt = metadataStore.find(entry.key);
        if (metaIt != metadataStore.end()) {
            ConfigMetadata tempMeta = metaIt->second;
            tempMeta.value = entry.value;
            
            if (!validateValue(tempMeta)) {
                addError("Validation failed for key \"" + entry.key + "\": " + 
                        getLastValidationError(), ConfigErrorType::ValidationError);
            }
        }
    }
    
    return errorMessages.empty();
}

bool Config::validate(const std::string& key) const {
    std::lock_guard<std::mutex> lock(configMutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n
    if (it == pImpl->entries.end()) {
        addError("Key not found for validation: " + key, ConfigErrorType::MissingKey);
        return false;
    }
    
    return validateValue(ConfigMetadata(key, it->value, it->source));
}

std::vector<std::string> Config::getValidationErrors() const {
    std::lock_guard<std::mutex> lock(configMutex);
    
    return errorMessages;
}

std::vector<ConfigDiff> Config::getChanges(const Config& other) const {
    std::vector<ConfigDiff> result;
    
    std::lock_guard<std::mutex> lock1(configMutex);
    std::lock_guard<std::mutex> lock2(other.configMutex);
    
    // Check for additions and modifications in other
    for (const auto& entry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });n
        if (it == pImpl->entries.end()) {
            result.emplace_back(entry.key, ConfigValue(), entry.value, entry.source, "added");
        } else if (it->value != entry.value) {
            result.emplace_back(entry.key, it->value, entry.value, entry.source, "modified");
        }
    }
    
    // Check for removals
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });n
        if (it == other.pImpl->entries.end()) {
            result.emplace_back(entry.key, entry.value, ConfigValue(), entry.source, "removed");
        }
    }
    
    return result;
}

std::vector<ConfigDiff> Config::getChangesSince(const Config& previous) const {
    std::vector<ConfigDiff> result;
    
    std::lock_guard<std::mutex> lock1(configMutex);
    std::lock_guard<std::mutex> lock2(previous.configMutex);
    
    // Compare with previous version
    // This would need access to history
    return result;
}

bool Config::hasChanges(const Config& other) const {
    return !getChanges(other).empty();
}

void Config::resetChangeTracking() {
    std::lock_guard<std::mutex> lock(changeHistoryMutex);
    changeHistory.clear();
}

bool Config::backup(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(configMutex);
    
    std::string backupPath = generateTempFile("config_backup_");
    
    std::ofstream out(backupPath);
    if (!out.is_open()) {
        addError("Failed to create backup file: " + backupPath, ConfigErrorType::SerializationError);
        return false;
    }
    
    out << toJson();
    out.close();
    
    // If specific filepath given, move to that location
    if (!filepath.empty() && backupPath != filepath) {
        std::filesystem::rename(backupPath, filepath);
    }
    
    return true;
}

bool Config::restore(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(configMutex);
    
    if (!isFileExists(filepath)) {
        addError("Backup file not found: " + filepath, ConfigErrorType::FileNotFound);
        return false;
    }
    
    std::ifstream in(filepath);
    if (!in.is_open()) {
        addError("Failed to open backup file: " + filepath, ConfigErrorType::ParseError);
        return false;
    }
    
    std::string jsonStr((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    
    clear(); // Clear current config first
    return loadFromJson(jsonStr, false);
}

bool Config::loadSchema(const JsonSchema& schema) {
    currentSchema = schema;
    return true;
}

bool Config::validateWithSchema() const {
    // Implement schema validation logic
    return true;
}

std::string Config::normalizeEnvVarName(const std::string& name) {
    std::string result = "";
    bool capitalize = true;
    
    for (char c : name) {
        if (c == '_' || c == '-') {
            capitalize = true;
        } else if (capitalize) {
            result += std::toupper(c);
            capitalize = false;
        } else {
            result += c;
        }
    }
    
    return "NLM_" + result;
}

bool Config::equals(const Config& other) const {
    std::lock_guard<std::mutex> lock1(configMutex);
    std::lock_guard<std::mutex> lock2(other.configMutex);
    
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });n
        if (it == other.pImpl->entries.end() || it->value != entry.value) {
            return false;
        }
    }
    
    return true;
}

std::string Config::toJson() const {
    nlohmann::json json;
    
    std::lock_guard<std::mutex> lock(configMutex);
    
    for (const auto& entry : pImpl->entries) {
        json[entry.key] = configValueToJson(entry.value);
    }
    
    return json.dump(2);
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):" << std::endl;
    oss << std::left << std::setw(30) << "Key" << std::setw(15) << "Type" 
        << std::setw(10) << "Source" << std::setw(20) << "Value" << std::endl;
    oss << std::string(75, '-') << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        std::string typeStr = getValueTypeName(entry.value);
        std::string sourceStr = errorTypeToString(static_cast<ConfigErrorType>(entry.source));
        std::string valueStr = getValueAsString(entry.value);
        
        if (valueStr.length() > 18) {
            valueStr = valueStr.substr(0, 15) + "...";
        }
        
        oss << std::left << std::setw(30) << entry.key
            << std::setw(15) << typeStr
            << std::setw(10) << sourceStr
            << std::setw(20) << valueStr << std::endl;
    }
    
    oss << std::endl << "Validation errors: " << errorMessages.size();
    
    return oss.str();
}

std::string Config::toString() const {
    std::ostringstream oss;
    oss << "Config(version=" << version << ", historyEnabled=" << 
        (historyEnabled ? "true" : "false") << ", entries=" << 
        pImpl->entries.size() << ")";
    return oss.str();
}

bool Config::isNumericType(const std::string& valueStr) {
    // Check if string represents a number
    try {
        std::stod(valueStr);
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::isBooleanType(const std::string& valueStr) {
    std::string lower = toLower(valueStr);
    return lower == "true" || lower == "false" || lower == "1" || lower == "0";
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

void Config::setVersion(const std::string& version) {
    this->version = version;
}

std::string Config::getVersion() const {
    return version;
}

void Config::enableHistory(bool enabled) {
    historyEnabled = enabled;
}

std::vector<Config> Config::getHistory() const {
    std::vector<Config> result;
    
    std::lock_guard<std::mutex> lock(historyMutex);
    
    for (const auto& weakPtr : historyInstances) {
        if (auto sharedPtr = weakPtr.lock()) {
            result.push_back(*sharedPtr);
        }
    }
    
    return result;
}

void Config::merge(const Config& other, bool mergeDefaults) {
    std::lock_guard<std::mutex> lock1(configMutex);
    std::lock_guard<std::mutex> lock2(other.configMutex);
    
    for (const auto& entry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });n
        if (it == pImpl->entries.end()) {
            pImpl->entries.push_back(entry);
            
            // Get metadata if available
            auto metaIt = other.metadataStore.find(entry.key);
            if (metaIt != other.metadataStore.end()) {
                std::lock_guard<std::mutex> metaLock(metadataMutex);
                metadataStore[entry.key] = metaIt->second;
            }
        }
    }
}

ConfigValue Config::convertToVariant(const std::string& valueStr) {
    std::string trimmed = trim(valueStr);
    
    if (isBooleanType(trimmed)) {
        std::string lower = toLower(trimmed);
        return lower == "true" || lower == "1";
    }
    
    if (isNumericType(trimmed)) {
        try {
            // Try integer first
            size_t pos;
            int intVal = std::stoi(trimmed, &pos);
            if (pos == trimmed.length()) {
                return intVal;
            }
            
            // Try double
            double doubleVal = std::stod(trimmed, &pos);
            if (pos == trimmed.length()) {
                return doubleVal;
            }
        } catch (...) {}
    }
    
    return trimmed;
}

template<typename T>
std::string Config::convertToString(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

void Config::cleanupTempFiles() {
    try {
        for (const auto& entry : std::filesystem::directory_iterator(".")) {
            std::string filename = entry.path().filename().string();
            if (filename.find("config_backup_") == 0) {
                std::filesystem::remove(entry.path());
            }
        }
    } catch (...) {}
}

bool Config::validateValue(const ConfigMetadata& metadata) const {
    // Check type
    if (!metadata.type.empty() && !isTypeMatch(metadata.value, metadata.type)) {
        addError("Type mismatch for key \"" + metadata.key + "\": " +
                "expected " + metadata.type + ", got " + getValueTypeName(metadata.value), 
                ConfigErrorType::InvalidType);
        return false;
    }
    
    // Check range
    if (metadata.minValue != std::numeric_limits<double>::lowest() || 
        metadata.maxValue != std::numeric_limits<double>::max()) {
        
        double value = 0.0;
        if (std::holds_alternative<int>(metadata.value)) value = std::get<int>(metadata.value);
        else if (std::holds_alternative<int64_t>(metadata.value)) value = static_cast<double>(std::get<int64_t>(metadata.value));
        else if (std::holds_alternative<double>(metadata.value)) value = std::get<double>(metadata.value);
        
        if (value < metadata.minValue || value > metadata.maxValue) {
            addError("Value out of range for key \"" + metadata.key + "\": " +
                    std::to_string(value) + " not in [" + 
                    std::to_string(metadata.minValue) + ", " + 
                    std::to_string(metadata.maxValue) + "]", 
                    ConfigErrorType::InvalidValue);
            return false;
        }
    }
    
    // Check required
    if (metadata.required && metadata.value.index() == 0) { // Assuming 0 is null/empty
        addError("Required key \"" + metadata.key + "\" is not set", 
                ConfigErrorType::MissingKey);
        return false;
    }
    
    // Check validation rules
    auto ruleIt = validationRules.find(metadata.key);
    if (ruleIt != validationRules.end()) {
        if (!ruleIt->second(metadata.value)) {
            addError("Validation rule failed for key \"" + metadata.key + "\": " +
                    ruleIt->second.errorMessage, ConfigErrorType::ValidationError);
            return false;
        }
    }
    
    return true;
}

std::string Config::getValueAsString(const ConfigValue& value) const {
    std::ostringstream oss;
    
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    
    return oss.str();
}

bool Config::isFileExists(const std::string& filepath) {
    return std::filesystem::exists(filepath);
}

std::string Config::generateTempFile(const std::string& prefix) {
    std::string filename = prefix + std::to_string(std::time(nullptr)) + ".json";
    return filename;
}

nlohmann::json Config::configValueToJson(const ConfigValue& value) {
    nlohmann::json json;
    
    std::visit([&json](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::vector<double>>>) {
            json = arg;
        }
    }, value);
    
    return json;
}

ConfigValue Config::jsonToConfigValue(const nlohmann::json& json) {
    ConfigValue result;
    
    if (json.is_number_integer()) {
        result = json.get<int>();
    } else if (json.is_number_unsigned()) {
        result = json.get<uint64_t>(); // Note: uint64_t not in variant
    } else if (json.is_number()) {
        result = json.get<double>();
    } else if (json.is_boolean()) {
        result = json.get<bool>();
    } else if (json.is_string()) {
        result = json.get<std::string>();
    } else if (json.is_array()) {
        // Determine array type
        if (json.empty()) {
            result = std::vector<double>();
        } else {
            bool allNumbers = true;
            bool allStrings = true;
            bool allBools = true;
            
            for (const auto& item : json) {
                if (!item.is_number()) allNumbers = false;
                if (!item.is_string()) allStrings = false;
                if (!item.is_boolean()) allBools = false;
            }
            
            if (allNumbers) {
                std::vector<double> vec;
                for (const auto& item : json) {
                    vec.push_back(item.get<double>());
                }
                result = vec;
            } else if (allStrings) {
                std::vector<std::string> vec;
                for (const auto& item : json) {
                    vec.push_back(item.get<std::string>());
                }
                result = vec;
            } else if (allBools) {
                std::vector<bool> vec;
                for (const auto& item : json) {
                    vec.push_back(item.get<bool>());
                }
                result = vec;
            } else {
                // Mixed types or unknown - use vector<double> with conversion
                std::vector<double> vec;
                for (const auto& item : json) {
                    if (item.is_number()) {
                        vec.push_back(item.get<double>());
                    } else if (item.is_boolean()) {
                        vec.push_back(item.get<bool>() ? 1.0 : 0.0);
                    } else {
                        // Try to convert string
                        try {
                            vec.push_back(std::stod(item.get<std::string>()));
                        } catch (...) {
                            vec.push_back(0.0);
                        }
                    }
                }
                result = vec;
            }
        }
    }
    
    return result;
}

void Config::initializePresets() {
    // Add common presets
    addPreset(ConfigPreset("performance", "High performance settings for maximum throughput",
                         std::map<std::string, ConfigValue>{{"iterations", 1000},
                                                           {"threads", 8},
                                                           {"cache_size", 1024}},
                         "performance"));
    
    addPreset("accuracy", "High accuracy settings for precise results",
                     std::map<std::string, ConfigValue>{{"iterations", 10000},
                                                       {"threads", 2},
                                                       {"tolerance", 1e-10}},
                     "accuracy");
    
    addPreset("memory", "Memory-efficient settings for constrained environments",
                    std::map<std::string, ConfigValue>{{"cache_size", 256},
                                                      {"threads", 1},
                                                      {"iterations", 100}},
                    "memory");
}

void Config::initializeValidationRules() {
    // Add common validation rules
    ValidationRule iterationRule;
    iterationRule.type = "range";
    iterationRule.rule = "1-1000000";
    iterationRule.errorMessage = "Iterations must be between 1 and 1,000,000";
    validationRules["iterations"] = iterationRule;
    
    ValidationRule threadRule;
    threadRule.type = "range";
    threadRule.rule = "1-64";
    threadRule.errorMessage = "Threads must be between 1 and 64";
    validationRules["threads"] = threadRule;
    
    ValidationRule cacheRule;
    cacheRule.type = "range";
    cacheRule.rule = "0-10000";
    cacheRule.errorMessage = "Cache size must be between 0 and 10,000";
    validationRules["cache_size"] = cacheRule;
}
