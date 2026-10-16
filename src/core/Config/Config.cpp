#include <limits>

// Validation helper functions
namespace {
    bool validateNeuronCount(int64_t value) {
        return value > 0 && value <= 1000000; // Reasonable bounds
    }
    
    bool validateRegionCount(int64_t value) {
        return value > 0 && value <= 100;
    }
    
    bool validateConnectionProbability(float value) {
        return value >= 0.0f && value <= 1.0f;
    }
    
    bool validateTimestep(double value) {
        return value > 0.0 && value <= 1.0; // Max 1 second timestep
    }
    
    bool validateRandomSeed(int64_t value) {
        return value >= 0 && value <= std::numeric_limits<uint64_t>::max();
    }
    
    bool validatePlasticityRate(float value) {
        return value >= 0.0f && value <= 1.0f;
    }
    
    bool validateStdpWeight(float value) {
        return value > -10.0f && value < 10.0f; // Reasonable range
    }
}

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
        // Log error and continue with defaults
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    bool hasErrors = false;
    
    while (std::getline(file, line)) {
        lineNumber++;
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
            
            // Validate key format
            if (key.empty()) {
                // Skip malformed lines but continue processing
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Attempt to set value with error handling
            try {
                set(key, value, ConfigSource::File);
            } catch (const std::exception& e) {
                // Log parsing error but continue with remaining lines
                hasErrors = true;
                continue;
            }
        } else {
            // Malformed line without '='
            hasErrors = true;
            continue;
        }
    }
    
    return !hasErrors;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 1) {
        return true; // No arguments to process
    }
    
    bool hasErrors = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        try {
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    // Malformed argument without '='
                    hasErrors = true;
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            } else {
                // Unknown argument format
                hasErrors = true;
            }
        } catch (const std::exception& e) {
            // Log parsing error but continue with remaining arguments
            hasErrors = true;
            continue;
        }
    }
    
    return !hasErrors;
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
    // Validate key
    if (key.empty()) {
        throw std::invalid_argument("Configuration key cannot be empty");
    }
    if (key.find('=') != std::string::npos) {
        throw std::invalid_argument("Configuration key cannot contain '=' character: " + key);
    }
    
    // Validate value format
    std::string trimmedValue = trim(value);
    if (trimmedValue.empty() && !value.empty()) {
        throw std::invalid_argument("Configuration value cannot be only whitespace: " + key);
    }
    
    set(key, ConfigValue(trimmedValue), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        throw std::invalid_argument("Configuration key cannot be empty");
    }
    
    // Validate known configuration keys
    if (key == "neuron_count" && !validateNeuronCount(value)) {
        throw std::invalid_argument("Invalid neuron count: " + std::to_string(value) + ". Must be between 1 and 1,000,000.");
    } else if (key == "region_count" && !validateRegionCount(value)) {
        throw std::invalid_argument("Invalid region count: " + std::to_string(value) + ". Must be between 1 and 100.");
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        throw std::invalid_argument("Configuration key cannot be empty");
    }
    
    // Validate known configuration keys
    if (key == "simulation_timestep" && !validateTimestep(value)) {
        throw std::invalid_argument("Invalid simulation timestep: " + std::to_string(value) + ". Must be between 0 and 1 seconds.");
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, float value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        throw std::invalid_argument("Configuration key cannot be empty");
    }
    
    // Validate known configuration keys
    if (key == "connection_probability" && !validateConnectionProbability(value)) {
        throw std::invalid_argument("Invalid connection probability: " + std::to_string(value) + ". Must be between 0 and 1.");
    } else if (key == "plasticity_learning_rate" && !validatePlasticityRate(value)) {
        throw std::invalid_argument("Invalid plasticity learning rate: " + std::to_string(value) + ". Must be between 0 and 1.");
    } else if (key == "synaptogenesis_rate" && !validatePlasticityRate(value)) {
        throw std::invalid_argument("Invalid synaptogenesis rate: " + std::to_string(value) + ". Must be between 0 and 1.");
    } else if (key == "pruning_rate" && !validatePlasticityRate(value)) {
        throw std::invalid_argument("Invalid pruning rate: " + std::to_string(value) + ". Must be between 0 and 1.");
    } else if (key == "stdp_ltp_weight" && !validateStdpWeight(value)) {
        throw std::invalid_argument("Invalid STDP LTP weight: " + std::to_string(value) + ". Must be between -10 and 10.");
    } else if (key == "stdp_ltd_weight" && !validateStdpWeight(value)) {
        throw std::invalid_argument("Invalid STDP LTD weight: " + std::to_string(value) + ". Must be between -10 and 10.");
    }
    
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

} // namespace nlm
