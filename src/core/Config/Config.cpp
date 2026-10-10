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
    // Enhanced configuration file parser supporting multiple formats
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string extension = ".";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos && dotPos < filepath.size() - 1) {
        extension = filepath.substr(dotPos);
    }
    
    bool success = false;
    
    if (extension == ".json") {
        success = loadFromJSON(file);
    } else if (extension == ".yaml" || extension == ".yml") {
        success = loadFromYAML(file);
    } else {
        success = loadFromSimpleFormat(file);
    }
    
    // Post-process configuration: validate required fields and set defaults
    applyConfigurationDefaults();
    validateConfiguration();
    
    return success;
}

bool bool Config::loadFromJSON(std::ifstream& file) {
    // Parse JSON configuration
    // For now, use enhanced simple format parser with brace counting
    std::string line;
    size_t brace_depth = 0;
    std::string current_key;
    bool in_string = false;
    bool escaped = false;
    char string_char = 0;
    
    while (std::getline(file, line)) {
        // Enhanced JSON parsing with brace counting for nested objects
        for (size_t i = 0; i < line.size(); ++i) {
            char c = line[i];
            
            if (!in_string) {
                if (c == '{') {
                    brace_depth++;
                } else if (c == '}') {
                    brace_depth--;
                } else if (c == '"') {
                    in_string = true;
                    string_char = c;
                } else if (c == ':') {
                    // Parse key:value pairs
                    if (!current_key.empty()) {
                        std::string value = line.substr(i + 1);
                        trim(value);
                        set(current_key, value, ConfigSource::File);
                        current_key.clear();
                    }
                } else if (c == ',' && brace_depth == 0) {
                    // End of key-value pair
                    std::string value = line.substr(0, i);
                    trim(value);
                    set(current_key, value, ConfigSource::File);
                    current_key.clear();
                } else if (c == '"' && i > 0 && line[i-1] != '\\') {
                    // Extract key name (check if previous char wasn't escape)
                    if (current_key.empty()) {
                        size_t start = 0;
                        while (start < i && line[start] == ' ') start++;
                        current_key = line.substr(start, i - start);
                        trim(current_key);
                        // Remove surrounding quotes
                        if (current_key.size() >= 2 && current_key.front() == '"' && current_key.back() == '"') {
                            current_key = current_key.substr(1, current_key.size() - 2);
                        }
                    }
                }
            } else {
                if (escaped) {
                    escaped = false;
                } else if (c == '\\') {
                    escaped = true;
                } else if (c == string_char) {
                    in_string = false;
                    string_char = 0;
                }
            }
        }
    }
    
    return true;
}

bool Config::loadFromYAML(std::ifstream& file) {
    // Parse YAML configuration
    // Simple YAML parser supporting key-value pairs and lists
    std::string line;
    size_t indent_level = 0;
    std::string current_key;
    
    while (std::getline(file, line)) {
        trim(line);
        
        if (line.empty() || line[0] == '#') {
            continue;  // Skip comments and empty lines
        }
        
        // Calculate indentation
        size_t spaces = 0;
        while (spaces < line.size() && line[spaces] == ' ') {
            spaces++;
        }
        
        if (spaces == 0) {
            // Top-level key
            size_t colon_pos = line.find(':');
            if (colon_pos != std::string::npos) {
                current_key = line.substr(0, colon_pos);
                trim(current_key);
                
                std::string value = line.substr(colon_pos + 1);
                trim(value);
                
                // Handle quoted strings
                if ((value.front() == '\"' && value.back() == '\"') ||
                    (value.front() == '\'' && value.back() == '\'')) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(current_key, value, ConfigSource::File);
            }
        } else {
            // Indented key (nested property)
            if (!current_key.empty()) {
                std::string full_key = current_key + "." + line.substr(spaces);
                size_t colon_pos = full_key.find(':');
                if (colon_pos != std::string::npos) {
                    std::string key = full_key.substr(0, colon_pos);
                    std::string value = full_key.substr(colon_pos + 1);
                    trim(value);
                    
                    // Handle lists
                    if (value.front() == '[' && value.back() == ']') {
                        // Simple list parsing
                        std::string list_content = value.substr(1, value.size() - 2);
                        std::vector<std::string> items = parseList(list_content);
                        if (!items.empty()) {
                            // For now, use first item as config value
                            set(key, items[0], ConfigSource::File);
                        }
                    } else {
                        set(key, value, ConfigSource::File);
                    }
                }
            }
        }
    }
    
    return true;
}

bool Config::loadFromSimpleFormat(std::ifstream& file) {
    // Original simple key=value format with enhanced parsing
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs with enhanced syntax
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Handle special value types
            if (value == "true" || value == "false") {
                set(key, value == "true", ConfigSource::File);
            } else if (value.find('.') != std::string::npos) {
                // Try to parse as float
                try {
                    float float_val = std::stof(value);
                    set(key, float_val, ConfigSource::File);
                } catch (...) {
                    set(key, value, ConfigSource::File);
                }
            } else {
                set(key, value, ConfigSource::File);
            }
        }
    }
    
    return true;
}

std::vector<std::string> Config::parseList(const std::string& list_str) {
    // Parse comma-separated list from YAML/JSON
    std::vector<std::string> items;
    std::string current;
    bool in_quotes = false;
    char quote_char = 0;
    
    for (char c : list_str) {
        if (!in_quotes) {
            if (c == ',' || c == ' ') {
                if (!current.empty()) {
                    items.push_back(current);
                    current.clear();
                }
            } else {
                current += c;
            }
        } else {
            if (c == quote_char) {
                in_quotes = false;
                quote_char = 0;
            } else {
                current += c;
            }
        }
    }
    
    if (!current.empty()) {
        items.push_back(current);
    }
    
    return items;
}

void Config::applyConfigurationDefaults() {
    // Apply sensible defaults for missing configuration values
    // This ensures the brain can always initialize even with incomplete config
    
    // Neural parameters
    if (!has("neuron_count")) set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    if (!has("region_count")) set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    if (!has("connection_probability")) set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Simulation parameters
    if (!has("simulation_timestep")) set("simulation_timestep", 0.001, ConfigSource::Default);
    if (!has("random_seed")) set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    
    // Plasticity parameters
    if (!has("stdp_ltp_weight")) set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    if (!has("stdp_ltd_weight")) set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    if (!has("stdp_tau")) set("stdp_tau", 20.0f, ConfigSource::Default);
    if (!has("synaptogenesis_rate")) set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    if (!has("pruning_rate")) set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Memory parameters
    if (!has("working_memory_capacity")) set("working_memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    if (!has("max_episodes")) set("max_episodes", static_cast<int64_t>(1000), ConfigSource::Default);
    
    // Development parameters
    if (!has("development_enabled")) set("development_enabled", true, ConfigSource::Default);
    if (!has("critical_period_start")) set("critical_period_start", 60.0, ConfigSource::Default);
    if (!has("critical_period_end")) set("critical_period_end", 300.0, ConfigSource::Default);
    
    // Neuromodulation parameters
    if (!has("novelty_threshold")) set("novelty_threshold", 0.5f, ConfigSource::Default);
    if (!has("curiosity_gain")) set("curiosity_gain", 0.8f, ConfigSource::Default);
    if (!has("reward_decay_rate")) set("reward_decay_rate", 0.95f, ConfigSource::Default);
    
    // Prediction parameters
    if (!has("prediction_horizon")) set("prediction_horizon", 10, ConfigSource::Default);
    if (!has("prediction_confidence_threshold")) set("prediction_confidence_threshold", 0.7f, ConfigSource::Default);
    
    // Cognitive parameters
    if (!has("attention_inhibition_strength")) set("attention_inhibition_strength", 0.5f, ConfigSource::Default);
    if (!has("attention_excitation_strength")) set("attention_excitation_strength", 1.5f, ConfigSource::Default);
    if (!has("planning_depth")) set("planning_depth", 5, ConfigSource::Default);
    
    // Checkpoint parameters
    if (!has("checkpoint_dir")) set("checkpoint_dir", "./checkpoints", ConfigSource::Default);
    if (!has("checkpoint_interval")) set("checkpoint_interval", 1000, ConfigSource::Default);
    if (!has("max_checkpoints")) set("max_checkpoints", 10, ConfigSource::Default);
    
    // Replay parameters
    if (!has("replay_interval")) set("replay_interval", 100, ConfigSource::Default);
    if (!has("consolidation_interval")) set("consolidation_interval", 1000, ConfigSource::Default);
}

void Config::validateConfiguration() {
    // Validate configuration values for consistency and sanity
    // Log warnings for suspicious values but don't fail hard
    
    // Validate neural counts
    auto neuronCountOpt = get<int64_t>("neuron_count");
    if (neuronCountOpt && neuronCountOpt.value() < 10) {
        NLM_LOG_WARNING("Neuron count is very low (" + std::to_string(neuronCountOpt.value()) + "). "
                       "Consider increasing to at least 100 for meaningful simulation.");
    }
    
    // Validate probabilities
    auto connProbOpt = get<float>("connection_probability");
    if (connProbOpt && (connProbOpt.value() < 0.0f || connProbOpt.value() > 1.0f)) {
        NLM_LOG_WARNING("Connection probability should be between 0.0 and 1.0. "
                       "Clamping to valid range.");
        set("connection_probability", std::clamp(connProbOpt.value(), 0.0f, 1.0f), ConfigSource::Runtime);
    }
    
    // Validate simulation timestep
    auto timestepOpt = get<double>("simulation_timestep");
    if (timestepOpt && timestepOpt.value() <= 0.0) {
        NLM_LOG_ERROR("Simulation timestep must be positive. "
                     "Using default value 0.001.");
        set("simulation_timestep", 0.001, ConfigSource::Runtime);
    }
    
    // Validate plasticity parameters
    auto stpdOpt = get<float>("stdp_ltp_weight");
    if (stpdOpt && stpdOpt.value() < 0.0f) {
        NLM_LOG_ERROR("STDP LTP weight cannot be negative. "
                     "Using absolute value.");
        set("stdp_ltp_weight", std::abs(stpdOpt.value()), ConfigSource::Runtime);
    }
    
    auto stddOpt = get<float>("stdp_ltd_weight");
    if (stddOpt && stddOpt.value() < 0.0f) {
        NLM_LOG_ERROR("STDP LTD weight cannot be negative. "
                     "Using absolute value.");
        set("stdp_ltd_weight", std::abs(stddOpt.value()), ConfigSource::Runtime);
    }
    
    // Validate development parameters
    auto devEnabledOpt = get<bool>("development_enabled");
    if (devEnabledOpt && devEnabledOpt.value()) {
        auto cpStartOpt = get<double>("critical_period_start");
        auto cpEndOpt = get<double>("critical_period_end");
        if (cpStartOpt && cpEndOpt && cpEndOpt.value() <= cpStartOpt.value()) {
            NLM_LOG_ERROR("Critical period end must be after start. "
                         "Swapping values.");
            set("critical_period_end", cpStartOpt.value(), ConfigSource::Runtime);
            set("critical_period_start", cpEndOpt.value(), ConfigSource::Runtime);
        }
    }
    
    // Validate memory parameters
    auto wmCapOpt = get<int64_t>("working_memory_capacity");
    if (wmCapOpt && wmCapOpt.value() < 1) {
        NLM_LOG_ERROR("Working memory capacity must be at least 1. "
                     "Setting to 1.");
        set("working_memory_capacity", static_cast<int64_t>(1), ConfigSource::Runtime);
    }
    
    auto maxEpOpt = get<int64_t>("max_episodes");
    if (maxEpOpt && maxEpOpt.value() < 1) {
        NLM_LOG_ERROR("Maximum episodes must be at least 1. "
                     "Setting to 1.");
        set("max_episodes", static_cast<int64_t>(1), ConfigSource::Runtime);
    }
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
    
    // Write configuration in simple key=value format
    // For JSON/YAML format, add proper formatting based on file extension
    std::string extension = ".";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos && dotPos < filepath.size() - 1) {
        extension = filepath.substr(dotPos);
    }
    
    if (extension == ".json") {
        file << "{\n";
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            file << "  \"" << entry.key << "\": ";
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else {
                    file << arg;
                }
            }, entry.value);
            if (i < pImpl->entries.size() - 1) {
                file << ",\n";
            } else {
                file << "\n";
            }
        }
        file << "}\n";
    } else {
        // Default: simple key=value format (original behavior)
        for (const auto& entry : pImpl->entries) {
            if (!entry.description.empty()) {
                file << "# " << entry.description << "\n";
            }
            file << entry.key << " = ";
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else {
                    file << arg;
                }
            }, entry.value);
            file << "\n\n";
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
