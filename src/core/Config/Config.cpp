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

template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Advanced configuration method implementations

// Configure for performance
void Config::configureForPerformance(int threadCount, bool enableSIMD) {
    set("performance.thread_count", static_cast<int64_t>(threadCount));
    set("performance.enable_simd", enableSIMD);
    set("performance.mode", "high_performance");
}

// Configure for accuracy
void Config::configureForAccuracy(bool enableHighPrecision) {
    set("accuracy.enable_high_precision", enableHighPrecision);
    set("accuracy.mode", "high_accuracy");
    // Increase simulation steps for better accuracy
    if (has("neuron_count")) {
        set("neuron_count", getOr<size_t>("neuron_count", 1000) * 2);
    }
}

// Configure for specific use cases
void Config::configureForExploration() {
    set("exploration.enabled", true);
    set("exploration.curiosity_threshold", 0.5f);
    set("exploration.novelty_threshold", 0.3f);
    set("mode", "exploration");
}

void Config::configureForLearning() {
    set("learning.enabled", true);
    set("learning.reward_modulation", true);
    set("learning.plasticity_rate", 0.01f);
    set("mode", "learning");
}

void Config::configureForSimulation() {
    set("simulation.fast_mode", true);
    set("simulation.save_checkpoints", true);
    set("simulation.checkpoint_interval", 1000);
    set("mode", "simulation");
}

void Config::configureForRealTime() {
    set("realtime.enabled", true);
    set("realtime.update_rate", 60.0f);
    set("performance.enable_simd", true);
    set("performance.enable_multithreading", true);
    set("mode", "realtime");
}

// Get system capabilities
bool Config::isPerformanceMode() const {
    return getOr<std::string>("mode", "") == "high_performance";
}

bool Config::isHighPrecisionMode() const {
    return getOr<std::string>("mode", "") == "high_accuracy";
}

bool Config::isRealTimeMode() const {
    return getOr<std::string>("mode", "") == "realtime";
}

// Configuration validation
bool Config::validateConfiguration() const {
    std::string errors = getValidationErrors();
    return errors.empty();
}

std::string Config::getValidationErrors() const {
    std::ostringstream oss;
    
    // Check for required basic settings
    if (!has("random_seed")) {
        oss << "Missing required configuration: random_seed\n";
    }
    
    if (!has("simulation_timestep")) {
        oss << "Missing required configuration: simulation_timestep\n";
    } else {
        try {
            double timestep = getOr<double>("simulation_timestep", 0.0);
            if (timestep <= 0.0) {
                oss << "Invalid simulation_timestep: must be positive\n";
            }
        } catch (...) {
            oss << "Invalid simulation_timestep format\n";
        }
    }
    
    if (!has("neuron_count")) {
        oss << "Missing required configuration: neuron_count\n";
    } else {
        try {
            size_t neuronCount = getOr<size_t>("neuron_count", 0);
            if (neuronCount == 0) {
                oss << "Invalid neuron_count: must be greater than 0\n";
            }
        } catch (...) {
            oss << "Invalid neuron_count format\n";
        }
    }
    
    // Check for potential inconsistencies
    if (has("neuron_count") && has("region_count")) {
        try {
            size_t neuronCount = getOr<size_t>("neuron_count", 0);
            size_t regionCount = getOr<size_t>("region_count", 0);
            if (regionCount > neuronCount) {
                oss << "region_count cannot be greater than neuron_count\n";
            }
        } catch (...) {
            // Ignore parsing errors for consistency check
        }
    }
    
    return oss.str();
}

// Configuration snapshots
bool Config::saveConfigurationSnapshot(const std::string& name) {
    // Simple snapshot implementation - in a real system, this would
    // serialize the configuration to a file or database
    try {
        std::string snapshotFile = "./config_snapshots/" + name + ".cfg";
        std::filesystem::create_directories("./config_snapshots");
        return saveToFile(snapshotFile);
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadConfigurationSnapshot(const std::string& name) {
    // Simple snapshot implementation
    try {
        std::string snapshotFile = "./config_snapshots/" + name + ".cfg";
        return loadFromFile(snapshotFile);
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<std::string> Config::getAvailableSnapshots() const {
    std::vector<std::string> snapshots;
    try {
        if (std::filesystem::exists("./config_snapshots")) {
            for (const auto& entry : std::filesystem::directory_iterator("./config_snapshots")) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();
                    if (filename.size() > 4 && filename.substr(filename.size() - 4) == ".cfg") {
                        snapshots.push_back(filename.substr(0, filename.size() - 4));
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        // If we can't read snapshots, return empty list
    }
    return snapshots;
}

// Explicit template instantiations

template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

// Configuration template methods that need to be fully defined
template bool Config::validateConfiguration() const;
template std::string Config::getValidationErrors() const;
template bool Config::saveConfigurationSnapshot(const std::string& name);
template bool Config::loadConfigurationSnapshot(const std::string& name);
template std::vector<std::string> Config::getAvailableSnapshots() const;

} // namespace nlm
