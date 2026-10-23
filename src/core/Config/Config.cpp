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
        throw ConfigFileErrorException(filepath, "Cannot open file for writing");
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Write the value based on its type
        std::visit([&file](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << value << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (value ? "true" : "false") << "\n";
            } else {
                file << entry.key << " = " << value << "\n";
            }
        }, entry.value);
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

// Forward declaration for validate function
static void validateConfigKey(const std::string& key, const nlm::ConfigValue& value);

// Validation implementation
void Config::validate(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        validateConfigKey(key, it->value);
    }
}

bool Config::validateAll() const {
    for (const auto& entry : pImpl->entries) {
        try {
            validateConfigKey(entry.key, entry.value);
        } catch (const ConfigValidationException& e) {
            return false;
        }
    }
    return true;
}

// Static validation helpers
bool Config::isValidProbability(double value) {
    return value >= MIN_CONNECTION_PROBABILITY && value <= MAX_CONNECTION_PROBABILITY;
}

bool Config::isValidNeuronCount(int value) {
    return value >= MIN_NEURON_COUNT && value <= MAX_NEURON_COUNT;
}

bool Config::isValidRegionCount(int value) {
    return value >= MIN_REGION_COUNT && value <= MAX_NEURON_COUNT; // regions <= neurons
}

bool Config::isValidTimestep(double value) {
    return value >= MIN_SIMULATION_TIMESTEP && value <= MAX_SIMULATION_TIMESTEP;
}

// Key-specific validation
static void validateConfigKey(const std::string& key, const nlm::ConfigValue& value) {
    // Convert key to lowercase for case-insensitive comparison
    std::string lowerKey = nlm::Config::toLower(key);
    
    if (lowerKey == "neuron_count") {
        if (!std::holds_alternative<int>(value)) {
            throw nlm::ConfigInvalidValueException(key, "int");
        }
        int neurons = std::get<int>(value);
        if (!nlm::Config::isValidNeuronCount(neurons)) {
            throw nlm::ConfigValidationException("Neuron count must be between 1 and " + std::to_string(nlm::Config::MAX_NEURON_COUNT));
        }
    }
    else if (lowerKey == "region_count") {
        if (!std::holds_alternative<int>(value)) {
            throw nlm::ConfigInvalidValueException(key, "int");
        }
        int regions = std::get<int>(value);
        if (!nlm::Config::isValidRegionCount(regions)) {
            throw nlm::ConfigValidationException("Region count must be between 1 and " + std::to_string(nlm::Config::MAX_NEURON_COUNT));
        }
    }
    else if (lowerKey == "connection_probability") {
        if (!std::holds_alternative<double>(value)) {
            throw nlm::ConfigInvalidValueException(key, "double");
        }
        double prob = std::get<double>(value);
        if (!nlm::Config::isValidProbability(prob)) {
            throw nlm::ConfigValidationException("Connection probability must be between 0.0 and 1.0");
        }
    }
    else if (lowerKey == "simulation_timestep") {
        if (!std::holds_alternative<double>(value)) {
            throw nlm::ConfigInvalidValueException(key, "double");
        }
        double timestep = std::get<double>(value);
        if (!nlm::Config::isValidTimestep(timestep)) {
            throw nlm::ConfigValidationException("Simulation timestep must be between 0.0 and 1.0");
        }
    }
    // Add more validation rules as needed
}
