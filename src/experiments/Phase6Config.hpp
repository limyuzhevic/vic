/**
 * Phase 6 Configuration System - Improved Version
 * 
 * Enhanced configuration management for NLM Phase 6 experiments.
 * Building upon the existing configuration system with additional features.
 */

#pragma once

#include "../core/Config/Config.hpp"
#include <string>
#include <memory>

namespace nlm {

/**
 * Phase 6 specific configuration options
 */
struct Phase6Config {
    // Core simulation parameters
    uint64_t maxSteps = 10000;
    size_t neuronCount = 1000;
    size_t regionCount = 1;
    float connectionProbability = 0.1f;
    float timestep = 0.001f;
    
    // Feature flags
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    bool enableNeuromodulation = true;
    bool enablePrediction = true;
    bool enableCognition = true;
    
    // Output and reporting
    std::string checkpointPath = "./nlm_checkpoint.bin";
    std::string outputPath = "";
    std::string configFile = "";
    std::string logLevel = "INFO";
    
    // Advanced options
    float rewardDecay = 0.95f;
    float noveltyThreshold = 0.5f;
    float curiosityThreshold = 0.3f;
    float learningRate = 0.01f;
    size_t explorationRate = 10; // percentage
    
    // Performance optimization
    bool parallelProcessing = false;
    bool vectorization = true;
    bool memoryPooling = true;
    
    // Debug and analysis
    bool debugMode = false;
    bool profilingEnabled = false;
    bool verboseLogging = false;
    
    // Validation and safety
    uint64_t maxNeurons = 10000;
    uint64_t maxStepsSafe = 50000;
    float minConnectionProbability = 0.01f;
    float maxConnectionProbability = 0.5f;
    
    Phase6Config() = default;
    
    /**
     * Convert to standard Config object
     */
    std::shared_ptr<Config> toConfig() const;
    
    /**
     * Load configuration from file
     */
    static Phase6Config loadFromFile(const std::string& filename);
    
    /**
     * Load configuration from command line arguments
     */
    static Phase6Config loadFromArgs(int argc, char* argv[]);
    
    /**
     * Get configuration preset name
     */
    static std::string getPresetName(const Phase6Config& config);
    
    /**
     * Apply preset to configuration
     */
    static void applyPreset(Phase6Config& config, const std::string& presetName);
    
    /**
     * Get available presets
     */
    static std::vector<std::string> getAvailablePresets();
    
    /**
     * Validate configuration
     */
    bool validate() const;
    
    /**
     * Print configuration summary
     */
    void printSummary() const;
    
    /**
     * Generate configuration string for export
     */
    std::string toString() const;
    
    /**
     * Parse configuration from string
     */
    static Phase6Config fromString(const std::string& configString);
};

} // namespace nlm
