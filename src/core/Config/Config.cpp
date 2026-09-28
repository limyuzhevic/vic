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
        
        // Convert value back to string based on its type
        std::visit([&file, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                // Escape quotes in the string
                std::string escaped = arg;
                size_t pos = 0;
                while ((pos = escaped.find('"', pos)) != std::string::npos) {
                    escaped.insert(pos, "\\");
                    pos += 2;
                }
                file << entry.key << " = \"" << escaped << "\"\n";
            } else if constexpr (std::is_same_v<T, int> || 
                                 std::is_same_v<T, int64_t>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, double>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = ["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = ["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = ["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]\n";
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

bool Config::validateAndFix() const {
    bool hasErrors = false;
    
    // Create a temporary mutable copy for fixing
    auto tempEntries = pImpl->entries;
    
    // Validate neural parameters
    for (auto& entry : tempEntries) {
        if (!validateSingleParameter(entry.key, entry.value)) {
            NLM_LOG_WARNING("Invalid configuration parameter: " + entry.key);
            hasErrors = true;
        }
    }
    
    // Apply fixes and update entries
    for (const auto& entry : tempEntries) {
        auto fixedValue = getFixedValue(entry.key, entry.value);
        if (fixedValue != entry.value) {
            NLM_LOG_INFO("Fixed configuration parameter: " + entry.key + " -> " + toString(fixedValue));
        }
    }
    
    return !hasErrors;
}

void Config::applyUserDefaults() {
    // Apply sensible defaults for common configuration parameters
    if (!has("random_seed")) {
        set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    }
    if (!has("simulation_timestep")) {
        set("simulation_timestep", 0.001, ConfigSource::Default);
    }
    if (!has("neuron_count")) {
        set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    }
    if (!has("region_count")) {
        set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    }
    if (!has("connection_probability")) {
        set("connection_probability", 0.1f, ConfigSource::Default);
    }
    if (!has("stdp_ltp_weight")) {
        set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    }
    if (!has("stdp_ltd_weight")) {
        set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
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

std::string Config::getConfigDescription() const {
    std::ostringstream oss;
    oss << "NLM Neural Learning Machine Configuration Reference\n";
    oss << std::string(60, '=') << "\n\n";
    
    oss << "Core Neural Network Parameters:\n";
    oss << "  neuron_count: Number of neurons in the network (1-1,000,000)\n";
    oss << "  region_count: Number of brain regions (1-100)\n";
    oss << "  connection_probability: Probability of creating connections (0.0-1.0)\n\n";
    
    oss << "Simulation Parameters:\n";
    oss << "  simulation_timestep: Time step duration (0.0001-1.0 seconds)\n\n";
    
    oss << "STDP Plasticity Parameters:\n";
    oss << "  stdp_ltp_weight: Long-term potentiation weight (0.0-1.0)\n";
    oss << "  stdp_ltd_weight: Long-term depression weight (0.0-1.0)\n";
    oss << "  stdp_tau: STDP time constant (1.0-100.0 ms)\n\n";
    
    oss << "Structural Plasticity Parameters:\n";
    oss << "  synaptogenesis_rate: Rate of new connection formation (0.0-0.001)\n";
    oss << "  pruning_rate: Rate of connection removal (0.0-0.0001)\n\n";
    
    oss << "Usage Examples:\n";
    oss << "  ./nlm --neuron_count=5000 --simulation_timestep=0.0005\n";
    oss << "  ./nlm --stdp_ltp_weight=0.05 --stdp_ltd_weight=0.06\n";
    oss << "  ./nlm --config=my_config.cfg\n";
    
    return oss.str();
}

bool Config::validateNeuralParameters() const {
    bool isValid = true;
    
    if (has("neuron_count")) {
        auto neuronCount = get<size_t>("neuron_count");
        if (neuronCount && !isValidNeuronCount(*neuronCount)) {
            NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(*neuronCount) + ". Must be 1-1,000,000");
            isValid = false;
        }
    }
    
    if (has("region_count")) {
        auto regionCount = get<size_t>("region_count");
        if (regionCount && (*regionCount < 1 || *regionCount > 100)) {
            NLM_LOG_ERROR("Invalid region_count: " + std::to_string(*regionCount) + ". Must be 1-100");
            isValid = false;
        }
    }
    
    if (has("connection_probability")) {
        auto connProb = get<float>("connection_probability");
        if (connProb && !isValidConnectionProbability(*connProb)) {
            NLM_LOG_ERROR("Invalid connection_probability: " + std::to_string(*connProb) + ". Must be 0.0-1.0");
            isValid = false;
        }
    }
    
    return isValid;
}

bool Config::validateSimulationParameters() const {
    bool isValid = true;
    
    if (has("simulation_timestep")) {
        auto timestep = get<double>("simulation_timestep");
        if (timestep && !isValidTimestep(*timestep)) {
            NLM_LOG_ERROR("Invalid simulation_timestep: " + std::to_string(*timestep) + ". Must be 0.0001-1.0");
            isValid = false;
        }
    }
    
    return isValid;
}

bool Config::validatePlasticityParameters() const {
    bool isValid = true;
    
    if (has("stdp_ltp_weight")) {
        auto ltpWeight = get<float>("stdp_ltp_weight");
        if (ltpWeight && !isValidWeight(*ltpWeight)) {
            NLM_LOG_ERROR("Invalid stdp_ltp_weight: " + std::to_string(*ltpWeight) + ". Must be -1.0-1.0");
            isValid = false;
        }
    }
    
    if (has("stdp_ltd_weight")) {
        auto ltdWeight = get<float>("stdp_ltd_weight");
        if (ltdWeight && !isValidWeight(*ltdWeight)) {
            NLM_LOG_ERROR("Invalid stdp_ltd_weight: " + std::to_string(*ltdWeight) + ". Must be -1.0-1.0");
            isValid = false;
        }
    }
    
    if (has("stdp_tau")) {
        auto tau = get<float>("stdp_tau");
        if (tau && (*tau < 1.0f || *tau > 100.0f)) {
            NLM_LOG_ERROR("Invalid stdp_tau: " + std::to_string(*tau) + ". Must be 1.0-100.0");
            isValid = false;
        }
    }
    
    return isValid;
}

template<typename T>
T Config::getSafeDefault(const std::string& key) const {
    // Return safe default values for common configuration keys
    if (key == "neuron_count") return static_cast<T>(1000);
    if (key == "region_count") return static_cast<T>(1);
    if (key == "connection_probability") return static_cast<T>(0.1f);
    if (key == "simulation_timestep") return static_cast<T>(0.001);
    if (key == "stdp_ltp_weight") return static_cast<T>(0.01f);
    if (key == "stdp_ltd_weight") return static_cast<T>(0.012f);
    if (key == "stdp_tau") return static_cast<T>(20.0f);
    if (key == "synaptogenesis_rate") return static_cast<T>(0.0001f);
    if (key == "pruning_rate") return static_cast<T>(0.00001f);
    
    // Default fallback
    if constexpr (std::is_same_v<T, int>) return 0;
    if constexpr (std::is_same_v<T, int64_t>) return 0;
    if constexpr (std::is_same_v<T, double>) return 0.0;
    if constexpr (std::is_same_v<T, float>) return 0.0f;
    if constexpr (std::is_same_v<T, bool>) return false;
    if constexpr (std::is_same_v<T, std::string>) return "";
    
    return T{}; // Default constructor
}
