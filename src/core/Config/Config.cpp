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

    // Add validation method to Config class
    bool validate() const {
        std::string errors;
        
        // Validate simulation_timestep
        if (auto ts = get<double>("simulation_timestep")) {
            if (*ts <= 0.0) {
                errors += "simulation_timestep must be positive (got " + std::to_string(*ts) + ")\n";
            }
            if (*ts > 0.1) {  // Reasonable upper bound
                errors += "simulation_timestep should be small (< 0.1s) for numerical stability (got " + std::to_string(*ts) + ")\n";
            }
        }
        
        // Validate neuron_count
        if (auto nc = get<size_t>("neuron_count")) {
            if (*nc == 0) {
                errors += "neuron_count must be positive (got " + std::to_string(*nc) + ")\n";
            }
            if (*nc > 1000000) {
                errors += "neuron_count seems too large for single simulation (got " + std::to_string(*nc) + ")\n";
            }
        }
        
        // Validate region_count
        if (auto rc = get<size_t>("region_count")) {
            if (*rc < 1) {
                errors += "region_count must be at least 1 (got " + std::to_string(*rc) + ")\n";
            }
        }
        
        // Validate connection_probability
        if (auto cp = get<double>("connection_probability")) {
            if (*cp < 0.0 || *cp > 1.0) {
                errors += "connection_probability must be between 0 and 1 (got " + std::to_string(*cp) + ")\n";
            }
        }
        
        // Validate dopamine_baseline
        if (auto db = get<double>("dopamine_baseline")) {
            if (*db < 0.0 || *db > 1.0) {
                errors += "dopamine_baseline should be between 0.0 and 1.0 (got " + std::to_string(*db) + ")\n";
            }
        }
        
        // Validate reward_discount_factor
        if (auto rdf = get<double>("reward_discount_factor")) {
            if (*rdf < 0.0 || *rdf > 1.0) {
                errors += "reward_discount_factor must be between 0 and 1 (got " + std::to_string(*rdf) + ")\n";
            }
        }
        
        // Validate environment dimensions
        if (auto ew = get<size_t>("environment_width")) {
            if (*ew == 0) {
                errors += "environment_width must be positive (got " + std::to_string(*ew) + ")\n";
            }
        }
        
        if (auto eh = get<size_t>("environment_height")) {
            if (*eh == 0) {
                errors += "environment_height must be positive (got " + std::to_string(*eh) + ")\n";
            }
        }
        
        // Validate max_simulation_steps
        if (auto max_steps = get<size_t>("max_simulation_steps")) {
            if (*max_steps == 0) {
                errors += "max_simulation_steps must be positive (got " + std::to_string(*max_steps) + ")\n";
            }
            if (*max_steps > 1000000000) {
                errors += "max_simulation_steps seems too large (got " + std::to_string(*max_steps) + ")\n";
            }
        }
        
        // Validate development rates
        if (auto sr = get<double>("synaptogenesis_rate")) {
            if (*sr < 0.0) {
                errors += "synaptogenesis_rate must be non-negative (got " + std::to_string(*sr) + ")\n";
            }
            if (*sr > 1.0) {
                errors += "synaptogenesis_rate should be <= 1.0 (got " + std::to_string(*sr) + ")\n";
            }
        }
        
        if (auto pr = get<double>("pruning_rate")) {
            if (*pr < 0.0) {
                errors += "pruning_rate must be non-negative (got " + std::to_string(*pr) + ")\n";
            }
            if (*pr > 1.0) {
                errors += "pruning_rate should be <= 1.0 (got " + std::to_string(*pr) + ")\n";
            }
        }
        
        // Validate plasticity parameters
        if (auto ltp = get<double>("stdp_ltp_weight")) {
            if (*ltp < 0.0) {
                errors += "stdp_ltp_weight must be non-negative (got " + std::to_string(*ltp) + ")\n";
            }
        }
        
        if (auto ltd = get<double>("stdp_ltd_weight")) {
            if (*ltd < 0.0) {
                errors += "stdp_ltd_weight must be non-negative (got " + std::to_string(*ltd) + ")\n";
            }
        }
        
        if (!errors.empty()) {
            std::cerr << "Configuration validation failed:\n" << errors;
            return false;
        }
        
        return true;
    }
