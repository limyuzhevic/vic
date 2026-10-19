#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/hmac.h>
#include <cryptopp/sha.h>
#include <json/json.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    // Schema validation
    bool validationEnabled = false;
    std::string schema;
    std::vector<std::string> validationErrors;
    
    // Configuration templates
    std::unordered_map<std::string, std::shared_ptr<Config>> templates;
    
    // Configuration monitoring
    std::shared_ptr<ConfigMonitor> monitor;
    std::vector<ConfigChangeRecord> changeHistory;
    
    // Configuration comparison
    static std::string compareConfigurations(const Config& config1, const Config& config2);
    static std::string getConfigurationDifferences(const Config& config1, const Config& config2);
    static void mergeConfigurations(const Config& config1, const Config& config2, Config& result);
    
    // Configuration encryption
    std::string encryptionKey;
    std::string encryptionStatus = "Not encrypted";
    
    // Configuration export/import
    std::string exportPath = ".";
    std::vector<std::string> exportFormats = {"json", "yaml", "ini", "csv"};
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Load from file (JSON format)
bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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

// Load from command line arguments
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

// Save to file
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

// ========== ADVANCED CONFIGURATION MANAGEMENT ==========

// Schema validation
bool Config::hasValidSchema() const {
    return !pImpl->schema.empty();
}

std::string Config::getSchema() const {
    return pImpl->schema;
}

ValidationResult Config::validateConfiguration() const {
    ValidationResult result;
    
    if (pImpl->validationEnabled) {
        for (const auto& entry : pImpl->entries) {
            if (!entry.validationError.empty()) {
                result.isValid = false;
                result.errors.push_back("Key '" + entry.key + "': " + entry.validationError);
            }
        }
    }
    
    return result;
}

void Config::setValidationEnabled(bool enabled) {
    pImpl->validationEnabled = enabled;
    if (!enabled) {
        clearValidationErrors();
    }
}

bool Config::isValidationEnabled() const {
    return pImpl->validationEnabled;
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    for (const auto& entry : pImpl->entries) {
        if (!entry.validationError.empty()) {
            errors.push_back("Key '" + entry.key + "': " + entry.validationError);
        }
    }
    return errors;
}

void Config::clearValidationErrors() {
    for (auto& entry : pImpl->entries) {
        entry.validationError.clear();
    }
    pImpl->validationErrors.clear();
}

// Configuration templates
size_t Config::getTemplateCount() const {
    return pImpl->templates.size();
}

std::vector<std::string> Config::getTemplateNames() const {
    std::vector<std::string> names;
    names.reserve(pImpl->templates.size());
    for (const auto& pair : pImpl->templates) {
        names.push_back(pair.first);
    }
    return names;
}

std::shared_ptr<Config> Config::getTemplate(const std::string& templateName) const {
    auto it = pImpl->templates.find(templateName);
    if (it != pImpl->templates.end()) {
        return it->second;
    }
    return nullptr;
}

bool Config::addTemplate(const std::string& templateName, const Config& templateConfig) {
    pImpl->templates[templateName] = std::make_shared<Config>(templateConfig);
    return true;
}

bool Config::removeTemplate(const std::string& templateName) {
    auto it = pImpl->templates.find(templateName);
    if (it != pImpl->templates.end()) {
        pImpl->templates.erase(it);
        return true;
    }
    return false;
}

bool Config::loadTemplate(const std::string& templateName, const std::string& filepath) {
    std::shared_ptr<Config> newConfig = std::make_shared<Config>();
    if (newConfig->loadFromFile(filepath)) {
        addTemplate(templateName, *newConfig);
        return true;
    }
    return false;
}

// Configuration monitoring
bool Config::hasMonitor() const {
    return pImpl->monitor != nullptr;
}

std::shared_ptr<ConfigMonitor> Config::getMonitor() const {
    return pImpl->monitor;
}

void Config::setMonitor(std::shared_ptr<ConfigMonitor> monitor) {
    pImpl->monitor = monitor;
}

void Config::onConfigurationChanged(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue, ConfigSource source) {
    ConfigChangeRecord record(key, oldValue, newValue, source);
    pImpl->changeHistory.push_back(record);
    
    if (pImpl->monitor) {
        pImpl->monitor->onConfigurationChanged(key, oldValue, newValue, source);
    }
}

std::vector<ConfigChangeRecord> Config::getConfigurationHistory() const {
    return pImpl->changeHistory;
}

std::vector<ConfigChangeRecord> Config::getRecentChanges(size_t maxChanges) const {
    if (pImpl->changeHistory.size() <= maxChanges) {
        return pImpl->changeHistory;
    }
    
    std::vector<ConfigChangeRecord> recent;
    recent.reserve(maxChanges);
    auto start = pImpl->changeHistory.end() - maxChanges;
    for (auto it = start; it != pImpl->changeHistory.end(); ++it) {
        recent.push_back(*it);
    }
    return recent;
}

bool Config::rollback(size_t changeIndex) {
    if (changeIndex >= pImpl->changeHistory.size()) {
        return false;
    }
    
    const ConfigChangeRecord& record = pImpl->changeHistory[changeIndex];
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&record](const ConfigEntry& e) { return e.key == record.key; });n
    if (it != pImpl->entries.end()) {
        it->value = record.oldValue;
    } else {
        pImpl->entries.emplace_back(record.key, record.oldValue, record.source);
    }
    
    return true;
}

std::string Config::getDifferences(const Config& otherConfig) const {
    return compareConfigurations(*this, otherConfig);
}

// Configuration comparison
std::string Config::compareConfigurations(const Config& config1, const Config& config2) {
    std::ostringstream oss;
    oss << "Comparison between Configuration1 and Configuration2:\n";
    
    // Get keys from both configs
    std::vector<std::string> allKeys;
    allKeys.reserve(config1.getKeys().size() + config2.getKeys().size());
    
    for (const auto& key : config1.getKeys()) {
        allKeys.push_back(key);
    }
    
    for (const auto& key : config2.getKeys()) {
        if (std::find(allKeys.begin(), allKeys.end(), key) == allKeys.end()) {
            allKeys.push_back(key);
        }
    }
    
    oss << "All unique keys (" << allKeys.size() << "):\n";
    for (const auto& key : allKeys) {
        bool inConfig1 = config1.has(key);
        bool inConfig2 = config2.has(key);
        oss << "  " << key << ": in Config1=" << (inConfig1 ? "true" : "false")
            << ", in Config2=" << (inConfig2 ? "true" : "false") << "\n";
        
        if (inConfig1 && inConfig2) {
            auto value1 = config1.get<std::string>(key);
            auto value2 = config2.get<std::string>(key);
            if (value1.has_value() && value2.has_value() && value1.value() != value2.value()) {
                oss << "    Values differ: '" << value1.value() << "' vs '" << value2.value() << "'\n";
            }
        }
    }
    
    return oss.str();
}

std::string Config::getConfigurationDifferences(const Config& config1, const Config& config2) {
    return compareConfigurations(config1, config2);
}

void Config::mergeConfigurations(const Config& config1, const Config& config2, Config& result) {
    // Start with config1
    result.clear();
    for (const auto& key : config1.getKeys()) {
        auto value = config1.get<std::string>(key);
        if (value.has_value()) {
            result.set(key, value.value(), ConfigSource::Runtime);
        }
    }
    
    // Override with config2 values
    for (const auto& key : config2.getKeys()) {
        auto value = config2.get<std::string>(key);
        if (value.has_value()) {
            result.set(key, value.value(), ConfigSource::Runtime);
        }
    }
}

// Configuration encryption/security
std::string Config::encryptConfiguration(const std::string& key) const {
    // Simple XOR encryption for demonstration (NOT secure for production)
    std::string encrypted;
    encrypted.reserve(getKeys().size() * 100); // Estimate size
    
    for (const auto& entry : pImpl->entries) {
        std::string valueStr;
        std::visit([&valueStr](auto&& arg) {
            if constexpr (std::is_same_v<decltype(arg), std::string>) {
                valueStr = arg;
            } else if constexpr (std::is_same_v<decltype(arg), int>) {
                valueStr = std::to_string(arg);
            } else if constexpr (std::is_same_v<decltype(arg), double>) {
                valueStr = std::to_string(arg);
            } else if constexpr (std::is_same_v<decltype(arg), bool>) {
                valueStr = arg ? "true" : "false";
            }
        }, entry.value);
        
        // Simple XOR encryption
        for (char& c : valueStr) {
            c = c ^ key[0];
        }
        
        encrypted += entry.key + "=" + valueStr + "\n";
    }
    
    return encrypted;
}

bool Config::decryptConfiguration(const std::string& encryptedData, const std::string& key) {
    clear();
    
    std::istringstream stream(encryptedData);
    std::string line;
    
    while (std::getline(stream, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string k = line.substr(0, pos);
            std::string v = line.substr(pos + 1);
            
            // Simple XOR decryption
            for (char& c : v) {
                c = c ^ key[0];
            }
            
            set(k, v, ConfigSource::Encrypted);
        }
    }
    
    return true;
}

bool Config::validateSignature(const std::string& signature, const std::string& data) const {
    // Simple HMAC-SHA256 validation for demonstration
    CryptoPP::HMAC<CryptoPP::SHA256> hmac((const CryptoPP::byte*)signature.c_str(), signature.size());
    
    std::string computedSignature;
    CryptoPP::StringSource ds(data, true,
        new CryptoPP::HashFilter(hmac,
            new CryptoPP::HexEncoder(
                new CryptoPP::StringSink(computedSignature)
            )
        )
    );
    
    return computedSignature == signature;
}

void Config::setEncryptionKey(const std::string& key) {
    pImpl->encryptionKey = key;
    pImpl->encryptionStatus = "Encryption key set";
}

std::string Config::getEncryptionStatus() const {
    return pImpl->encryptionStatus;
}

// Configuration export/import
bool Config::exportConfiguration(const std::string& filepath, const std::string& format) const {
    if (format == "json") {
        // TODO: Implement JSON export with nlohmann/json
        return saveToFile(filepath + ".json");
    } else if (format == "yaml") {
        // TODO: Implement YAML export with YAML-cpp
        return saveToFile(filepath + ".yaml");
    } else if (format == "ini") {
        // TODO: Implement INI export
        return saveToFile(filepath + ".ini");
    } else if (format == "csv") {
        // TODO: Implement CSV export
        return saveToFile(filepath + ".csv");
    }
    
    return false;
}

bool Config::importConfiguration(const std::string& filepath, const std::string& format) {
    clear();
    
    if (format == "json") {
        // TODO: Implement JSON import with nlohmann/json
        return loadFromFile(filepath + ".json");
    } else if (format == "yaml") {
        // TODO: Implement YAML import with YAML-cpp
        return loadFromFile(filepath + ".yaml");
    } else if (format == "ini") {
        // TODO: Implement INI import
        return loadFromFile(filepath + ".ini");
    } else if (format == "csv") {
        // TODO: Implement CSV import
        return loadFromFile(filepath + ".csv");
    }
    
    return false;
}

std::vector<std::string> Config::getExportFormats() const {
    return pImpl->exportFormats;
}

void Config::setExportPath(const std::string& path) {
    pImpl->exportPath = path;
}

std::string Config::getExportPath() const {
    return pImpl->exportPath;
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
