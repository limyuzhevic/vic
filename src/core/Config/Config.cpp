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

// Validation method implementations
bool Config::validate(std::vector<std::string>& errors) const {
    errors.clear();
    
    for (const auto& entry : pImpl->entries) {
        bool isValid = true;
        std::string errorMsg;
        
        if (entry.key == "neuron_count") {
            if (!validateNeuronCount(std::get<int>(entry.value))) {
                isValid = false;
                errorMsg = "neuron_count must be between 1 and 100,000";
            }
        } else if (entry.key == "region_count") {
            if (!validateRegionCount(std::get<int>(entry.value))) {
                isValid = false;
                errorMsg = "region_count must be between 1 and 10";
            }
        } else if (entry.key == "connection_probability") {
            if (!validateConnectionProbability(std::get<double>(entry.value))) {
                isValid = false;
                errorMsg = "connection_probability must be between 0.0 and 1.0";
            }
        } else if (entry.key == "plasticity_learning_rate") {
            if (!validatePlasticityRate(std::get<double>(entry.value))) {
                isValid = false;
                errorMsg = "plasticity_learning_rate must be between 0.0 and 1.0";
            }
        } else if (entry.key == "dopamine_baseline") {
            if (!validateDopamineBaseline(std::get<double>(entry.value))) {
                isValid = false;
                errorMsg = "dopamine_baseline must be between 0.0 and 1.0";
            }
        } else if (entry.key == "novelty_threshold") {
            if (!validateNoveltyThreshold(std::get<double>(entry.value))) {
                isValid = false;
                errorMsg = "novelty_threshold must be between 0.0 and 1.0";
            }
        }
        
        if (!isValid) {
            errors.push_back(entry.key + ": " + errorMsg);
        }
    }
    
    return errors.empty();
}

bool Config::isValid() const {
    std::vector<std::string> errors;
    return validate(errors);
}

std::string Config::validationErrorMessage() const {
    std::vector<std::string> errors;
    if (validate(errors)) {
        return "Configuration is valid";
    }
    
    std::ostringstream oss;
    oss << "Configuration validation errors:\n";
    for (const auto& error : errors) {
        oss << "  - " << error << "\n";
    }
    return oss.str();
}

bool Config::validateNeuronCount(int count) {
    return count > 0 && count <= 100000;
}

bool Config::validateRegionCount(int count) {
    return count > 0 && count <= 10;
}

bool Config::validateConnectionProbability(float prob) {
    return prob >= 0.0f && prob <= 1.0f;
}

bool Config::validatePlasticityRate(float rate) {
    return rate >= 0.0f && rate <= 1.0f;
}

bool Config::validateDopamineBaseline(float baseline) {
    return baseline >= 0.0f && baseline <= 1.0f;
}

bool Config::validateNoveltyThreshold(float threshold) {
    return threshold >= 0.0f && threshold <= 1.0f;
}

bool Config::hasValidationErrors() const {
    std::vector<std::string> errors;
    return !validate(errors);
}

} // namespace nlm
