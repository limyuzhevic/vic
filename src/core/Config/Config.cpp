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

bool Config::parseCommandLine(int argc, char** argv) {
    // Parse command line arguments and populate configuration
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
        // Handle boolean flags like --help, --verbose
        else if (arg.substr(0, 2) == "--") {
            std::string key = arg.substr(2);
            set(key, "true", ConfigSource::CommandLine);
        }
    }
    return true;
}

std::string Config::getUsage() const {
    std::ostringstream oss;
    oss << "Usage: nlm [options]\n";
    oss << "\nCore Options:\n";
    oss << "  --help, -h                    Show this help message\n";
    oss << "  --config file, -c file        Load configuration from file\n";
    oss << "  --version                     Show version information\n";
    oss << "  --verbose                      Enable verbose logging\n";
    oss << "  --quiet, -q                   Suppress logging output\n";
    oss << "\nBrain Configuration:\n";
    oss << "  --neuron_count N              Number of neurons (default: 1000)\n";
    oss << "  --region_count R              Number of brain regions (default: 2)\n";
    oss << "  --connection_probability P    Connection probability (default: 0.1)\n";
    oss << "  --random_seed S               Random seed (default: 42)\n";
    oss << "\nPlasticity Settings:\n";
    oss << "  --stdp_ltp_weight V           STDP LTP weight (default: 0.01)\n";
    oss << "  --stdp_ltd_weight V           STDP LTD weight (default: 0.012)\n";
    oss << "  --stdp_tau T                  STDP time constant (default: 20.0)\n";
    oss << "\nDevelopment Settings:\n";
    oss << "  --synaptogenesis_rate R       Synaptogenesis rate (default: 0.001)\n";
    oss << "  --pruning_rate R              Pruning rate (default: 0.0001)\n";
    oss << "\nNeuromodulation Settings:\n";
    oss << "  --dopamine_baseline V         Dopamine baseline (default: 0.1)\n";
    oss << "  --curiosity_strength C        Curiosity strength (default: 1.0)\n";
    oss << "\nSimulation Settings:\n";
    oss << "  --simulation_timestep T        Time step in seconds (default: 0.001)\n";
    oss << "  --max_simulation_steps S       Maximum simulation steps (default: 10000)\n";
    oss << "\nMemory Settings:\n";
    oss << "  --working_memory_capacity C    Working memory capacity (default: 100)\n";
    oss << "  --episodic_memory_max E       Maximum episodic memories (default: 1000)\n";
    oss << "\nAdvanced Options:\n";
    oss << "  --enable_curiosity             Enable curiosity-driven exploration\n";
    oss << "  --enable_reward_modulation    Enable reward-modulated plasticity\n";
    oss << "  --enable_structural_plasticity Enable structural plasticity\n";
    oss << "  --enable_development           Enable developmental stages\n";
    oss << "\nExperiment Configuration:\n";
    oss << "  --environment_name NAME        Environment type (default: GridWorld)\n";
    oss << "  --environment_width W          Environment width (default: 10)\n";
    oss << "  --environment_height H         Environment height (default: 10)\n";
    return oss.str();
}

bool Config::validateConfig() const {
    // Basic validation rules
    bool isValid = true;
    
    // Check neuron count is positive
    auto neuronCount = get<int64_t>("neuron_count");
    if (neuronCount.has_value() && neuronCount.value() <= 0) {
        NLM_LOG_ERROR("Invalid neuron_count: must be positive, got " + std::to_string(neuronCount.value()));
        isValid = false;
    }
    
    // Check region count is positive
    auto regionCount = get<int64_t>("region_count");
    if (regionCount.has_value() && regionCount.value() <= 0) {
        NLM_LOG_ERROR("Invalid region_count: must be positive, got " + std::to_string(regionCount.value()));
        isValid = false;
    }
    
    // Check connection probability is in valid range
    auto connectionProb = get<double>("connection_probability");
    if (connectionProb.has_value() && (connectionProb.value() < 0.0 || connectionProb.value() > 1.0)) {
        NLM_LOG_ERROR("Invalid connection_probability: must be in [0, 1], got " + std::to_string(connectionProb.value()));
        isValid = false;
    }
    
    // Check simulation timestep is positive
    auto timestep = get<double>("simulation_timestep");
    if (timestep.has_value() && timestep.value() <= 0.0) {
        NLM_LOG_ERROR("Invalid simulation_timestep: must be positive, got " + std::to_string(timestep.value()));
        isValid = false;
    }
    
    // Check synaptic parameters are positive
    auto ltpWeight = get<double>("stdp_ltp_weight");
    if (ltpWeight.has_value() && ltpWeight.value() < 0.0) {
        NLM_LOG_ERROR("Invalid stdp_ltp_weight: must be positive, got " + std::to_string(ltpWeight.value()));
        isValid = false;
    }
    
    auto ltdWeight = get<double>("stdp_ltd_weight");
    if (ltdWeight.has_value() && ltdWeight.value() < 0.0) {
        NLM_LOG_ERROR("Invalid stdp_ltd_weight: must be positive, got " + std::to_string(ltdWeight.value()));
        isValid = false;
    }
    
    return isValid;
}

std::string Config::getConfigValue(const std::string& key) const {
    auto value = get<std::string>(key);
    return value.has_value() ? value.value() : "";
}

bool Config::parseCommandLine(int argc, char** argv) {
    // Parse command line arguments and populate configuration
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
        // Handle boolean flags like --help, --verbose
        else if (arg.substr(0, 2) == "--") {
            std::string key = arg.substr(2);
            set(key, "true", ConfigSource::CommandLine);
        }
    }
    return true;
}

std::string Config::getUsage() const {
    std::ostringstream oss;
    oss << "Usage: nlm [options]\n";
    oss << "\nCore Options:\n";
    oss << "  --help, -h                    Show this help message\n";
    oss << "  --config file, -c file        Load configuration from file\n";
    oss << "  --version                     Show version information\n";
    oss << "  --verbose                      Enable verbose logging\n";
    oss << "  --quiet, -q                   Suppress logging output\n";
    oss << "\nBrain Configuration:\n";
    oss << "  --neuron_count N              Number of neurons (default: 1000)\n";
    oss << "  --region_count R              Number of brain regions (default: 2)\n";
    oss << "  --connection_probability P    Connection probability (default: 0.1)\n";
    oss << "  --random_seed S               Random seed (default: 42)\n";
    oss << "\nPlasticity Settings:\n";
    oss << "  --stdp_ltp_weight V           STDP LTP weight (default: 0.01)\n";
    oss << "  --stdp_ltd_weight V           STDP LTD weight (default: 0.012)\n";
    oss << "  --stdp_tau T                  STDP time constant (default: 20.0)\n";
    oss << "\nDevelopment Settings:\n";
    oss << "  --synaptogenesis_rate R       Synaptogenesis rate (default: 0.001)\n";
    oss << "  --pruning_rate R              Pruning rate (default: 0.0001)\n";
    oss << "\nNeuromodulation Settings:\n";
    oss << "  --dopamine_baseline V         Dopamine baseline (default: 0.1)\n";
    oss << "  --curiosity_strength C        Curiosity strength (default: 1.0)\n";
    oss << "\nSimulation Settings:\n";
    oss << "  --simulation_timestep T        Time step in seconds (default: 0.001)\n";
    oss << "  --max_simulation_steps S       Maximum simulation steps (default: 10000)\n";
    oss << "\nMemory Settings:\n";
    oss << "  --working_memory_capacity C    Working memory capacity (default: 100)\n";
    oss << "  --episodic_memory_max E       Maximum episodic memories (default: 1000)\n";
    oss << "\nAdvanced Options:\n";
    oss << "  --enable_curiosity             Enable curiosity-driven exploration\n";
    oss << "  --enable_reward_modulation    Enable reward-modulated plasticity\n";
    oss << "  --enable_structural_plasticity Enable structural plasticity\n";
    oss << "  --enable_development           Enable developmental stages\n";
    oss << "\nExperiment Configuration:\n";
    oss << "  --environment_name NAME        Environment type (default: GridWorld)\n";
    oss << "  --environment_width W          Environment width (default: 10)\n";
    oss << "  --environment_height H         Environment height (default: 10)\n";
    return oss.str();
}

bool Config::validateConfig() const {
    // Basic validation rules
    bool isValid = true;
    
    // Check neuron count is positive
    auto neuronCount = get<int64_t>("neuron_count");
    if (neuronCount.has_value() && neuronCount.value() <= 0) {
        NLM_LOG_ERROR("Invalid neuron_count: must be positive, got " + std::to_string(neuronCount.value()));
        isValid = false;
    }
    
    // Check region count is positive
    auto regionCount = get<int64_t>("region_count");
    if (regionCount.has_value() && regionCount.value() <= 0) {
        NLM_LOG_ERROR("Invalid region_count: must be positive, got " + std::to_string(regionCount.value()));
        isValid = false;
    }
    
    // Check connection probability is in valid range
    auto connectionProb = get<double>("connection_probability");
    if (connectionProb.has_value() && (connectionProb.value() < 0.0 || connectionProb.value() > 1.0)) {
        NLM_LOG_ERROR("Invalid connection_probability: must be in [0, 1], got " + std::to_string(connectionProb.value()));
        isValid = false;
    }
    
    // Check simulation timestep is positive
    auto timestep = get<double>("simulation_timestep");
    if (timestep.has_value() && timestep.value() <= 0.0) {
        NLM_LOG_ERROR("Invalid simulation_timestep: must be positive, got " + std::to_string(timestep.value()));
        isValid = false;
    }
    
    // Check synaptic parameters are positive
    auto ltpWeight = get<double>("stdp_ltp_weight");
    if (ltpWeight.has_value() && ltpWeight.value() < 0.0) {
        NLM_LOG_ERROR("Invalid stdp_ltp_weight: must be positive, got " + std::to_string(ltpWeight.value()));
        isValid = false;
    }
    
    auto ltdWeight = get<double>("stdp_ltd_weight");
    if (ltdWeight.has_value() && ltdWeight.value() < 0.0) {
        NLM_LOG_ERROR("Invalid stdp_ltd_weight: must be positive, got " + std::to_string(ltdWeight.value()));
        isValid = false;
    }
    
    return isValid;
}

std::string Config::getConfigValue(const std::string& key) const {
    auto value = get<std::string>(key);
    return value.has_value() ? value.value() : "";
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
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

bool Config::isNumeric(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

ConfigValue Config::stringToConfigValue(const std::string& str) {
    // Try to parse as different types in order of specificity
    if (str.empty()) {
        return std::string("");
    }
    
    // Boolean values
    if (str == "true" || str == "false") {
        return str == "true";
    }
    
    // Integer values
    try {
        size_t pos;
        int64_t intVal = std::stoll(str, &pos);
        if (pos == str.size()) {
            return intVal;
        }
    } catch (...) {}
    
    // Double values
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    // String values (default)
    return str;
}

std::string Config::configValueToString(const ConfigValue& value) {
    return std::visit([](auto&& v) -> std::string {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return v;
        } else if constexpr (std::is_same_v<T, int>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, bool>) {
            return v ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i) oss << ", ";
                oss << v[i];
            }
            oss << "]";
            return oss.str();
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i) oss << ", ";
                oss << v[i];
            }
            oss << "]";
            return oss.str();
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i) oss << ", ";
                oss << "\"" << v[i] << "\"";
            }
            oss << "]";
            return oss.str();
        }
        return "";
    }, value);
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
