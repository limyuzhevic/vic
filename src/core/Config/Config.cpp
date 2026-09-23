#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    
    // Detect file format and parse accordingly
    if (detectFileFormat(filepath) == FileFormat::JSON) {
        return loadFromJSON(content);
    } else if (detectFileFormat(filepath) == FileFormat::YAML) {
        return loadFromYAML(content);
    } else {
        // Fallback to simple key=value format for backwards compatibility
        return loadFromKeyValue(content);
    }
}

Config::FileFormat Config::detectFileFormat(const std::string& filepath) const {
    // Check file extension
    if (filepath.size() >= 5) {
        std::string ext = filepath.substr(filepath.size() - 4);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext == ".json") return FileFormat::JSON;
        if (ext == ".yaml" || ext == ".yml") return FileFormat::YAML;
    }
    
    // Heuristic detection by content
    std::ifstream file(filepath);
    if (!file.is_open()) return FileFormat::KeyValue;
    
    std::string line;
    std::getline(file, line);
    file.clear();
    file.seekg(0);
    
    // Simple heuristic: if first non-empty line starts with '{', it's JSON
    size_t pos = 0;
    while (pos < line.size() && (std::isspace(line[pos]) || line[pos] == '#')) pos++;
    if (pos < line.size() && line[pos] == '{') {
        return FileFormat::JSON;
    }
    
    // If file contains ': ' followed by values, it might be YAML or key=value
    bool hasColonSpace = false;
    bool hasEquals = false;
    while (std::getline(file, line)) {
        if (line.find(": ") != std::string::npos) hasColonSpace = true;
        if (line.find("=") != std::string::npos) hasEquals = true;
        if (hasColonSpace && hasEquals) break;
    }
    
    if (hasColonSpace && !hasEquals) return FileFormat::YAML;
    return FileFormat::KeyValue;
}

bool Config::loadFromKeyValue(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
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

bool Config::loadFromJSON(const std::string& content) {
    // TODO: Implement JSON parsing using nlohmann::json or similar
    // For now, provide a placeholder implementation
    return false;
}

bool Config::loadFromYAML(const std::string& content) {
    // TODO: Implement YAML parsing using yaml-cpp or similar
    // For now, provide a placeholder implementation
    return false;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle help flag
        if (arg == "--help" || arg == "-h") {
            showHelp();
            return true; // Return true to indicate help was shown
        }
        
        // Handle version flag
        if (arg == "--version" || arg == "-v") {
            showVersion();
            return true; // Return true to indicate version was shown
        }
        
        // Handle verbose flag
        if (arg == "--verbose" || arg == "-V") {
            verboseMode = true;
            continue;
        }
        
        // Handle log file flag
        if (arg == "--log-file" && i + 1 < argc) {
            logFile = argv[++i];
            set("log_filename", logFile, ConfigSource::CommandLine);
            continue;
        }
        
        // Handle configuration file format flag
        if (arg == "--config-format" && i + 1 < argc) {
            configFormat = argv[++i];
            continue;
        }
        
        // Handle schema flag
        if (arg == "--schema" && i + 1 < argc) {
            std::string schemaStr = argv[++i];
            try {
                currentSchema = stringToSchema(schemaStr);
            } catch (const std::invalid_argument&) {
                std::cerr << "ERROR: Invalid schema specified: " << schemaStr << std::endl;
                return false;
            }
            continue;
        }
        
        // Handle batch mode flag
        if (arg == "--batch" || arg == "-b") {
            batchMode = true;
            continue;
        }
        
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
    
    // Determine output format based on file extension
    std::string ext = getFileExtension(filepath);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return saveToJSON(file);
    } else if (ext == ".yaml" || ext == ".yml") {
        return saveToYAML(file);
    } else {
        // Default to key=value format for backwards compatibility
        return saveToKeyValue(file);
    }
}

std::string Config::getFileExtension(const std::string& filepath) const {
    size_t pos = filepath.find_last_of('.');
    if (pos == std::string::npos) {
        return "";
    }
    return filepath.substr(pos);
}

bool Config::saveToKeyValue(std::ostream& output) const {
    for (const auto& entry : pImpl->entries) {
        output << "# " << entry.description << "\n";
        output << entry.key << " = ";
        
        std::visit([&output](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                output << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                output << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    output << arg[i];
                    if (i < arg.size() - 1) output << ", ";
                }
                output << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                output << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    output << arg[i];
                    if (i < arg.size() - 1) output << ", ";
                }
                output << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                output << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    output << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) output << ", ";
                }
                output << "]";
            } else {
                output << arg;
            }
        }, entry.value);
        
        output << "\n\n";
    }
    return true;
}

bool Config::saveToJSON(std::ostream& output) const {
    output << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        output << "  \"" << entry.key << "\": ";
        
        output << visit([&output](auto&& arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;
            std::ostringstream oss;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    oss << arg[j];
                    if (j < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    oss << arg[j];
                    if (j < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    oss << "\"" << arg[j] << "\"";
                    if (j < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else {
                oss << arg;
            }
            return oss.str();
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) output << ",";
        output << "\n";
    }
    output << "}\n";
    return true;
}

bool Config::saveToYAML(std::ostream& output) const {
    output << "# Configuration saved in YAML format\n";
    output << "config:\n";
    
    for (const auto& entry : pImpl->entries) {
        output << "  " << entry.key << ": ";
        
        output << visit([&output](auto&& arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;
            std::ostringstream oss;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else {
                oss << arg;
            }
            return oss.str();
        }, entry.value);
        
        output << "\n";
    }
    
    return true;
}

void Config::showHelp() const {
    std::cout << "NLM Configuration Options:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --version, -v           Show version information" << std::endl;
    std::cout << "  --verbose, -V           Enable verbose logging" << std::endl;
    std::cout << "  --log-file <file>       Set log output file" << std::endl;
    std::cout << "  --config-format <format> Set configuration file format (json/yaml)" << std::endl;
    std::cout << "  --schema <name>         Set configuration schema (phase1,phase2,phase3,phase4,phase5,phase6)" << std::endl;
    std::cout << "  --batch, -b             Run in batch mode (no interactive output)" << std::endl;
    std::cout << "  --config <file>         Load configuration from file" << std::endl;
    std::cout << "  --key=value             Set configuration key to value" << std::endl;
    std::cout << "  -key value              Set configuration key to value (alternative format)" << std::endl;
}

void Config::showVersion() const {
    std::cout << "NLM Configuration System v1.0.0 (Phase 6 Integration)" << std::endl;
}

Config::FileFormat Config::detectFileFormat(const std::string& filepath) const {
    std::string ext = getFileExtension(filepath);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext == ".json") return FileFormat::JSON;
    if (ext == ".yaml" || ext == ".yml") return FileFormat::YAML;
    return FileFormat::KeyValue;
}

Config::Schema Config::stringToSchema(const std::string& schemaStr) const {
    std::string lower = schemaStr;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "phase1") return Schema::Phase1;
    if (lower == "phase2") return Schema::Phase2;
    if (lower == "phase3") return Schema::Phase3;
    if (lower == "phase4") return Schema::Phase4;
    if (lower == "phase5") return Schema::Phase5;
    if (lower == "phase6") return Schema::Phase6;
    
    throw std::invalid_argument("Invalid schema: " + schemaStr);
}

void Config::applySchema(Schema schema) {
    // Apply configuration values based on the specified schema
    switch (schema) {
        case Schema::Phase1:
            applyPhase1Schema();
            break;
        case Schema::Phase2:
            applyPhase2Schema();
            break;
        case Schema::Phase3:
            applyPhase3Schema();
            break;
        case Schema::Phase4:
            applyPhase4Schema();
            break;
        case Schema::Phase5:
            applyPhase5Schema();
            break;
        case Schema::Phase6:
            applyPhase6Schema();
            break;
    }
}

void Config::applyPhase1Schema() {
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    set("connection_probability", 0.1f, ConfigSource::Default);
    set("plasticity_learning_rate", 0.01f, ConfigSource::Default);
    set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    set("stdp_time_constant", 20.0f, ConfigSource::Default);
    set("development_synaptogenesis_rate", 0.001f, ConfigSource::Default);
    set("development_pruning_rate", 0.0001f, ConfigSource::Default);
}

void Config::applyPhase2Schema() {
    applyPhase1Schema();
    set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    set("stdp_tau", 20.0f, ConfigSource::Default);
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
}

void Config::applyPhase3Schema() {
    applyPhase2Schema();
    set("environment_name", std::string("GridWorld"), ConfigSource::Default);
    set("environment_width", static_cast<int64_t>(16), ConfigSource::Default);
    set("environment_height", static_cast<int64_t>(16), ConfigSource::Default);
    set("reward_discount_factor", 0.99, ConfigSource::Default);
    set("max_simulation_steps", static_cast<int64_t>(10000), ConfigSource::Default);
}

void Config::applyPhase4Schema() {
    applyPhase3Schema();
    set("curiosity_baseline", 0.1, ConfigSource::Default);
    set("novelty_threshold", 0.5, ConfigSource::Default);
    set("attention_focus_weight", 0.8, ConfigSource::Default);
    set("planner_planning_horizon", static_cast<int64_t>(10), ConfigSource::Default);
}

void Config::applyPhase5Schema() {
    applyPhase4Schema();
    set("checkpoint_path", std::string("./checkpoint.bin"), ConfigSource::Default);
    set("enable_checkpointing", true, ConfigSource::Default);
    set("enable_replay", true, ConfigSource::Default);
    set("enable_development", true, ConfigSource::Default);
    set("validation_interval", static_cast<int64_t>(100), ConfigSource::Default);
}

void Config::applyPhase6Schema() {
    applyPhase5Schema();
    set("validation_interval", static_cast<int64_t>(50), ConfigSource::Default);
    set("experimental_mode", true, ConfigSource::Default);
    set("extended_logging", true, ConfigSource::Default);
}

ConfigEntry Config::createEntryFromSchema(const std::string& key, const ConfigValue& value, Schema schema) const {
    ConfigEntry entry(key, value, ConfigSource::Default);
    
    // Add descriptions based on schema
    switch (schema) {
        case Schema::Phase1:
            entry.description = getPhase1Description(key);
            break;
        case Schema::Phase2:
            entry.description = getPhase2Description(key);
            break;
        case Schema::Phase3:
            entry.description = getPhase3Description(key);
            break;
        case Schema::Phase4:
            entry.description = getPhase4Description(key);
            break;
        case Schema::Phase5:
            entry.description = getPhase5Description(key);
            break;
        case Schema::Phase6:
            entry.description = getPhase6Description(key);
            break;
    }
    
    return entry;
}

std::string Config::getPhase1Description(const std::string& key) const {
    if (key == "random_seed") return "Random seed for reproducible experiments";
    if (key == "simulation_timestep") return "Simulation timestep in seconds";
    if (key == "neuron_count") return "Number of neurons in the brain";
    if (key == "region_count") return "Number of brain regions";
    if (key == "connection_probability") return "Connection probability for random connectivity";
    if (key == "plasticity_learning_rate") return "Base plasticity learning rate";
    if (key == "stdp_ltp_weight") return "STDP long-term potentiation weight";
    if (key == "stdp_ltd_weight") return "STDP long-term depression weight";
    if (key == "stdp_time_constant") return "STDP time constant for synaptic traces";
    if (key == "development_synaptogenesis_rate") return "Rate of synapse formation during development";
    if (key == "development_pruning_rate") return "Rate of synapse elimination during development";
    return "";
}

std::string Config::getPhase2Description(const std::string& key) const {
    std::string desc = getPhase1Description(key);
    if (desc.empty()) {
        if (key == "stdp_ltp_weight") return "STDP LTP weight for real neural computation (Phase 2)";
        if (key == "stdp_ltd_weight") return "STDP LTD weight for real neural computation (Phase 2)";
        if (key == "stdp_tau") return "STDP time constant (Phase 2)";
        if (key == "synaptogenesis_rate") return "Synaptogenesis rate (Phase 2)";
        if (key == "pruning_rate") return "Pruning rate (Phase 2)";
    }
    return desc;
}

std::string Config::getPhase3Description(const std::string& key) const {
    std::string desc = getPhase2Description(key);
    if (desc.empty()) {
        if (key == "environment_name") return "Name of the environment for interaction";
        if (key == "environment_width") return "Environment width in cells";
        if (key == "environment_height") return "Environment height in cells";
        if (key == "reward_discount_factor") return "Factor for future reward discounting";
        if (key == "max_simulation_steps") return "Maximum number of simulation steps";
    }
    return desc;
}

std::string Config::getPhase4Description(const std::string& key) const {
    std::string desc = getPhase3Description(key);
    if (desc.empty()) {
        if (key == "curiosity_baseline") return "Baseline curiosity level (Phase 4)";
        if (key == "novelty_threshold") return "Threshold for novelty detection (Phase 4)";
        if (key == "attention_focus_weight") return "Weight for attention focus mechanism (Phase 4)";
        if (key == "planner_planning_horizon") return "Planning horizon for neural planner (Phase 4)";
    }
    return desc;
}

std::string Config::getPhase5Description(const std::string& key) const {
    std::string desc = getPhase4Description(key);
    if (desc.empty()) {
        if (key == "checkpoint_path") return "Path for saving checkpoints (Phase 5)";
        if (key == "enable_checkpointing") return "Enable checkpoint save/load functionality (Phase 5)";
        if (key == "enable_replay") return "Enable episodic memory replay (Phase 5)";
        if (key == "enable_development") return "Enable developmental plasticity (Phase 5)";
        if (key == "validation_interval") return "Interval between validation checks (Phase 5)";
    }
    return desc;
}

std::string Config::getPhase6Description(const std::string& key) const {
    std::string desc = getPhase5Description(key);
    if (desc.empty()) {
        if (key == "experimental_mode") return "Enable experimental features (Phase 6)";
        if (key == "extended_logging") return "Enable extended logging for debugging (Phase 6)";
        if (key == "validation_interval") return "Interval between validation checks (Phase 6)";
    }
    return desc;
}

void Config::validateConfig() const {
    // Validate critical configuration values
    auto val = getOr<double>("simulation_timestep", 0.001);
    if (val <= 0.0) {
        throw std::invalid_argument("Simulation timestep must be positive");
    }
    
    auto neuronCount = getOr<int64_t>("neuron_count", 1000);
    if (neuronCount <= 0) {
        throw std::invalid_argument("Neuron count must be positive");
    }
    
    auto regionCount = getOr<int64_t>("region_count", 1);
    if (regionCount <= 0) {
        throw std::invalid_argument("Region count must be positive");
    }
    
    auto connProb = getOr<float>("connection_probability", 0.1f);
    if (connProb < 0.0f || connProb > 1.0f) {
        throw std::invalid_argument("Connection probability must be between 0 and 1");
    }
    
    // Validate STDP parameters
    auto stdpLtp = getOr<float>("stdp_ltp_weight", 0.01f);
    auto stdpLtd = getOr<float>("stdp_ltd_weight", 0.012f);
    if (stdpLtp <= 0.0f || stdpLtd <= 0.0f) {
        throw std::invalid_argument("STDP weights must be positive");
    }
    
    auto stdpTime = getOr<float>("stdp_time_constant", 20.0f);
    if (stdpTime <= 0.0f) {
        throw std::invalid_argument("STDP time constant must be positive");
    }
}

void Config::mergeConfig(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, entry.source);
    }
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
