#include <algorithm>
#include <cctype>
#include <sstream>
#include <unordered_set>

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

// Configuration validation functions
bool Config::validate(std::vector<std::string>& errors) const {
    errors.clear();
    
    // Basic validation rules
    std::unordered_set<std::string> requiredKeys = {
        "brain.neuron_count",
        "brain.synapse_density",
        "brain.connection_probability"
    };
    
    // Check for required keys
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            errors.push_back("Missing required configuration key: " + key);
        }
    }
    
    // Validate neuron count
    if (has("brain.neuron_count")) {
        auto neurons = get<size_t>("brain.neuron_count");
        if (neurons == 0) {
            errors.push_back("brain.neuron_count cannot be zero");
        } else if (neurons > 1000000) {
            errors.push_back("brain.neuron_count too large (max: 1000000)");
        }
    }
    
    // Validate synapse density
    if (has("brain.synapse_density")) {
        auto density = get<float>("brain.synapse_density");
        if (density < 0.0f || density > 1.0f) {
            errors.push_back("brain.synapse_density must be between 0.0 and 1.0");
        }
    }
    
    // Validate connection probability
    if (has("brain.connection_probability")) {
        auto prob = get<float>("brain.connection_probability");
        if (prob < 0.0f || prob > 1.0f) {
            errors.push_back("brain.connection_probability must be between 0.0 and 1.0");
        }
    }
    
    // Validate STDP parameters
    if (has("stdp_ltp_weight")) {
        auto weight = get<float>("stdp_ltp_weight");
        if (weight < 0.0f) {
            errors.push_back("stdp_ltp_weight must be non-negative");
        }
    }
    
    if (has("stdp_ltd_weight")) {
        auto weight = get<float>("stdp_ltd_weight");
        if (weight < 0.0f) {
            errors.push_back("stdp_ltd_weight must be non-negative");
        }
    }
    
    if (has("stdp_tau")) {
        auto tau = get<float>("stdp_tau");
        if (tau <= 0.0f) {
            errors.push_back("stdp_tau must be positive");
        }
    }
    
    // Validate developmental stage if present
    if (has("developmental_stage")) {
        auto stage = get<int>("developmental_stage");
        if (stage < 0 || stage > 5) {
            errors.push_back("developmental_stage must be between 0 and 5");
        }
    }
    
    // Validate memory capacity
    if (has("working_memory.capacity")) {
        auto capacity = get<size_t>("working_memory.capacity");
        if (capacity > 10000) {
            errors.push_back("working_memory.capacity too large (max: 10000)");
        }
    }
    
    return errors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    validate(errors);
    return errors;
}

bool Config::isValid() const {
    std::vector<std::string> errors;
    return validate(errors);
}

std::string Config::getValidationReport() const {
    std::vector<std::string> errors;
    validate(errors);
    
    if (errors.empty()) {
        return "✓ Configuration is valid";
    }
    
    std::ostringstream oss;
    oss << "✗ Configuration validation failed with " << errors.size() << " error(s):\n";
    for (size_t i = 0; i < errors.size(); ++i) {
        oss << "  " << (i + 1) << ". " << errors[i] << "\n";
    }
    return oss.str();
}

void Config::applyConfigPresets(const std::string& preset) {
    // Apply common configuration presets
    if (preset == "debug") {
        set("brain.neuron_count", static_cast<int64_t>(100), ConfigSource::Runtime);
        set("brain.synapse_density", 0.2f, ConfigSource::Runtime);
        set("simulation_timestep", 0.0001, ConfigSource::Runtime);
        set("plasticity.stdp.enable", true, ConfigSource::Runtime);
        set("plasticity.hebbian.enable", true, ConfigSource::Runtime);
        set("neuromodulation.curiosity.enable", true, ConfigSource::Runtime);
        set("neuromodulation.novelty.enable", true, ConfigSource::Runtime);
        set("checkpoint.enable", true, ConfigSource::Runtime);
        
    } else if (preset == "production") {
        set("brain.neuron_count", static_cast<int64_t>(5000), ConfigSource::Runtime);
        set("brain.synapse_density", 0.1f, ConfigSource::Runtime);
        set("simulation_timestep", 0.001, ConfigSource::Runtime);
        set("plasticity.stdp.enable", true, ConfigSource::Runtime);
        set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Runtime);
        set("plasticity.hebbian.enable", true, ConfigSource::Runtime);
        set("neuromodulation.curiosity.enable", true, ConfigSource::Runtime);
        set("neuromodulation.novelty.enable", true, ConfigSource::Runtime);
        set("checkpoint.enable", false, ConfigSource::Runtime);
        
    } else if (preset == "research") {
        set("brain.neuron_count", static_cast<int64_t>(10000), ConfigSource::Runtime);
        set("brain.synapse_density", 0.15f, ConfigSource::Runtime);
        set("simulation_timestep", 0.0005, ConfigSource::Runtime);
        set("plasticity.stdp.enable", true, ConfigSource::Runtime);
        set("plasticity.stdp.learning_rate", 0.002f, ConfigSource::Runtime);
        set("plasticity.structural.enable", true, ConfigSource::Runtime);
        set("development.enable", true, ConfigSource::Runtime);
        set("neuromodulation.curiosity.enable", true, ConfigSource::Runtime);
        set("neuromodulation.novelty.enable", true, ConfigSource::Runtime);
        set("checkpoint.enable", true, ConfigSource::Runtime);
        set("checkpoint.save_interval", 1000, ConfigSource::Runtime);
    }
    
    // Unknown preset - silently ignore
}

std::string Config::getPresetList() {
    return "Available presets:\n"
           "  debug    - Low neuron count, high logging, detailed output\n"
           "  production - Medium neuron count, optimized for speed\n"
           "  research  - High neuron count, maximum features enabled\n";
}

// Explicit template instantiations
// (keeping existing template instantiations...)

} // namespace nlm
