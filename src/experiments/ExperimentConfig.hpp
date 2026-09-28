#pragma once

#include "../core/Config/Config.hpp"
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <mutex>
#include <stdexcept>

namespace nlm {

/**
 * @brief Configuration for NLM experiments
 * 
 * This class provides a comprehensive configuration system for experiments
 * including parameter validation, serialization support, and error handling.
 */
class ExperimentConfig {
public:
    // Constructor and destructor
    ExperimentConfig();
    explicit ExperimentConfig(const std::string& name);
    ~ExperimentConfig();
    
    // Copy control
    ExperimentConfig(const ExperimentConfig& other);
    ExperimentConfig& operator=(const ExperimentConfig& other);
    ExperimentConfig(ExperimentConfig&& other) noexcept;
    ExperimentConfig& operator=(ExperimentConfig&& other) noexcept;
    
    // Basic configuration properties
    const std::string& getName() const;
    void setName(const std::string& name);
    
    uint64_t getMaxSteps() const;
    void setMaxSteps(uint64_t steps);
    
    size_t getNeuronCount() const;
    void setNeuronCount(size_t count);
    
    size_t getRegionCount() const;
    void setRegionCount(size_t count);
    
    float getConnectionProbability() const;
    void setConnectionProbability(float probability);
    
    // Optional configuration parameters
    bool getEnableCheckpointing() const;
    void setEnableCheckpointing(bool enable);
    
    bool getEnableReplay() const;
    void setEnableReplay(bool enable);
    
    bool getEnableDevelopment() const;
    void setEnableDevelopment(bool enable);
    
    const std::string& getCheckpointPath() const;
    void setCheckpointPath(const std::string& path);
    
    // Random seed management
    uint64_t getSeed() const;
    void setSeed(uint64_t seed);
    
    // Configuration validation
    bool validate() const;
    std::string validateAndGetError() const;
    
    // Serialization support
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);
    
    // Configuration merging
    void mergeWith(const ExperimentConfig& other);
    
    // Configuration comparison
    bool equals(const ExperimentConfig& other) const;
    
    // Convenience methods for common configurations
    static ExperimentConfig createQuickTestConfig();
    static ExperimentConfig createTrainingConfig();
    static ExperimentConfig createBenchmarkConfig();
    static ExperimentConfig createAblationConfig();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal validation helpers
    void validateParameters() const;
    void validateCrossParameterConstraints() const;
};

} // namespace nlm
