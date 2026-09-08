#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool validationEnabled = true;
    std::vector<std::string> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->validationEnabled = true;
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

// Validation implementations
void Config::enableValidation(bool enable) {
    pImpl->validationEnabled = enable;
    pImpl->validationErrors.clear();
}

bool Config::isValidationEnabled() const {
    return pImpl->validationEnabled;
}

bool Config::validate() const {
    pImpl->validationErrors.clear();
    bool valid = true;
    
    if (!checkDuplicateKeys()) valid = false;
    if (!validateParameterRanges()) valid = false;
    if (!checkParameterConsistency()) valid = false;
    
    return valid;
}

std::string Config::validateAndGetErrors() const {
    pImpl->validationErrors.clear();
    validate();
    return pImpl->validationErrors.empty() ? "All configurations are valid." : "Validation failed:\n" + joinErrors();
}

bool Config::checkDuplicateKeys() const {
    bool valid = true;
    std::vector<std::string> seenKeys;
    
    for (const auto& entry : pImpl->entries) {
        bool isDuplicate = false;
        for (const auto& key : seenKeys) {
            if (toLower(key) == toLower(entry.key)) {
                isDuplicate = true;
                break;
            }
        }
        
        if (isDuplicate) {
            std::string error = "Duplicate key: '" + entry.key + "' (first appeared at source " + 
                               std::to_string(static_cast<int>(entry.source)) + ")";
            pImpl->validationErrors.push_back(error);
            valid = false;
        } else {
            seenKeys.push_back(entry.key);
        }
    }
    
    return valid;
}

bool Config::validateParameterRanges() const {
    bool valid = true;
    
    for (const auto& entry : pImpl->entries) {
        std::string error;
        if (!validateParameter(entry.key, entry.value, error)) {
            pImpl->validationErrors.push_back(error);
            valid = false;
        }
    }
    
    return valid;
}

bool Config::checkParameterConsistency() const {
    bool valid = true;
    
    // Check plasticity parameters for consistency
    auto plasticityEnabled = get<bool>("plasticity_enabled");
    auto learningRate = get<double>("learning_rate");
    auto decayRate = get<double>("decay_rate");
    
    if (plasticityEnabled && plasticityEnabled.value()) {
        if (learningRate.has_value()) {
            double rate = learningRate.value();
            if (rate < 0.0 || rate > 1.0) {
                pImpl->validationErrors.push_back("learning_rate must be between 0.0 and 1.0 when plasticity_enabled is true");
                valid = false;
            }
        }
        
        if (decayRate.has_value()) {
            double rate = decayRate.value();
            if (rate < 0.0 || rate > 1.0) {
                pImpl->validationErrors.push_back("decay_rate must be between 0.0 and 1.0 when plasticity_enabled is true");
                valid = false;
            }
        }
        
        // Check that learning rate is greater than decay rate (if both present)
        if (learningRate.has_value() && decayRate.has_value()) {
            if (learningRate.value() <= decayRate.value()) {
                pImpl->validationErrors.push_back("learning_rate must be greater than decay_rate");
                valid = false;
            }
        }
    }
    
    // Check network size parameters
    auto neuronCount = get<int>("neuron_count");
    auto inputSize = get<int>("input_size");
    
    if (neuronCount.has_value() && inputSize.has_value()) {
        if (neuronCount.value() <= 0) {
            pImpl->validationErrors.push_back("neuron_count must be positive");
            valid = false;
        }
        if (inputSize.value() <= 0) {
            pImpl->validationErrors.push_back("input_size must be positive");
            valid = false;
        }
        if (neuronCount.value() <= inputSize.value()) {
            pImpl->validationErrors.push_back("neuron_count must be greater than input_size");
            valid = false;
        }
    }
    
    // Check connection probability
    auto connProb = get<double>("connection_probability");
    if (connProb.has_value()) {
        if (connProb.value() < 0.0 || connProb.value() > 1.0) {
            pImpl->validationErrors.push_back("connection_probability must be between 0.0 and 1.0");
            valid = false;
        }
    }
    
    return valid;
}

bool Config::validateParameter(const std::string& key, const ConfigValue& value, std::string& errorMsg) const {
    double min, max;
    if (!getParameterBounds(key, min, max)) {
        return true; // No bounds defined for this parameter
    }
    
    try {
        if (std::holds_alternative<int>(value)) {
            double val = static_cast<double>(std::get<int>(value));
            if (val < min || val > max) {
                errorMsg = key + " must be between " + std::to_string(min) + " and " + std::to_string(max);
                return false;
            }
        } else if (std::holds_alternative<int64_t>(value)) {
            double val = static_cast<double>(std::get<int64_t>(value));
            if (val < min || val > max) {
                errorMsg = key + " must be between " + std::to_string(min) + " and " + std::to_string(max);
                return false;
            }
        } else if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (val < min || val > max) {
                errorMsg = key + " must be between " + std::to_string(min) + " and " + std::to_string(max);
                return false;
            }
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            for (const auto& item : std::get<std::vector<int>>(value)) {
                if (static_cast<double>(item) < min || static_cast<double>(item) > max) {
                    errorMsg = key + " elements must be between " + std::to_string(min) + " and " + std::to_string(max);
                    return false;
                }
            }
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            for (const auto& item : std::get<std::vector<double>>(value)) {
                if (item < min || item > max) {
                    errorMsg = key + " elements must be between " + std::to_string(min) + " and " + std::to_string(max);
                    return false;
                }
            }
        }
    } catch (const std::bad_variant_access&) {
        errorMsg = "Invalid type for parameter: " + key;
        return false;
    }
    
    return true;
}

bool Config::getParameterBounds(const std::string& key, double& min, double& max) const {
    // Define bounds for critical parameters
    std::string lowerKey = toLower(key);
    
    // Neuron-related parameters
    if (lowerKey == "neuron_count" || lowerKey == "num_neurons" || lowerKey == "hidden_size") {
        min = 1.0;
        max = 10000.0;
        return true;
    }
    
    // Input/output sizes
    if (lowerKey == "input_size" || lowerKey == "output_size") {
        min = 1.0;
        max = 1000.0;
        return true;
    }
    
    // Learning rate
    if (lowerKey == "learning_rate" || lowerKey == "lr") {
        min = 0.0;
        max = 1.0;
        return true;
    }
    
    // Decay rate
    if (lowerKey == "decay_rate" || lowerKey == "decay" || lowerKey == "gamma") {
        min = 0.0;
        max = 1.0;
        return true;
    }
    
    // Connection probability
    if (lowerKey == "connection_probability" || lowerKey == "conn_prob" || lowerKey == "sparsity") {
        min = 0.0;
        max = 1.0;
        return true;
    }
    
    // Threshold/firing rate
    if (lowerKey == "threshold" || lowerKey == "threshold_voltage" || lowerKey == "vth") {
        min = -100.0;
        max = 100.0;
        return true;
    }
    
    // Weight range
    if (lowerKey == "weight_min" || lowerKey == "weight_max" || lowerKey == "weight_range") {
        min = -10.0;
        max = 10.0;
        return true;
    }
    
    // Time constant
    if (lowerKey == "time_constant" || lowerKey == "tau" || lowerKey == "dt") {
        min = 0.001;
        max = 1000.0;
        return true;
    }
    
    // Batch size
    if (lowerKey == "batch_size") {
        min = 1.0;
        max = 10000.0;
        return true;
    }
    
    // Temperature
    if (lowerKey == "temperature") {
        min = 0.0;
        max = 1000.0;
        return true;
    }
    
    // Number of epochs
    if (lowerKey == "epochs" || lowerKey == "num_epochs") {
        min = 1.0;
        max = 100000.0;
        return true;
    }
    
    // Memory/limit parameters
    if (lowerKey == "memory_limit_mb" || lowerKey == "memory_limit") {
        min = 1.0;
        max = 1000000.0;
        return true;
    }
    
    // Voltage range
    if (lowerKey == "v_min" || lowerKey == "v_max" || lowerKey == "voltage_min" || lowerKey == "voltage_max") {
        min = -500.0;
        max = 500.0;
        return true;
    }
    
    // Default bounds for numeric parameters
    if (isNumericType(lowerKey)) {
        min = -1e9;
        max = 1e9;
        return true;
    }
    
    return false;
}

void Config::addError(const std::string& error) {
    pImpl->validationErrors.push_back(error);
}

std::string Config::joinErrors() const {
    std::ostringstream oss;
    for (const auto& error : pImpl->validationErrors) {
        oss << "  - " << error << "\n";
    }
    return oss.str();
}

bool Config::isNumericType(const std::string& key) const {
    std::string lowerKey = toLower(key);
    return lowerKey.find("rate") != std::string::npos || 
           lowerKey.find("size") != std::string::npos ||
           lowerKey.find("count") != std::string::npos ||
           lowerKey.find("number") != std::string::npos ||
           lowerKey.find("amount") != std::string::npos ||
           lowerKey.find("limit") != std::string::npos ||
           lowerKey.find("threshold") != std::string::npos ||
           lowerKey.find("value") != std::string::npos;
}
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
