#include "ExperimentConfig.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

bool ExperimentConfig::initialize(const Phase6Config& config) {
    try {
        config_ = std::make_shared<Config>();
        
        // Set configuration parameters
        config_->set("neuron_count", config.neuronCount);
        config_->set("region_count", config.regionCount);
        config_->set("connection_probability", config.connectionProbability);
        config_->set("max_steps", static_cast<int64_t>(config.maxSteps));
        config_->set("enable_checkpointing", config.enableCheckpointing);
        config_->set("enable_replay", config.enableReplay);
        config_->set("enable_development", config.enableDevelopment);
        config_->set("checkpoint_path", config.checkpointPath);
        
        // Set brain constants
        config_->set("stdp_ltp_weight", ExperimentConstants::DEFAULT_STDP_LTP_WEIGHT);
        config_->set("stdp_ltd_weight", ExperimentConstants::DEFAULT_STDP_LTD_WEIGHT);
        config_->set("synaptogenesis_rate", ExperimentConstants::DEFAULT_SYNAPTOGENESIS_RATE);
        config_->set("pruning_rate", ExperimentConstants::DEFAULT_PRUNING_RATE);
        
        // Validate configuration
        if (!validate()) {
            NLM_LOG_ERROR("Configuration validation failed");
            for (const auto& error : validationErrors_) {
                NLM_LOG_ERROR(error);
            }
            return false;
        }
        
        isValid_ = true;
        NLM_LOG_INFO("Experiment configuration initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize configuration: " + std::string(e.what()));
        return false;
    }
}

bool ExperimentConfig::validate() const {
    validationErrors_.clear();
    
    if (!config_) {
        validationErrors_.push_back("Config object not initialized");
        return false;
    }
    
    // Validate neuron count
    auto neuronCount = config_->get<int>("neuron_count");
    if (neuronCount < ExperimentConstants::MIN_NEURONS_FOR_MEMORY) {
        validationErrors_.push_back("Neuron count below minimum threshold for memory systems");
    }
    
    // Validate region count
    auto regionCount = config_->get<int>("region_count");
    if (regionCount <= 0) {
        validationErrors_.push_back("Region count must be positive");
    }
    
    // Validate connection probability
    auto connectionProb = config_->get<float>("connection_probability");
    if (connectionProb < 0.0f || connectionProb > 1.0f) {
        validationErrors_.push_back("Connection probability must be between 0.0 and 1.0");
    }
    
    // Validate max steps
    auto maxSteps = config_->get<int64_t>("max_steps");
    if (maxSteps <= 0) {
        validationErrors_.push_back("Max steps must be positive");
    }
    
    // Validate checkpoint path
    auto checkpointPath = config_->get<std::string>("checkpoint_path");
    if (checkpointPath.empty()) {
        validationErrors_.push_back("Checkpoint path cannot be empty");
    }
    
    return validationErrors_.empty();
}

std::string ExperimentConfig::getParameter(const std::string& key) const {
    if (!config_) return "";
    
    try {
        if (config_->has<int>(key)) {
            return std::to_string(config_->get<int>(key));
        } else if (config_->has<float>(key)) {
            return std::to_string(config_->get<float>(key));
        } else if (config_->has<int64_t>(key)) {
            return std::to_string(config_->get<int64_t>(key));
        } else if (config_->has<std::string>(key)) {
            return config_->get<std::string>(key);
        } else if (config_->has<bool>(key)) {
            return config_->get<bool>(key) ? "true" : "false";
        }
    } catch (const std::exception&) {}
    
    return "";
}

void ExperimentConfig::setParameter(const std::string& key, const std::string& value) {
    if (!config_) {
        NLM_LOG_ERROR("Cannot set parameter: config not initialized");
        return;
    }
    
    try {
        // Try to parse as boolean first
        if (value == "true" || value == "false") {
            config_->set(key, value == "true");
            return;
        }
        
        // Try to parse as integer
        try {
            size_t pos;
            int intVal = std::stoi(value, &pos);
            if (pos == value.length()) {
                config_->set(key, intVal);
                return;
            }
        } catch (...) {}
        
        // Try to parse as float
        try {
            size_t pos;
            float floatVal = std::stof(value, &pos);
            if (pos == value.length()) {
                config_->set(key, floatVal);
                return;
            }
        } catch (...) {}
        
        // Treat as string
        config_->set(key, value);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to set parameter " + key + ": " + std::string(e.what()));
    }
}

} // namespace nlm
