#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <cmath>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ParameterConstraints> parameterConstraints;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Register default parameters with constraints
    registerParameter("neuron_count", {"Number of neurons in the network", "neural", 1, 10000, "neurons", true, "500"});
    registerParameter("region_count", {"Number of neural regions", "neural", 1, 100, "regions", true, "1"});
    registerParameter("connection_probability", {"Probability of connection between neurons", "neural", 0.0, 1.0, "probability", true, "0.15"});
    registerParameter("simulation_timestep", {"Time step for simulation", "simulation", 0.0001, 0.1, "seconds", true, "0.001"});
    registerParameter("random_seed", {"Random seed for reproducibility", "neural", 0, 2147483647, "seed", true, "42"});
    
    registerParameter("stdp_ltp_weight", {"STDP Long-Term Potentiation weight", "plasticity", 0.0, 1.0, "weight", false, "0.02"});
    registerParameter("stdp_ltd_weight", {"STDP Long-Term Depression weight", "plasticity", 0.0, 1.0, "weight", false, "0.015"});
    registerParameter("stdp_tau", {"STDP time constant", "plasticity", 1.0, 100.0, "ms", false, "20.0"});
    
    registerParameter("synaptogenesis_rate", {"Rate of new synapse formation", "structural", 0.0, 1.0, "per_step", false, "0.0001"});
    registerParameter("pruning_rate", {"Rate of synapse removal", "structural", 0.0, 1.0, "per_step", false, "0.00001"});
    
    registerParameter("verbose_logging", {"Enable verbose logging", "logging", std::nullopt, std::nullopt, "", false, "false"});
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
        
        // Get constraints for proper formatting
        if (pImpl->parameterConstraints.find(entry.key) != pImpl->parameterConstraints.end()) {
            const auto& constraints = pImpl->parameterConstraints.at(entry.key);
            file << entry.key << " = " << constraints.default_example << "  # " << constraints.category << " parameter\n";
        } else {
            file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
        }
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
    std::string error;
    if (!validateParameter(key, value, error)) {
        NLM_LOG_WARNING("Parameter validation failed for key '" + key + "': " + error);
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
    pImpl->parameterConstraints.erase(key);
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
    pImpl->parameterConstraints.clear();
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

bool Config::validateParameter(const std::string& key, const ConfigValue& value, std::string& error) const {
    // Check if parameter exists
    if (!hasParameterConstraints(key)) {
        error = "Unknown parameter: " + key;
        return false;
    }
    
    const auto& constraints = pImpl->parameterConstraints.at(key);
    
    // Type checking based on expected type from constraints
    bool typeValid = false;
    
    if (std::holds_alternative<int>(value)) {
        double val = std::get<int>(value);
        typeValid = validateValue(val, constraints);
    } else if (std::holds_alternative<int64_t>(value)) {
        double val = std::get<int64_t>(value);
        typeValid = validateValue(val, constraints);
    } else if (std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        typeValid = validateValue(val, constraints);
    } else if (std::holds_alternative<bool>(value)) {
        typeValid = true;  // Booleans are always valid
    } else if (std::holds_alternative<std::string>(value)) {
        typeValid = true;  // Strings are always valid
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        typeValid = true;  // Vectors are always valid
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        typeValid = true;  // Vectors are always valid
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        typeValid = true;  // Vectors are always valid
    }
    
    if (!typeValid) {
        error = "Value type mismatch for parameter: " + key;
        return false;
    }
    
    // Range checking (if applicable)
    if (std::holds_alternative<int>(value)) {
        double val = std::get<int>(value);
        if (constraints.min_value.has_value() && val < constraints.min_value.value()) {
            error = "Value " + std::to_string(val) + " is less than minimum " + std::to_string(constraints.min_value.value()) + " for parameter: " + key;
            return false;
        }
        if (constraints.max_value.has_value() && val > constraints.max_value.value()) {
            error = "Value " + std::to_string(val) + " exceeds maximum " + std::to_string(constraints.max_value.value()) + " for parameter: " + key;
            return false;
        }
    } else if (std::holds_alternative<int64_t>(value)) {
        double val = std::get<int64_t>(value);
        if (constraints.min_value.has_value() && val < constraints.min_value.value()) {
            error = "Value " + std::to_string(val) + " is less than minimum " + std::to_string(constraints.min_value.value()) + " for parameter: " + key;
            return false;
        }
        if (constraints.max_value.has_value() && val > constraints.max_value.value()) {
            error = "Value " + std::to_string(val) + " exceeds maximum " + std::to_string(constraints.max_value.value()) + " for parameter: " + key;
            return false;
        }
    } else if (std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        if (constraints.min_value.has_value() && val < constraints.min_value.value()) {
            error = "Value " + std::to_string(val) + " is less than minimum " + std::to_string(constraints.min_value.value()) + " for parameter: " + key;
            return false;
        }
        if (constraints.max_value.has_value() && val > constraints.max_value.value()) {
            error = "Value " + std::to_string(val) + " exceeds maximum " + std::to_string(constraints.max_value.value()) + " for parameter: " + key;
            return false;
        }
    }
    
    return true;
}

bool Config::validateAllParameters(std::vector<std::string>& errors) const {
    errors.clear();
    
    for (const auto& entry : pImpl->entries) {
        std::string error;
        if (!validateParameter(entry.key, entry.value, error)) {
            errors.push_back("Parameter '" + entry.key + "': " + error);
        }
    }
    
    return errors.empty();
}

bool Config::hasParameterConstraints(const std::string& key) const {
    return pImpl->parameterConstraints.find(key) != pImpl->parameterConstraints.end();
}

ParameterConstraints Config::getParameterConstraints(const std::string& key) const {
    auto it = pImpl->parameterConstraints.find(key);
    if (it != pImpl->parameterConstraints.end()) {
        return it->second;
    }
    return ParameterConstraints();
}

std::string Config::getHelpForParameter(const std::string& key) const {
    if (!hasParameterConstraints(key)) {
        return "Unknown parameter: " + key;
    }
    
    const auto& constraints = pImpl->parameterConstraints.at(key);
    std::ostringstream oss;
    oss << "Parameter: " << key << "\n";
    oss << "  Description: " << constraints.description << "\n";
    oss << "  Category: " << constraints.category << "\n";
    
    if (constraints.min_value.has_value() && constraints.max_value.has_value()) {
        oss << "  Range: " << constraints.min_value.value() << " to " << constraints.max_value.value() << "\n";
    } else if (constraints.min_value.has_value()) {
        oss << "  Minimum: " << constraints.min_value.value() << "\n";
    } else if (constraints.max_value.has_value()) {
        oss << "  Maximum: " << constraints.max_value.value() << "\n";
    }
    
    if (constraints.unit.has_value()) {
        oss << "  Unit: " << constraints.unit.value() << "\n";
    }
    
    oss << "  Required: " << (constraints.is_required ? "yes" : "no") << "\n";
    oss << "  Default example: " << constraints.default_example << "\n";
    
    return oss.str();
}

std::string Config::getHelpForCategory(const std::string& category) const {
    std::ostringstream oss;
    oss << "Category: " << category << "\n";
    
    bool found = false;
    for (const auto& pair : pImpl->parameterConstraints) {
        if (pair.second.category == category) {
            oss << "  - " << pair.first;
            if (pair.second.is_required) {
                oss << " (required)";
            }
            oss << "\n";
            found = true;
        }
    }
    
    if (!found) {
        oss << "  (no parameters in this category)\n";
    }
    
    return oss.str();
}

std::vector<std::string> Config::getAllCategories() const {
    std::vector<std::string> categories;
    std::unordered_map<std::string, bool> uniqueCategories;
    
    for (const auto& pair : pImpl->parameterConstraints) {
        if (uniqueCategories.find(pair.second.category) == uniqueCategories.end()) {
            categories.push_back(pair.second.category);
            uniqueCategories[pair.second.category] = true;
        }
    }
    
    return categories;
}

std::string Config::getFullHelp() const {
    std::ostringstream oss;
    oss << "=== NLM Configuration Help ===\n\n";
    
    oss << "Available categories:\n";
    for (const auto& category : getAllCategories()) {
        oss << "  " << category << "\n";
    }
    
    oss << "\n";
    oss << "To get help for a specific parameter, use: getHelpForParameter(\"parameter_name\")\n";
    oss << "To get help for a category: getHelpForCategory(\"category_name\")\n";
    oss << "To validate all parameters: validateAllParameters(errors)\n";
    
    oss << "\n";
    oss << "Common usage examples:\n";
    oss << "  --neuron_count=1000     Set neuron count to 1000\n";
    oss << "  --connection_probability=0.5  Set connection probability\n";
    oss << "  --stdp_ltp_weight=0.05  Set STDP LTP weight\n";
    oss << "  --verbose               Enable verbose logging\n";
    
    return oss.str();
}

void Config::registerParameter(const std::string& key, const ParameterConstraints& constraints) {
    pImpl->parameterConstraints[key] = constraints;
}

void Config::unregisterParameter(const std::string& key) {
    pImpl->parameterConstraints.erase(key);
}

bool Config::validateValue(double value, const ParameterConstraints& constraints) const {
    if (constraints.min_value.has_value() && value < constraints.min_value.value()) {
        return false;
    }
    if (constraints.max_value.has_value() && value > constraints.max_value.value()) {
        return false;
    }
    return true;
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
