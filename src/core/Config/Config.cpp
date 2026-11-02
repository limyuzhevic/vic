#include "Config.hpp"

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ValidationRule> validationRules;
    ConfigStats stats;
    std::function<void(const ConfigError&)> validationCallback;
    std::function<void(const ConfigError&)> errorCallback;
    std::function<void(const std::string& key, const ConfigValue& value)> changeCallback;
    
    Impl() {
        stats.lastModified = std::chrono::system_clock::now();
        initializeDefaultEntries();
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::loadFromFile(const std::string& filepath) {
    validateKeyName(filepath);
    
    std::string content;
    if (!readFile(filepath, content)) {
        throw ConfigLoadError(filepath, "Failed to read file");
    }
    
    // Detect file format
    std::string format = "json";
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".yaml") {
        format = "yaml";
    } else if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".yml") {
        format = "yaml";
    } else if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        format = "json";
    }
    
    // Parse based on format
    bool success = false;
    if (format == "json") {
        success = parseJSON(content);
    } else if (format == "yaml") {
        success = parseYAML(content);
    } else {
        success = parseSimple(content);
    }
    
    if (!success) {
        throw ConfigLoadError(filepath, "Failed to parse configuration");
    }
    
    updateStatistics();
}

void Config::loadFromArgs(int argc, char** argv) {
    parseCommandLineArgs(argc, argv);
    updateStatistics();
}

void Config::saveToFile(const std::string& filepath, const std::string& format) const {
    validateKeyName(filepath);
    
    if (!isFileWritable(filepath)) {
        throw ConfigIOException(filepath, "File is not writable", false, false);
    }
    
    std::string content;
    if (format == "json") {
        content = exportToJSON();
    } else if (format == "yaml") {
        // TODO: Implement YAML export
        content = exportToJSON(); // Fallback to JSON
    } else {
        // TODO: Implement simple format export
        content = exportToJSON(); // Fallback to JSON
    }
    
    if (!writeFile(filepath, content)) {
        throw ConfigIOException(filepath, "Failed to write file", true);
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    validateKeyName(key);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::visit([](auto&& v) -> std::optional<T> {
            using V = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<V, T>) {
                return v;
            } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
                return static_cast<T>(v);
            } else {
                throw ConfigValueError("", "Type mismatch", typeid(T).name(), typeid(V).name());
            }
        }, it->value);
    } catch (const ConfigError&) {
        throw;
    } catch (...) {
        throw ConfigValueError(key, "", typeid(T).name(), "Unknown conversion error");
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, 
                 ConfigSource source, bool validate) {
    validateKeyName(key);
    
    if (validate) {
        validateValue(key, value, source);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    ConfigEntry entry;
    if (it != pImpl->entries.end()) {
        entry = *it;
        pImpl->entries.erase(it);
    }
    
    entry.key = key;
    entry.value = value;
    entry.source = source;
    entry.resetErrors();
    updateLastModified();
    
    pImpl->entries.push_back(entry);
    
    // Call change callback if set
    if (changeCallback_) {
        changeCallback_(key, value);
    }
}

void Config::set(const std::string& key, const std::string& value,
                 ConfigSource source) {
    set(key, ConfigValue(value), source, true);
}

void Config::set(const std::string& key, int value,
                 ConfigSource source) {
    set(key, ConfigValue(value), source, true);
}

void Config::set(const std::string& key, int64_t value,
                 ConfigSource source) {
    set(key, ConfigValue(value), source, true);
}

void Config::set(const std::string& key, double value,
                 ConfigSource source) {
    set(key, ConfigValue(value), source, true);
}

void Config::set(const std::string& key, bool value,
                 ConfigSource source) {
    set(key, ConfigValue(value), source, true);
}

bool Config::has(const std::string& key) const {
    validateKeyName(key);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    validateKeyName(key);
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    pImpl->entries.erase(it, pImpl->entries.end());
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
    updateStatistics();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary:\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "Age: " << getAge() << " seconds\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = " << valueToString(entry.value) 
            << " (source: " << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}

std::vector<ConfigEntry> Config::getEntriesBySource(ConfigSource source) const {
    std::vector<ConfigEntry> result;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            result.push_back(entry);
        }
    }
    return result;
}

std::vector<ConfigErrorPtr> Config::validateAll() const {
    std::vector<ConfigErrorPtr> errors;
    
    for (const auto& entry : pImpl->entries) {
        try {
            validateEntry(entry);
        } catch (const ConfigError& e) {
            errors.push_back(std::make_shared<ConfigError>(e));
        }
    }
    
    return errors;
}

void Config::setValidationCallback(std::function<void(const ConfigError&)> callback) {
    validationCallback_ = callback;
}

void Config::setErrorCallback(std::function<void(const ConfigError&)> callback) {
    errorCallback_ = callback;
}

size_t Config::size() const {
    return pImpl->entries.size();
}

bool Config::empty() const {
    return pImpl->entries.empty();
}

double Config::getAge() const {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->stats.lastModified);
    return static_cast<double>(duration.count());
}

void Config::resetToDefaults() {
    clear();
    initializeDefaultEntries();
    updateStatistics();
}

void Config::merge(const Config& other, bool override) {
    for (const auto& entry : other.pImpl->entries) {
        if (!override && has(entry.key)) {
            continue;
        }
        pImpl->entries.push_back(entry);
    }
    updateStatistics();
}

std::string Config::getStatistics() const {
    std::ostringstream oss;
    oss << "Configuration Statistics:\n";
    oss << "Total entries: " << pImpl->stats.totalEntries << "\n";
    oss << "Default entries: " << pImpl->stats.entriesBySource[0] << "\n";
    oss << "File entries: " << pImpl->stats.entriesBySource[1] << "\n";
    oss << "CommandLine entries: " << pImpl->stats.entriesBySource[2] << "\n";
    oss << "Runtime entries: " << pImpl->stats.entriesBySource[3] << "\n";
    oss << "Validation errors: " << pImpl->stats.validationErrors << "\n";
    oss << "Age: " << getAge() << " seconds\n";
    
    return oss.str();
}

std::string Config::exportToJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  \"" << entry.key << "\": " << valueToString(entry.value);
        if (i < pImpl->entries.size() - 1) {
            oss << ",\n";
        } else {
            oss << "\n";
        }
    }
    oss << "}\n";
    return oss.str();
}

void Config::importFromJSON(const std::string& jsonString) {
    // Simple JSON parsing (TODO: Use proper JSON parser)
    // This is a placeholder implementation
    clear();
    // Parse jsonString and populate entries
    updateStatistics();
}

const ConfigEntry* Config::getEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    return (it != pImpl->entries.end()) ? &(*it) : nullptr;
}

void Config::setEntryDescription(const std::string& key, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        it->description = description;
    }
}

void Config::addValidationRule(const std::string& key, const std::string& rules) {
    ValidationRule rule;
    rule.key = key;
    rule.rules = rules;
    rule.timestamp = std::chrono::system_clock::now();
    pImpl->validationRules.push_back(rule);
}

void Config::removeValidationRule(const std::string& key) {
    auto it = std::remove_if(pImpl->validationRules.begin(), pImpl->validationRules.end(),
        [&key](const ValidationRule& r) { return r.key == key; });
    pImpl->validationRules.erase(it, pImpl->validationRules.end());
}

std::string Config::getValidationRules(const std::string& key) const {
    auto it = std::find_if(pImpl->validationRules.begin(), pImpl->validationRules.end(),
        [&key](const ValidationRule& r) { return r.key == key; });n    
    return (it != pImpl->validationRules.end()) ? it->rules : "";
}

void Config::setChangeCallback(std::function<void(const std::string& key, const ConfigValue& value)> callback) {
    changeCallback_ = callback;
}

std::string Config::getVersion() {
    return "2.0";
}

std::string Config::getDefaultFilepath() {
    return "config.json";
}

bool Config::fileExists(const std::string& filepath) {
    return std::filesystem::exists(filepath);
}

std::vector<std::string> Config::getSupportedFormats() {
    return {"json", "yaml", "simple"};
}

std::vector<std::string> Config::getSupportedTypes() {
    return {"int", "int64_t", "uint64_t", "float", "double", "bool", "string", "vector<int>", "vector<double>", "vector<string>"};
}

std::string Config::getDocumentation() {
    return "Enhanced Configuration System with comprehensive error handling and validation.";
}

void Config::validateKeyName(const std::string& key) {
    if (key.empty()) {
        throw ConfigValidationError("", "Key name cannot be empty");
    }
    
    // Check for forbidden characters
    for (char c : key) {
        if (std::ispunct(c) && c != '_' && c != '-') {
            throw ConfigSecurityError(key, std::string("Key contains invalid characters: ") + c);
        }
    }
    
    // Check for forbidden patterns
    for (const auto& pattern : FORBIDDEN_KEY_PATTERNS) {
        if (key.find(pattern) != std::string::npos) {
            throw ConfigSecurityError(key, "Key contains forbidden pattern: " + pattern);
        }
    }
}

void Config::validateValue(const std::string& key, const ConfigValue& value, ConfigSource source) {
    validateKeyName(key);
    
    if (!isValidValue(value)) {
        throw ConfigValidationError(key, "Invalid value type or format");
    }
    
    // Check range validation
    double val = 0.0;
    if (std::holds_alternative<int>(value)) {
        val = static_cast<double>(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        val = static_cast<double>(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        val = std::get<double>(value);
    }
    
    double min = getDefaultMin(key);
    double max = getDefaultMax(key);
    
    if (val < min || val > max) {
        throw ConfigValidationError(key, val, min, max);
    }
    
    // Check security threats
    checkSecurityThreats(key, valueToString(value));
}

void Config::checkSecurityThreats(const std::string& key, const ConfigValue& value) {
    std::string strValue = valueToString(value);
    checkSecurityThreats(key, strValue);
}

bool Config::isValidKeyName(const std::string& key) const {
    if (key.empty()) return false;
    
    for (char c : key) {
        if (std::ispunct(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    for (const auto& pattern : FORBIDDEN_KEY_PATTERNS) {
        if (key.find(pattern) != std::string::npos) {
            return false;
        }
    }
    
    return true;
}

bool Config::isValidValue(const ConfigValue& value) const {
    return std::visit([](auto&& v) -> bool {
        using T = std::decay_t<decltype(v)>;
        return std::is_same_v<T, int> || std::is_same_v<T, int64_t> ||
               std::is_same_v<T, double> || std::is_same_v<T, bool> ||
               std::is_same_v<T, std::string> || std::is_same_v<T, std::vector<int>> ||
               std::is_same_v<T, std::vector<double>> || std::is_same_v<T, std::vector<std::string>>;
    }, value);
}

std::string Config::getValueTypeName(const ConfigValue& value) const {
    return std::visit([](auto&& v) -> std::string {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) return "vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) return "vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "vector<string>";
        else return "unknown";
    }, value);
}

std::vector<std::string> Config::splitString(const std::string& str, char delimiter) const {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    return result;
}

std::string Config::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool Config::stringContainsThreat(const std::string& str) const {
    for (char c : SECURITY_THREATS) {
        if (str.find(c) != std::string::npos) {
            return true;
        }
    }
    return false;
}

double Config::getDefaultMin(const std::string& key) const {
    // Default minimum values for different key types
    if (key.find("count") != std::string::npos || key.find("neuron") != std::string::npos) {
        return 0.0;
    } else if (key.find("rate") != std::string::npos || key.find("learning") != std::string::npos) {
        return 0.0;
    } else if (key.find("probability") != std::string::npos) {
        return 0.0;
    } else {
        return -1e9;
    }
}

double Config::getDefaultMax(const std::string& key) const {
    // Default maximum values for different key types
    if (key.find("count") != std::string::npos || key.find("neuron") != std::string::npos) {
        return 1e6;
    } else if (key.find("rate") != std::string::npos || key.find("learning") != std::string::npos) {
        return 1.0;
    } else if (key.find("probability") != std::string::npos) {
        return 1.0;
    } else {
        return 1e9;
    }
}

std::string Config::getDefaultDescription(const std::string& key) const {
    // Default descriptions for different key types
    if (key.find("neuron") != std::string::npos) {
        return "Number of neurons in the brain";
    } else if (key.find("synapse") != std::string::npos) {
        return "Synaptic connectivity density";
    } else if (key.find("learning") != std::string::npos) {
        return "Learning rate for plasticity";
    } else if (key.find("environment") != std::string::npos) {
        return "Environment dimensions";
    } else {
        return "Configuration parameter";
    }
}

ConfigSource Config::getDefaultSource(const std::string& key) const {
    return ConfigSource::Default;
}

void Config::updateStatistics() {
    stats.totalEntries = pImpl->entries.size();
    std::fill_n(stats.entriesBySource, 4, 0);
    
    for (const auto& entry : pImpl->entries) {
        stats.entriesBySource[static_cast<int>(entry.source)]++;
    }
    
    updateLastModified();
}

void Config::validateEntry(const ConfigEntry& entry) {
    validateKeyName(entry.key);
    validateValue(entry.key, entry.value, entry.source);
}

void Config::throwValidationError(const std::string& key, const std::string& message) const {
    if (errorCallback_) {
        errorCallback_(ConfigValidationError(key, message));
    }
    throw ConfigValidationError(key, message);
}

void Config::throwSecurityError(const std::string& key, const std::string& message) const {
    if (errorCallback_) {
        errorCallback_(ConfigSecurityError(key, message));
    }
    throw ConfigSecurityError(key, message);
}

void Config::throwLoadError(const std::string& filepath, const std::string& message) const {
    if (errorCallback_) {
        errorCallback_(ConfigLoadError(filepath, message));
    }
    throw ConfigLoadError(filepath, message);
}

void Config::throwArgError(const std::string& message) const {
    if (errorCallback_) {
        errorCallback_(ConfigArgError(message));
    }
    throw ConfigArgError(message);
}

void Config::throwIOError(const std::string& filepath, const std::string& message, bool isWrite) const {
    if (errorCallback_) {
        errorCallback_(ConfigIOException(filepath, message, isWrite));
    }
    throw ConfigIOException(filepath, message, isWrite);
}

void Config::throwValueError(const std::string& key, const std::string& message) const {
    if (errorCallback_) {
        errorCallback_(ConfigValueError(key, message));
    }
    throw ConfigValueError(key, message);
}

bool Config::readFile(const std::string& filepath, std::string& content) const {
    std::ifstream file(filepath, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.seekg(0, std::ios::end);
    content.reserve(file.tellg());
    file.seekg(0, std::ios::beg);
    
    content.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return true;
}

bool Config::writeFile(const std::string& filepath, const std::string& content) const {
    std::ofstream file(filepath, std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.write(content.c_str(), content.size());
    return file.good();
}

bool Config::isFileReadable(const std::string& filepath) const {
    std::ifstream file(filepath, std::ios::in);
    return file.good();
}

bool Config::isFileWritable(const std::string& filepath) const {
    std::ofstream file(filepath, std::ios::out);
    return file.good();
}

void Config::initializeDefaultEntries() {
    // Initialize with default configuration entries
    set("brain.neuron_count", 1000);
    set("brain.synapse_density", 0.1);
    set("brain.connection_probability", 0.1);
    set("brain.initial_weight_mean", 0.5);
    set("brain.initial_weight_std", 0.1);
    set("brain.v_thresh", -50.0);
    set("brain.v_rest", -70.0);
    set("brain.v_reset", -75.0);
    set("brain.tau_mem", 20.0);
    set("brain.tau_ref", 2.0);
    set("plasticity.stdp.enable", true);
    set("plasticity.stdp.learning_rate", 0.001);
    set("plasticity.stdp.tau_plus", 20.0);
    set("plasticity.stdp.tau_minus", 20.0);
    set("plasticity.hebbian.enable", true);
    set("plasticity.structural.enable", true);
    set("development.synaptogenesis_rate", 0.001);
    set("development.pruning_rate", 0.0001);
    set("neuromod.dopamine_baseline", 0.0);
    set("neuromod.curiosity.enable", true);
    set("neuromod.novelty.enable", true);
    set("reward.discount_factor", 0.99);
    set("environment.name", "GridWorld");
    set("environment.width", 10);
    set("environment.height", 10);
    set("log.level", "INFO");
    set("log.to_file", false);
    set("log.filename", "nlm.log");
    set("simulation.max_steps", 10000);
    set("simulation.time_limit", 0.0);
    set("visualization.enabled", false);
    set("visualization.update_rate", 30.0);
}

void Config::updateLastModified() {
    pImpl->stats.lastModified = std::chrono::system_clock::now();
}

void Config::incrementEntryCount(ConfigSource source) {
    pImpl->stats.totalEntries++;
    pImpl->stats.entriesBySource[static_cast<int>(source)]++;
}

void Config::incrementValidationError() {
    pImpl->stats.validationErrors++;
}

} // namespace nlm