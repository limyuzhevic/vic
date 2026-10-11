#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <random>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::tm creationTime;
    std::string version;
    std::string author;
    std::string description;
};

struct AdvancedConfig::Impl {
    std::unordered_map<std::string, double> performanceMetrics;
    std::unordered_map<std::string, size_t> statistics;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    std::time_t now = std::time(nullptr);
    pImpl->creationTime = *std::localtime(&now);
    pImpl->version = "1.0.0";
    pImpl->author = "";
    pImpl->description = "";
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

void Config::set(const std::string& key, const ConfigValue& value, 
                 ConfigSource source, const std::string& description, 
                 const std::string& category, const std::string& type,
                 const std::optional<double>& minValue,
                 const std::optional<double>& maxValue,
                 const std::vector<std::string>& allowedValues) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    ConfigEntry entry(key, value, source, description, category, type);
    entry.minValue = minValue;
    entry.maxValue = maxValue;
    entry.allowedValues = allowedValues;
    
    if (it != pImpl->entries.end()) {
        *it = entry;
    } else {
        pImpl->entries.push_back(entry);
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

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    clear();
    
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
            
            set(key, value, ConfigSource::File);
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << TypeConverter::toString(entry.value) << "\n";
    }
    
    return true;
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
    oss << "Created: " << std::put_time(&pImpl->creationTime, "%Y-%m-%d %H:%M:%S") << "\n";
    oss << "Version: " << pImpl->version << "\n";
    if (!pImpl->author.empty()) oss << "Author: " << pImpl->author << "\n";
    if (!pImpl->description.empty()) oss << "Description: " << pImpl->description << "\n";
    
    oss << "\nEntries:";
    for (const auto& entry : pImpl->entries) {
        oss << "\n  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
        oss << "    Category: " << entry.category << ", Type: " << entry.type << "\n";
        if (!entry.description.empty()) oss << "    Desc: " << entry.description << "\n";
    }
    return oss.str();
}

void Config::validate() const {
    std::vector<ValidationError> errors = validateWithErrors();
    if (!errors.empty()) {
        std::ostringstream oss;
        oss << "Configuration validation failed with " << errors.size() << " error(s):\n";
        for (const auto& error : errors) {
            oss << "  Error: " << error.what() << " (key: " << error.getKey() << ")\n";
        }
        throw ValidationError(oss.str());
    }
}

std::vector<ValidationError> Config::validateWithErrors() const {
    std::vector<ValidationError> errors;
    
    for (const auto& entry : pImpl->entries) {
        auto validators = createValidators(entry);
        
        for (auto* validator : validators) {
            try {
                validator->validate(entry.value, entry.key);
            } catch (const ValidationError& e) {
                errors.push_back(e);
            }
        }
    }
    
    return errors;
}

std::string Config::validateReport() const {
    std::vector<ValidationError> errors = validateWithErrors();
    if (errors.empty()) {
        return "Configuration is valid.";
    }
    
    std::ostringstream oss;
    oss << "Configuration validation report - " << errors.size() << " error(s):\n";
    for (const auto& error : errors) {
        oss << "  Key: " << error.getKey() << " - " << error.what() << "\n";
    }
    return oss.str();
}

bool Config::savePreset(const PresetConfig& preset) {
    if (presets_.find(preset.name) != presets_.end()) {
        return false;
    }
    presets_[preset.name] = preset;
    return true;
}

std::optional<PresetConfig> Config::loadPreset(const std::string& name) {
    auto it = presets_.find(name);
    if (it == presets_.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::vector<std::string> Config::getPresetNames() {
    std::vector<std::string> names;
    for (const auto& pair : presets_) {
        names.push_back(pair.first);
    }
    return names;
}

bool Config::mergeFrom(const Config& other, ConfigSource source) {
    bool changed = false;
    
    for (const auto& entry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == pImpl->entries.end()) {
            ConfigEntry newEntry = entry;
            newEntry.source = source;
            pImpl->entries.push_back(newEntry);
            changed = true;
        } else if (entry.source == ConfigSource::Default || 
                   source == ConfigSource::Runtime) {
            // Update if new entry has higher priority
            *it = entry;
            changed = true;
        }
    }
    
    return changed;
}

std::shared_ptr<Config> Config::createSubset(const std::vector<std::string>& keys) const {
    auto subset = std::make_shared<Config>();
    
    for (const auto& key : keys) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            subset->set(it->key, it->value, it->source, it->description, 
                       it->category, it->type, it->minValue, it->maxValue, 
                       it->allowedValues);
        }
    }
    
    return subset;
}

void Config::applyPreset(const PresetConfig& preset) {
    for (const auto& pair : preset.values) {
        set(pair.first, pair.second, preset.source);
    }
}

void Config::optimize(const std::string& optimizationStrategy) {
    // Placeholder optimization implementation
    // In a real implementation, this would use optimization algorithms
    // to find better parameter values based on the strategy
    if (optimizationStrategy == "balance") {
        // Balance between performance and memory usage
        if (has("neuron_count") && has("batch_size")) {
            auto neuronCount = getOr<int64_t>("neuron_count", 500);
            auto batchSize = getOr<int64_t>("batch_size", 32);
            
            // Adjust batch size based on neuron count for balance
            if (neuronCount > 1000 && batchSize < 64) {
                set("batch_size", std::min(64, static_cast<int64_t>(neuronCount / 50)), ConfigSource::Runtime);
            }
        }
    }
}

bool Config::createTemplate(const std::string& name, const std::string& description) {
    if (templates_.find(name) != templates_.end()) {
        return false;
    }
    templates_[name] = description;
    return true;
}

bool Config::applyTemplate(const std::string& name) {
    auto it = templates_.find(name);
    if (it == templates_.end()) {
        return false;
    }
    
    // In a real implementation, apply template configuration
    // For now, just create a marker
    set("_template_name", name, ConfigSource::Preset);
    set("_template_desc", it->second, ConfigSource::Preset);
    
    return true;
}

std::vector<std::string> Config::getTemplateNames() {
    std::vector<std::string> names;
    for (const auto& pair : templates_) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> Config::searchKeys(const std::string& pattern) const {
    std::vector<std::string> matches;
    std::string lowerPattern = toLower(pattern);
    
    for (const auto& entry : pImpl->entries) {
        std::string lowerKey = toLower(entry.key);
        if (lowerKey.find(lowerPattern) != std::string::npos) {
            matches.push_back(entry.key);
        }
    }
    
    return matches;
}

std::string Config::getConfigurationId() const {
    return generateConfigId();
}

std::tm Config::getCreationTime() const {
    return pImpl->creationTime;
}

void Config::setCreationTime(const std::tm& time) {
    pImpl->creationTime = time;
}

bool Config::validateValue(const ConfigValue& value, const std::string& type) {
    if (type == TYPE_INT) {
        return std::holds_alternative<int>(value) || 
               std::holds_alternative<int64_t>(value);
    } else if (type == TYPE_DOUBLE) {
        return std::holds_alternative<double>(value);
    } else if (type == TYPE_STRING) {
        return std::holds_alternative<std::string>(value);
    } else if (type == TYPE_BOOL) {
        return std::holds_alternative<bool>(value);
    } else if (type == TYPE_INT_VECTOR) {
        return std::holds_alternative<std::vector<int>>(value);
    } else if (type == TYPE_DOUBLE_VECTOR) {
        return std::holds_alternative<std::vector<double>>(value);
    } else if (type == TYPE_STRING_VECTOR) {
        return std::holds_alternative<std::vector<std::string>>(value);
    }
    return false;
}

std::string Config::getDefaultValue(const std::string& key) {
    auto it = defaultValues_.find(key);
    if (it == defaultValues_.end()) {
        return "";
    }
    return it->second;
}

std::string Config::getValueTypeName(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return TYPE_INT;
    if (std::holds_alternative<int64_t>(value)) return TYPE_INT;
    if (std::holds_alternative<double>(value)) return TYPE_DOUBLE;
    if (std::holds_alternative<bool>(value)) return TYPE_BOOL;
    if (std::holds_alternative<std::string>(value)) return TYPE_STRING;
    if (std::holds_alternative<std::vector<int>>(value)) return TYPE_INT_VECTOR;
    if (std::holds_alternative<std::vector<double>>(value)) return TYPE_DOUBLE_VECTOR;
    if (std::holds_alternative<std::vector<std::string>>(value)) return TYPE_STRING_VECTOR;
    return "unknown";
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == other.pImpl->entries.end() || it->value != entry.value) {
            return false;
        }
    }
    
    return true;
}

std::string Config::diff(const Config& other) const {
    std::ostringstream oss;
    bool hasDiff = false;
    
    // Check entries in this config not in other
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == other.pImpl->entries.end()) {
            if (hasDiff) oss << "\n";
            oss << "Added: " << entry.key << " = " << TypeConverter::toString(entry.value);
            hasDiff = true;
        } else if (it->value != entry.value) {
            if (hasDiff) oss << "\n";
            oss << "Changed: " << entry.key << " (this: " << TypeConverter::toString(entry.value) << 
                   ", other: " << TypeConverter::toString(it->value) << ")";
            hasDiff = true;
        }
    }
    
    // Check entries in other config not in this
    for (const auto& entry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == pImpl->entries.end()) {
            if (hasDiff) oss << "\n";
            oss << "Removed: " << entry.key << " = " << TypeConverter::toString(entry.value);
            hasDiff = true;
        }
    }
    
    return hasDiff ? oss.str() : "Configurations are identical.";
}

std::string Config::generateConfigId() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::string uuid = "";
    const char* charset = "0123456789abcdef";
    for (int i = 0; i < 32; ++i) {
        uuid += charset[dis(gen)];
    }
    
    return uuid;
}

std::tm Config::currentTime() {
    std::time_t now = std::time(nullptr);
    return *std::localtime(&now);
}

std::vector<Validator*> Config::createValidators(const ConfigEntry& entry) {
    std::vector<Validator*> validators;
    
    // Create range validator if min/max specified
    if (entry.minValue.has_value() || entry.maxValue.has_value()) {
        double min = entry.minValue.value_or(std::numeric_limits<double>::lowest());
        double max = entry.maxValue.value_or(std::numeric_limits<double>::max());
        validators.push_back(new RangeValidator(min, max));
    }
    
    // Create allowed values validator if allowed values specified
    if (!entry.allowedValues.empty()) {
        validators.push_back(new AllowedValuesValidator(entry.allowedValues));
    }
    
    return validators;
}

// Initialize static members
std::unordered_map<std::string, PresetConfig> Config::presets_;
std::unordered_map<std::string, std::string> Config::templates_;
std::unordered_map<std::string, std::string> Config::defaultValues_;

// Initialize constants
const std::string Config::NEURAL_NETWORK_CATEGORY = "neural_network";
const std::string Config::TRAINING_CATEGORY = "training";
const std::string Config::MEMORY_CATEGORY = "memory";
const std::string Config::NEUROMODULATION_CATEGORY = "neuromodulation";
const std::string Config::SYSTEM_CATEGORY = "system";
const std::string Config::PLASTICITY_CATEGORY = "plasticity";
const std::string Config::DEVELOPMENT_CATEGORY = "development";

const std::string Config::TYPE_INT = "int";
const std::string Config::TYPE_DOUBLE = "double";
const std::string Config::TYPE_STRING = "string";
const std::string Config::TYPE_BOOL = "bool";
const std::string Config::TYPE_INT_VECTOR = "int_vector";
const std::string Config::TYPE_DOUBLE_VECTOR = "double_vector";
const std::string Config::TYPE_STRING_VECTOR = "string_vector";

// ConfigUtils namespace implementations
namespace ConfigUtils {
    std::vector<ValidationError> validateConfig(const Config& config) {
        return config.validateWithErrors();
    }
    
    std::string generateValidationReport(const Config& config) {
        return config.validateReport();
    }
    
    bool configsEqual(const Config& config1, const Config& config2) {
        return config1.equals(config2);
    }
    
    std::string compareConfigs(const Config& config1, const Config& config2) {
        return config1.diff(config2);
    }
    
    Config mergeConfigs(const Config& config1, const Config& config2, 
                        ConfigSource source) {
        Config result = config1;
        result.mergeFrom(config2, source);
        return result;
    }
    
    Config optimizeConfig(const Config& config, 
                         const std::string& strategy) {
        Config result = config;
        result.optimize(strategy);
        return result;
    }
    
    std::vector<std::string> searchConfigs(const Config& config, 
                                           const std::string& pattern) {
        return config.searchKeys(pattern);
    }
    
    Benchmarker::Benchmarker(Config config) : config_(std::move(config)) {}
    
    double Benchmarker::runBenchmark() const {
        // Simple benchmark based on configuration
        double score = 100.0;
        
        if (config_.has("neuron_count")) {
            auto neuronCount = config_.getOr<int64_t>("neuron_count", 500);
            score -= std::log10(neuronCount / 500.0) * 20;
        }
        
        if (config_.has("batch_size")) {
            auto batchSize = config_.getOr<int64_t>("batch_size", 32);
            if (batchSize > 256) score -= 10;
        }
        
        return std::max(0.0, score);
    }
    
    std::string Benchmarker::getBenchmarkReport() const {
        std::ostringstream oss;
        oss << "Benchmark for configuration " << config_.getConfigurationId() << ":\n";
        oss << "Score: " << runBenchmark() << " (higher is better)\n";
        oss << "Entries: " << config_.getKeys().size() << "\n";
        return oss.str();
    }
    
    bool exportConfig(const Config& config, const std::string& filepath) {
        return config.saveToFile(filepath);
    }
    
    std::optional<Config> importConfig(const std::string& filepath) {
        Config config;
        if (config.loadFromFile(filepath)) {
            return config;
        }
        return std::nullopt;
    }
    
    bool saveTemplate(const std::string& name, const std::string& description, 
                      const Config& config) {
        return config.createTemplate(name, description);
    }
    
    std::optional<Config> applyTemplate(const std::string& name) {
        Config config;
        if (config.applyTemplate(name)) {
            return config;
        }
        return std::nullopt;
    }
    
    std::vector<std::string> getAvailableTemplates() {
        return Config::getTemplateNames();
    }
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