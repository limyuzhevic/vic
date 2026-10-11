#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <mutex>
#include <chrono>
#include <map>
#include <variant>

namespace nlm {

// High-level brain interface for user-friendly API
class BrainInterface {
public:
    virtual ~BrainInterface() = default;
    
    // Core brain operations
    virtual bool initialize() = 0;
    virtual void step() = 0;
    virtual void reset() = 0;
    virtual bool isInitialized() const = 0;
    
    // State management
    virtual std::string saveState() const = 0;
    virtual bool loadState(const std::string& state) = 0;
    
    // Configuration
    virtual void setParameter(const std::string& name, const std::string& value) = 0;
    virtual std::string getParameter(const std::string& name) const = 0;
    
    // Sensors and actuators
    virtual void injectSensoryInput(const std::vector<float>& input) = 0;
    virtual std::vector<float> getMotorOutput() const = 0;
    
    // Monitoring
    virtual float getAverageFiringRate() const = 0;
    virtual size_t getActiveNeuronCount() const = 0;
    virtual size_t getTotalNeuronCount() const = 0;
    virtual size_t getFiringNeuronCount() const = 0;
    virtual std::string getBrainStatus() const = 0;
};

// Simple brain interface for rapid prototyping
class SimpleBrainInterface : public BrainInterface {
public:
    SimpleBrainInterface();
    ~SimpleBrainInterface() override;
    
    // BrainInterface implementation
    bool initialize() override;
    void step() override;
    void reset() override;
    bool isInitialized() const override;
    
    std::string saveState() const override;
    bool loadState(const std::string& state) override;
    
    void setParameter(const std::string& name, const std::string& value) override;
    std::string getParameter(const std::string& name) const override;
    
    void injectSensoryInput(const std::vector<float>& input) override;
    std::vector<float> getMotorOutput() const override;
    
    float getAverageFiringRate() const override;
    size_t getActiveNeuronCount() const override;
    size_t getTotalNeuronCount() const override;
    size_t getFiringNeuronCount() const override;
    std::string getBrainStatus() const override;
    
    // Simple convenience methods
    void runExperiment(const std::string& experimentType, uint64_t steps);
    void runLearningExperiment(uint64_t steps);
    void runAdaptationExperiment(uint64_t steps);
    void runSocialExperiment(uint64_t steps);
    void runMemoryExperiment(uint64_t steps);
    
    // Machine learning interface
    void train(const std::vector<std::vector<float>>& inputs,
              const std::vector<std::vector<float>>& targets,
              uint64_t epochs,
              float learningRate = 0.01f);
    
    std::vector<std::vector<float>> predict(const std::vector<std::vector<float>>& inputs) const;
    
    // Neuroscience interface
    void setLearningRate(float rate);
    void setConnectionStrength(uint64_t source, uint64_t target, float strength);
    float getConnectionStrength(uint64_t source, uint64_t target) const;
    void enablePlasticity(bool enable);
    void enableRewardLearning(bool enable);
    void enableCuriosity(bool enable);
    
    // Environment interface
    void setEnvironment(const std::string& environmentType);
    std::string getEnvironmentType() const;
    
    // Visualization interface
    void enableVisualization(bool enable);
    void setVisualizationFormat(const std::string& format);
    std::string captureScreenshot() const;
    
    // Data interface
    void exportData(const std::string& filepath, const std::string& format = "csv") const;
    void importData(const std::string& filepath, const std::string& format = "csv");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced brain interface for complex applications
class AdvancedBrainInterface : public BrainInterface {
public:
    AdvancedBrainInterface();
    ~AdvancedBrainInterface() override;
    
    // BrainInterface implementation (with advanced features)
    bool initialize() override;
    void step() override;
    void reset() override;
    bool isInitialized() const override;
    
    std::string saveState() const override;
    bool loadState(const std::string& state) override;
    
    void setParameter(const std::string& name, const std::string& value) override;
    std::string getParameter(const std::string& name) const override;
    
    void injectSensoryInput(const std::vector<float>& input) override;
    std::vector<float> getMotorOutput() const override;
    
    float getAverageFiringRate() const override;
    size_t getActiveNeuronCount() const override;
    size_t getTotalNeuronCount() const override;
    size_t getFiringNeuronCount() const override;
    std::string getBrainStatus() const override;
    
    // Advanced neuroscience operations
    void enableAllSystems(bool enable);
    void configureNeuromodulation(const std::string& type, float level);
    void configureMemorySystem(const std::string& type, bool enable);
    void configureCognitionSystem(const std::string& type, bool enable);
    
    // Advanced learning operations
    void trainWithReinforcement(const std::vector<float>& state,
                              int action,
                              float reward,
                              const std::vector<float>& nextState,
                              bool done);
    
    std::vector<float> getValueEstimate(const std::vector<float>& state) const;
    void updatePolicy(const std::vector<float>& state, int action, float advantage);
    
    // Advanced simulation control
    void pauseSimulation();
    void resumeSimulation();
    bool isSimulationPaused() const;
    void setSimulationSpeed(float speed);
    float getSimulationSpeed() const;
    
    // Advanced monitoring and analysis
    std::string getDetailedStatus() const;
    std::string getPerformanceMetrics() const;
    std::string getSystemHealth() const;
    
    // Advanced export/import
    bool exportBrain(const std::string& filepath) const;
    bool importBrain(const std::string& filepath);
    bool exportModel(const std::string& filepath) const;
    bool importModel(const std::string& filepath);
    
    // Advanced configuration
    void loadConfiguration(const std::string& filepath);
    void saveConfiguration(const std::string& filepath) const;
    void applyConfigurationProfile(const std::string& profile);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// High-level API functions for easy usage
namespace api {
    // Global brain instances
    BrainInterface& getSimpleBrain();
    BrainInterface& getAdvancedBrain();
    
    // Convenience functions
    bool initializeAllBrains();
    void shutdownAllBrains();
    bool isAnyBrainInitialized();
    
    // Configuration management
    void loadDefaultConfiguration();
    void loadConfigurationFromFile(const std::string& filepath);
    void saveConfigurationToFile(const std::string& filepath);
    
    // Experiment management
    void runExperiment(const std::string& experimentType, uint64_t steps);
    void runAllExperiments(uint64_t steps);
    void stopAllExperiments();
    
    // Data management
    void exportAllData(const std::string& directory, const std::string& format = "csv");
    void importAllData(const std::string& directory, const std::string& format = "csv");
    
    // Monitoring and analysis
    std::string getSystemOverview();
    std::string getPerformanceReport();
    std::string getHealthReport();
    std::string getExperimentReport();
    
    // Utility functions
    std::string generateReport(const std::string& type = "comprehensive");
    bool validateEnvironment();
    std::vector<std::string> getAvailableExperimentTypes();
    std::vector<std::string> getAvailableBrainTypes();
}

// Convenience macros for common operations
#define NLM_INIT_BRAIN brain = &nlm::api::getSimpleBrain(); brain->initialize()
#define NLM_RUN_EXPERIMENT(type, steps) nlm::api::runExperiment(type, steps)
#define NLM_SHUTDOWN_BRAIN nlm::api::shutdownAllBrains()

#define NLM_TRAIN_SUPERVISED(inputs, targets, epochs) brain->train(inputs, targets, epochs)
#define NLM_PREDICT(inputs) brain->predict(inputs)

#define NLM_SET_ENV(env) brain->setEnvironment(env)
#define NLM_GET_STATUS() brain->getBrainStatus()

} // namespace nlm
