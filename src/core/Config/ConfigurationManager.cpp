#include "ConfigurationManager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <thread>
#include <iomanip>
#include <ctime>
#include <numeric>

namespace nlm {

struct ConfigurationManager::Impl {
    std::map<std::string, ConfigurationSection> sections;
    std::map<std::string, ConfigurationTemplate> templates;
    std::map<std::string, std::string> environmentMappings;
    std::map<std::string, std::string> sectionEnvironmentMappings;
    std::mutex sectionsMutex;
    std::mutex templatesMutex;
    bool threadSafe = true;
    bool inheritanceEnabled = false;
    std::string defaultConfigFile;
};

ConfigurationManager::ConfigurationManager()
    : autoReloadEnabled(false)
    , reloadInterval(std::chrono::milliseconds(5000))
    , reloadStopFlag(false)
    , threadSafe(true)
    , inheritanceEnabled(false)
    , defaultConfigFile("") {
    pImpl = std::make_unique<Impl>();
    reloadThread = std::thread(&ConfigurationManager::reloadThreadFunction, this);
}

ConfigurationManager::~ConfigurationManager() {
    stopAutoReload();
    if (reloadThread.joinable()) {
        reloadThread.join();
    }
}

ConfigurationManager& ConfigurationManager::getGlobal() {
    static std::shared_ptr<ConfigurationManager> instance;
    if (!instance) {
        instance = std::make_shared<ConfigurationManager>();
    }
    return *instance;
}

void ConfigurationManager::setGlobal(std::shared_ptr<ConfigurationManager> manager) {
    globalManager = manager;
}

void ConfigurationManager::addTemplate(const std::string& name, const ConfigurationTemplate& template_) {
    std::lock_guard<std::mutex> lock(pImpl->templatesMutex);
    pImpl->templates[name] = template_;
}

void ConfigurationManager::removeTemplate(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->templatesMutex);
    pImpl->templates.erase(name);
}

bool ConfigurationManager::hasTemplate(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->templatesMutex);
    return pImpl->templates.find(name) != pImpl->templates.end();
}

const ConfigurationTemplate* ConfigurationManager::getTemplate(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->templatesMutex);
    auto it = pImpl->templates.find(name);
    return it != pImpl->templates.end() ? &it->second : nullptr;
}

ConfigurationSection& ConfigurationManager::getSection(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->sectionsMutex);
    std::string normalizedName = normalizeSection(name);
    if (!hasSection(normalizedName)) {
        pImpl->sections[normalizedName] = ConfigurationSection(normalizedName);
    }
    return pImpl->sections[normalizedName];
}

const ConfigurationSection& ConfigurationManager::getSection(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->sectionsMutex);
    std::string normalizedName = normalizeSection(name);
    auto it = pImpl->sections.find(normalizedName);
    static const ConfigurationSection emptySection;
    return it != pImpl->sections.end() ? it->second : emptySection;
}

bool ConfigurationManager::hasSection(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->sectionsMutex);
    std::string normalizedName = normalizeSection(name);
    return pImpl->sections.find(normalizedName) != pImpl->sections.end();
}

void ConfigurationManager::removeSection(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->sectionsMutex);
    std::string normalizedName = normalizeSection(name);
    pImpl->sections.erase(normalizedName);
}

std::vector<std::string> ConfigurationManager::getSectionNames() const {
    std::lock_guard<std::mutex> lock(pImpl->sectionsMutex);
    std::vector<std::string> names;
    for (const auto& pair : pImpl->sections) {
        names.push_back(pair.first);
    }
    return names;
}

template<typename T>
std::optional<T> ConfigurationManager::get(const std::string& key) const {
    return get<T>("", key);
}

template<typename T>
std::optional<T> ConfigurationManager::get(const std::string& section, const std::string& key) const {
    const ConfigurationSection* targetSection = nullptr;
    
    if (section.empty()) {
        targetSection = findRootSection();
    } else {
        targetSection = getSectionPtr(section);
    }
    
    if (!targetSection) return std::nullopt;
    
    const ConfigEntry* entry = targetSection->getEntry(key);
    if (!entry) return std::nullopt;
    
    try {
        return std::get<T>(entry->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T ConfigurationManager::getOr(const std::string& key, const T& defaultValue) const {
    return getOr<T>("", key, defaultValue);
}

template<typename T>
T ConfigurationManager::getOr(const std::string& section, const std::string& key, const T& defaultValue) const {
    auto val = get<T>(section, key);
    return val.has_value() ? val.value() : defaultValue;
}

void ConfigurationManager::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    set("", key, value, source);
}

void ConfigurationManager::set(const std::string& section, const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->sectionsMutex);
    
    std::string normalizedSection = normalizeSection(section);
    std::string normalizedKey = normalizeKey(key);
    
    ConfigurationSection* targetSection = nullptr;
    if (normalizedSection.empty()) {
        targetSection = findRootSection();
    } else {
        targetSection = getSectionPtr(normalizedSection);
    }
    
    if (!targetSection) {
        targetSection = &pImpl->sections[normalizedSection];
    }
    
    ConfigEntry entry;
    entry.key = normalizedKey;
    entry.value = value;
    entry.source = source;
    entry.section = normalizedSection;
    
    targetSection->addEntry(normalizedKey, entry);
}

void ConfigurationManager::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void ConfigurationManager::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void ConfigurationManager::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void ConfigurationManager::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void ConfigurationManager::set(const std::string& section, const std::string& key, int value, ConfigSource source) {
    set(section, key, ConfigValue(value), source);
}

void ConfigurationManager::set(const std::string& section, const std::string& key, double value, ConfigSource source) {
    set(section, key, ConfigValue(value), source);
}

void ConfigurationManager::set(const std::string& section, const std::string& key, bool value, ConfigSource source) {
    set(section, key, ConfigValue(value), source);
}

void ConfigurationManager::set(const std::string& section, const std::string& key, const std::string& value, ConfigSource source) {
    set(section, key, ConfigValue(value), source);
}

void ConfigurationManager::loadFromEnvironment(const std::string& prefix) {
    std::lock_guard<std::mutex> lock(pImpl->sectionsMutex);
    
    // Root section
    auto& root = getSection("");
    
    // Load from environment with prefix
    loadFromEnvironmentPrefix(prefix, ConfigSource::Environment);
    
    // Apply environment mappings
    for (const auto& mapping : pImpl->environmentMappings) {
        const char* envValue = std::getenv(mapping.second.c_str());
        if (envValue) {
            root.addEntry(mapping.first, ConfigEntry(mapping.first, ConfigValue(std::string(envValue)), ConfigSource::Environment));
        }
    }
}

void ConfigurationManager::addEnvironmentMapping(const std::string& key, const std::string& envVar) {
    std::lock_guard<std::mutex> lock(pImpl->sectionsMutex);
    pImpl->environmentMappings[key] = envVar;
}

bool ConfigurationManager::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        parseCommandLineArg(std::string(argv[i]), ConfigSource::CommandLine);
    }
    return true;
}

bool ConfigurationManager::loadFromFile(const std::string& filepath) {
    return loadFromFile(std::filesystem::path(filepath));
}

bool ConfigurationManager::loadFromFile(const std::filesystem::path& filepath) {
    // First check if file exists and is readable
    if (!std::filesystem::exists(filepath) || !std::filesystem::is_regular_file(filepath)) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file format by extension
    std::string extension = filepath.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    bool success = false;
    if (extension == ".json") {
        success = loadFromJSON(content);
    } else if (extension == ".yaml" || extension == ".yml") {
        success = loadFromYAML(content);
    } else if (extension == ".ini" || extension == ".cfg") {
        success = loadFromINI(content);
    } else {
        // Default to simple key=value format
        std::istringstream iss(content);
        std::string line;
        while (std::getline(iss, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                if (value.size() >= 2 && 
                    ((value.front() == '\"' && value.back() == '\"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        success = true;
    }
    
    // Track last reload time
    if (success) {
        std::lock_guard<std::mutex> lock(pImpl->sectionsMutex);
        lastReloadTime = std::chrono::system_clock::now();
    }
    
    return success;
}

bool ConfigurationManager::saveToFile(const std::string& filepath) const {
    return saveToFile(std::filesystem::path(filepath));
}

bool ConfigurationManager::saveToFile(const std::filesystem::path& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file format by extension
    std::string extension = filepath.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        file << "{\n";
        size_t count = 0;
        for (const auto& [sectionName, section] : pImpl->sections) {
            if (sectionName.empty()) {
                for (const auto& [key, entry] : section.entries) {
                    file << "  \"" << key << "\": " << jsonToString(entry.value);
                    if (++count < pImpl->sections.size() * section.entries.size()) {
                        file << ",\n";
                    } else {
                        file << "\n";
                    }
                }
            }
        }
        file << "}\n";
    } else if (extension == ".yaml" || extension == ".yml") {
        for (const auto& [sectionName, section] : pImpl->sections) {
            if (!sectionName.empty()) {
                file << sectionName << ":\n";
            }
            for (const auto& [key, entry] : section.entries) {
                file << "  " << key << ": " << jsonToString(entry.value) << "\n";
            }
            if (!sectionName.empty() || !pImpl->sections.empty()) {
                file << "\n";
            }
        }
    } else {
        // Default to simple key=value format
        for (const auto& [sectionName, section] : pImpl->sections) {
            if (!sectionName.empty()) {
                file << "[" << sectionName << "]\n";
            }
            for (const auto& [key, entry] : section.entries) {
                file << "# " << entry.description << "\n";
                file << key << " = " << jsonToString(entry.value) << "\n";
            }
            if (!sectionName.empty() || !pImpl->sections.empty()) {
                file << "\n";
            }
        }
    }
    
    return true;
}

bool ConfigurationManager::loadFromJSON(const std::string& content) {
    // Simple JSON parsing (can be enhanced with a proper JSON library)
    std::istringstream iss(content);
    std::string line;
    std::string currentSection = "";
    
    while (std::getline(iss, line)) {
        line = trim(line);
        
        // Skip comments and empty lines
        if (line.empty() || line[0] == '/') {
            continue;
        }
        
        // Check for section
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        
        // Parse key:value pairs
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove surrounding quotes
            if (value.size() >= 2 && ((value.front() == '\"' && value.back() == '\"') ||
                                     (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Parse value based on content
            ConfigValue configValue;
            parseINIValue(value, configValue);
            
            set(currentSection, key, configValue, ConfigSource::File);
        }
    }
    
    return true;
}

bool ConfigurationManager::loadFromYAML(const std::string& content) {
    // Simple YAML parsing (can be enhanced with a proper YAML library)
    return loadFromINI(content); // For now, use INI parser as YAML is similar
}

bool ConfigurationManager::loadFromINI(const std::string& content) {
    // INI format parsing
    std::istringstream iss(content);
    std::string line;
    std::string currentSection = "";
    
    while (std::getline(iss, line)) {
        line = trim(line);
        
        // Skip comments and empty lines
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }
        
        // Check for section
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes
            if (value.size() >= 2 && ((value.front() == '\"' && value.back() == '\"') ||
                                     (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Parse value
            ConfigValue configValue;
            parseINIValue(value, configValue);
            
            set(currentSection, key, configValue, ConfigSource::File);
        }
    }
    
    return true;
}

std::vector<ValidationResult> ConfigurationManager::validate() const {
    std::vector<ValidationResult> results;
    
    for (const auto& [sectionName, section] : pImpl->sections) {
        for (const auto& [key, entry] : section.entries) {
            ValidationResult result;
            result.fieldName = sectionName.empty() ? key : sectionName + "." + key;
            
            // Check type constraints (simplified)
            // Note: ConfigValue variant includes all supported types, so no type validation needed
            if (entry.source == ConfigSource::File && entry.isLocked) {
                result.isValid = false;
                result.errorMessage = "File-configured value is locked";
                results.push_back(result);
                result.isValid = false;
                result.errorMessage = "File-configured value is locked";
                results.push_back(result);
            }
        }
    }
    
    return results;
}

bool ConfigurationManager::isValid() const {
    return validate().empty();
}

std::string ConfigurationManager::getValidationError() const {
    auto results = validate();
    if (results.empty()) return "";
    
    return results[0].errorMessage + " (field: " + results[0].fieldName + ")";
}

void ConfigurationManager::applyTemplate(const std::string& templateName, const std::map<std::string, ConfigValue>& values) {
    std::lock_guard<std::mutex> lock(pImpl->templatesMutex);
    auto it = pImpl->templates.find(templateName);
    if (it == pImpl->templates.end()) return;
    
    const auto& schema = it->second.schema;
    
    for (const auto& [key, value] : values) {
        auto schemaIt = schema.find(key);
        if (schemaIt != schema.end() && schemaIt->second.section.empty()) {
            set(key, value, ConfigSource::Template);
        }
    }
}

bool ConfigurationManager::validateAgainstTemplate(const std::string& templateName) const {
    std::lock_guard<std::mutex> lock(pImpl->templatesMutex);
    auto it = pImpl->templates.find(templateName);
    if (it == pImpl->templates.end()) return false;
    
    const auto& template_ = it->second;
    
    // Collect all root-level values
    std::map<std::string, ConfigValue> values;
    for (const auto& [sectionName, section] : pImpl->sections) {
        if (sectionName.empty()) {
            for (const auto& [key, entry] : section.entries) {
                values[key] = entry.value;
            }
        }
    }
    
    return template_.validate(values);
}

ConfigSource ConfigurationManager::getSource(const std::string& key) const {
    return getSource("", key);
}

ConfigSource ConfigurationManager::getSource(const std::string& section, const std::string& key) const {
    const ConfigurationSection* targetSection = nullptr;
    
    if (section.empty()) {
        targetSection = findRootSection();
    } else {
        targetSection = getSectionPtr(section);
    }
    
    if (!targetSection) return ConfigSource::Default;
    
    const ConfigEntry* entry = targetSection->getEntry(key);
    return entry ? entry->source : ConfigSource::Default;
}

void ConfigurationManager::lock(const std::string& key) {
    lock("", key);
}

void ConfigurationManager::lock(const std::string& section, const std::string& key) {
    ConfigEntry* entry = getEntryPtr(section, key);
    if (entry) {
        entry->isLocked = true;
    }
}

void ConfigurationManager::unlock(const std::string& key) {
    unlock("", key);
}

void ConfigurationManager::unlock(const std::string& section, const std::string& key) {
    ConfigEntry* entry = getEntryPtr(section, key);
    if (entry) {
        entry->isLocked = false;
    }
}

bool ConfigurationManager::isLocked(const std::string& key) const {
    return isLocked("", key);
}

bool ConfigurationManager::isLocked(const std::string& section, const std::string& key) const {
    const ConfigEntry* entry = getEntryPtr(section, key);
    return entry ? entry->isLocked : false;
}

void ConfigurationManager::remove(const std::string& key) {
    remove("", key);
}

void ConfigurationManager::remove(const std::string& section, const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->sectionsMutex);
    
    std::string normalizedSection = normalizeSection(section);
    std::string normalizedKey = normalizeKey(key);
    
    if (normalizedSection.empty()) {
        auto& root = getSection("");
        root.entries.erase(normalizedKey);
    } else {
        auto it = pImpl->sections.find(normalizedSection);
        if (it != pImpl->sections.end()) {
            it->second.entries.erase(normalizedKey);
            
            // Remove empty section
            if (it->second.entries.empty() && it->second.subsections.empty()) {
                pImpl->sections.erase(it);
            }
        }
    }
}

bool ConfigurationManager::has(const std::string& key) const {
    return has("", key);
}

bool ConfigurationManager::has(const std::string& section, const std::string& key) const {
    const ConfigurationSection* targetSection = nullptr;
    
    if (section.empty()) {
        targetSection = findRootSection();
    } else {
        targetSection = getSectionPtr(section);
    }
    
    return targetSection && targetSection->hasEntry(key);
}

std::vector<std::string> ConfigurationManager::getKeys() const {
    return getKeys("");
}

std::vector<std::string> ConfigurationManager::getKeys(const std::string& section) const {
    const ConfigurationSection* targetSection = nullptr;
    
    if (section.empty()) {
        targetSection = findRootSection();
    } else {
        targetSection = getSectionPtr(section);
    }
    
    if (!targetSection) return {};
    return targetSection->getKeys();
}

void ConfigurationManager::clear() {
    std::lock_guard<std::mutex> lock(pImpl->sectionsMutex);
    pImpl->sections.clear();
}

void ConfigurationManager::clear(const std::string& section) {
    std::lock_guard<std::mutex> lock(pImpl->sectionsMutex);
    
    std::string normalizedSection = normalizeSection(section);
    
    if (normalizedSection.empty()) {
        clear();
    } else {
        auto it = pImpl->sections.find(normalizedSection);
        if (it != pImpl->sections.end()) {
            it->second.entries.clear();
            it->second.subsections.clear();
            
            // Remove empty section
            if (it->second.entries.empty() && it->second.subsections.empty()) {
                pImpl->sections.erase(it);
            }
        }
    }
}

void ConfigurationManager::setAutoReload(bool enable, std::chrono::milliseconds interval) {
    if (threadSafe) {
        std::lock_guard<std::mutex> lock(mutex);
    }
    
    autoReloadEnabled = enable;
    reloadInterval = interval;
    
    if (enable && !reloadThread.joinable()) {
        reloadThread = std::thread(&ConfigurationManager::reloadThreadFunction, this);
    } else if (!enable && reloadThread.joinable()) {
        reloadStopFlag = true;
        if (reloadThread.joinable()) {
            reloadThread.join();
        }
    }
}

void ConfigurationManager::stopAutoReload() {
    setAutoReload(false);
}

std::chrono::milliseconds ConfigurationManager::getReloadInterval() const {
    return reloadInterval;
}

std::string ConfigurationManager::summary() const {
    std::ostringstream oss;
    oss << "ConfigurationManager Summary (" << getSectionCount() << " sections)\n";
    oss << "Auto-reload: " << (autoReloadEnabled ? "Enabled" : "Disabled") << " (" << reloadInterval.count() << "ms)\n";
    oss << "Thread-safe: " << (threadSafe ? "Yes" : "No") << "\n";
    oss << "Inheritance: " << (inheritanceEnabled ? "Enabled" : "Disabled") << "\n";
    oss << "Last reload: " << getLastReloadTime() << "\n\n";
    
    for (const auto& [sectionName, section] : pImpl->sections) {
        oss << section.summary() << "\n";
    }
    
    return oss.str();
}

std::string ConfigurationManager::summary(const std::string& section) const {
    const ConfigurationSection* targetSection = nullptr;
    
    if (section.empty()) {
        targetSection = findRootSection();
    } else {
        targetSection = getSectionPtr(section);
    }
    
    if (!targetSection) return "Section not found: " + section + "\n";
    
    return targetSection->summary();
}

void ConfigurationManager::setThreadSafe(bool enable) {
    threadSafe = enable;
}

bool ConfigurationManager::isThreadSafe() const {
    return threadSafe;
}

void ConfigurationManager::setInheritance(bool enable) {
    inheritanceEnabled = enable;
}

bool ConfigurationManager::hasInheritance() const {
    return inheritanceEnabled;
}

void ConfigurationManager::setDefaultFile(const std::string& filepath) {
    defaultConfigFile = filepath;
}

bool ConfigurationManager::loadDefaultConfiguration() {
    if (defaultConfigFile.empty()) return false;
    return loadFromFile(defaultConfigFile);
}

size_t ConfigurationManager::getEntryCount() const {
    size_t count = 0;
    for (const auto& [sectionName, section] : pImpl->sections) {
        count += section.entries.size();
        for (const auto& [subName, subSection] : section.subsections) {
            count += subSection.entries.size();
        }
    }
    return count;
}

size_t ConfigurationManager::getSectionCount() const {
    return pImpl->sections.size();
}

// Private implementation methods
ConfigurationSection* ConfigurationManager::getSectionPtr(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->sectionsMutex);
    std::string normalizedName = normalizeSection(name);
    auto it = pImpl->sections.find(normalizedName);
    return it != pImpl->sections.end() ? &it->second : nullptr;
}

const ConfigurationSection* ConfigurationManager::getSectionPtr(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->sectionsMutex);
    std::string normalizedName = normalizeSection(name);
    auto it = pImpl->sections.find(normalizedName);
    return it != pImpl->sections.end() ? &it->second : nullptr;
}

ConfigEntry* ConfigurationManager::getEntryPtr(const std::string& section, const std::string& key) {
    ConfigurationSection* targetSection = getSectionPtr(section);
    if (!targetSection) return nullptr;
    
    auto it = targetSection->entries.find(key);
    return it != targetSection->entries.end() ? &it->second : nullptr;
}

const ConfigEntry* ConfigurationManager::getEntryPtr(const std::string& section, const std::string& key) const {
    const ConfigurationSection* targetSection = getSectionPtr(section);
    if (!targetSection) return nullptr;
    
    auto it = targetSection->entries.find(key);
    return it != targetSection->entries.end() ? &it->second : nullptr;
}

std::string ConfigurationManager::normalizeKey(const std::string& key) const {
    std::string result = key;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string ConfigurationManager::normalizeSection(const std::string& section) const {
    std::string result = section;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

ConfigurationSection* ConfigurationManager::findRootSection() {
    auto it = pImpl->sections.find("");
    return it != pImpl->sections.end() ? &it->second : nullptr;
}

const ConfigurationSection* ConfigurationManager::findRootSection() const {
    auto it = pImpl->sections.find("");
    return it != pImpl->sections.end() ? &it->second : nullptr;
}

bool ConfigurationManager::loadFromEnvironmentPrefix(const std::string& prefix, ConfigSource source) {
    bool success = false;
    
    for (const char* env = std::getenv("PATH"); env != nullptr; env = std::getenv(nullptr)) {
        // This is a simplified environment variable loader
        // In a real implementation, you would iterate through actual environment variables
    }
    
    // For demonstration, add some common NLM environment variables
    const char* home = std::getenv("HOME");
    if (home) {
        set("home_path", std::string(home), source);
        success = true;
    }
    
    const char* user = std::getenv("USER");
    if (user) {
        set("user_name", std::string(user), source);
        success = true;
    }
    
    return success;
}

bool ConfigurationManager::parseCommandLineArg(const std::string& arg, ConfigSource source) {
    // Handle --key=value format
    if (arg.substr(0, 2) == "--") {
        size_t pos = arg.find('=');
        if (pos != std::string::npos) {
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            set(key, value, source);
            return true;
        }
    }
    // Handle -key value format (simplified - would need lookahead in real implementation)
    else if (arg[0] == '-') {
        // For now, treat as simple key
        set(arg.substr(1), "true", source);
        return true;
    }
    
    return false;
}

void ConfigurationManager::mergeConfiguration(const ConfigurationManager& other, bool override) {
    std::lock_guard<std::mutex> lock1(pImpl->sectionsMutex);
    std::lock_guard<std::mutex> lock2(other.pImpl->sectionsMutex);
    
    for (const auto& [sectionName, section] : other.pImpl->sections) {
        auto* targetSection = getSectionPtr(sectionName);
        if (targetSection) {
            targetSection->mergeFrom(section, override);
        } else {
            pImpl->sections[sectionName] = section;
        }
    }
}

void ConfigurationManager::applyEnvironmentVariables() {
    loadFromEnvironment();
}

std::string ConfigurationManager::jsonToString(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(6) << std::get<double>(value);
        return oss.str();
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        return "\"" + std::get<std::string>(value) + "\"";
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        std::ostringstream oss;
        oss << "[";
        const auto& vec = std::get<std::vector<int>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        oss << "]";
        return oss.str();
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        std::ostringstream oss;
        oss << "[";
        const auto& vec = std::get<std::vector<double>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        oss << "]";
        return oss.str();
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        std::ostringstream oss;
        oss << "[";
        const auto& vec = std::get<std::vector<std::string>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << vec[i] << "\"";
        }
        oss << "]";
        return oss.str();
    }
    
    return "null";
}

void ConfigurationManager::reloadThreadFunction() {
    while (!reloadStopFlag) {
        std::this_thread::sleep_for(reloadInterval);
        
        if (reloadStopFlag) break;
        
        // Check if default config file exists and reload if needed
        if (!defaultConfigFile.empty() && std::filesystem::exists(defaultConfigFile)) {
            auto now = std::chrono::system_clock::now();
            auto fileTime = std::chrono::filesystem::last_write_time(defaultConfigFile);
            
            if (now - fileTime > reloadInterval) {
                loadDefaultConfiguration();
            }
        }
    }
}

std::string ConfigurationManager::getLastReloadTime() const {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastReloadTime);
    
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
    
    return std::string(buffer) + " (" + std::to_string(duration.count()) + "s ago)";
}

bool ConfigurationManager::parseINIValue(const std::string& str, ConfigValue& outValue) const {
    // Trim whitespace
    std::string value = trim(str);
    
    // Try to detect boolean
    if (value == "true" || value == "false") {
        outValue = (value == "true");
        return true;
    }
    
    // Try to detect integer
    bool isInt = true;
    bool negative = false;
    size_t start = 0;
    if (value[0] == '-') {
        negative = true;
        start = 1;
    }
    for (size_t i = start; i < value.size(); ++i) {
        if (!std::isdigit(value[i])) {
            isInt = false;
            break;
        }
    }
    if (isInt) {
        long long intVal = std::stoll(value);
        if (intVal >= std::numeric_limits<int>::min() && intVal <= std::numeric_limits<int>::max()) {
            outValue = static_cast<int>(intVal);
            return true;
        } else if (intVal >= std::numeric_limits<int64_t>::min() && intVal <= std::numeric_limits<int64_t>::max()) {
            outValue = intVal;
            return true;
        }
    }
    
    // Try to detect double
    try {
        size_t pos;
        double doubleVal = std::stod(value, &pos);
        if (pos == value.size()) {
            outValue = doubleVal;
            return true;
        }
    } catch (...) {}
    
    // Default to string (remove surrounding quotes if present)
    if (value.size() >= 2 && ((value.front() == '\"' && value.back() == '\"') ||
                             (value.front() == '\'' && value.back() == '\''))) {
        outValue = value.substr(1, value.size() - 2);
    } else {
        outValue = value;
    }
    
    return true;
}

std::string ConfigurationManager::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Explicit template instantiations for ConfigurationManager
template std::optional<int> ConfigurationManager::get<int>(const std::string&) const;
template std::optional<int> ConfigurationManager::get<int>(const std::string&, const std::string&) const;

template std::optional<int64_t> ConfigurationManager::get<int64_t>(const std::string&) const;
template std::optional<int64_t> ConfigurationManager::get<int64_t>(const std::string&, const std::string&) const;

template std::optional<double> ConfigurationManager::get<double>(const std::string&) const;
template std::optional<double> ConfigurationManager::get<double>(const std::string&, const std::string&) const;

template std::optional<bool> ConfigurationManager::get<bool>(const std::string&) const;
template std::optional<bool> ConfigurationManager::get<bool>(const std::string&, const std::string&) const;

template std::optional<std::string> ConfigurationManager::get<std::string>(const std::string&) const;
template std::optional<std::string> ConfigurationManager::get<std::string>(const std::string&, const std::string&) const;

template int ConfigurationManager::getOr<int>(const std::string&, const int&) const;
template int ConfigurationManager::getOr<int>(const std::string&, const std::string&, const int&) const;

template int64_t ConfigurationManager::getOr<int64_t>(const std::string&, const int64_t&) const;
template int64_t ConfigurationManager::getOr<int64_t>(const std::string&, const std::string&, const int64_t&) const;

template double ConfigurationManager::getOr<double>(const std::string&, const double&) const;
template double ConfigurationManager::getOr<double>(const std::string&, const std::string&, const double&) const;

template bool ConfigurationManager::getOr<bool>(const std::string&, const bool&) const;
template bool ConfigurationManager::getOr<bool>(const std::string&, const std::string&, const bool&) const;

template std::string ConfigurationManager::getOr<std::string>(const std::string&, const std::string&) const;
template std::string ConfigurationManager::getOr<std::string>(const std::string&, const std::string&, const std::string&) const;

#endif // namespace nlm