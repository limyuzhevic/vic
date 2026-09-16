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
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
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

    std::optional<int> Config::get<int>(const std::string& key) const {
        auto val = get<int>(key);
        return val;
    }
    
    std::optional<int64_t> Config::get<int64_t>(const std::string& key) const {
        auto val = get<int64_t>(key);
        return val;
    }
    
    std::optional<double> Config::get<double>(const std::string& key) const {
        auto val = get<double>(key);
        return val;
    }
    
    std::optional<bool> Config::get<bool>(const std::string& key) const {
        auto val = get<bool>(key);
        return val;
    }
    
    std::optional<std::string> Config::get<std::string>(const std::string& key) const {
        auto val = get<std::string>(key);
        return val;
    }
    
    int Config::getOr<int>(const std::string& key, const int& defaultValue) const {
        auto val = get<int>(key);
        return val.has_value() ? val.value() : defaultValue;
    }
    
    int64_t Config::getOr<int64_t>(const std::string& key, const int64_t& defaultValue) const {
        auto val = get<int64_t>(key);
        return val.has_value() ? val.value() : defaultValue;
    }
    
    double Config::getOr<double>(const std::string& key, const double& defaultValue) const {
        auto val = get<double>(key);
        return val.has_value() ? val.value() : defaultValue;
    }
    
    bool Config::getOr<bool>(const std::string& key, const bool& defaultValue) const {
        auto val = get<bool>(key);
        return val.has_value() ? val.value() : defaultValue;
    }
    
    std::string Config::getOr<std::string>(const std::string& key, const std::string& defaultValue) const {
        auto val = get<std::string>(key);
        return val.has_value() ? val.value() : defaultValue;
    }
    
    // ===== CONVENIENCE METHODS =====
    
    int Config::getInt(const std::string& key) const {
        auto val = get<int>(key);
        if (!val) throw ConfigurationError("Configuration key not found: " + key);
        return *val;
    }
    
    int64_t Config::getInt64(const std::string& key) const {
        auto val = get<int64_t>(key);
        if (!val) throw ConfigurationError("Configuration key not found: " + key);
        return *val;
    }
    
    double Config::getDouble(const std::string& key) const {
        auto val = get<double>(key);
        if (!val) throw ConfigurationError("Configuration key not found: " + key);
        return *val;
    }
    
    bool Config::getBool(const std::string& key) const {
        auto val = get<bool>(key);
        if (!val) throw ConfigurationError("Configuration key not found: " + key);
        return *val;
    }
    
    std::string Config::getString(const std::string& key) const {
        auto val = get<std::string>(key);
        if (!val) throw ConfigurationError("Configuration key not found: " + key);
        return *val;
    }
    
    std::vector<int> Config::getIntArray(const std::string& key) const {
        auto val = get<std::vector<int>>(key);
        return val ? *val : std::vector<int>();
    }
    
    std::vector<double> Config::getDoubleArray(const std::string& key) const {
        auto val = get<std::vector<double>>(key);
        return val ? *val : std::vector<double>();
    }
    
    std::vector<std::string> Config::getStringArray(const std::string& key) const {
        auto val = get<std::vector<std::string>>(key);
        return val ? *val : std::vector<std::string>();
    }
    
    // ===== CONVENIENCE SETTERS =====
    
    void Config::setInt(const std::string& key, int value, ConfigSource source) {
        set(key, ConfigValue(value), source);
    }
    
    void Config::setInt64(const std::string& key, int64_t value, ConfigSource source) {
        set(key, ConfigValue(value), source);
    }
    
    void Config::setDouble(const std::string& key, double value, ConfigSource source) {
        set(key, ConfigValue(value), source);
    }
    
    void Config::setBool(const std::string& key, bool value, ConfigSource source) {
        set(key, ConfigValue(value), source);
    }
    
    void Config::setString(const std::string& key, const std::string& value, ConfigSource source) {
        set(key, ConfigValue(value), source);
    }
    
    void Config::setIntArray(const std::string& key, const std::vector<int>& value, ConfigSource source) {
        set(key, ConfigValue(value), source);
    }
    
    void Config::setDoubleArray(const std::string& key, const std::vector<double>& value, ConfigSource source) {
        set(key, ConfigValue(value), source);
    }
    
    void Config::setStringArray(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
        set(key, ConfigValue(value), source);
    }
    
    // ===== VALIDATION =====
    
    bool Config::validate(const std::string& key, const std::string& type) const {
        // Simple validation based on type
        auto val = get<std::string>(key);
        if (!val) return false;
        
        if (type == "int") {
            try { std::stoi(*val); return true; }
            catch (...) { return false; }
        } else if (type == "double") {
            try { std::stod(*val); return true; }
            catch (...) { return false; }
        } else if (type == "bool") {
            std::string lower = *val;
            for (char& c : lower) c = std::tolower(c);
            return lower == "true" || lower == "false" || lower == "1" || lower == "0";
        }
        return true;
    }
    
    bool Config::validateInt(const std::string& key) const {
        auto val = get<std::string>(key);
        if (!val) return false;
        try { std::stoi(*val); return true; }
        catch (...) { return false; }
    }
    
    bool Config::validateDouble(const std::string& key) const {
        auto val = get<std::string>(key);
        if (!val) return false;
        try { std::stod(*val); return true; }
        catch (...) { return false; }
    }
    
    bool Config::validateBool(const std::string& key) const {
        auto val = get<std::string>(key);
        if (!val) return false;
        std::string lower = *val;
        for (char& c : lower) c = std::tolower(c);
        return lower == "true" || lower == "false" || lower == "1" || lower == "0";
    }
    
    bool Config::validateString(const std::string& key) const {
        return has(key);
    }
    
    bool Config::validateIntArray(const std::string& key) const {
        auto val = get<std::vector<int>>(key);
        return val.has_value();
    }
    
    bool Config::validateDoubleArray(const std::string& key) const {
        auto val = get<std::vector<double>>(key);
        return val.has_value();
    }
    
    bool Config::validateStringArray(const std::string& key) const {
        auto val = get<std::vector<std::string>>(key);
        return val.has_value();
    }
    
    // ===== MERGING AND COMPARISON =====
    
    void Config::merge(const Config& other) {
        auto keys = other.getKeys();
        for (const auto& key : keys) {
            auto val = other.get<std::string>(key);
            if (val) {
                set(key, ConfigValue(*val), ConfigSource::Runtime);
            }
        }
    }
    
    bool Config::equals(const Config& other) const {
        auto keys1 = getKeys();
        auto keys2 = other.getKeys();
        
        if (keys1.size() != keys2.size()) return false;
        
        for (const auto& key : keys1) {
            if (!other.has(key)) return false;
            
            auto val1 = get<std::string>(key);
            auto val2 = other.get<std::string>(key);
            
            if (!val1 || !val2 || *val1 != *val2) return false;
        }
        
        return true;
    }
    
    // ===== EXPORT/IMPORT =====
    
    bool Config::exportToJSON(const std::string& filepath) const {
        return saveToFile(filepath);
    }
    
    bool Config::importFromJSON(const std::string& filepath) {
        return loadFromFile(filepath);
    }
    
    // ===== STATISTICS =====
    
    size_t Config::getEntryCount() const {
        return pImpl->entries.size();
    }
    
    std::vector<std::string> Config::getKeysBySource(ConfigSource source) const {
        std::vector<std::string> keys;
        for (const auto& entry : pImpl->entries) {
            if (entry.source == source) {
                keys.push_back(entry.key);
            }
        }
        return keys;
    }
    
    std::string Config::getSourceString(ConfigSource source) {
        switch (source) {
            case ConfigSource::Default: return "Default";
            case ConfigSource::File: return "File";
            case ConfigSource::CommandLine: return "CommandLine";
            case ConfigSource::Runtime: return "Runtime";
            default: return "Unknown";
        }
    }
    
    // ===== PRESETS =====
    
    std::shared_ptr<Config> Config::createDefaultConfig() {
        return std::make_shared<Config>();
    }
    
    std::shared_ptr<Config> Config::createEfficientConfig() {
        auto config = std::make_shared<Config>();
        config->setInt("brain.neuronCount", 10000);
        config->setDouble("brain.synapticDensity", 0.1);
        config->setBool("brain.enablePlasticity", true);
        config->setBool("brain.enableDevelopment", false);
        config->setDouble("simulation.timeStep", 0.001);
        config->setInt("simulation.maxSteps", 10000);
        return config;
    }
    
    std::shared_ptr<Config> Config::createLearningConfig() {
        auto config = std::make_shared<Config>();
        config->setInt("brain.neuronCount", 50000);
        config->setDouble("brain.synapticDensity", 0.2);
        config->setBool("brain.enablePlasticity", true);
        config->setBool("brain.enableDevelopment", true);
        config->setBool("brain.enableCuriosity", true);
        config->setDouble("simulation.timeStep", 0.001);
        config->setInt("simulation.maxSteps", 50000);
        return config;
    }
    
    std::shared_ptr<Config> Config::createExplorationConfig() {
        auto config = std::make_shared<Config>();
        config->setInt("brain.neuronCount", 20000);
        config->setDouble("brain.synapticDensity", 0.15);
        config->setBool("brain.enablePlasticity", true);
        config->setBool("brain.enableDevelopment", true);
        config->setBool("brain.enableCuriosity", true);
        config->setBool("brain.enableNovelty", true);
        config->setDouble("simulation.timeStep", 0.002);
        config->setInt("simulation.maxSteps", 20000);
        return config;
    }
    
    std::shared_ptr<Config> Config::createMinimalConfig() {
        auto config = std::make_shared<Config>();
        config->setInt("brain.neuronCount", 100);
        config->setDouble("brain.synapticDensity", 0.05);
        config->setBool("brain.enablePlasticity", false);
        config->setBool("brain.enableDevelopment", false);
        config->setDouble("simulation.timeStep", 0.01);
        config->setInt("simulation.maxSteps", 1000);
        return config;
    }
    
    std::shared_ptr<Config> Config::createHighPerformanceConfig() {
        auto config = std::make_shared<Config>();
        config->setInt("brain.neuronCount", 100000);
        config->setDouble("brain.synapticDensity", 0.05);
        config->setBool("brain.enablePlasticity", true);
        config->setBool("brain.enableDevelopment", true);
        config->setBool("brain.enableCuriosity", true);
        config->setBool("brain.enableNovelty", true);
        config->setBool("brain.enablePrediction", true);
        config->setDouble("simulation.timeStep", 0.0005);
        config->setInt("simulation.maxSteps", 100000);
        config->setBool("simulation.parallelProcessing", true);
        config->setBool("memory.efficientMode", true);
        return config;
    }
    
    // ===== HELPER =====
    
    std::string Config::getSourceName(ConfigSource source) {
        return getSourceString(source);
    }

} // namespace nlm
