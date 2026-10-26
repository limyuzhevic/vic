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
    // Validate input parameter
    if (filepath.empty()) {
        throw std::invalid_argument("Config filepath cannot be empty");
    }
    
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
            
            // Validate key
            if (key.empty()) {
                continue;  // Skip lines with empty keys
            }
            
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
    if (argc <= 0) {
        throw std::invalid_argument("Config: argc cannot be negative or zero");
    }
    
    if (!argv) {
        throw std::invalid_argument("Config: argv cannot be null");
    }
    
    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) {
            throw std::invalid_argument("Config: argv[" + std::to_string(i) + "] cannot be null");
        }
        
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key
                if (key.empty()) {
                    continue;  // Skip lines with empty keys
                }
                
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            
            // Validate key
            if (key.empty()) {
                continue;  // Skip lines with empty keys
            }
            
            if (!argv[i + 1]) {
                throw std::invalid_argument("Config: argv[" + std::to_string(i + 1) + "] cannot be null");
            }
            
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Validate input parameter
    if (filepath.empty()) {
        throw std::invalid_argument("Config filepath cannot be empty");
    }
    
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
    // Validate input parameter
    if (key.empty()) {
        throw std::invalid_argument("Config key cannot be empty");
    }
    
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
    // Validate input parameters
    if (key.empty()) {
        throw std::invalid_argument("Config key cannot be empty");
    }
    
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
        throw std::invalid_argument("Config key cannot be empty");
    }
    
    // Add specific validation for known keys
    if (key == "neuron_count") {
        try {
            size_t count = std::stoul(value);
            if (count == 0) {
                throw std::invalid_argument("Neuron count must be greater than 0");
            }
            if (count > 1000000) {
                throw std::invalid_argument("Neuron count too large (max 1000000)");
            }
        } catch (const std::exception& e) {
            throw std::invalid_argument("Invalid neuron count value '" + value + "': " + std::string(e.what()));
        }
    } else if (key == "connection_probability") {
        try {
            double prob = std::stod(value);
            if (prob < 0.0 || prob > 1.0) {
                throw std::invalid_argument("Connection probability must be between 0.0 and 1.0");
            }
        } catch (const std::exception& e) {
            throw std::invalid_argument("Invalid connection probability value '" + value + "': " + std::string(e.what()));
        }
    } else if (key == "simulation_timestep") {
        try {
            double timestep = std::stod(value);
            if (timestep <= 0.0) {
                throw std::invalid_argument("Simulation timestep must be greater than 0");
            }
            if (timestep > 1.0) {
                throw std::invalid_argument("Simulation timestep too large (max 1.0)");
            }
        } catch (const std::exception& e) {
            throw std::invalid_argument("Invalid simulation timestep value '" + value + "': " + std::string(e.what()));
        }
    } else if (key == "random_seed") {
        try {
            uint64_t seed = std::stoull(value);
            if (seed == 0) {
                throw std::invalid_argument("Random seed must be non-zero");
            }
        } catch (const std::exception& e) {
            throw std::invalid_argument("Invalid random seed value '" + value + "': " + std::string(e.what()));
        }
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        throw std::invalid_argument("Config key cannot be empty");
    }
    
    // Add specific validation for known keys
    if (key == "neuron_count") {
        if (value == 0) {
            throw std::invalid_argument("Neuron count must be greater than 0");
        }
        if (value > 1000000) {
            throw std::invalid_argument("Neuron count too large (max 1000000)");
        }
    } else if (key == "region_count") {
        if (value == 0) {
            throw std::invalid_argument("Region count must be greater than 0");
        }
    } else if (key == "random_seed") {
        if (value == 0) {
            throw std::invalid_argument("Random seed must be non-zero");
        }
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        throw std::invalid_argument("Config key cannot be empty");
    }
    
    // Add specific validation for known keys
    if (key == "simulation_timestep") {
        if (value <= 0.0) {
            throw std::invalid_argument("Simulation timestep must be greater than 0");
        }
        if (value > 1.0) {
            throw std::invalid_argument("Simulation timestep too large (max 1.0)");
        }
    } else if (key == "plasticity.stdp.learning_rate") {
        if (value < 0.0) {
            throw std::invalid_argument("STDP learning rate must be non-negative");
        }
    } else if (key == "plasticity.hebbian.learning_rate") {
        if (value < 0.0) {
            throw std::invalid_argument("Hebbian learning rate must be non-negative");
        }
    } else if (key == "neuromod.dopamine.scale") {
        if (value < 0.0) {
            throw std::invalid_argument("Dopamine scale must be non-negative");
        }
    } else if (key == "neuromod.curiosity.threshold") {
        if (value < 0.0 || value > 1.0) {
            throw std::invalid_argument("Curiosity threshold must be between 0.0 and 1.0");
        }
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        throw std::invalid_argument("Config key cannot be empty");
    }
    
    // Add specific validation for known keys
    if (key == "plasticity.stdp.enable" || key == "plasticity.hebbian.enable" || 
        key == "plasticity.structural.enable" || key == "neuromod.curiosity.enable" ||
        key == "neuromod.novelty.enable") {
        // These are boolean flags, no additional validation needed
    }
    
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    // Validate input parameter
    if (key.empty()) {
        throw std::invalid_argument("Config key cannot be empty");
    }
    
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    // Validate input parameter
    if (key.empty()) {
        throw std::invalid_argument("Config key cannot be empty");
    }
    
    if (!has(key)) {
        throw std::invalid_argument("Config key does not exist: " + key);
    }
    
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
