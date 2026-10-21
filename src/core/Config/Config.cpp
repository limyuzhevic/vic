#include "Config.hpp"
#include "ConfigValidator.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <stdexcept>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigSource> keySources;
    std::string validationError;
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
        file << entry.key << " = " << entry.key << "\n";
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
    pImpl->keySources.clear();
    pImpl->validationError.clear();
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
    
    // Add validation status if any
    if (!pImpl->validationError.empty()) {
        oss << "  VALIDATION_ERROR: " << pImpl->validationError << "\n";
    }
    
    return oss.str();
}

// Enhanced getters with error handling and validation
int64_t Config::getInt(const std::string& key) const {
    auto val = get<int64_t>(key);
    if (!val.has_value()) {
        throw ConfigValidationError("Configuration key '" + key + "' not found");
    }
    return val.value();
}

double Config::getDouble(const std::string& key) const {
    auto val = get<double>(key);
    if (!val.has_value()) {
        throw ConfigValidationError("Configuration key '" + key + "' not found");
    }
    return val.value();
}

float Config::getFloat(const std::string& key) const {
    auto val = get<double>(key);
    if (!val.has_value()) {
        throw ConfigValidationError("Configuration key '" + key + "' not found");
    }
    return static_cast<float>(val.value());
}

std::string Config::getString(const std::string& key) const {
    auto val = get<std::string>(key);
    if (!val.has_value()) {
        throw ConfigValidationError("Configuration key '" + key + "' not found");
    }
    return val.value();
}

bool Config::getBool(const std::string& key) const {
    auto val = get<bool>(key);
    if (!val.has_value()) {
        throw ConfigValidationError("Configuration key '" + key + "' not found");
    }
    return val.value();
}

// Convenience methods with safe defaults
int64_t Config::getIntSafe(const std::string& key, int64_t defaultValue) const {
    return getOr<int64_t>(key, defaultValue);
}

double Config::getDoubleSafe(const std::string& key, double defaultValue) const {
    return getOr<double>(key, defaultValue);
}

float Config::getFloatSafe(const std::string& key, float defaultValue) const {
    return getOr<double>(key, static_cast<double>(defaultValue));
}

std::string Config::getStringSafe(const std::string& key, const std::string& defaultValue) const {
    return getOr<std::string>(key, defaultValue);
}

bool Config::getBoolSafe(const std::string& key, bool defaultValue) const {
    return getOr<bool>(key, defaultValue);
}

// Type-specific getters with validation
template<typename T>
T Config::getWithValidation(const std::string& key, const std::function<bool(const T&)>& validator,
                           const std::string& errorMessage) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        throw ConfigValidationError("Configuration key '" + key + "' not found");
    }
    
    if (!validator(val.value())) {
        throw ConfigValidationError(errorMessage);
    }
    
    return val.value();
}

// Validation methods
bool Config::validate() const {
    try {
        ConfigValidator::validate(*this);
        pImpl->validationError.clear();
        return true;
    } catch (const ConfigValidationError& e) {
        pImpl->validationError = e.what();
        return false;
    } catch (...) {
        pImpl->validationError = "Unknown validation error";
        return false;
    }
}

std::string Config::getValidationError() const {
    return pImpl->validationError;
}

// Configuration merging
void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, source);
    }
}

// Enhanced configuration methods
void Config::setNeuronCount(size_t count) {
    set("neuron_count", static_cast<int64_t>(count), ConfigSource::Runtime);
}

void Config::setRegionCount(size_t count) {
    set("region_count", static_cast<int64_t>(count), ConfigSource::Runtime);
}

void Config::setConnectionProbability(float probability) {
    set("connection_probability", probability, ConfigSource::Runtime);
}

void Config::setSimulationTimestep(double timestep) {
    set("simulation_timestep", timestep, ConfigSource::Runtime);
}

void Config::setRandomSeed(uint64_t seed) {
    set("random_seed", static_cast<int64_t>(seed), ConfigSource::Runtime);
}

// Preset configurations
Config Config::createDefaultConfig() {
    Config config;
    config.setNeuronCount(1000);
    config.setRegionCount(1);
    config.setConnectionProbability(0.1f);
    config.setSimulationTimestep(0.001);
    config.setRandomSeed(42);
    config.set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config.set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config.set("pruning_rate", 0.00001f, ConfigSource::Default);
    return config;
}

Config Config::createMinimalConfig() {
    Config config;
    config.setNeuronCount(100);
    config.setRegionCount(1);
    config.setConnectionProbability(0.05f);
    config.setSimulationTimestep(0.001);
    config.setRandomSeed(42);
    return config;
}

Config Config::createDevelopmentConfig() {
    Config config;
    config.setNeuronCount(500);
    config.setRegionCount(2);
    config.setConnectionProbability(0.15f);
    config.setSimulationTimestep(0.001);
    config.setRandomSeed(123);
    config.set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.018f, ConfigSource::Default);
    config.set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);
    config.set("pruning_rate", 0.00002f, ConfigSource::Default);
    config.set("development_enabled", true, ConfigSource::Default);
    config.set("curiosity_enabled", true, ConfigSource::Default);
    return config;
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
// Basic types
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

// GetOr overloads

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm