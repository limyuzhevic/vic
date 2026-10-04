// Enhanced Python bindings for NLM
// Header file for enhanced bindings functionality

#pragma once

#include <memory>
#include <string>
#include <vector>

// Forward declarations
namespace nlm {
    class Config;
    class Brain;
    class NeuralRegion;
    class Action;
    class SensoryPercept;
    class AgentBrain;
    class SimpleWorld;
    class SensoryInput;
    class Vision;
    class Audio;
    class InternalSignals;
    
    // Type definitions
    using NeuronId = uint64_t;
    using SynapseId = uint64_t;
    using RegionId = uint64_t;
    using PopulationId = uint64_t;
    
    enum class NeuronType;
    enum class SynapseType;
    enum class DevelopmentalStage;
    enum class FiringState;
    enum class ActionType;
    enum class MotorCommand;
    enum class WorldObjectType;
}

// Enhanced exception classes
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& msg) : NLMError("Validation Error: " + msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

// Forward declarations for wrapper classes
namespace nlm {
    class ConfigWrapper;
    class BrainWrapper;
}

// Helper functions namespace
namespace nlm::bindings_helpers {
    
    // Type conversion utilities
    template<typename T>
    T toValue(const py::object& obj);
    
    // Validation functions
    template<typename T>
    T validateNumber(const py::object& obj, const std::string& paramName);
    
    // Configuration utilities
    void validateKey(const nlm::Config& config, const std::string& key);
    
    // Brain utilities
    std::vector<nlm::NeuronId> getNeuronIdsInRegion(const nlm::Brain& brain, nlm::RegionId regionId);
    float computeNetworkActivity(const nlm::Brain& brain);
    
    // Statistical utilities
    float computeFiringRate(const nlm::Brain& brain, size_t steps);
    float computePlasticity(const nlm::Brain& brain);
    
    // Export/import utilities
    bool exportBrainState(const nlm::Brain& brain, const std::string& filename);
    bool importBrainState(nlm::Brain& brain, const std::string& filename);
}

// Factory functions
namespace nlm::factories {
    
    // Configuration factory functions
    std::shared_ptr<nlm::ConfigWrapper> createDefaultConfig();
    std::shared_ptr<nlm::ConfigWrapper> createConfigFromFile(const std::string& filename);
    std::shared_ptr<nlm::ConfigWrapper> createConfigFromDict(const py::dict& config_dict);
    
    // Brain factory functions
    std::shared_ptr<nlm::BrainWrapper> createBrain(std::shared_ptr<nlm::ConfigWrapper> config);
    std::shared_ptr<nlm::BrainWrapper> createBrainFromFile(const std::string& configFile, const std::string& brainFile = "");
    
    // Agent factory functions
    std::shared_ptr<nlm::AgentBrain> createAgentBrain(std::shared_ptr<nlm::BrainWrapper> brain);
    
    // World factory functions
    std::shared_ptr<nlm::SimpleWorld> createSimpleWorld(size_t width, size_t height, size_t visionWidth, size_t visionHeight);
    
    // Simulation factory functions
    std::shared_ptr<nlm::BrainWrapper> createSimulationBrain(size_t neuronCount = 1000, size_t regionCount = 2);
}

// Utility functions
namespace nlm::utils {
    
    // Configuration utilities
    std::string formatConfig(const nlm::ConfigWrapper& config);
    py::dict configToDict(const nlm::ConfigWrapper& config);
    nlm::ConfigWrapper dictToConfig(const py::dict& config_dict);
    
    // Brain utilities
    std::string brainToString(const nlm::BrainWrapper& brain);
    py::dict brainToDict(const nlm::BrainWrapper& brain);
    void logBrainState(const nlm::BrainWrapper& brain, const std::string& filename);
    
    // Validation utilities
    bool validateBrain(const nlm::BrainWrapper& brain);
    std::vector<std::string> validateConfig(const nlm::ConfigWrapper& config);
    
    // Export/import utilities
    bool exportBrain(const nlm::BrainWrapper& brain, const std::string& filename);
    bool importBrain(nlm::BrainWrapper& brain, const std::string& filename);
    
    // Statistics utilities
    py::dict computeStatistics(const nlm::BrainWrapper& brain, const std::string& type = "standard");
    float computePerformance(const nlm::BrainWrapper& brain);
    float computeLearningRate(const nlm::BrainWrapper& brain);
    
    // Batch processing utilities
    void batchProcess(nlm::BrainWrapper& brain, size_t numSteps, double timestep = 0.001);
    std::vector<py::dict> batchComputeStatistics(nlm::BrainWrapper& brain, size_t numSteps, double timestep = 0.001);
    
    // Analysis utilities
    float analyzePlasticity(const nlm::BrainWrapper& brain);
    float analyzeMemory(const nlm::BrainWrapper& brain);
    float analyzePrediction(const nlm::BrainWrapper& brain);
    
    // Simulation utilities
    void runSimulation(nlm::BrainWrapper& brain, size_t numSteps, double timestep = 0.001);
    std::vector<py::dict> runSimulationWithLogging(nlm::BrainWrapper& brain, size_t numSteps, double timestep = 0.001);
    
    // Advanced utilities
    void enableAdvancedFeatures(nlm::BrainWrapper& brain);
    void disableAdvancedFeatures(nlm::BrainWrapper& brain);
    bool isAdvancedFeaturesEnabled(const nlm::BrainWrapper& brain);
    
    // Performance monitoring utilities
    struct PerformanceMetrics {
        double wallTime;
        double cpuTime;
        size_t stepsPerSecond;
        float memoryUsage;
        float networkActivity;
        float plasticity;
        
        std::string toString() const;
        py::dict toDict() const;
    };
    
    PerformanceMetrics measurePerformance(nlm::BrainWrapper& brain, size_t numSteps = 1000);
    
    // Development utilities
    void advanceDevelopment(nlm::BrainWrapper& brain, size_t steps);
    std::string getDevelopmentalStageName(nlm::DevelopmentalStage stage);
    float getPlasticityRate(nlm::BrainWrapper& brain);
    
    // Export utilities
    bool exportToCSV(const nlm::BrainWrapper& brain, const std::string& filename, size_t steps = 100);
    bool exportToJSON(const nlm::BrainWrapper& brain, const std::string& filename, size_t steps = 100);
    bool exportToMatlab(const nlm::BrainWrapper& brain, const std::string& filename, size_t steps = 100);
    
    // Import utilities
    bool importFromCSV(nlm::BrainWrapper& brain, const std::string& filename);
    bool importFromJSON(nlm::BrainWrapper& brain, const std::string& filename);
    bool importFromMatlab(nlm::BrainWrapper& brain, const std::string& filename);
}