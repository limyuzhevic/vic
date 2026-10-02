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

// Get configuration summary
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\\\"" << arg << "\\\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

// Configuration validation
bool Config::validate() const {
    // Define validation rules for key configuration values
    std::vector<std::string> errors = getValidationErrors();
    return errors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Validate simulation parameters
    if (has("neuron_count")) {
        auto val = get<int64_t>("neuron_count");
        if (val.has_value()) {
            if (val.value() < 1) {
                errors.push_back("neuron_count must be at least 1, got " + std::to_string(val.value()));
            } else if (val.value() > 1000000) {
                errors.push_back("neuron_count too large (max 1,000,000), got " + std::to_string(val.value()));
            }
        }
    }
    
    if (has("region_count")) {
        auto val = get<int64_t>("region_count");
        if (val.has_value()) {
            if (val.value() < 1) {
                errors.push_back("region_count must be at least 1, got " + std::to_string(val.value()));
            } else if (val.value() > 1000) {
                errors.push_back("region_count too large (max 1000), got " + std::to_string(val.value()));
            }
        }
    }
    
    if (has("connection_probability")) {
        auto val = get<float>("connection_probability");
        if (val.has_value()) {
            if (val.value() < 0.0f) {
                errors.push_back("connection_probability must be >= 0.0, got " + std::to_string(val.value()));
            } else if (val.value() > 1.0f) {
                errors.push_back("connection_probability must be <= 1.0, got " + std::to_string(val.value()));
            }
        }
    }
    
    // Validate plasticity parameters
    if (has("stdp_ltp_weight")) {
        auto val = get<float>("stdp_ltp_weight");
        if (val.has_value()) {
            if (std::abs(val.value()) > 1.0f) {
                errors.push_back("stdp_ltp_weight must be in [-1.0, 1.0], got " + std::to_string(val.value()));
            }
        }
    }
    
    if (has("stdp_ltd_weight")) {
        auto val = get<float>("stdp_ltd_weight");
        if (val.has_value()) {
            if (std::abs(val.value()) > 1.0f) {
                errors.push_back("stdp_ltd_weight must be in [-1.0, 1.0], got " + std::to_string(val.value()));
            }
        }
    }
    
    // Validate simulation parameters
    if (has("simulation_timestep")) {
        auto val = get<double>("simulation_timestep");
        if (val.has_value()) {
            if (val.value() <= 0.0) {
                errors.push_back("simulation_timestep must be positive, got " + std::to_string(val.value()));
            } else if (val.value() > 0.1) {
                errors.push_back("simulation_timestep too large (max 0.1s), got " + std::to_string(val.value()));
            }
        }
    }
    
    if (has("random_seed")) {
        auto val = get<int64_t>("random_seed");
        if (val.has_value()) {
            if (val.value() < 0) {
                errors.push_back("random_seed must be non-negative, got " + std::to_string(val.value()));
            }
        }
    }
    
// Set with validation (returns false if validation fails)
template<typename T>
bool Config::setValidated(const std::string& key, const T& value,
                         ConfigSource source,
                         const std::function<bool(const T&)>& validator) {
    // Check if validation function is provided
    if (validator && !validator(value)) {
        return false;
    }
    
    // Check for known validation errors
    set(key, ConfigValue(value), source);
    
    // Quick validation for common parameters
    if (key == "neuron_count") {
        auto val = get<int64_t>(key);
        if (val.has_value() && (val.value() < 1 || val.value() > 1000000)) {
            return false;
        }
    } else if (key == "connection_probability") {
        auto val = get<float>(key);
        if (val.has_value() && (val.value() < 0.0f || val.value() > 1.0f)) {
            return false;
        }
    }
    
    return validate();
}

// Helper function to create validator for integer ranges
template<typename T>
std::function<bool(const T&)> createRangeValidator(T min, T max) {
    return [min, max](const T& value) {
        return value >= min && value <= max;
    };
}

// Helper function to create validator for positive values
template<typename T>
std::function<bool(const T&)> createPositiveValidator() {
    return [](const T& value) {
        return value > 0;
    };
}

// Template instantiations for validator helpers
template std::function<bool(const int&)> createRangeValidator<int>(int, int);
template std::function<bool(const int64_t&)> createRangeValidator<int64_t>(int64_t, int64_t);
template std::function<bool(const double&)> createRangeValidator<double>(double, double);
template std::function<bool(const float&)> createRangeValidator<float>(float, float);

template std::function<bool(const int&)> createPositiveValidator<int>();
template std::function<bool(const int64_t&)> createPositiveValidator<int64_t>();
template std::function<bool(const double&)> createPositiveValidator<double>();
template std::function<bool(const float&)> createPositiveValidator<float>();

} // namespace nlm
