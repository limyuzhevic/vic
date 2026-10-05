#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ValidationRule> validationRules;
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
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
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

// Helper method to find entry by key
const ConfigEntry* Config::findEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    return (it != pImpl->entries.end()) ? &(*it) : nullptr;
}

// Helper method to get config value
std::optional<ConfigValue> Config::getConfigValue(const std::string& key) const {
    const ConfigEntry* entry = findEntry(key);
    if (entry) {
        return entry->value;
    }
    return std::nullopt;
}

// Type-safe getters implementation
bool Config::getBool(const std::string& key, bool& value) const {
    try {
        value = std::get<bool>(findEntry(key)->value);
        return true;
    } catch (const std::bad_variant_access&) {
        return false;
    }
}

int Config::getInt(const std::string& key, int& value) const {
    try {
        value = std::get<int>(findEntry(key)->value);
        return 1;
    } catch (const std::bad_variant_access&) {
        return 0;
    }
}

int64_t Config::getInt64(const std::string& key, int64_t& value) const {
    try {
        value = std::get<int6464_t>(findEntry(key)->value);
        return 1;
    } catch (const std::bad_variant_access&) {
        return 0;
    }
}

double Config::getDouble(const std::string& key, double& value) const {
    try {
        value = std::get<double>(findEntry(key)->value);
        return 1;
    } catch (const std::bad_variant_access&) {
        return 0;
    }
}

std::string Config::getString(const std::string& key, std::string& value) const {
    try {
        value = std::get<std::string>(findEntry(key)->value);
        return 1;
    } catch (const std::bad_variant_access&) {
        return 0;
    }
}

std::vector<int> Config::getIntVector(const std::string& key, std::vector<int>& value) const {
    try {
        value = std::get<std::vector<int>>(findEntry(key)->value);
        return 1;
    } catch (const std::bad_variant_access&) {
        return 0;
    }
}

std::vector<double> Config::getDoubleVector(const std::string& key, std::vector<double>& value) const {
    try {
        value = std::get<std::vector<double>>(findEntry(key)->value);
        return 1;
    } catch (const std::bad_variant_access&) {
        return 0;
    }
}

std::vector<std::string> Config::getStringVector(const std::string& key, std::vector<std::string>& value) const {
    try {
        value = std::get<std::vector<std::string>>(findEntry(key)->value);
        return 1;
    } catch (const std::bad_variant_access&) {
        return 0;
    }
}

// Type-safe setters implementation
void Config::setBool(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setInt(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setInt64(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setDouble(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setString(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setIntVector(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setDoubleVector(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setStringVector(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// Category-based getters implementation
std::vector<std::string> Config::getBrainKeys() const {
    std::vector<std::string> brainKeys;
    for (const auto& entry : pImpl->entries) {
        if (toLower(entry.key).find("brain.") == 0) {
            brainKeys.push_back(entry.key);
        }
    }
    return brainKeys;
}

std::vector<std::string> Config::getPlasticityKeys() const {
    std::vector<std::string> plasticityKeys;
    for (const auto& entry : pImpl->entries) {
        if (toLower(entry.key).find("plasticity.") == 0) {
            plasticityKeys.push_back(entry.key);
        }
    }
    return plasticityKeys;
}

std::vector<std::string> Config::getNeuromodulationKeys() const {
    std::vector<std::string> neuromodulationKeys;
    for (const auto& entry : pImpl->entries) {
        if (toLower(entry.key).find("neuromod.") == 0) {
            neuromodulationKeys.push_back(entry.key);
        }
    }
    return neuromodulationKeys;
}

std::vector<std::string> Config::getDevelopmentKeys() const {
    std::vector<std::string> developmentKeys;
    for (const auto& entry : pImpl->entries) {
        if (toLower(entry.key).find("development.") == 0) {
            developmentKeys.push_back(entry.key);
        }
    }
    return developmentKeys;
}

std::vector<std::string> Config::getEnvironmentKeys() const {
    std::vector<std::string> environmentKeys;
    for (const auto& entry : pImpl->entries) {
        if (toLower(entry.key).find("environment.") == 0) {
            environmentKeys.push_back(entry.key);
        }
    }
    return environmentKeys;
}

std::vector<std::string> Config::getLoggingKeys() const {
    std::vector<std::string> loggingKeys;
    for (const auto& entry : pImpl->entries) {
        if (toLower(entry.key).find("log.") == 0) {
            loggingKeys.push_back(entry.key);
        }
    }
    return loggingKeys;
}

// Configuration description methods implementation
std::string Config::getBrainDescription() const {
    std::ostringstream oss;
    oss << "Brain Configuration (" << getBrainKeys().size() << " entries):\n";
    for (const auto& key : getBrainKeys()) {
        auto val = getConfigValue(key);
        oss << "  " << key << " = ";
        if (val.has_value()) {
            if (std::holds_alternative<std::string>(val.value())) {
                oss << "\"" << std::get<std::string>(val.value()) << "\"";
            } else {
                oss << val.value();
            }
        } else {
            oss << "[NOT SET]";
        }
        oss << "\n";
    }
    return oss.str();
}

std::string Config::getPlasticityDescription() const {
    std::ostringstream oss;
    oss << "Plasticity Configuration (" << getPlasticityKeys().size() << " entries):\n";
    for (const auto& key : getPlasticityKeys()) {
        auto val = getConfigValue(key);
        oss << "  " << key << " = ";
        if (val.has_value()) {
            if (std::holds_alternative<std::string>(val.value())) {
                oss << "\"" << std::get<std::string>(val.value()) << "\"";
            } else {
                oss << val.value();
            }
        } else {
            oss << "[NOT SET]";
        }
        oss << "\n";
    }
    return oss.str();
}

std::string Config::getNeuromodulationDescription() const {
    std::ostringstream oss;
    oss << "Neuromodulation Configuration (" << getNeuromodulationKeys().size() << " entries):\n";
    for (const auto& key : getNeuromodulationKeys()) {
        auto val = getConfigValue(key);
        oss << "  " << key << " = ";
        if (val.has_value()) {
            if (std::holds_alternative<std::string>(val.value())) {
                oss << "\"" << std::get<std::string>(val.value()) << "\"";
            } else {
                oss << val.value();
            }
        } else {
            oss << "[NOT SET]";
        }
        oss << "\n";
    }
    return oss.str();
}

std::string Config::getDevelopmentDescription() const {
    std::ostringstream oss;
    oss << "Development Configuration (" << getDevelopmentKeys().size() << " entries):\n";
    for (const auto& key : getDevelopmentKeys()) {
        auto val = getConfigValue(key);
        oss << "  " << key << " = ";
        if (val.has_value()) {
            if (std::holds_alternative<std::string>(val.value())) {
                oss << "\"" << std::get<std::string>(val.value()) << "\"";
            } else {
                oss << val.value();
            }
        } else {
            oss << "[NOT SET]";
        }
        oss << "\n";
    }
    return oss.str();
}

std::string Config::getEnvironmentDescription() const {
    std::ostringstream oss;
    oss << "Environment Configuration (" << getEnvironmentKeys().size() << " entries):\n";
    for (const auto& key : getEnvironmentKeys()) {
        auto val = getConfigValue(key);
        oss << "  " << key << " = ";
        if (val.has_value()) {
            if (std::holds_alternative<std::string>(val.value())) {
                oss << "\"" << std::get<std::string>(val.value()) << "\"";
            } else {
                oss << val.value();
            }
        } else {
            oss << "[NOT SET]";
        }
        oss << "\n";
    }
    return oss.str();
}

std::string Config::getLoggingDescription() const {
    std::ostringstream oss;
    oss << "Logging Configuration (" << getLoggingKeys().size() << " entries):\n";
    for (const auto& key : getLoggingKeys()) {
        auto val = getConfigValue(key);
        oss << "  " << key << " = ";
        if (val.has_value()) {
            if (std::holds_alternative<std::string>(val.value())) {
                oss << "\"" << std::get<std::string>(val.value()) << "\"";
            } else {
                oss << val.value();
            }
        } else {
            oss << "[NOT SET]";
        }
        oss << "\n";
    }
    return oss.str();
}

// Configuration templates implementation
void Config::setBrainDefaults() {
    set("brain.neuron_count", 1000);
    set("brain.region_count", 2);
    set("brain.connection_probability", 0.1);
    set("brain.initial_weight_mean", 0.5);
    set("brain.initial_weight_std", 0.1);
    set("brain.v_thresh", -50.0);
    set("brain.v_rest", -70.0);
    set("brain.v_reset", -75.0);
    set("brain.tau_mem", 20.0);
    set("brain.tau_ref", 2.0);
}

void Config::setDevelopmentDefaults() {
    set("development.synaptogenesis_rate", 0.001);
    set("development.pruning_rate", 0.0001);
}

void Config::setNeuromodulationDefaults() {
    set("neuromod.dopamine_baseline", 0.1);
    set("neuromod.curiosity_scale", 1.0);
    set("neuromod.novelty_scale", 1.0);
}

void Config::setEnvironmentDefaults() {
    set("environment.name", "GridWorld");
    set("environment.width", 10);
    set("environment.height", 10);
    set("environment.vision_width", 8);
    set("environment.vision_height", 8);
}

void Config::setLoggingDefaults() {
    set("log.level", "INFO");
    set("log.to_file", false);
    set("log.filename", "nlm.log");
}

// Configuration validation implementation
bool Config::validate() const {
    // Basic validation rules
    if (has("brain.neuron_count")) {
        int64_t neuronCount = 0;
        if (getInt64("brain.neuron_count", neuronCount)) {
            if (neuronCount <= 0) {
                return false;
            }
        }
    }
    
    if (has("brain.connection_probability")) {
        double prob = 0.0;
        if (getDouble("brain.connection_probability", prob)) {
            if (prob < 0.0 || prob > 1.0) {
                return false;
            }
        }
    }
    
    return true;
}

bool Config::validate(const std::string& key, const ConfigValue& expectedValue) const {
    if (!has(key)) {
        return false;
    }
    
    const auto& entry = *findEntry(key);
    return entry.value == expectedValue;
}

void Config::addValidationRule(const ValidationRule& rule) {
    pImpl->validationRules.push_back(rule);
}

// Configuration comparison and merging implementation
bool Config::equals(const Config& other) const {
    return pImpl->entries == other.pImpl->entries;
}

void Config::merge(const Config& other, bool override) {
    for (const auto& entry : other.pImpl->entries) {
        if (override || !has(entry.key)) {
            set(entry.key, entry.value, entry.source);
        }
    }
}
