#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <json/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<Config> history;
    std::unordered_map<std::string, ConfigTemplate> templates;
    ConfigSource currentSource;
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validators;
    std::unordered_map<std::string, std::vector<std::string>> dependencies;
    
    Impl() : currentSource(ConfigSource::Default) {}
};

// Static built-in templates
std::unordered_map<std::string, ConfigTemplate> Config::builtInTemplates;

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize built-in templates
    if (builtInTemplates.empty()) {
        initializeBuiltInTemplates();
    }
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, ConfigSource source) {
    // Try to parse as JSON first
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Try to parse as JSON
    try {
        json jsonData = json::parse(content);
        return loadFromJSON(jsonData.dump(), source);
    } catch (const json::parse_error&) {
        // Fallback to simple key=value format
        return loadFromFileSimple(filepath, source);
    }
}

bool Config::loadFromFileSimple(const std::string& filepath, ConfigSource source) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
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
            
            set(key, value, source);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::loadFromJSON(const std::string& jsonStr, ConfigSource source) {
    try {
        json jsonData = json::parse(jsonStr);
        
        // Convert JSON to configuration entries
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            
            if (it->is_string()) {
                value = it->get<std::string>();
            }
            else if (it->is_number_integer()) {
                value = it->get<int>();
            }
            else if (it->is_number_unsigned()) {
                value = static_cast<int64_t>(it->get<uint64_t>());
            }
            else if (it->is_number_float()) {
                value = it->get<double>();
            }
            else if (it->is_boolean()) {
                value = it->get<bool>();
            }
            else if (it->is_array()) {
                if (it->empty()) {
                    value = std::vector<std::string>();
                }
                else if (it->front().is_string()) {
                    value = it->get<std::vector<std::string>>();
                }
                else if (it->front().is_number_integer()) {
                    value = it->get<std::vector<int>>();
                }
                else {
                    value = it->get<std::vector<double>>();
                }
            }
            else if (it->is_object()) {
                value = it->dump();
            }
            
            set(key, value, source);
        }
        
        return true;
    } catch (const json::parse_error& e) {
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath, bool pretty) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (pretty) {
        json jsonData;
        for (const auto& entry : pImpl->entries) {
            jsonData[entry.key] = entry.value;
        }
        file << jsonData.dump(2);
    } else {
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
        }
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
        it->validated = false;
    } else {
        pImpl->entries.emplace_back(key, value, source);
        it = std::prev(pImpl->entries.end());
        
        // Initialize validation status
        it->validated = (validateValue(key, value) == ValidationResult(true));
    }
    
    // Add to history
    pImpl->history.push_back(*this);
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

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

std::string Config::getNested(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "";
    }
    
    try {
        return std::get<std::string>(it->value);
    } catch (const std::bad_variant_access&) {
        return "";
    }
}

double Config::getNestedFloat(const std::string& key, double defaultValue) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return defaultValue;
    }
    
    try {
        return std::get<double>(it->value);
    } catch (const std::bad_variant_access&) {
        return defaultValue;
    }
}

int Config::getNestedInt(const std::string& key, int defaultValue) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return defaultValue;
    }
    
    try {
        return std::get<int>(it->value);
    } catch (const std::bad_variant_access&) {
        return defaultValue;
    }
}

bool Config::getNestedBool(const std::string& key, bool defaultValue) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return defaultValue;
    }
    
    try {
        return std::get<bool>(it->value);
    } catch (const std::bad_variant_access&) {
        return defaultValue;
    }
}

void Config::setNested(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, value, source);
}

void Config::setNestedFloat(const std::string& key, double value, ConfigSource source) {
    set(key, value, source);
}

void Config::setNestedInt(const std::string& key, int value, ConfigSource source) {
    set(key, value, source);
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

std::vector<std::string> Config::getKeys(const std::string& category) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.category == category) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::vector<std::string> Config::getCategories() const {
    std::vector<std::string> categories;
    for (const auto& entry : pImpl->entries) {
        if (std::find(categories.begin(), categories.end(), entry.category) == categories.end()) {
            categories.push_back(entry.category);
        }
    }
    return categories;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->history.clear();
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

ConfigStats Config::getStats() const {
    ConfigStats stats;
    stats.totalEntries = pImpl->entries.size();
    
    for (const auto& entry : pImpl->entries) {
        stats.entriesBySource[static_cast<size_t>(entry.source)]++;
        if (std::find(stats.categories.begin(), stats.categories.end(), entry.category) == stats.categories.end()) {
            stats.categories.push_back(entry.category);
        }
    }
    
    return stats;
}

ValidationResult Config::validate() const {
    ValidationResult result;
    
    // Validate types
    ValidationResult typeResult = validateTypes();
    if (!typeResult.valid) {
        result.errors.insert(result.errors.end(), typeResult.errors.begin(), typeResult.errors.end());
    }
    
    // Validate ranges
    ValidationResult rangeResult = validateRanges();
    if (!rangeResult.valid) {
        result.errors.insert(result.errors.end(), rangeResult.errors.begin(), rangeResult.errors.end());
    }
    
    // Validate dependencies
    ValidationResult dependencyResult = validateDependencies();
    if (!dependencyResult.valid) {
        result.errors.insert(result.errors.end(), dependencyResult.errors.begin(), dependencyResult.errors.end());
    }
    
    // Validate consistency
    ValidationResult consistencyResult = validateConsistency();
    if (!consistencyResult.valid) {
        result.errors.insert(result.errors.end(), consistencyResult.errors.begin(), consistencyResult.errors.end());
    }
    
    result.valid = result.errors.empty();
    return result;
}

std::vector<std::string> Config::getAvailableTemplates() {
    std::vector<std::string> names;
    for (const auto& pair : builtInTemplates) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<ConfigTemplate> Config::getAllTemplates() {
    std::vector<ConfigTemplate> templates;
    for (const auto& pair : builtInTemplates) {
        templates.push_back(pair.second);
    }
    return templates;
}

ConfigTemplate Config::getTemplate(const std::string& name) {
    auto it = builtInTemplates.find(name);
    if (it != builtInTemplates.end()) {
        return it->second;
    }
    return ConfigTemplate();
}

bool Config::applyTemplate(const std::string& name, ConfigSource source) {
    auto it = builtInTemplates.find(name);
    if (it == builtInTemplates.end()) {
        return false;
    }
    
    return applyTemplate(it->second, source);
}

bool Config::applyTemplate(const ConfigTemplate& template) {
    clear();
    
    for (const auto& entry : template.entries) {
        ConfigEntry newEntry = entry;
        newEntry.source = ConfigSource::Template;
        pImpl->entries.push_back(newEntry);
        pImpl->entries.back().validated = (validateValue(entry.key, entry.value) == ValidationResult(true));
    }
    
    return true;
}

ConfigComparison Config::compare(const Config& other) const {
    ConfigComparison comparison;
    
    std::unordered_map<std::string, ConfigValue> thisMap;
    std::unordered_map<std::string, ConfigValue> otherMap;
    
    for (const auto& entry : pImpl->entries) {
        thisMap[entry.key] = entry.value;
    }
    
    for (const auto& entry : other.pImpl->entries) {
        otherMap[entry.key] = entry.value;
    }
    
    for (const auto& pair : thisMap) {
        const std::string& key = pair.first;
        const ConfigValue& val1 = pair.second;
        
        if (otherMap.find(key) != otherMap.end()) {
            const ConfigValue& val2 = otherMap.at(key);
            if (val1 != val2) {
                comparison.differences.push_back(key + ": " + valueToString(val1) + " -> " + valueToString(val2));
            } else {
                comparison.similarities.push_back(key + ": " + valueToString(val1));
            }
        } else {
            comparison.differences.push_back(key + ": " + valueToString(val1) + " (not in other config)");
        }
    }
    
    for (const auto& pair : otherMap) {
        const std::string& key = pair.first;
        if (thisMap.find(key) == thisMap.end()) {
            comparison.differences.push_back(key + ": (not in this config) -> " + valueToString(pair.second));
        }
    }
    
    return comparison;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, source);
    }
}

Config Config::subset(const std::vector<std::string>& keys) const {
    Config result;
    for (const auto& key : keys) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        if (it != pImpl->entries.end()) {
            result.set(it->key, it->value, it->source);
            result.pImpl->entries.back().description = it->description;
            result.pImpl->entries.back().category = it->category;
            result.pImpl->entries.back().tags = it->tags;
        }
    }
    return result;
}

Config Config::filter(const std::function<bool(const ConfigEntry&)>& predicate) const {
    Config result;
    for (const auto& entry : pImpl->entries) {
        if (predicate(entry)) {
            result.set(entry.key, entry.value, entry.source);
            result.pImpl->entries.back().description = entry.description;
            result.pImpl->entries.back().category = entry.category;
            result.pImpl->entries.back().tags = entry.tags;
        }
    }
    return result;
}

Config Config::transform(const std::function<ConfigValue(const ConfigEntry&)>& transform) const {
    Config result;
    for (const auto& entry : pImpl->entries) {
        result.set(entry.key, transform(entry), entry.source);
        result.pImpl->entries.back().description = entry.description;
        result.pImpl->entries.back().category = entry.category;
        result.pImpl->entries.back().tags = entry.tags;
    }
    return result;
}

std::string Config::toJSON() const {
    json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        jsonData[entry.key] = entry.value;
    }
    
    return jsonData.dump(2);
}

std::string Config::toString() const {
    std::ostringstream oss;
    oss << "Configuration with " << pImpl->entries.size() << " entries:";
    for (const auto& entry : pImpl->entries) {
        oss << "\n  " << entry.key << " = " << valueToString(entry.value) << " (" << static_cast<int>(entry.source) << ")";
    }
    return oss.str();
}

Config Config::clone() const {
    Config result;
    result.pImpl->entries = pImpl->entries;
    result.pImpl->history = pImpl->history;
    result.pImpl->templates = pImpl->templates;
    result.pImpl->validators = pImpl->validators;
    result.pImpl->dependencies = pImpl->dependencies;
    return result;
}

bool Config::saveToArchive(const std::string& filepath) const {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    return saveToFile(filepath + ".json", true);
}

bool Config::loadFromArchive(const std::string& filepath) {
    bool result = loadFromFile(filepath + ".json", ConfigSource::File);
    if (!result) {
        result = loadFromFileSimple(filepath, ConfigSource::File);
    }
    return result;
}

bool Config::createBackup(const std::string& name) const {
    if (pImpl->entries.empty()) {
        return false;
    }
    
    std::time_t now = std::time(nullptr);
    std::tm tm = *std::localtime(&now);
    
    std::ostringstream oss;
    oss << "backup_" << name << "_";
    oss << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".cfg";
    
    return saveToFile(oss.str(), false);
}

bool Config::restoreBackup(const std::string& name) {
    std::time_t now = std::time(nullptr);
    std::tm tm = *std::localtime(&now);
    
    std::ostringstream oss;
    oss << "backup_" << name << "_";
    oss << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".cfg";
    
    return loadFromFileSimple(oss.str(), ConfigSource::File);
}

std::vector<Config> Config::getHistory() const {
    return pImpl->history;
}

void Config::addToHistory() {
    pImpl->history.push_back(*this);
}

std::string Config::getStatisticsReport() const {
    std::ostringstream oss;
    
    ConfigStats stats = getStats();
    
    oss << "Configuration Statistics Report\n";
    oss << "==============================\n\n";
    oss << "Total entries: " << stats.totalEntries << "\n";
    oss << "Categories: " << stats.categories.size() << "\n";
    
    for (const auto& category : stats.categories) {
        int count = 0;
        for (const auto& entry : pImpl->entries) {
            if (entry.category == category) {
                count++;
            }
        }
        oss << "  " << category << ": " << count << "\n";
    }
    
    oss << "\nEntries by source:\n";
    for (int i = 0; i < 4; ++i) {
        oss << "  " << static_cast<ConfigSource>(i) << ": " << stats.entriesBySource[i] << "\n";
    }
    
    oss << "\nTop 10 most recent changes:\n";
    int count = 0;
    for (auto it = pImpl->history.rbegin(); it != pImpl->history.rend() && count < 10; ++it, ++count) {
        oss << "  " << it->summary() << "\n";
    }
    
    return oss.str();
}

ValidationResult Config::validateTypes() const {
    ValidationResult result;
    
    for (const auto& entry : pImpl->entries) {
        ValidationResult entryResult = validateValue(entry.key, entry.value);
        if (!entryResult.valid) {
            for (const auto& error : entryResult.errors) {
                result.addError("Entry '" + entry.key + "': " + error);
            }
        }
    }
    
    return result;
}

ValidationResult Config::validateRanges() const {
    ValidationResult result;
    
    for (const auto& entry : pImpl->entries) {
        ValidationResult rangeResult = validateRange(entry.key, entry.value);
        if (!rangeResult.valid) {
            for (const auto& error : rangeResult.errors) {
                result.addError("Entry '" + entry.key + "': " + error);
            }
        }
    }
    
    return result;
}

ValidationResult Config::validateDependencies() const {
    ValidationResult result;
    
    for (const auto& entry : pImpl->entries) {
        if (pImpl->dependencies.find(entry.key) != pImpl->dependencies.end()) {
            for (const auto& depKey : pImpl->dependencies.at(entry.key)) {
                if (!has(depKey)) {
                    result.addError("Entry '" + entry.key + "' depends on missing key '" + depKey + "'");
                }
            }
        }
    }
    
    return result;
}

ValidationResult Config::validateConsistency() const {
    ValidationResult result;
    
    if (has("brain.development.enabled") && has("brain.development.critical_period_start")) {
        int start = getNestedInt("brain.development.critical_period_start");
        int end = getNestedInt("brain.development.critical_period_end", 1000);
        if (start > end) {
            result.addError("Critical period start must be before end");
        }
    }
    
    if (has("plasticity.stdp.ltp_weight") && has("plasticity.stdp.ltd_weight")) {
        double ltp = getNestedFloat("plasticity.stdp.ltp_weight");
        double ltd = getNestedFloat("plasticity.stdp.ltd_weight");
        if (ltp <= 0 || ltd <= 0) {
            result.addError("STDP weights must be positive");
        }
    }
    
    if (has("memory.working_memory.capacity") && has("memory.episodic_memory.max_episodes")) {
        int wmCapacity = getNestedInt("memory.working_memory.capacity");
        int emEpisodes = getNestedInt("memory.episodic_memory.max_episodes");
        if (wmCapacity < emEpisodes / 10) {
            result.addError("Working memory capacity too small for episodic memory");
        }
    }
    
    return result;
}

ValidationResult Config::validateValue(const std::string& key, const ConfigValue& value) const {
    ValidationResult result;
    
    try {
        if (std::holds_alternative<int>(value)) {
            int val = std::get<int>(value);
            if (key.find("count") != std::string::npos || key.find("neuron") != std::string::npos) {
                if (val <= 0) {
                    result.addError("Count must be positive");
                }
            }
        }
        else if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (key.find("rate") != std::string::npos || key.find("probability") != std::string::npos) {
                if (val < 0 || val > 1) {
                    result.addError("Probability/rate must be between 0 and 1");
                }
            }
        }
    } catch (const std::bad_variant_access&) {
        result.addError("Invalid value type");
    }
    
    return result;
}

ValidationResult Config::validateRange(const std::string& key, const ConfigValue& value) const {
    ValidationResult result;
    
    if (key.find("brain.neuron_count") != std::string::npos) {
        if (std::holds_alternative<int>(value)) {
            int val = std::get<int>(value);
            if (val < 10 || val > 100000) {
                result.addError("Neuron count must be between 10 and 100000");
            }
        }
    }
    
    if (key.find("brain.connection_probability") != std::string::npos) {
        if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (val < 0 || val > 1) {
                result.addError("Connection probability must be between 0 and 1");
            }
        }
    }
    
    return result;
}

std::string Config::valueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    
    try {
        if (std::holds_alternative<int>(value)) {
            oss << std::get<int>(value);
        }
        else if (std::holds_alternative<int64_t>(value)) {
            oss << std::get<int64_t>(value);
        }
        else if (std::holds_alternative<double>(value)) {
            oss << std::get<double>(value);
        }
        else if (std::holds_alternative<bool>(value)) {
            oss << (std::get<bool>(value) ? "true" : "false");
        }
        else if (std::holds_alternative<std::string>(value)) {
            oss << "\"" << std::get<std::string>(value) << "\"";
        }
        else if (std::holds_alternative<std::vector<int>>(value)) {
            oss << "[";
            auto& vec = std::get<std::vector<int>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << vec[i];
            }
            oss << "]";
        }
        else if (std::holds_alternative<std::vector<double>>(value)) {
            oss << "[";
            auto& vec = std::get<std::vector<double>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << vec[i];
            }
            oss << "]";
        }
        else if (std::holds_alternative<std::vector<std::string>>(value)) {
            oss << "[";
            auto& vec = std::get<std::vector<std::string>>(value);
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << vec[i] << "\"";
            }
            oss << "]";
        }
        else {
            oss << "unknown";
        }
    } catch (const std::bad_variant_access&) {
        oss << "invalid";
    }
    
    return oss.str();
}

void Config::initializeBuiltInTemplates() {
    // Standard configuration template
    ConfigTemplate standardTemplate("standard", "Standard NLM configuration");
    standardTemplate.entries = {
        ConfigEntry("brain.neuron_count", 1000, ConfigSource::Default, "Total number of neurons", "brain"),
        ConfigEntry("brain.region_count", 1, ConfigSource::Default, "Number of neural regions", "brain"),
        ConfigEntry("brain.connection_probability", 0.1, ConfigSource::Default, "Connection probability", "brain"),
        ConfigEntry("plasticity.stdp.ltp_weight", 0.01, ConfigSource::Default, "STDP potentiation weight", "plasticity"),
        ConfigEntry("plasticity.stdp.ltd_weight", 0.012, ConfigSource::Default, "STDP depression weight", "plasticity"),
        ConfigEntry("plasticity.hebbian.learning_rate", 0.001, ConfigSource::Default, "Hebbian learning rate", "plasticity"),
        ConfigEntry("memory.working_memory.capacity", 100, ConfigSource::Default, "Working memory capacity", "memory"),
        ConfigEntry("memory.episodic_memory.max_episodes", 1000, ConfigSource::Default, "Maximum episodic memories", "memory"),
    };
    builtInTemplates["standard"] = standardTemplate;
    
    // Research configuration template
    ConfigTemplate researchTemplate("research", "Configuration for research experiments");
    researchTemplate.entries = {
        ConfigEntry("brain.neuron_count", 2000, ConfigSource::Default, "Total number of neurons", "brain"),
        ConfigEntry("brain.region_count", 2, ConfigSource::Default, "Number of neural regions", "brain"),
        ConfigEntry("brain.connection_probability", 0.15, ConfigSource::Default, "Connection probability", "brain"),
        ConfigEntry("plasticity.stdp.ltp_weight", 0.02, ConfigSource::Default, "STDP potentiation weight", "plasticity"),
        ConfigEntry("plasticity.stdp.ltd_weight", 0.018, ConfigSource::Default, "STDP depression weight", "plasticity"),
        ConfigEntry("plasticity.hebbian.learning_rate", 0.002, ConfigSource::Default, "Hebbian learning rate", "plasticity"),
        ConfigEntry("neuromodulation.curiosity.enable", true, ConfigSource::Default, "Enable curiosity", "neuromodulation"),
        ConfigEntry("memory.working_memory.capacity", 500, ConfigSource::Default, "Working memory capacity", "memory"),
        ConfigEntry("memory.episodic_memory.max_episodes", 5000, ConfigSource::Default, "Maximum episodic memories", "memory"),
    };
    builtInTemplates["research"] = researchTemplate;
    
    // Performance configuration template
    ConfigTemplate performanceTemplate("performance", "Configuration optimized for performance");
    performanceTemplate.entries = {
        ConfigEntry("brain.neuron_count", 500, ConfigSource::Default, "Total number of neurons", "brain"),
        ConfigEntry("brain.region_count", 1, ConfigSource::Default, "Number of neural regions", "brain"),
        ConfigEntry("brain.connection_probability", 0.05, ConfigSource::Default, "Connection probability", "brain"),
        ConfigEntry("plasticity.stdp.ltp_weight", 0.005, ConfigSource::Default, "STDP potentiation weight", "plasticity"),
        ConfigEntry("plasticity.stdp.ltd_weight", 0.006, ConfigSource::Default, "STDP depression weight", "plasticity"),
        ConfigEntry("plasticity.hebbian.learning_rate", 0.0005, ConfigSource::Default, "Hebbian learning rate", "plasticity"),
        ConfigEntry("memory.working_memory.capacity", 50, ConfigSource::Default, "Working memory capacity", "memory"),
        ConfigEntry("memory.episodic_memory.max_episodes", 500, ConfigSource::Default, "Maximum episodic memories", "memory"),
    };
    builtInTemplates["performance"] = performanceTemplate;
}

} // namespace nlm

// Explicit template instantiations
namespace nlm {
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
}