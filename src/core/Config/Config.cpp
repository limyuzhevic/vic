#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <typeinfo>

namespace nlm {

// Type name to string mapping for serialization
std::string TypeNameMapper::toString(const ConfigValue& value) {
    std::visit([](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) return "std::vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) return "std::vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "std::vector<std::string>";
        else return "unknown";
    }, value);
}

std::optional<ConfigValue> TypeNameMapper::fromString(const std::string& typeName, const std::string& value) {
    if (typeName == "int") {
        try { return std::stoi(value); } catch (...) { return std::nullopt; }
    } else if (typeName == "int64_t") {
        try { return static_cast<int64_t>(std::stoll(value)); } catch (...) { return std::nullopt; }
    } else if (typeName == "double") {
        try { return std::stod(value); } catch (...) { return std::nullopt; }
    } else if (typeName == "bool") {
        if (value == "true" || value == "1") return true;
        if (value == "false" || value == "0") return false;
    } else if (typeName == "string") {
        return value;
    }
    return std::nullopt;
}

// Configuration schema validation
void ConfigSchema::addField(const Field& field) {
    fields_.push_back(field);
}

bool ConfigSchema::validate(const Config& config) const {
    validationError_.clear();
    
    for (const auto& field : fields_) {
        if (field.required && !config.has(field.name)) {
            validationError_ = "Missing required field: " + field.name;
            return false;
        }
        
        if (config.has(field.name)) {
            auto value = config.get(field.name);
            if (value.has_value()) {
                // Type validation would go here
                if (field.validator && !field.validator(value.value())) {
                    validationError_ = "Validation failed for field: " + field.name;
                    return false;
                }
            }
        }
    }
    
    return true;
}

std::string ConfigSchema::getValidationError() const {
    return validationError_;
}

// Configuration validation
bool ConfigValidator::validate(const std::string& key, const ConfigValue& value) {
    // Basic validation - can be extended
    if (key.empty()) return false;
    return true;
}

std::string ConfigValidator::getValidationError(const std::string& key, const ConfigValue& value) {
    if (key.empty()) return "Key cannot be empty";
    if (std::holds_alternative<std::monostate>(value)) return "Value is empty";
    return "";
}

// JSON serialization helper
nlohmann::json configValueToJson(const ConfigValue& value) {
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            // Return as integer
        } else if constexpr (std::is_same_v<T, double>) {
            // Return as float/double
        } else if constexpr (std::is_same_v<T, bool>) {
            // Return as boolean
        } else if constexpr (std::is_same_v<T, std::string>) {
            // Return as string
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            // Return as array
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            // Return as array
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            // Return as array
        }
    }, value);
    return nlohmann::json(nullptr); // Placeholder
}

// YAML serialization helper (if YAML support is needed)
// Can be implemented similarly

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    ConfigSchema schema;  // Schema for validation
    std::unordered_map<std::string, std::type_index> typeMap;  // Type information for each key
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize type mapping for validation
    pImpl->typeMap["random_seed"] = typeid(int64_t);
    pImpl->typeMap["simulation_timestep"] = typeid(double);
    pImpl->typeMap["neuron_count"] = typeid(size_t);
    pImpl->typeMap["region_count"] = typeid(size_t);
    pImpl->typeMap["connection_probability"] = typeid(float);
    pImpl->typeMap["stdp_ltp_weight"] = typeid(float);
    pImpl->typeMap["stdp_ltd_weight"] = typeid(float);
    pImpl->typeMap["stdp_tau"] = typeid(float);
    pImpl->typeMap["synaptogenesis_rate"] = typeid(float);
    pImpl->typeMap["pruning_rate"] = typeid(float);
    pImpl->typeMap["replay_interval"] = typeid(size_t);
    pImpl->typeMap["consolidation_interval"] = typeid(size_t);
    pImpl->typeMap["checkpoint_dir"] = typeid(std::string);
    
    // Create default schema
    ConfigSchema::Field seedField("random_seed", typeid(int64_t), "42", true, "Random seed for reproducible results", nullptr);
    pImpl->schema.addField(seedField);
    
    ConfigSchema::Field timestepField("simulation_timestep", typeid(double), "0.001", true, "Simulation timestep in seconds", nullptr);
    pImpl->schema.addField(timestepField);
    
    ConfigSchema::Field neuronCountField("neuron_count", typeid(size_t), "1000", true, "Number of neurons in brain", nullptr);
    pImpl->schema.addField(neuronCountField);
    
    ConfigSchema::Field regionCountField("region_count", typeid(size_t), "1", true, "Number of brain regions", nullptr);
    pImpl->schema.addField(regionCountField);
    
    ConfigSchema::Field connProbField("connection_probability", typeid(float), "0.1", false, "Probability of synaptic connections", nullptr);
    pImpl->schema.addField(connProbField);
    
    ConfigSchema::Field stdpLtpField("stdp_ltp_weight", typeid(float), "0.01", false, "STDP long-term potentiation weight", nullptr);
    pImpl->schema.addField(stdpLtpField);
    
    ConfigSchema::Field stdpLtdField("stdp_ltd_weight", typeid(float), "0.012", false, "STDP long-term depression weight", nullptr);
    pImpl->schema.addField(stdpLtdField);
    
    ConfigSchema::Field stdpTauField("stdp_tau", typeid(float), "20.0", false, "STDP time constant", nullptr);
    pImpl->schema.addField(stdpTauField);

    // Type name to string mapping for serialization
    pImpl->typeMap["enable_checkpointing"] = typeid(bool);
    pImpl->typeMap["enable_replay"] = typeid(bool);
    pImpl->typeMap["enable_development"] = typeid(bool);
    
    // Add more fields as needed
}

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
    // Validate the key-value pair
    if (!ConfigValidator::validate(key, value)) {
        throw std::invalid_argument("Invalid key-value pair: " + key);
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

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm