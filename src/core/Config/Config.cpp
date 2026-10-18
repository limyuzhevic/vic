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

// Configuration category definitions
struct ConfigCategory {
    std::string name;
    std::string description;
    std::vector<std::string> keys;
};

// Configuration metadata for help system
struct ConfigMetadata {
    std::string key;
    std::string description;
    std::string defaultValue;
    std::string type;
    std::string category;
    std::string validation;
};

// Static metadata (would be populated from documentation)
const std::vector<ConfigMetadata>& getConfigMetadata() {
    static const std::vector<ConfigMetadata> metadata = {
        // Core configuration
        {"random_seed", "Random seed for reproducible experiments", "42", "int", "core", "nonnegative"},
        {"simulation_timestep", "Simulation timestep in seconds", "0.001", "float", "core", "positive"},
        
        // Brain configuration
        {"neuron_count", "Number of neurons in the brain", "1000", "int", "brain", "positive"},
        {"region_count", "Number of brain regions", "2", "int", "brain", "positive"},
        {"connection_probability", "Connection probability for random connectivity", "0.1", "float", "brain", "range[0,1]"},
        
        // Plasticity configuration
        {"plasticity_learning_rate", "Plasticity learning rate", "0.01", "float", "plasticity", "positive"},
        {"stdp_ltp_weight", "STDP LTP weight", "0.01", "float", "plasticity", "positive"},
        {"stdp_ltd_weight", "STDP LTD weight", "0.012", "float", "plasticity", "positive"},
        {"stdp_time_constant", "STDP time constant", "20.0", "float", "plasticity", "positive"},
        
        // Development configuration
        {"development_synaptogenesis_rate", "Development synaptogenesis rate", "0.001", "float", "development", "range[0,1]"},
        {"development_pruning_rate", "Development pruning rate", "0.0001", "float", "development", "range[0,1]"},
        
        // Neuromodulation configuration
        {"dopamine_baseline", "Dopamine baseline level", "0.1", "float", "neuromodulation", "nonnegative"},
        
        // Reward configuration
        {"reward_discount_factor", "Reward discount factor", "0.99", "float", "reward", "range[0,1]"},
        
        // Environment configuration
        {"environment_name", "Environment name", "GridWorld", "string", "environment", "nonempty"},
        {"environment_width", "Environment width", "10", "int", "environment", "positive"},
        {"environment_height", "Environment height", "10", "int", "environment", "positive"},
        
        // Logging configuration
        {"log_level", "Log level", "INFO", "string", "logging", "enum[ERROR,WARN,INFO,DEBUG]"},
        {"log_to_file", "Log to file", "false", "boolean", "logging", "boolean"},
        {"log_filename", "Log filename", "nlm.log", "string", "logging", "nonempty"},
        
        // Simulation configuration
        {"max_simulation_steps", "Maximum simulation steps", "10000", "int", "simulation", "positive"},
        {"simulation_time_limit", "Simulation time limit", "0.0", "float", "simulation", "nonnegative"},
        
        // Visualization configuration
        {"visualization_enabled", "Enable visualization", "false", "boolean", "visualization", "boolean"},
        {"visualization_update_rate", "Visualization update rate", "30.0", "float", "visualization", "positive"},
    };
    return metadata;
}

} // namespace nlm

std::string Config::getHelp(const std::string& category) {
    const auto& metadata = getConfigMetadata();
    
    std::ostringstream oss;
    oss << "NLM Configuration Options" << std::endl;
    oss << std::string(60, '=') << std::endl;
    
    if (category.empty()) {
        // Show all categories
        oss << "Available Categories:" << std::endl;
        std::set<std::string> categories;
        for (const auto& meta : metadata) {
            categories.insert(meta.category);
        }
        for (const auto& cat : categories) {
            oss << "  " << cat << std::endl;
        }
        oss << std::endl;
        oss << "Use --help=<category> for details on a specific category." << std::endl;
    } else {
        // Show specific category
        oss << "Configuration for category: " << category << std::endl;
        oss << std::string(60, '-') << std::endl;
        
        for (const auto& meta : metadata) {
            if (meta.category == category) {
                oss << meta.key << std::endl;
                oss << "  Type: " << meta.type << std::endl;
                oss << "  Default: " << meta.defaultValue << std::endl;
                oss << "  Description: " << meta.description << std::endl;
                if (!meta.validation.empty()) {
                    oss << "  Validation: " << meta.validation << std::endl;
                }
                oss << std::endl;
            }
        }
    }
    
    return oss.str();
}

std::vector<std::string> Config::getCategories() {
    std::set<std::string> categories;
    const auto& metadata = getConfigMetadata();
    for (const auto& meta : metadata) {
        categories.insert(meta.category);
    }
    return std::vector<std::string>(categories.begin(), categories.end());
}

std::vector<ConfigEntry> Config::getByCategory(const std::string& category) const {
    std::vector<ConfigEntry> result;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key == category) {  // TODO: This should check if entry.key belongs to category
            result.push_back(entry);
        }
    }
    
    return result;
}

std::string Config::generateTemplate(const std::vector<std::string>& categories) const {
    std::ostringstream oss;
    oss << "# NLM Configuration Template" << std::endl;
    oss << "# Generated on " << __DATE__ << " " << __TIME__ << std::endl;
    oss << std::endl;
    
    // Get metadata
    const auto& metadata = getConfigMetadata();
    
    // Filter by categories
    std::vector<ConfigMetadata> filtered;
    if (categories.empty()) {
        filtered = metadata;
    } else {
        for (const auto& cat : categories) {
            for (const auto& meta : metadata) {
                if (meta.category == cat) {
                    filtered.push_back(meta);
                }
            }
        }
    }
    
    // Group by category
    std::map<std::string, std::vector<ConfigMetadata>> byCategory;
    for (const auto& meta : filtered) {
        byCategory[meta.category].push_back(meta);
    }
    
    // Generate template
    for (const auto& categoryPair : byCategory) {
        const std::string& category = categoryPair.first;
        oss << "# " << category << " configuration" << std::endl;
        oss << std::string(60, '-') << std::endl;
        
        for (const auto& meta : categoryPair.second) {
            oss << meta.key << " = " << meta.defaultValue << "  # " << meta.description << std::endl;
        }
        
        oss << std::endl;
    }
    
    return oss.str();
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
