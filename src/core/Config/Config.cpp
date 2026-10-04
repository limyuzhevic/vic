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

bool Config::validateConfiguration() const {
    std::vector<ConfigEntry> entries;
    entries.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        entries.push_back(entry);
    }
    
    lastValidationResult = getConfigurationSchema().validateAll(entries);
    return lastValidationResult.valid;
}

std::string Config::getValidationReport() const {
    return getConfigurationSchema().createValidationReport(lastValidationResult);
}

const ValidationResult& Config::getLastValidationResult() const {
    return lastValidationResult;
}

// Initialize the configuration schema
static void initializeConfigurationSchema() {
    ConfigurationSchema& schema = ConfigurationSchema::getInstance();
    
    // Initialize the schema with all configuration parameters and their validation rules
    // Based on main.cpp and config file specifications
    
    // int64_t types
    schema.addParameter("random_seed", ConfigurationSchema::Type::INT64_T, 
                       "Random seed for reproducible experiments", false);
    
    schema.addParameter("neuron_count", ConfigurationSchema::Type::INT64_T,
                       "Number of neurons in the brain", true,
                       std::int64_t(500), {
                           {ConfigurationSchema::ValidationType::POSITIVE, {}, {}, "must be positive"}
                       });
    
    schema.addParameter("region_count", ConfigurationSchema::Type::INT64_T,
                       "Number of brain regions", true,
                       std::int64_t(1), {
                           {ConfigurationSchema::ValidationType::POSITIVE, {}, {}, "must be positive"}
                       });
    
    schema.addParameter("environment_width", ConfigurationSchema::Type::INT64_T,
                       "Environment width", true,
                       std::int64_t(10), {
                           {ConfigurationSchema::ValidationType::POSITIVE, {}, {}, "must be positive"}
                       });
    
    schema.addParameter("environment_height", ConfigurationSchema::Type::INT64_T,
                       "Environment height", true,
                       std::int64_t(10), {
                           {ConfigurationSchema::ValidationType::POSITIVE, {}, {}, "must be positive"}
                       });
    
    schema.addParameter("max_simulation_steps", ConfigurationSchema::Type::INT64_T,
                       "Maximum simulation steps", true,
                       std::int64_t(10000), {
                           {ConfigurationSchema::ValidationType::NON_NEGATIVE, {}, {}, "must be non-negative"}
                       });
    
    // double types
    schema.addParameter("simulation_timestep", ConfigurationSchema::Type::DOUBLE,
                       "Simulation timestep in seconds", true,
                       double(0.001), {
                           {ConfigurationSchema::ValidationType::RANGE_MIN_MAX, std::any(0.0), std::any(1.0), "must be in range (0, 1.0]"}
                       });
    
    // float types
    schema.addParameter("connection_probability", ConfigurationSchema::Type::FLOAT,
                       "Probability of connection between neurons", true,
                       float(0.15f), {
                           {ConfigurationSchema::ValidationType::ZERO_TO_ONE, {}, {}, "must be in range [0, 1]"}
                       });
    
    schema.addParameter("plasticity_learning_rate", ConfigurationSchema::Type::FLOAT,
                       "Plasticity learning rate", true,
                       float(0.01f), {
                           {ConfigurationSchema::ValidationType::ZERO_TO_ONE, {}, {}, "must be in range [0, 1]"}
                       });
    
    schema.addParameter("stdp_ltp_weight", ConfigurationSchema::Type::FLOAT,
                       "STDP LTP weight", true,
                       float(0.02f), {
                           {ConfigurationSchema::ValidationType::RANGE_MIN_MAX, std::any(-1.0f), std::any(1.0f), "must be in reasonable range"}
                       });
    
    schema.addParameter("stdp_ltd_weight", ConfigurationSchema::Type::FLOAT,
                       "STDP LTD weight", true,
                       float(0.015f), {
                           {ConfigurationSchema::ValidationType::RANGE_MIN_MAX, std::any(-1.0f), std::any(1.0f), "must be in reasonable range"}
                       });
    
    schema.addParameter("stdp_time_constant", ConfigurationSchema::Type::FLOAT,
                       "STDP time constant", true,
                       float(20.0f), {
                           {ConfigurationSchema::ValidationType::POSITIVE, {}, {}, "must be positive"}
                       });
    
    schema.addParameter("synaptogenesis_rate", ConfigurationSchema::Type::FLOAT,
                       "Synaptogenesis rate", true,
                       float(0.0001f), {
                           {ConfigurationSchema::ValidationType::RANGE_MIN_MAX, std::any(0.0f), std::any(0.001f), "must be small positive rate"}
                       });
    
    schema.addParameter("pruning_rate", ConfigurationSchema::Type::FLOAT,
                       "Pruning rate", true,
                       float(0.00001f), {
                           {ConfigurationSchema::ValidationType::RANGE_MIN_MAX, std::any(0.0f), std::any(0.0001f), "must be small positive rate"}
                       });
    
    schema.addParameter("dopamine_baseline", ConfigurationSchema::Type::FLOAT,
                       "Dopamine baseline", true,
                       float(0.1f), {
                           {ConfigurationSchema::ValidationType::ZERO_TO_ONE, {}, {}, "must be in range [0, 1]"}
                       });
    
    schema.addParameter("reward_discount_factor", ConfigurationSchema::Type::FLOAT,
                       "Reward discount factor", true,
                       float(0.99f), {
                           {ConfigurationSchema::ValidationType::ZERO_TO_ONE, {}, {}, "must be in range [0, 1]"}
                       });
    
    schema.addParameter("visualization_update_rate", ConfigurationSchema::Type::FLOAT,
                       "Visualization update rate", true,
                       float(30.0f), {
                           {ConfigurationSchema::ValidationType::POSITIVE, {}, {}, "must be positive"}
                       });
    
    // bool types
    schema.addParameter("log_to_file", ConfigurationSchema::Type::BOOL,
                       "Log to file flag", true,
                       false);
    
    schema.addParameter("visualization_enabled", ConfigurationSchema::Type::BOOL,
                       "Visualization enabled flag", true,
                       false);
    
    // string types
    schema.addParameter("environment_name", ConfigurationSchema::Type::STRING,
                       "Environment name", true,
                       std::string("GridWorld"));
    
    schema.addParameter("log_level", ConfigurationSchema::Type::STRING,
                       "Log level", true,
                       std::string("INFO"));
    
    schema.addParameter("log_filename", ConfigurationSchema::Type::STRING,
                       "Log filename", true,
                       std::string("nlm.log"));
    
    // vector types (optional)
    schema.addParameter("sensor_parameters", ConfigurationSchema::Type::VECTOR_DOUBLE,
                       "Sensor parameters", false);
    
    // Log schema initialization
    // std::cout << "Configuration schema initialized with " << schema.getParameterNames().size() << " parameters" << std::endl;
}

// Ensure schema is initialized on first use
namespace {
    struct SchemaInitializer {
        SchemaInitializer() {
            initializeConfigurationSchema();
        }
    };
    SchemaInitializer schemaInitializer;
}

} // namespace nlm
