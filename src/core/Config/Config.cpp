#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()), validationErrors_() {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
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
        } else if (arg[0] == '-' && i + 1 < argc) {
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

void Config::validate() const {
    clearValidationErrors();
    
    // Apply Phase 2 defaults first
    const_cast<Config*>(this)->applyPhase2Defaults();
    
    // Validate neuron count
    if (has("neuron_count")) {
        auto neuronsOpt = get<int64_t>("neuron_count");
        if (neuronsOpt) {
            validateNeurons(*neuronsOpt);
        }
    }
    
    // Validate timestep
    if (has("simulation_timestep")) {
        auto timestepOpt = get<double>("simulation_timestep");
        if (timestepOpt) {
            validateTimestep(*timestepOpt);
        }
    }
    
    // Validate region count
    if (has("region_count")) {
        auto regionsOpt = get<int64_t>("region_count");
        if (regionsOpt) {
            validateRegionCount(*regionsOpt);
        }
    }
    
    // Validate connection probability
    if (has("connection_probability")) {
        auto probOpt = get<double>("connection_probability");
        if (probOpt) {
            validateConnectionProbability(static_cast<float>(*probOpt));
        }
    }
    
    // Validate plasticity parameters
    validatePlasticityParameters();
}

bool Config::isValid() const {
    validate();
    return !hasValidationErrors();
}

std::vector<std::string> Config::getValidationErrors() const {
    validate();
    return validationErrors_;
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
    clearValidationErrors();
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

void Config::applyPhase2Defaults() {
    // Set default values for Phase 2 if not already set
    if (!has("random_seed")) {
        set("random_seed", int64_t(42), ConfigSource::Default);
    }
    
    if (!has("simulation_timestep")) {
        set("simulation_timestep", 0.001, ConfigSource::Default);
    }
    
    if (!has("neuron_count")) {
        set("neuron_count", int64_t(500), ConfigSource::Default);  // Smaller for faster test
    }
    
    if (!has("region_count")) {
        set("region_count", int64_t(1), ConfigSource::Default);
    }
    
    if (!has("connection_probability")) {
        set("connection_probability", 0.15f, ConfigSource::Default);
    }
    
    if (!has("stdp_ltp_weight")) {
        set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    }
    
    if (!has("stdp_ltd_weight")) {
        set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    }
    
    if (!has("stdp_tau")) {
        set("stdp_tau", 20.0f, ConfigSource::Default);
    }
    
    if (!has("synaptogenesis_rate")) {
        set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    }
    
    if (!has("pruning_rate")) {
        set("pruning_rate", 0.00001f, ConfigSource::Default);
    }
}

void Config::validateNeurons(int64_t neurons) const {
    if (!isPositive(neurons)) {
        addValidationError("neuron_count must be positive, got " + std::to_string(neurons));
    } else if (neurons > 10000) {
        addValidationError("neuron_count too large (max 10000), got " + std::to_string(neurons));
    }
}

void Config::validateTimestep(double timestep) const {
    if (!isPositive(timestep)) {
        addValidationError("simulation_timestep must be positive, got " + std::to_string(timestep));
    } else if (timestep > 0.1) {
        addValidationError("simulation_timestep too large (max 0.1s), got " + std::to_string(timestep));
    } else if (timestep < 0.0001) {
        addValidationError("simulation_timestep too small (min 0.0001s), got " + std::to_string(timestep));
    }
}

void Config::validateRegionCount(int64_t regions) const {
    if (!isPositive(regions) || regions > 10) {
        addValidationError("region_count must be between 1 and 10, got " + std::to_string(regions));
    }
}

void Config::validateConnectionProbability(float prob) const {
    if (!isBetween(prob, 0.0f, 1.0f)) {
        addValidationError("connection_probability must be between 0.0 and 1.0, got " + std::to_string(prob));
    }
}

void Config::validatePlasticityParameters() const {
    // Validate STDP parameters
    if (has("stdp_ltp_weight")) {
        auto ltpOpt = get<double>("stdp_ltp_weight");
        if (ltpOpt) {
            if (!isBetween(*ltpOpt, 0.0, 0.1)) {
                addValidationError("stdp_ltp_weight must be between 0.0 and 0.1, got " + std::to_string(*ltpOpt));
            }
        }
    }
    
    if (has("stdp_ltd_weight")) {
        auto ltdOpt = get<double>("stdp_ltd_weight");
        if (ltdOpt) {
            if (!isBetween(*ltdOpt, 0.0, 0.1)) {
                addValidationError("stdp_ltd_weight must be between 0.0 and 0.1, got " + std::to_string(*ltdOpt));
            }
        }
    }
    
    if (has("stdp_tau")) {
        auto tauOpt = get<double>("stdp_tau");
        if (tauOpt) {
            if (!isPositive(*tauOpt) || *tauOpt > 100.0) {
                addValidationError("stdp_tau must be between 0.1 and 100.0, got " + std::to_string(*tauOpt));
            }
        }
    }
    
    // Validate structural plasticity parameters
    if (has("synaptogenesis_rate")) {
        auto synOpt = get<double>("synaptogenesis_rate");
        if (synOpt) {
            if (!isBetween(*synOpt, 0.0, 0.01)) {
                addValidationError("synaptogenesis_rate must be between 0.0 and 0.01, got " + std::to_string(*synOpt));
            }
        }
    }
    
    if (has("pruning_rate")) {
        auto pruneOpt = get<double>("pruning_rate");
        if (pruneOpt) {
            if (!isBetween(*pruneOpt, 0.0, 0.001)) {
                addValidationError("pruning_rate must be between 0.0 and 0.001, got " + std::to_string(*pruneOpt));
            }
        }
    }
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

bool Config::isBoolean(const std::string& value) {
    std::string lower = toLower(value);
    return (lower == "true" || lower == "false" || lower == "1" || lower == "0");
}

bool Config::isNumeric(const std::string& value) {
    try {
        std::stod(value);
        return true;
    } catch (...) {
        return false;
    }
}

std::string Config::convertToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        oss << arg;
    }, value);
    return oss.str();
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