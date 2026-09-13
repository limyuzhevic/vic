#include "ResearchConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <functional>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <random>
#include <unordered_set>

namespace nlm {

struct ResearchConfigManager::Impl {
    ResearchConfig currentConfig;
    std::string configDirectory;
    
    // Default configuration values
    static constexpr size_t DEFAULT_NEURON_COUNT = 1000;
    static constexpr size_t DEFAULT_REGION_COUNT = 2;
    static constexpr double DEFAULT_SIMULATION_TIMESTEP = 0.001;
    static constexpr uint64_t DEFAULT_RANDOM_SEED = 42;
    static constexpr double DEFAULT_PLASTICITY_LEARNING_RATE = 0.01;
    static constexpr double DEFAULT_STDP_LTP_WEIGHT = 0.01;
    static constexpr double DEFAULT_STDP_LTD_WEIGHT = 0.012;
    static constexpr double DEFAULT_STDP_TIME_CONSTANT = 20.0;
    static constexpr double DEFAULT_DEVELOPMENT_SYNAPTOGENESIS_RATE = 0.001;
    static constexpr double DEFAULT_DEVELOPMENT_PRUNING_RATE = 0.0001;
    static constexpr double DEFAULT_DOPAMINE_BASELINE = 0.0;
    static constexpr double DEFAULT_DOPAMINE_RELEASE_RATE = 0.1;
    static constexpr double DEFAULT_REWARD_DISCOUNT_FACTOR = 0.99;
    static constexpr const char* DEFAULT_ENVIRONMENT_NAME = "GridWorld";
    static constexpr size_t DEFAULT_ENVIRONMENT_WIDTH = 10;
    static constexpr size_t DEFAULT_ENVIRONMENT_HEIGHT = 10;
    static constexpr const char* DEFAULT_LOG_LEVEL = "INFO";
    static constexpr bool DEFAULT_LOG_TO_FILE = false;
    static constexpr const char* DEFAULT_LOG_FILENAME = "nlm.log";
    static constexpr size_t DEFAULT_MAX_SIMULATION_STEPS = 10000;
    static constexpr double DEFAULT_SIMULATION_TIME_LIMIT = 0.0;
    static constexpr bool DEFAULT_VISUALIZATION_ENABLED = false;
    static constexpr double DEFAULT_VISUALIZATION_UPDATE_RATE = 30.0;
    static constexpr size_t DEFAULT_NUM_TRIALS = 10;
    static constexpr size_t DEFAULT_NUM_SEEDS = 3;
    static constexpr size_t DEFAULT_MAX_CONCURRENT_EXPERIMENTS = 1;
    static constexpr const char* DEFAULT_OUTPUT_FORMAT = "JSON";
    static constexpr bool DEFAULT_SAVE_CHECKPOINTS = false;
    static constexpr size_t DEFAULT_CHECKPOINT_FREQUENCY = 1000;
    static constexpr bool DEFAULT_ENABLE_BATCH_PROCESSING = false;
    static constexpr size_t DEFAULT_BATCH_SIZE = 5;
    static constexpr bool DEFAULT_ENABLE_PARALLEL_PROCESSING = false;
    static constexpr size_t DEFAULT_PARALLEL_WORKERS = 2;
    static constexpr bool DEFAULT_ENABLE_PERFORMANCE_MONITORING = false;
    static constexpr bool DEFAULT_ENABLE_HARDWARE_ACCELERATION = false;
    static constexpr bool DEFAULT_OPTIMIZE_FOR_SCALE = false;
};

ResearchConfigManager::ResearchConfigManager() : pImpl(std::make_unique<Impl>()) {
    initializeDefaults();
}

ResearchConfigManager::~ResearchConfigManager() = default;

bool ResearchConfigManager::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Parse INI-like format
        std::string line;
        std::string currentSection;
        
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Handle sections
            if (line.find('[') != std::string::npos && line.find(']') != std::string::npos) {
                currentSection = line.substr(line.find('[') + 1, line.find(']') - line.find('[') - 1);
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
                
                setParameter(key, value);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool ResearchConfigManager::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# NLM Research Configuration\n";
        file << "# Generated: " << formatTimestamp(getTimestamp()) << "\n\n";
        
        file << "[core]\n";
        file << "name = " << currentConfig.name << "\n";
        file << "description = " << currentConfig.description << "\n";
        file << "mode = " << static_cast<int>(currentConfig.mode) << "\n\n";
        
        file << "[simulation]\n";
        file << "neuron_count = " << currentConfig.neuron_count << "\n";
        file << "region_count = " << currentConfig.region_count << "\n";
        file << "simulation_timestep = " << currentConfig.simulation_timestep << "\n";
        file << "random_seed = " << currentConfig.random_seed << "\n\n";
        
        file << "[plasticity]\n";
        file << "plasticity_learning_rate = " << currentConfig.plasticity_learning_rate << "\n";
        file << "stdp_ltp_weight = " << currentConfig.stdp_ltp_weight << "\n";
        file << "stdp_ltd_weight = " << currentConfig.stdp_ltd_weight << "\n";
        file << "stdp_time_constant = " << currentConfig.stdp_time_constant << "\n\n";
        
        file << "[development]\n";
        file << "development_synaptogenesis_rate = " << currentConfig.development_synaptogenesis_rate << "\n";
        file << "development_pruning_rate = " << currentConfig.development_pruning_rate << "\n\n";
        
        file << "[neuromodulation]\n";
        file << "dopamine_baseline = " << currentConfig.dopamine_baseline << "\n";
        file << "dopamine_release_rate = " << currentConfig.dopamine_release_rate << "\n\n";
        
        file << "[reward]\n";
        file << "reward_discount_factor = " << currentConfig.reward_discount_factor << "\n\n";
        
        file << "[environment]\n";
        file << "environment_name = " << currentConfig.environment_name << "\n";
        file << "environment_width = " << currentConfig.environment_width << "\n";
        file << "environment_height = " << currentConfig.environment_height << "\n\n";
        
        file << "[logging]\n";
        file << "log_level = " << currentConfig.log_level << "\n";
        file << "log_to_file = " << (currentConfig.log_to_file ? "true" : "false") << "\n";
        file << "log_filename = " << currentConfig.log_filename << "\n\n";
        
        file << "[simulation_settings]\n";
        file << "max_simulation_steps = " << currentConfig.max_simulation_steps << "\n";
        file << "simulation_time_limit = " << currentConfig.simulation_time_limit << "\n\n";
        
        file << "[visualization]\n";
        file << "visualization_enabled = " << (currentConfig.visualization_enabled ? "true" : "false") << "\n";
        file << "visualization_update_rate = " << currentConfig.visualization_update_rate << "\n\n";
        
        file << "[research_specific]\n";
        file << "num_trials = " << currentConfig.num_trials << "\n";
        file << "num_seeds = " << currentConfig.num_seeds << "\n";
        file << "max_concurrent_experiments = " << currentConfig.max_concurrent_experiments << "\n";
        file << "output_format = " << currentConfig.output_format << "\n";
        file << "save_checkpoints = " << (currentConfig.save_checkpoints ? "true" : "false") << "\n";
        file << "checkpoint_frequency = " << currentConfig.checkpoint_frequency << "\n\n";
        
        file << "[batch_processing]\n";
        file << "enable_batch_processing = " << (currentConfig.enable_batch_processing ? "true" : "false") << "\n";
        file << "batch_size = " << currentConfig.batch_size << "\n";
        file << "enable_parallel_processing = " << (currentConfig.enable_parallel_processing ? "true" : "false") << "\n";
        file << "parallel_workers = " << currentConfig.parallel_workers << "\n\n";
        
        file << "[performance]\n";
        file << "enable_performance_monitoring = " << (currentConfig.enable_performance_monitoring ? "true" : "false") << "\n";
        file << "enable_hardware_acceleration = " << (currentConfig.enable_hardware_acceleration ? "true" : "false") << "\n";
        file << "optimize_for_scale = " << (currentConfig.optimize_for_scale ? "true" : "false") << "\n";
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool ResearchConfigManager::loadFromJSON(const std::string& filepath) {
    // TODO: Implement JSON parser
    return false;
}

nlm::Config ResearchConfigManager::toCoreConfig() const {
    nlm::Config coreConfig;
    
    // Map research configuration to core configuration
    coreConfig.set("neuron_count", currentConfig.neuron_count, nlm::ConfigSource::File);
    coreConfig.set("region_count", currentConfig.region_count, nlm::ConfigSource::File);
    coreConfig.set("simulation_timestep", currentConfig.simulation_timestep, nlm::ConfigSource::File);
    coreConfig.set("random_seed", currentConfig.random_seed, nlm::ConfigSource::File);
    coreConfig.set("plasticity_learning_rate", currentConfig.plasticity_learning_rate, nlm::ConfigSource::File);
    coreConfig.set("stdp_ltp_weight", currentConfig.stdp_ltp_weight, nlm::ConfigSource::File);
    coreConfig.set("stdp_ltd_weight", currentConfig.stdp_ltd_weight, nlm::ConfigSource::File);
    coreConfig.set("stdp_time_constant", currentConfig.stdp_time_constant, nlm::ConfigSource::File);
    coreConfig.set("development_synaptogenesis_rate", currentConfig.development_synaptogenesis_rate, nlm::ConfigSource::File);
    coreConfig.set("development_pruning_rate", currentConfig.development_pruning_rate, nlm::ConfigSource::File);
    coreConfig.set("dopamine_baseline", currentConfig.dopamine_baseline, nlm::ConfigSource::File);
    coreConfig.set("dopamine_release_rate", currentConfig.dopamine_release_rate, nlm::ConfigSource::File);
    coreConfig.set("reward_discount_factor", currentConfig.reward_discount_factor, nlm::ConfigSource::File);
    coreConfig.set("environment_name", currentConfig.environment_name, nlm::ConfigSource::File);
    coreConfig.set("environment_width", currentConfig.environment_width, nlm::ConfigSource::File);
    coreConfig.set("environment_height", currentConfig.environment_height, nlm::ConfigSource::File);
    coreConfig.set("log_level", currentConfig.log_level, nlm::ConfigSource::File);
    coreConfig.set("log_to_file", currentConfig.log_to_file, nlm::ConfigSource::File);
    coreConfig.set("log_filename", currentConfig.log_filename, nlm::ConfigSource::File);
    coreConfig.set("max_simulation_steps", currentConfig.max_simulation_steps, nlm::ConfigSource::File);
    coreConfig.set("simulation_time_limit", currentConfig.simulation_time_limit, nlm::ConfigSource::File);
    coreConfig.set("visualization_enabled", currentConfig.visualization_enabled, nlm::ConfigSource::File);
    coreConfig.set("visualization_update_rate", currentConfig.visualization_update_rate, nlm::ConfigSource::File);
    
    return coreConfig;
}

ResearchConfig ResearchConfigManager::createMinimalConfig() {
    ResearchConfig config("minimal_research");
    config.mode = ResearchMode::Minimal;
    config.description = "Minimal configuration for quick prototyping and testing";
    config.neuron_count = 100;
    config.region_count = 1;
    config.simulation_timestep = 0.001;
    config.random_seed = 42;
    config.plasticity_learning_rate = 0.01;
    config.stdp_ltp_weight = 0.01;
    config.stdp_ltd_weight = 0.012;
    config.stdp_time_constant = 20.0;
    config.development_synaptogenesis_rate = 0.001;
    config.development_pruning_rate = 0.0001;
    config.dopamine_baseline = 0.0;
    config.dopamine_release_rate = 0.1;
    config.reward_discount_factor = 0.99;
    config.environment_name = "GridWorld";
    config.environment_width = 10;
    config.environment_height = 10;
    config.log_level = "INFO";
    config.log_to_file = false;
    config.log_filename = "nlm_minimal.log";
    config.max_simulation_steps = 1000;
    config.simulation_time_limit = 0.0;
    config.visualization_enabled = false;
    config.visualization_update_rate = 30.0;
    config.num_trials = 3;
    config.num_seeds = 2;
    config.max_concurrent_experiments = 1;
    config.output_format = "CSV";
    config.save_checkpoints = false;
    config.checkpoint_frequency = 1000;
    config.enable_batch_processing = false;
    config.batch_size = 3;
    config.enable_parallel_processing = false;
    config.parallel_workers = 1;
    config.enable_performance_monitoring = false;
    config.enable_hardware_acceleration = false;
    config.optimize_for_scale = false;
    return config;
}

ResearchConfig ResearchConfigManager::createStandardConfig() {
    ResearchConfig config("standard_research");
    config.mode = ResearchMode::Standard;
    config.description = "Standard configuration for typical research experiments";
    config.neuron_count = 1000;
    config.region_count = 2;
    config.simulation_timestep = 0.001;
    config.random_seed = 42;
    config.plasticity_learning_rate = 0.01;
    config.stdp_ltp_weight = 0.01;
    config.stdp_ltd_weight = 0.012;
    config.stdp_time_constant = 20.0;
    config.development_synaptogenesis_rate = 0.001;
    config.development_pruning_rate = 0.0001;
    config.dopamine_baseline = 0.0;
    config.dopamine_release_rate = 0.1;
    config.reward_discount_factor = 0.99;
    config.environment_name = "GridWorld";
    config.environment_width = 20;
    config.environment_height = 20;
    config.log_level = "INFO";
    config.log_to_file = true;
    config.log_filename = "nlm_research.log";
    config.max_simulation_steps = 10000;
    config.simulation_time_limit = 0.0;
    config.visualization_enabled = true;
    config.visualization_update_rate = 30.0;
    config.num_trials = 10;
    config.num_seeds = 5;
    config.max_concurrent_experiments = 1;
    config.output_format = "JSON";
    config.save_checkpoints = true;
    config.checkpoint_frequency = 1000;
    config.enable_batch_processing = true;
    config.batch_size = 5;
    config.enable_parallel_processing = true;
    config.parallel_workers = 2;
    config.enable_performance_monitoring = true;
    config.enable_hardware_acceleration = false;
    config.optimize_for_scale = false;
    return config;
}

ResearchConfig ResearchConfigManager::createLargeScaleConfig() {
    ResearchConfig config("large_scale_research");
    config.mode = ResearchMode::LargeScale;
    config.description = "Large-scale configuration for comprehensive research studies";
    config.neuron_count = 10000;
    config.region_count = 4;
    config.simulation_timestep = 0.001;
    config.random_seed = 42;
    config.plasticity_learning_rate = 0.005;
    config.stdp_ltp_weight = 0.01;
    config.stdp_ltd_weight = 0.012;
    config.stdp_time_constant = 20.0;
    config.development_synaptogenesis_rate = 0.0005;
    config.development_pruning_rate = 0.00005;
    config.dopamine_baseline = 0.0;
    config.dopamine_release_rate = 0.05;
    config.reward_discount_factor = 0.99;
    config.environment_name = "GridWorld";
    config.environment_width = 50;
    config.environment_height = 50;
    config.log_level = "INFO";
    config.log_to_file = true;
    config.log_filename = "nlm_large_scale.log";
    config.max_simulation_steps = 50000;
    config.simulation_time_limit = 0.0;
    config.visualization_enabled = false;
    config.visualization_update_rate = 30.0;
    config.num_trials = 20;
    config.num_seeds = 10;
    config.max_concurrent_experiments = 2;
    config.output_format = "BINARY";
    config.save_checkpoints = true;
    config.checkpoint_frequency = 5000;
    config.enable_batch_processing = true;
    config.batch_size = 10;
    config.enable_parallel_processing = true;
    config.parallel_workers = 4;
    config.enable_performance_monitoring = true;
    config.enable_hardware_acceleration = true;
    config.optimize_for_scale = true;
    return config;
}

ResearchConfig ResearchConfigManager::createDevelopmentalConfig() {
    ResearchConfig config("developmental_research");
    config.mode = ResearchMode::Developmental;
    config.description = "Developmental configuration for studying developmental processes";
    config.neuron_count = 500;
    config.region_count = 2;
    config.simulation_timestep = 0.001;
    config.random_seed = 42;
    config.plasticity_learning_rate = 0.02;
    config.stdp_ltp_weight = 0.02;
    config.stdp_ltd_weight = 0.025;
    config.stdp_time_constant = 10.0;
    config.development_synaptogenesis_rate = 0.01;
    config.development_pruning_rate = 0.002;
    config.dopamine_baseline = 0.1;
    config.dopamine_release_rate = 0.2;
    config.reward_discount_factor = 0.95;
    config.environment_name = "GridWorld";
    config.environment_width = 15;
    config.environment_height = 15;
    config.log_level = "DEBUG";
    config.log_to_file = true;
    config.log_filename = "nlm_developmental.log";
    config.max_simulation_steps = 20000;
    config.simulation_time_limit = 0.0;
    config.visualization_enabled = true;
    config.visualization_update_rate = 15.0;
    config.num_trials = 15;
    config.num_seeds = 8;
    config.max_concurrent_experiments = 1;
    config.output_format = "JSON";
    config.save_checkpoints = true;
    config.checkpoint_frequency = 2000;
    config.enable_batch_processing = true;
    config.batch_size = 5;
    config.enable_parallel_processing = false;
    config.parallel_workers = 2;
    config.enable_performance_monitoring = true;
    config.enable_hardware_acceleration = true;
    config.optimize_for_scale = false;
    return config;
}

ResearchConfig ResearchConfigManager::createCustomConfig(const std::string& name) {
    ResearchConfig config(name);
    config.mode = ResearchMode::Custom;
    config.description = "Custom configuration with user-defined parameters";
    initializeDefaults();
    return config;
}

std::vector<std::string> ResearchConfigManager::validate() const {
    std::vector<std::string> errors;
    
    // Check basic parameters
    if (currentConfig.neuron_count == 0) {
        errors.push_back("neuron_count must be greater than 0");
    }
    
    if (currentConfig.region_count == 0) {
        errors.push_back("region_count must be greater than 0");
    }
    
    if (currentConfig.simulation_timestep <= 0.0) {
        errors.push_back("simulation_timestep must be positive");
    }
    
    if (currentConfig.plasticity_learning_rate < 0.0) {
        errors.push_back("plasticity_learning_rate must be non-negative");
    }
    
    if (currentConfig.stdp_ltp_weight < 0.0) {
        errors.push_back("stdp_ltp_weight must be non-negative");
    }
    
    if (currentConfig.stdp_ltd_weight < 0.0) {
        errors.push_back("stdp_ltd_weight must be non-negative");
    }
    
    if (currentConfig.stdp_time_constant <= 0.0) {
        errors.push_back("stdp_time_constant must be positive");
    }
    
    if (currentConfig.development_synaptogenesis_rate < 0.0) {
        errors.push_back("development_synaptogenesis_rate must be non-negative");
    }
    
    if (currentConfig.development_pruning_rate < 0.0) {
        errors.push_back("development_pruning_rate must be non-negative");
    }
    
    if (currentConfig.dopamine_release_rate < 0.0) {
        errors.push_back("dopamine_release_rate must be non-negative");
    }
    
    if (currentConfig.reward_discount_factor < 0.0 || currentConfig.reward_discount_factor > 1.0) {
        errors.push_back("reward_discount_factor must be between 0.0 and 1.0");
    }
    
    if (currentConfig.environment_width == 0 || currentConfig.environment_height == 0) {
        errors.push_back("environment dimensions must be greater than 0");
    }
    
    if (currentConfig.max_simulation_steps == 0) {
        errors.push_back("max_simulation_steps must be greater than 0");
    }
    
    if (currentConfig.num_trials == 0) {
        errors.push_back("num_trials must be greater than 0");
    }
    
    if (currentConfig.num_seeds == 0) {
        errors.push_back("num_seeds must be greater than 0");
    }
    
    if (currentConfig.checkpoint_frequency == 0 && currentConfig.save_checkpoints) {
        errors.push_back("checkpoint_frequency must be greater than 0 when save_checkpoints is true");
    }
    
    if (currentConfig.parallel_workers == 0 && currentConfig.enable_parallel_processing) {
        errors.push_back("parallel_workers must be greater than 0 when enable_parallel_processing is true");
    }
    
    if (!isValidLogLevel(currentConfig.log_level)) {
        errors.push_back("Invalid log_level: " + currentConfig.log_level);
    }
    
    if (!isValidEnvironment(currentConfig.environment_name)) {
        errors.push_back("Invalid environment_name: " + currentConfig.environment_name);
    }
    
    return errors;
}

bool ResearchConfigManager::isValid() const {
    return validate().empty();
}

std::string ResearchConfigManager::generateReport() const {
    std::ostringstream oss;
    oss << "Research Configuration Report\n";
    oss << "===========================" << "\n\n";
    oss << "Name: " << currentConfig.name << "\n";
    oss << "Description: " << currentConfig.description << "\n";
    oss << "Mode: " << static_cast<int>(currentConfig.mode) << "\n";
    oss << "\n";
    oss << "Simulation Parameters:\n";
    oss << "  - Neurons: " << currentConfig.neuron_count << "\n";
    oss << "  - Regions: " << currentConfig.region_count << "\n";
    oss << "  - Timestep: " << currentConfig.simulation_timestep << "\n";
    oss << "  - Random Seed: " << currentConfig.random_seed << "\n";
    oss << "\n";
    oss << "Plasticity:\n";
    oss << "  - Learning Rate: " << currentConfig.plasticity_learning_rate << "\n";
    oss << "  - STDP LTP: " << currentConfig.stdp_ltp_weight << "\n";
    oss << "  - STDP LTD: " << currentConfig.stdp_ltd_weight << "\n";
    oss << "  - STDP Time Constant: " << currentConfig.stdp_time_constant << "\n";
    oss << "\n";
    oss << "Development:\n";
    oss << "  - Synaptogenesis Rate: " << currentConfig.development_synaptogenesis_rate << "\n";
    oss << "  - Pruning Rate: " << currentConfig.development_pruning_rate << "\n";
    oss << "\n";
    oss << "Neuromodulation:\n";
    oss << "  - Dopamine Baseline: " << currentConfig.dopamine_baseline << "\n";
    oss << "  - Dopamine Release Rate: " << currentConfig.dopamine_release_rate << "\n";
    oss << "\n";
    oss << "Reward:\n";
    oss << "  - Discount Factor: " << currentConfig.reward_discount_factor << "\n";
    oss << "\n";
    oss << "Environment:\n";
    oss << "  - Name: " << currentConfig.environment_name << "\n";
    oss << "  - Width: " << currentConfig.environment_width << "\n";
    oss << "  - Height: " << currentConfig.environment_height << "\n";
    oss << "\n";
    oss << "Logging:\n";
    oss << "  - Level: " << currentConfig.log_level << "\n";
    oss << "  - To File: " << (currentConfig.log_to_file ? "Yes" : "No") << "\n";
    oss << "  - Filename: " << currentConfig.log_filename << "\n";
    oss << "\n";
    oss << "Simulation Settings:\n";
    oss << "  - Max Steps: " << currentConfig.max_simulation_steps << "\n";
    oss << "  - Time Limit: " << currentConfig.simulation_time_limit << "\n";
    oss << "\n";
    oss << "Visualization:\n";
    oss << "  - Enabled: " << (currentConfig.visualization_enabled ? "Yes" : "No") << "\n";
    oss << "  - Update Rate: " << currentConfig.visualization_update_rate << "\n";
    oss << "\n";
    oss << "Research Specific:\n";
    oss << "  - Trials: " << currentConfig.num_trials << "\n";
    oss << "  - Seeds: " << currentConfig.num_seeds << "\n";
    oss << "  - Concurrent Experiments: " << currentConfig.max_concurrent_experiments << "\n";
    oss << "  - Output Format: " << currentConfig.output_format << "\n";
    oss << "  - Save Checkpoints: " << (currentConfig.save_checkpoints ? "Yes" : "No") << "\n";
    oss << "  - Checkpoint Frequency: " << currentConfig.checkpoint_frequency << "\n";
    oss << "\n";
    oss << "Batch Processing:\n";
    oss << "  - Enabled: " << (currentConfig.enable_batch_processing ? "Yes" : "No") << "\n";
    oss << "  - Batch Size: " << currentConfig.batch_size << "\n";
    oss << "  - Parallel Processing: " << (currentConfig.enable_parallel_processing ? "Yes" : "No") << "\n";
    oss << "  - Parallel Workers: " << currentConfig.parallel_workers << "\n";
    oss << "\n";
    oss << "Performance:\n";
    oss << "  - Performance Monitoring: " << (currentConfig.enable_performance_monitoring ? "Yes" : "No") << "\n";
    oss << "  - Hardware Acceleration: " << (currentConfig.enable_hardware_acceleration ? "Yes" : "No") << "\n";
    oss << "  - Optimize for Scale: " << (currentConfig.optimize_for_scale ? "Yes" : "No") << "\n";
    oss << "\n";
    
    return oss.str();
}

std::string ResearchConfigManager::generateValidationReport() const {
    auto errors = validate();
    auto warnings = std::vector<std::string>(); // TODO: Add warnings
    
    std::ostringstream oss;
    oss << "Configuration Validation Report\n";
    oss << "===============================" << "\n\n";
    
    if (errors.empty() && warnings.empty()) {
        oss << "✓ Configuration is valid\n";
    } else {
        if (!errors.empty()) {
            oss << "Errors:\n";
            for (const auto& error : errors) {
                oss << "  ✗ " << error << "\n";
            }
            oss << "\n";
        }
        
        if (!warnings.empty()) {
            oss << "Warnings:\n";
            for (const auto& warning : warnings) {
                oss << "  ⚠ " << warning << "\n";
            }
            oss << "\n";
        }
    }
    
    return oss.str();
}

void ResearchConfigManager::setParameter(const std::string& key, const std::string& value) {
    std::string sanitizedKey = sanitizeKey(key);
    
    if (sanitizedKey == "name") {
        currentConfig.name = value;
    } else if (sanitizedKey == "description") {
        currentConfig.description = value;
    } else if (sanitizedKey == "mode") {
        currentConfig.mode = static_cast<ResearchMode>(std::stoi(value));
    } else if (sanitizedKey == "neuron_count") {
        currentConfig.neuron_count = std::stoul(value);
    } else if (sanitizedKey == "region_count") {
        currentConfig.region_count = std::stoul(value);
    } else if (sanitizedKey == "simulation_timestep") {
        currentConfig.simulation_timestep = std::stod(value);
    } else if (sanitizedKey == "random_seed") {
        currentConfig.random_seed = std::stoull(value);
    } else if (sanitizedKey == "plasticity_learning_rate") {
        currentConfig.plasticity_learning_rate = std::stod(value);
    } else if (sanitizedKey == "stdp_ltp_weight") {
        currentConfig.stdp_ltp_weight = std::stod(value);
    } else if (sanitizedKey == "stdp_ltd_weight") {
        currentConfig.stdp_ltd_weight = std::stod(value);
    } else if (sanitizedKey == "stdp_time_constant") {
        currentConfig.stdp_time_constant = std::stod(value);
    } else if (sanitizedKey == "development_synaptogenesis_rate") {
        currentConfig.development_synaptogenesis_rate = std::stod(value);
    } else if (sanitizedKey == "development_pruning_rate") {
        currentConfig.development_pruning_rate = std::stod(value);
    } else if (sanitizedKey == "dopamine_baseline") {
        currentConfig.dopamine_baseline = std::stod(value);
    } else if (sanitizedKey == "dopamine_release_rate") {
        currentConfig.dopamine_release_rate = std::stod(value);
    } else if (sanitizedKey == "reward_discount_factor") {
        currentConfig.reward_discount_factor = std::stod(value);
    } else if (sanitizedKey == "environment_name") {
        currentConfig.environment_name = value;
    } else if (sanitizedKey == "environment_width") {
        currentConfig.environment_width = std::stoul(value);
    } else if (sanitizedKey == "environment_height") {
        currentConfig.environment_height = std::stoul(value);
    } else if (sanitizedKey == "log_level") {
        currentConfig.log_level = value;
    } else if (sanitizedKey == "log_to_file") {
        currentConfig.log_to_file = (value == "true" || value == "1" || value == "yes");
    } else if (sanitizedKey == "log_filename") {
        currentConfig.log_filename = value;
    } else if (sanitizedKey == "max_simulation_steps") {
        currentConfig.max_simulation_steps = std::stoul(value);
    } else if (sanitizedKey == "simulation_time_limit") {
        currentConfig.simulation_time_limit = std::stod(value);
    } else if (sanitizedKey == "visualization_enabled") {
        currentConfig.visualization_enabled = (value == "true" || value == "1" || value == "yes");
    } else if (sanitizedKey == "visualization_update_rate") {
        currentConfig.visualization_update_rate = std::stod(value);
    } else if (sanitizedKey == "num_trials") {
        currentConfig.num_trials = std::stoul(value);
    } else if (sanitizedKey == "num_seeds") {
        currentConfig.num_seeds = std::stoul(value);
    } else if (sanitizedKey == "max_concurrent_experiments") {
        currentConfig.max_concurrent_experiments = std::stoul(value);
    } else if (sanitizedKey == "output_format") {
        currentConfig.output_format = value;
    } else if (sanitizedKey == "save_checkpoints") {
        currentConfig.save_checkpoints = (value == "true" || value == "1" || value == "yes");
    } else if (sanitizedKey == "checkpoint_frequency") {
        currentConfig.checkpoint_frequency = std::stoul(value);
    } else if (sanitizedKey == "enable_batch_processing") {
        currentConfig.enable_batch_processing = (value == "true" || value == "1" || value == "yes");
    } else if (sanitizedKey == "batch_size") {
        currentConfig.batch_size = std::stoul(value);
    } else if (sanitizedKey == "enable_parallel_processing") {
        currentConfig.enable_parallel_processing = (value == "true" || value == "1" || value == "yes");
    } else if (sanitizedKey == "parallel_workers") {
        currentConfig.parallel_workers = std::stoul(value);
    } else if (sanitizedKey == "enable_performance_monitoring") {
        currentConfig.enable_performance_monitoring = (value == "true" || value == "1" || value == "yes");
    } else if (sanitizedKey == "enable_hardware_acceleration") {
        currentConfig.enable_hardware_acceleration = (value == "true" || value == "1" || value == "yes");
    } else if (sanitizedKey == "optimize_for_scale") {
        currentConfig.optimize_for_scale = (value == "true" || value == "1" || value == "yes");
    }
}

void ResearchConfigManager::setParameter(const std::string& key, size_t value) {
    std::string sanitizedKey = sanitizeKey(key);
    
    if (sanitizedKey == "neuron_count") {
        currentConfig.neuron_count = value;
    } else if (sanitizedKey == "region_count") {
        currentConfig.region_count = value;
    } else if (sanitizedKey == "environment_width") {
        currentConfig.environment_width = value;
    } else if (sanitizedKey == "environment_height") {
        currentConfig.environment_height = value;
    } else if (sanitizedKey == "max_simulation_steps") {
        currentConfig.max_simulation_steps = value;
    } else if (sanitizedKey == "num_trials") {
        currentConfig.num_trials = value;
    } else if (sanitizedKey == "num_seeds") {
        currentConfig.num_seeds = value;
    } else if (sanitizedKey == "max_concurrent_experiments") {
        currentConfig.max_concurrent_experiments = value;
    } else if (sanitizedKey == "batch_size") {
        currentConfig.batch_size = value;
    } else if (sanitizedKey == "parallel_workers") {
        currentConfig.parallel_workers = value;
    } else if (sanitizedKey == "checkpoint_frequency") {
        currentConfig.checkpoint_frequency = value;
    }
}

void ResearchConfigManager::setParameter(const std::string& key, double value) {
    std::string sanitizedKey = sanitizeKey(key);
    
    if (sanitizedKey == "simulation_timestep") {
        currentConfig.simulation_timestep = value;
    } else if (sanitizedKey == "random_seed") {
        currentConfig.random_seed = static_cast<uint64_t>(value);
    } else if (sanitizedKey == "plasticity_learning_rate") {
        currentConfig.plasticity_learning_rate = value;
    } else if (sanitizedKey == "stdp_ltp_weight") {
        currentConfig.stdp_ltp_weight = value;
    } else if (sanitizedKey == "stdp_ltd_weight") {
        currentConfig.stdp_ltd_weight = value;
    } else if (sanitizedKey == "stdp_time_constant") {
        currentConfig.stdp_time_constant = value;
    } else if (sanitizedKey == "development_synaptogenesis_rate") {
        currentConfig.development_synaptogenesis_rate = value;
    } else if (sanitizedKey == "development_pruning_rate") {
        currentConfig.development_pruning_rate = value;
    } else if (sanitizedKey == "dopamine_baseline") {
        currentConfig.dopamine_baseline = value;
    } else if (sanitizedKey == "dopamine_release_rate") {
        currentConfig.dopamine_release_rate = value;
    } else if (sanitizedKey == "reward_discount_factor") {
        currentConfig.reward_discount_factor = value;
    } else if (sanitizedKey == "simulation_time_limit") {
        currentConfig.simulation_time_limit = value;
    } else if (sanitizedKey == "visualization_update_rate") {
        currentConfig.visualization_update_rate = value;
    }
}

void ResearchConfigManager::setParameter(const std::string& key, bool value) {
    std::string sanitizedKey = sanitizeKey(key);
    
    if (sanitizedKey == "log_to_file") {
        currentConfig.log_to_file = value;
    } else if (sanitizedKey == "visualization_enabled") {
        currentConfig.visualization_enabled = value;
    } else if (sanitizedKey == "save_checkpoints") {
        currentConfig.save_checkpoints = value;
    } else if (sanitizedKey == "enable_batch_processing") {
        currentConfig.enable_batch_processing = value;
    } else if (sanitizedKey == "enable_parallel_processing") {
        currentConfig.enable_parallel_processing = value;
    } else if (sanitizedKey == "enable_performance_monitoring") {
        currentConfig.enable_performance_monitoring = value;
    } else if (sanitizedKey == "enable_hardware_acceleration") {
        currentConfig.enable_hardware_acceleration = value;
    } else if (sanitizedKey == "optimize_for_scale") {
        currentConfig.optimize_for_scale = value;
    }
}

std::string ResearchConfigManager::getParameter(const std::string& key) const {
    std::string sanitizedKey = sanitizeKey(key);
    
    if (sanitizedKey == "name") return currentConfig.name;
    if (sanitizedKey == "description") return currentConfig.description;
    if (sanitizedKey == "mode") return std::to_string(static_cast<int>(currentConfig.mode));
    if (sanitizedKey == "neuron_count") return std::to_string(currentConfig.neuron_count);
    if (sanitizedKey == "region_count") return std::to_string(currentConfig.region_count);
    if (sanitizedKey == "simulation_timestep") return std::to_string(currentConfig.simulation_timestep);
    if (sanitizedKey == "random_seed") return std::to_string(currentConfig.random_seed);
    if (sanitizedKey == "plasticity_learning_rate") return std::to_string(currentConfig.plasticity_learning_rate);
    if (sanitizedKey == "stdp_ltp_weight") return std::to_string(currentConfig.stdp_ltp_weight);
    if (sanitizedKey == "stdp_ltd_weight") return std::to_string(currentConfig.stdp_ltd_weight);
    if (sanitizedKey == "stdp_time_constant") return std::to_string(currentConfig.stdp_time_constant);
    if (sanitizedKey == "development_synaptogenesis_rate") return std::to_string(currentConfig.development_synaptogenesis_rate);
    if (sanitizedKey == "development_pruning_rate") return std::to_string(currentConfig.development_pruning_rate);
    if (sanitizedKey == "dopamine_baseline") return std::to_string(currentConfig.dopamine_baseline);
    if (sanitizedKey == "dopamine_release_rate") return std::to_string(currentConfig.dopamine_release_rate);
    if (sanitizedKey == "reward_discount_factor") return std::to_string(currentConfig.reward_discount_factor);
    if (sanitizedKey == "environment_name") return currentConfig.environment_name;
    if (sanitizedKey == "environment_width") return std::to_string(currentConfig.environment_width);
    if (sanitizedKey == "environment_height") return std::to_string(currentConfig.environment_height);
    if (sanitizedKey == "log_level") return currentConfig.log_level;
    if (sanitizedKey == "log_to_file") return (currentConfig.log_to_file ? "true" : "false");
    if (sanitizedKey == "log_filename") return currentConfig.log_filename;
    if (sanitizedKey == "max_simulation_steps") return std::to_string(currentConfig.max_simulation_steps);
    if (sanitizedKey == "simulation_time_limit") return std::to_string(currentConfig.simulation_time_limit);
    if (sanitizedKey == "visualization_enabled") return (currentConfig.visualization_enabled ? "true" : "false");
    if (sanitizedKey == "visualization_update_rate") return std::to_string(currentConfig.visualization_update_rate);
    if (sanitizedKey == "num_trials") return std::to_string(currentConfig.num_trials);
    if (sanitizedKey == "num_seeds") return std::to_string(currentConfig.num_seeds);
    if (sanitizedKey == "max_concurrent_experiments") return std::to_string(currentConfig.max_concurrent_experiments);
    if (sanitizedKey == "output_format") return currentConfig.output_format;
    if (sanitizedKey == "save_checkpoints") return (currentConfig.save_checkpoints ? "true" : "false");
    if (sanitizedKey == "checkpoint_frequency") return std::to_string(currentConfig.checkpoint_frequency);
    if (sanitizedKey == "enable_batch_processing") return (currentConfig.enable_batch_processing ? "true" : "false");
    if (sanitizedKey == "batch_size") return std::to_string(currentConfig.batch_size);
    if (sanitizedKey == "enable_parallel_processing") return (currentConfig.enable_parallel_processing ? "true" : "false");
    if (sanitizedKey == "parallel_workers") return std::to_string(currentConfig.parallel_workers);
    if (sanitizedKey == "enable_performance_monitoring") return (currentConfig.enable_performance_monitoring ? "true" : "false");
    if (sanitizedKey == "enable_hardware_acceleration") return (currentConfig.enable_hardware_acceleration ? "true" : "false");
    if (sanitizedKey == "optimize_for_scale") return (currentConfig.optimize_for_scale ? "true" : "false");
    
    return "";
}

ResearchConfig ResearchConfigManager::createCurrentConfig() const {
    return currentConfig;
}

void ResearchConfigManager::setMode(ResearchMode mode) {
    currentConfig.mode = mode;
}

ResearchMode ResearchConfigManager::getMode() const {
    return currentConfig.mode;
}

void ResearchConfigManager::setName(const std::string& name) {
    currentConfig.name = name;
}

const std::string& ResearchConfigManager::getName() const {
    return currentConfig.name;
}

void ResearchConfigManager::setDescription(const std::string& desc) {
    currentConfig.description = desc;
}

const std::string& ResearchConfigManager::getDescription() const {
    return currentConfig.description;
}

bool ResearchConfigManager::matches(const ResearchConfig& other) const {
    return currentConfig.name == other.name && 
           currentConfig.mode == other.mode &&
           currentConfig.neuron_count == other.neuron_count &&
           currentConfig.region_count == other.region_count;
}

std::string ResearchConfigManager::hash() const {
    std::hash<std::string> hasher;
    std::string data = 
        currentConfig.name + 
        std::to_string(currentConfig.neuron_count) + 
        std::to_string(currentConfig.region_count);
    return std::to_string(hasher(data));
}

std::string ResearchConfigManager::generateConfigName(const std::string& prefix) {
    auto now = getTimestamp();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << prefix << "_" << std::put_time(std::localtime(&time_t_now), "%Y%m%d_%H%M%S");
    return ss.str();
}

std::chrono::system_clock::time_point ResearchConfigManager::getTimestamp() {
    return std::chrono::system_clock::now();
}

std::string ResearchConfigManager::formatTimestamp(const std::chrono::system_clock::time_point& tp) {
    auto time_t_now = std::chrono::system_clock::to_time_t(tp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::vector<std::string> ResearchConfigManager::getPresetNames() {
    return {"minimal", "standard", "large_scale", "developmental"};
}

bool ResearchConfigManager::isPreset(const std::string& name) {
    static const std::unordered_set<std::string> presets = {
        "minimal", "standard", "large_scale", "developmental"
    };
    return presets.find(toLower(name)) != presets.end();
}

ResearchConfig ResearchConfigManager::getPresetConfig(const std::string& name) {
    std::string lowerName = toLower(name);
    if (lowerName == "minimal") return createMinimalConfig();
    if (lowerName == "standard") return createStandardConfig();
    if (lowerName == "large_scale") return createLargeScaleConfig();
    if (lowerName == "developmental") return createDevelopmentalConfig();
    return createCustomConfig(name);
}

std::shared_ptr<ResearchConfigManager> ResearchConfigManager::clone() const {
    auto cloned = std::make_shared<ResearchConfigManager>();
    cloned->pImpl->currentConfig = pImpl->currentConfig;
    cloned->pImpl->configDirectory = pImpl->configDirectory;
    return cloned;
}

void ResearchConfigManager::initializeDefaults() {
    currentConfig.name = generateConfigName();
    currentConfig.description = "Automatically generated configuration";
    currentConfig.mode = ResearchMode::Custom;
    
    // Set defaults from Impl
    currentConfig.neuron_count = pImpl->DEFAULT_NEURON_COUNT;
    currentConfig.region_count = pImpl->DEFAULT_REGION_COUNT;
    currentConfig.simulation_timestep = pImpl->DEFAULT_SIMULATION_TIMESTEP;
    currentConfig.random_seed = pImpl->DEFAULT_RANDOM_SEED;
    currentConfig.plasticity_learning_rate = pImpl->DEFAULT_PLASTICITY_LEARNING_RATE;
    currentConfig.stdp_ltp_weight = pImpl->DEFAULT_STDP_LTP_WEIGHT;
    currentConfig.stdp_ltd_weight = pImpl->DEFAULT_STDP_LTD_WEIGHT;
    currentConfig.stdp_time_constant = pImpl->DEFAULT_STDP_TIME_CONSTANT;
    currentConfig.development_synaptogenesis_rate = pImpl->DEFAULT_DEVELOPMENT_SYNAPTOGENESIS_RATE;
    currentConfig.development_pruning_rate = pImpl->DEFAULT_DEVELOPMENT_PRUNING_RATE;
    currentConfig.dopamine_baseline = pImpl->DEFAULT_DOPAMINE_BASELINE;
    currentConfig.dopamine_release_rate = pImpl->DEFAULT_DOPAMINE_RELEASE_RATE;
    currentConfig.reward_discount_factor = pImpl->DEFAULT_REWARD_DISCOUNT_FACTOR;
    currentConfig.environment_name = pImpl->DEFAULT_ENVIRONMENT_NAME;
    currentConfig.environment_width = pImpl->DEFAULT_ENVIRONMENT_WIDTH;
    currentConfig.environment_height = pImpl->DEFAULT_ENVIRONMENT_HEIGHT;
    currentConfig.log_level = pImpl->DEFAULT_LOG_LEVEL;
    currentConfig.log_to_file = pImpl->DEFAULT_LOG_TO_FILE;
    currentConfig.log_filename = pImpl->DEFAULT_LOG_FILENAME;
    currentConfig.max_simulation_steps = pImpl->DEFAULT_MAX_SIMULATION_STEPS;
    currentConfig.simulation_time_limit = pImpl->DEFAULT_SIMULATION_TIME_LIMIT;
    currentConfig.visualization_enabled = pImpl->DEFAULT_VISUALIZATION_ENABLED;
    currentConfig.visualization_update_rate = pImpl->DEFAULT_VISUALIZATION_UPDATE_RATE;
    currentConfig.num_trials = pImpl->DEFAULT_NUM_TRIALS;
    currentConfig.num_seeds = pImpl->DEFAULT_NUM_SEEDS;
    currentConfig.max_concurrent_experiments = pImpl->DEFAULT_MAX_CONCURRENT_EXPERIMENTS;
    currentConfig.output_format = pImpl->DEFAULT_OUTPUT_FORMAT;
    currentConfig.save_checkpoints = pImpl->DEFAULT_SAVE_CHECKPOINTS;
    currentConfig.checkpoint_frequency = pImpl->DEFAULT_CHECKPOINT_FREQUENCY;
    currentConfig.enable_batch_processing = pImpl->DEFAULT_ENABLE_BATCH_PROCESSING;
    currentConfig.batch_size = pImpl->DEFAULT_BATCH_SIZE;
    currentConfig.enable_parallel_processing = pImpl->DEFAULT_ENABLE_PARALLEL_PROCESSING;
    currentConfig.parallel_workers = pImpl->DEFAULT_PARALLEL_WORKERS;
    currentConfig.enable_performance_monitoring = pImpl->DEFAULT_ENABLE_PERFORMANCE_MONITORING;
    currentConfig.enable_hardware_acceleration = pImpl->DEFAULT_ENABLE_HARDWARE_ACCELERATION;
    currentConfig.optimize_for_scale = pImpl->DEFAULT_OPTIMIZE_FOR_SCALE;
}

std::string ResearchConfigManager::sanitizeKey(const std::string& key) {
    std::string result = key;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    // Remove common prefixes
    if (result.find("config_") == 0) result.erase(0, 7);
    if (result.find("nlm_") == 0) result.erase(0, 4);
    return result;
}

bool ResearchConfigManager::isValidLogLevel(const std::string& level) {
    static const std::unordered_set<std::string> validLevels = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
    return validLevels.find(toUpper(level)) != validLevels.end();
}

bool ResearchConfigManager::isValidEnvironment(const std::string& env) {
    static const std::unordered_set<std::string> validEnvs = {"GridWorld", "SimpleWorld", "Maze", "OpenField"};
    return validEnvs.find(env) != validEnvs.end();
}

std::string ResearchConfigManager::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string ResearchConfigManager::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

// Template specializations for size_t parameter access
size_t ResearchConfigManager::getSizeParameter(const std::string& key) const {
    return std::stoul(getParameter(key));
}

double ResearchConfigManager::getDoubleParameter(const std::string& key) const {
    return std::stod(getParameter(key));
}

bool ResearchConfigManager::getBoolParameter(const std::string& key) const {
    std::string value = getParameter(key);
    return (value == "true" || value == "1" || value == "yes");
}

} // namespace nlm