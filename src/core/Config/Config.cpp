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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
T Config::get(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
bool Config::tryGet(const std::string& key, T& outValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        outValue = val.value();
        return true;
    }
    return false;
}

// Configuration presets for common use cases
static std::shared_ptr<Config> createStandardBrainConfig(size_t neuronCount = 1000) {
    auto config = std::make_shared<Config>();
    
    // Set standard brain parameters
    config->set("brain.neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("brain.synapse_density", 0.1f, ConfigSource::Default);
    config->set("brain.connection_probability", 0.15f, ConfigSource::Default);
    config->set("brain.initial_weight_mean", 0.5f, ConfigSource::Default);
    config->set("brain.initial_weight_std", 0.1f, ConfigSource::Default);
    config->set("brain.v_thresh", -50.0f, ConfigSource::Default);
    config->set("brain.v_rest", -70.0f, ConfigSource::Default);
    config->set("brain.v_reset", -75.0f, ConfigSource::Default);
    config->set("brain.tau_mem", 20.0f, ConfigSource::Default);
    config->set("brain.tau_ref", 2.0f, ConfigSource::Default);
    
    // Plasticity settings
    config->set("plasticity.stdp.enable", true, ConfigSource::Default);
    config->set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Default);
    config->set("plasticity.stdp.tau_plus", 20.0f, ConfigSource::Default);
    config->set("plasticity.stdp.tau_minus", 20.0f, ConfigSource::Default);
    
    // Learning settings
    config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
    config->set("neuromod.novelty.enable", true, ConfigSource::Default);
    
    return config;
}

static std::shared_ptr<Config> createLearningConfig(float learningRate) {
    auto config = createStandardBrainConfig();
    config->set("plasticity.stdp.learning_rate", learningRate, ConfigSource::Default);
    config->set("plasticity.hebbian.learning_rate", learningRate * 0.5f, ConfigSource::Default);
    config->set("neuromod.curiosity.scale", 1.0f / learningRate, ConfigSource::Default);
    return config;
}

static std::shared_ptr<Config> createDebugConfig(bool verboseLogging) {
    auto config = createStandardBrainConfig();
    config->set("logging.level", verboseLogging ? "debug" : "info", ConfigSource::Default);
    config->set("brain.debug_mode", true, ConfigSource::Default);
    config->set("simulation.verbose", verboseLogging, ConfigSource::Default);
    return config;
}

// Get configuration category (for organizing settings)
static std::string getConfigCategory(const std::string& key) {
    std::string lowerKey = toLower(key);
    if (lowerKey.find("neuron") != std::string::npos ||
        lowerKey.find("synapse") != std::string::npos ||
        lowerKey.find("brain") != std::string::npos) {
        return "brain";
    } else if (lowerKey.find("plastic") != std::string::npos) {
        return "plasticity";
    } else if (lowerKey.find("memory") != std::string::npos) {
        return "memory";
    } else if (lowerKey.find("neuromod") != std::string::npos) {
        return "neuromodulation";
    } else if (lowerKey.find("predict") != std::string::npos) {
        return "prediction";
    } else if (lowerKey.find("cognit") != std::string::npos) {
        return "cognition";
    } else if (lowerKey.find("sensor") != std::string::npos) {
        return "sensory";
    } else if (lowerKey.find("motor") != std::string::npos) {
        return "motor";
    } else if (lowerKey.find("devel") != std::string::npos) {
        return "development";
    } else if (lowerKey.find("world") != std::string::npos) {
        return "world";
    } else if (lowerKey.find("action") != std::string::npos) {
        return "action";
    }
    return "general";
}

static bool validateValue(const std::string& key, const ConfigValue& value) {
    // Basic validation - can be extended with more sophisticated checks
    std::string lowerKey = toLower(key);
    
    if (lowerKey.find("rate") != std::string::npos ||
        lowerKey.find("ratio") != std::string::npos) {
        // Check if it's a float within reasonable bounds
        return std::holds_alternative<double>(value) ||
               std::holds_alternative<float>(value) ||
               std::holds_alternative<int>(value) ||
               std::holds_alternative<int64_t>(value);
    } else if (lowerKey.find("count") != std::string::npos ||
               lowerKey.find("size") != std::string::npos) {
        // Check if it's an integer
        return std::holds_alternative<int>(value) ||
               std::holds_alternative<int64_t>(value) ||
               std::holds_alternative<uint64_t>(value);
    }
    return true;
}

bool Config::isInt(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) return false;
    
    return std::holds_alternative<int>(it->value) || std::holds_alternative<int64_t>(it->value);
}

bool Config::isFloat(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) return false;
    
    return std::holds_alternative<double>(it->value) || std::holds_alternative<float>(it->value);
}

bool Config::isString(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) return false;
    
    return std::holds_alternative<std::string>(it->value);
}

bool Config::isBool(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) return false;
    
    return std::holds_alternative<bool>(it->value);
}

bool Config::isVector(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) return false;
    
    return std::holds_alternative<std::vector<int>>(it->value) ||
           std::holds_alternative<std::vector<double>>(it->value) ||
           std::holds_alternative<std::vector<std::string>>(it->value);
}

std::vector<std::string> Config::validate() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        if (!validateValue(entry.key, entry.value)) {
            errors.push_back("Invalid value for key '" + entry.key + "'");
        }
    }
    
    return errors;
}

std::shared_ptr<Config> Config::clone() const {
    auto cloned = std::make_shared<Config>();
    cloned->pImpl->entries = pImpl->entries;
    return cloned;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        ConfigSource entrySource = (source == ConfigSource::Default) ? entry.source : source;
        set(entry.key, entry.value, entrySource);
    }
}

void Config::applyPreset(const std::string& presetName) {
    std::string lowerPreset = toLower(presetName);
    
    if (lowerPreset == "standard") {
        clear();
        *this = *createStandardBrainConfig();
    } else if (lowerPreset == "learning") {
        clear();
        set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Default);
        set("neuromod.curiosity.scale", 1.0f, ConfigSource::Default);
    } else if (lowerPreset == "debug") {
        clear();
        set("logging.level", "debug", ConfigSource::Default);
        set("brain.debug_mode", true, ConfigSource::Default);
    } else {
        // Unknown preset - do nothing
    }
}

// Add missing static method implementations

std::shared_ptr<Config> Config::createStandardBrainConfig(size_t neuronCount) {
    auto config = std::make_shared<Config>();
    
    // Set standard brain parameters
    config->set("brain.neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
    config->set("brain.synapse_density", 0.1f, ConfigSource::Default);
    config->set("brain.connection_probability", 0.15f, ConfigSource::Default);
    config->set("brain.initial_weight_mean", 0.5f, ConfigSource::Default);
    config->set("brain.initial_weight_std", 0.1f, ConfigSource::Default);
    config->set("brain.v_thresh", -50.0f, ConfigSource::Default);
    config->set("brain.v_rest", -70.0f, ConfigSource::Default);
    config->set("brain.v_reset", -75.0f, ConfigSource::Default);
    config->set("brain.tau_mem", 20.0f, ConfigSource::Default);
    config->set("brain.tau_ref", 2.0f, ConfigSource::Default);
    
    // Plasticity settings
    config->set("plasticity.stdp.enable", true, ConfigSource::Default);
    config->set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Default);
    config->set("plasticity.stdp.tau_plus", 20.0f, ConfigSource::Default);
    config->set("plasticity.stdp.tau_minus", 20.0f, ConfigSource::Default);
    
    // Learning settings
    config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
    config->set("neuromod.novelty.enable", true, ConfigSource::Default);
    
    return config;
}

std::shared_ptr<Config> Config::createLearningConfig(float learningRate) {
    auto config = createStandardBrainConfig();
    config->set("plasticity.stdp.learning_rate", learningRate, ConfigSource::Default);
    config->set("plasticity.hebbian.learning_rate", learningRate * 0.5f, ConfigSource::Default);
    config->set("neuromod.curiosity.scale", 1.0f / learningRate, ConfigSource::Default);
    return config;
}

std::shared_ptr<Config> Config::createDebugConfig(bool verboseLogging) {
    auto config = createStandardBrainConfig();
    config->set("logging.level", verboseLogging ? "debug" : "info", ConfigSource::Default);
    config->set("brain.debug_mode", true, ConfigSource::Default);
    config->set("simulation.verbose", verboseLogging, ConfigSource::Default);
    return config;
}

std::string Config::getCategory(const std::string& key) {
    return getConfigCategory(key);
}

static std::string getConfigCategory(const std::string& key) {
    std::string lowerKey = toLower(key);
    if (lowerKey.find("neuron") != std::string::npos ||
        lowerKey.find("synapse") != std::string::npos ||
        lowerKey.find("brain") != std::string::npos) {
        return "brain";
    } else if (lowerKey.find("plastic") != std::string::npos) {
        return "plasticity";
    } else if (lowerKey.find("memory") != std::string::npos) {
        return "memory";
    } else if (lowerKey.find("neuromod") != std::string::npos) {
        return "neuromodulation";
    } else if (lowerKey.find("predict") != std::string::npos) {
        return "prediction";
    } else if (lowerKey.find("cognit") != std::string::npos) {
        return "cognition";
    } else if (lowerKey.find("sensor") != std::string::npos) {
        return "sensory";
    } else if (lowerKey.find("motor") != std::string::npos) {
        return "motor";
    } else if (lowerKey.find("devel") != std::string::npos) {
        return "development";
    } else if (lowerKey.find("world") != std::string::npos) {
        return "world";
    } else if (lowerKey.find("action") != std::string::npos) {
        return "action";
    }
    return "general";
}

// Add missing static method implementations

std::string Config::getCategory(const std::string& key) {
    return getConfigCategory(key);
}

static std::string getConfigCategory(const std::string& key) {
    std::string lowerKey = toLower(key);
    if (lowerKey.find("neuron") != std::string::npos ||
        lowerKey.find("synapse") != std::string::npos ||
        lowerKey.find("brain") != std::string::npos) {
        return "brain";
    } else if (lowerKey.find("plastic") != std::string::npos) {
        return "plasticity";
    } else if (lowerKey.find("memory") != std::string::npos) {
        return "memory";
    } else if (lowerKey.find("neuromod") != std::string::npos) {
        return "neuromodulation";
    } else if (lowerKey.find("predict") != std::string::npos) {
        return "prediction";
    } else if (lowerKey.find("cognit") != std::string::npos) {
        return "cognition";
    } else if (lowerKey.find("sensor") != std::string::npos) {
        return "sensory";
    } else if (lowerKey.find("motor") != std::string::npos) {
        return "motor";
    } else if (lowerKey.find("devel") != std::string::npos) {
        return "development";
    } else if (lowerKey.find("world") != std::string::npos) {
        return "world";
    } else if (lowerKey.find("action") != std::string::npos) {
        return "action";
    }
    return "general";
}

std::vector<std::string> Config::getKeysInCategory(const std::string& category) {
    std::string lowerCategory = toLower(category);
    std::vector<std::string> result;
    
    for (const auto& entry : pImpl->entries) {
        std::string entryCategory = getConfigCategory(entry.key);
        if (entryCategory == lowerCategory) {
            result.push_back(entry.key);
        }
    }
    
    return result;
}
