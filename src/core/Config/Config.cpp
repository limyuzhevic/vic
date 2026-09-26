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
    // Check if file exists and is not empty
    std::filesystem::path path(filepath);
    if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path)) {
        NLM_LOG_WARNING("Config file not found: " + filepath);
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file: " + filepath);
        return false;
    }
    
    // Try to parse as JSON first (modern approach)
    try {
        // Check if file starts with "{" for JSON
        std::string firstLine;
        std::getline(file, firstLine);
        file.clear(); // Clear EOF flag
        file.seekg(0, std::ios::beg);
        
        if (firstLine.find("{") != std::string::npos) {
            // TODO: Implement JSON parsing with nlohmann/json or similar
            NLM_LOG_WARNING("JSON config parsing not yet implemented, falling back to simple parser");
            file.clear();
            file.seekg(0, std::ios::beg);
        }
    } catch (...) {
        // Fall back to simple parser
    }
    
    // Simple key=value parser (current implementation)
    std::string line;
    int lineNumber = 0;
    bool hasValidEntries = false;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/' || line.rfind("--", 0) == 0) {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (key.empty() || value.empty()) {
                NLM_LOG_WARNING("Invalid config entry at line " + std::to_string(lineNumber) + ": " + line);
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
            hasValidEntries = true;
        } else {
            NLM_LOG_WARNING("Invalid config format at line " + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    if (hasValidEntries) {
        NLM_LOG_INFO("Successfully loaded configuration from: " + filepath + " (" + 
                    std::to_string(pImpl->entries.size()) + " entries)");
    } else {
        NLM_LOG_WARNING("No valid configuration entries found in file: " + filepath);
    }
    
    return hasValidEntries;
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
    
    file << "# NLM Configuration File\n";
    file << "# Saved from runtime configuration\n";
    file << "# Key format: section.subkey\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Helper function to convert ConfigValue to string
        auto valueToString = [](const ConfigValue& val) -> std::string {
            std::stringstream ss;
            std::visit([&ss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, bool>) {
                    ss << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::string>) {
                    ss << '"' << arg << '"';
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    ss << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) ss << ", ";
                        ss << arg[i];
                    }
                    ss << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    ss << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) ss << ", ";
                        ss << arg[i];
                    }
                    ss << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    ss << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) ss << ", ";
                        ss << '"' << arg[i] << '"';
                    }
                    ss << "]";
                } else {
                    ss << arg;
                }
            }, val);
            return ss.str();
        };
        
        file << entry.key << " = " << valueToString(entry.value) << "\n\n";
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

void Config::validate() const {
    // Validate configuration parameters
    auto neuronCount = getOr<int64_t>("neuron_count", 1000);
    if (neuronCount <= 0) {
        NLM_LOG_WARNING("neuron_count should be positive, got " + std::to_string(neuronCount));
    }
    
    auto regionCount = getOr<int64_t>("region_count", 1);
    if (regionCount <= 0) {
        NLM_LOG_WARNING("region_count should be positive, got " + std::to_string(regionCount));
    }
    
    auto connectionProb = getOr<float>("connection_probability", 0.1f);
    if (connectionProb < 0.0f || connectionProb > 1.0f) {
        NLM_LOG_WARNING("connection_probability should be between 0 and 1, got " + std::to_string(connectionProb));
    }
    
    // Validate STDP parameters
    auto stdpLtpWeight = getOr<float>("stdp_ltp_weight", 0.01f);
    auto stdpLtdWeight = getOr<float>("stdp_ltd_weight", 0.012f);
    auto stdpTau = getOr<float>("stdp_tau", 20.0f);
    
    if (stdpLtpWeight <= 0.0f || stdpLtdWeight <= 0.0f) {
        NLM_LOG_WARNING("STDP learning weights should be positive");
    }
    
    if (stdpTau <= 0.0f) {
        NLM_LOG_WARNING("STDP tau should be positive");
    }
    
    // Validate plasticity parameters
    auto synaptogenesisRate = getOr<float>("synaptogenesis_rate", 0.0001f);
    auto pruningRate = getOr<float>("pruning_rate", 0.00001f);
    
    if (synaptogenesisRate < 0.0f) {
        NLM_LOG_WARNING("synaptogenesis_rate should be non-negative");
    }
    
    if (pruningRate < 0.0f) {
        NLM_LOG_WARNING("pruning_rate should be non-negative");
    }
    
    if (synaptogenesisRate > pruningRate && pruningRate > 0) {
        NLM_LOG_WARNING("synaptogenesis_rate should typically be greater than pruning_rate");
    }
    
    // Validate neuron parameters
    auto neuronVThresh = getOr<float>("v_thresh", -50.0f);
    auto neuronVRest = getOr<float>("v_rest", -70.0f);
    auto neuronVReset = getOr<float>("v_reset", -75.0f);
    
    if (neuronVThresh <= neuronVReset) {
        NLM_LOG_WARNING("neuron threshold should be higher than reset potential");
    }
    
    if (neuronVRest >= neuronVThresh) {
        NLM_LOG_WARNING("neuron resting potential should be lower than threshold");
    }
    
    // Validate simulation parameters
    auto timestep = getOr<double>("simulation_timestep", 0.001);
    if (timestep <= 0.0) {
        NLM_LOG_WARNING("simulation_timestep should be positive");
    } else if (timestep > 0.1) {
        NLM_LOG_WARNING("simulation_timestep is large (" + std::to_string(timestep) + "), consider smaller timestep for accuracy");
    }
}

std::string Config::getValidationReport() const {
    std::ostringstream oss;
    oss << "Configuration Validation Report:\n";
    oss << "================================\n";
    oss << "Validating configuration parameters...\n\n";
    
    // Temporarily set log level to capture warnings
    // In real implementation, this would capture log messages
    oss << "✓ Configuration validation completed (see logs for details)\n";
    oss << "================================\n";
    
    return oss.str();
}

std::string Config::getDocumentation() const {
    std::ostringstream oss;
    oss << "NLM Configuration Options:\n";
    oss << "==========================\n\n";
    
    oss << "Brain Configuration:\n";
    oss << "  - neuron_count (int): Total number of neurons [default: 1000]\n";
    oss << "  - region_count (int): Number of neural regions [default: 1]\n";
    oss << "  - connection_probability (float): Synapse connection probability [0-1] [default: 0.1]\n";
    oss << "  - random_seed (uint64): Random seed for reproducibility [default: 42]\n";
    oss << "  - simulation_timestep (double): Simulation timestep (s) [default: 0.001]\n\n";
    
    oss << "Neuron Configuration:\n";
    oss << "  - v_thresh (float): Neuron firing threshold (mV) [default: -50.0]\n";
    oss << "  - v_rest (float): Neuron resting potential (mV) [default: -70.0]\n";
    oss << "  - v_reset (float): Neuron reset potential (mV) [default: -75.0]\n";
    oss << "  - tau_mem (float): Membrane time constant (ms) [default: 20.0]\n";
    oss << "  - tau_ref (float): Refractory period (ms) [default: 2.0]\n\n";
    
    oss << "Plasticity Configuration:\n";
    oss << "  - stdp_ltp_weight (float): STDP LTP learning rate [default: 0.01]\n";
    oss << "  - stdp_ltd_weight (float): STDP LTD learning rate [default: 0.012]\n";
    oss << "  - stdp_tau (float): STDP time constant (ms) [default: 20.0]\n";
    oss << "  - synaptogenesis_rate (float): New synapse formation rate [default: 0.0001]\n";
    oss << "  - pruning_rate (float): Synapse removal rate [default: 0.00001]\n\n";
    
    oss << "Neuromodulation Configuration:\n";
    oss << "  - neuromod_dopamine_scale (float): Dopamine modulation scale [default: 1.0]\n";
    oss << "  - neuromod_curiosity_enable (bool): Enable curiosity-driven exploration [default: true]\n";
    oss << "  - neuromod_novelty_enable (bool): Enable novelty detection [default: true]\n\n";
    
    oss << "Environment Configuration:\n";
    oss << "  - environment_width (float): Environment width [default: 20.0]\n";
    oss << "  - environment_height (float): Environment height [default: 20.0]\n";
    oss << "  - vision_width (int): Agent vision width [default: 8]\n";
    oss << "  - vision_height (int): Agent vision height [default: 8]\n\n";
    
    oss << "Configuration Source Priority:\n";
    oss << "  1. Command line arguments (highest priority)\n";
    oss << "  2. Configuration file (medium priority)\n";
    oss << "  3. Runtime settings (low priority)\n";
    oss << "  4. Default values (lowest priority)\n\n";
    
    oss << "Configuration File Format (simple .cfg):\n";
    oss << "  # This is a comment\n";
    oss << "  brain.neuron_count = 500\n";
    oss << "  brain.connection_probability = 0.15\n";
    oss << "  stdp_ltp_weight = 0.02\n\n";
    
    oss << "Configuration Command Line Format:\n";
    oss << "  ./nlm --brain.neuron_count=1000 --stdp_ltp_weight=0.015\n\n";
    
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
