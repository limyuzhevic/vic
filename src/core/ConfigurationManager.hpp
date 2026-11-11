#pragma once

/**
 * Enhanced Configuration Manager for NLM
 * 
 * Provides validation, presets, and advanced configuration options for NLM brains.
 * 
 * This module extends the basic Config class with:
 * - Configuration validation with detailed error messages
 * - Preset configurations for different use cases
 * - Configuration merging and compatibility checking
 * - Advanced configuration options for research and experimentation
 * 
 * Configuration is organized into functional categories:
 * - Core brain parameters (neurons, regions, connectivity)
 * - Simulation parameters (timestep, random seed)
 * - Plasticity configuration (STDP, Hebbian, structural)
 * - Neuromodulation configuration (dopamine, curiosity, novelty)
 * - Development configuration (synaptogenesis, pruning, maturation)
 * - Memory configuration (working, episodic, associative)
 * - Cognition configuration (planning, concept formation, attention)
 * - Prediction configuration
 * - Environment configuration
 */

#include "core/Config/Config.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <limits>
#include <algorithm>

namespace nlm {

class ConfigurationManager {
public:
    ConfigurationManager();
    
    /**
     * Validate configuration and return list of errors
     * 
     * @param config Configuration to validate
     * @return Vector of error messages, empty if configuration is valid
     */
    std::vector<std::string> validateConfig(const Config& config) const;
    
    /**
     * Get all available configuration options with descriptions
     * 
     * @return Vector of key-value pairs (key = option path, value = description)
     */
    std::vector<std::pair<std::string, std::string>> getAllConfigOptions() const;
    
    /**
     * Get available preset configurations
     * 
     * @return Vector of preset names
     */
    std::vector<std::string> getAvailablePresets() const;
    
    /**
     * Get a preset configuration by name
     * 
     * @param presetName Name of the preset (e.g., "default", "fast", "accurate", "curious", "social", "developmental", "experimental")
     * @return Configuration object for the specified preset
     */
    Config getPreset(const std::string& presetName) const;
    
    /**
     * Merge two configurations (source overrides destination)
     * 
     * @param source Source configuration (takes precedence)
     * @param destination Destination configuration
     * @return Merged configuration
     */
    Config mergeConfigs(const Config& source, const Config& destination) const;
    
    /**
     * Check if a configuration is compatible with another
     * 
     * @param config1 First configuration
     * @param config2 Second configuration
     * @return true if configurations are compatible, false otherwise
     */
    bool areCompatible(const Config& config1, const Config& config2) const;
    
    /**
     * Get configuration compatibility score
     * 
     * @param config1 First configuration
     * @param config2 Second configuration
     * @return Compatibility score (0.0 = incompatible, 1.0 = identical)
     */
    float getCompatibilityScore(const Config& config1, const Config& config2) const;
    
    /**
     * Get configuration statistics
     * 
     * @param config Configuration to analyze
     * @return String containing configuration statistics
     */
    std::string getConfigurationStatistics(const Config& config) const;
    
    /**
     * Create a configuration from environment variables
     * 
     * @param prefix Prefix for environment variable names (e.g., "NLM_" for NLM_BRION_COUNT)
     * @return Configuration loaded from environment variables
     */
    static Config loadFromEnvironment(const std::string& prefix = "NLM_");
    
    /**
     * Export configuration to JSON format
     * 
     * @param config Configuration to export
     * @param indent JSON indentation (default: 2 spaces)
     * @return JSON string representation
     */
    static std::string exportToJson(const Config& config, int indent = 2);
    
    /**
     * Import configuration from JSON format
     * 
     * @param json JSON string to import
     * @return Configuration object
     */
    static Config importFromJson(const std::string& json);
    
    /**
     * Get configuration recommendations based on use case
     * 
     * @param useCase Use case description (e.g., "testing", "research", "production")
     * @return Vector of recommended configuration options
     */
    std::vector<std::pair<std::string, std::string>> getRecommendations(const std::string& useCase) const;
    
    /**
     * Validate configuration against constraints
     * 
     * @param config Configuration to validate
     * @param constraints Constraints to validate against
     * @return Vector of constraint violation messages
     */
    std::vector<std::string> validateConstraints(const Config& config, 
                                                  const std::vector<std::string>& constraints) const;

private:
    struct ValidationRange {
        float min;
        float max;
        std::string errorMessage;
    };
    
    void initializeValidationRules();
    void initializePresets();
    
    std::vector<std::pair<std::pair<std::string, double>, 
                          std::pair<std::string, double>>> validationRanges_;
};

} // namespace nlm
