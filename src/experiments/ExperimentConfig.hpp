#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "ExperimentConstants.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace nlm {

/**
 * Experiment configuration management
 * Centralizes configuration parameter handling and validation
 */
class ExperimentConfig {
public:
    ExperimentConfig() = default;
    ~ExperimentConfig() = default;
    
    /**
     * Initialize configuration from Phase 6 config parameters
     */
    bool initialize(const Phase6Config& config);
    
    /**
     * Get the internal config object
     */
    std::shared_ptr<Config> getConfig() const { return config_; }
    
    /**
     * Validate all configuration parameters
     */
    bool validate() const;
    
    /**
     * Get validation error messages
     */
    const std::vector<std::string>& getValidationErrors() const { return validationErrors_; }
    
    /**
     * Get a configuration parameter
     */
    std::string getParameter(const std::string& key) const;
    
    /**
     * Set a configuration parameter
     */
    void setParameter(const std::string& key, const std::string& value);
    
    /**
     * Check if configuration is valid
     */
    bool isValid() const { return isValid_; }
    
private:
    std::shared_ptr<Config> config_;
    bool isValid_ = false;
    std::vector<std::string> validationErrors_;
};

} // namespace nlm
