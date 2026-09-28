#pragma once

/**
 * Advanced Simulation Modes and Presets
 * 
 * Provides advanced simulation modes, presets, and configuration templates
 * for power users and advanced use cases.
 * 
 * Features:
 * - Multiple simulation presets (learning, exploration, development, etc.)
 * - Advanced simulation modes with specialized configurations
 * - Custom simulation templates
 * - Dynamic configuration adjustment during simulation
 * - Performance profiling and optimization presets
 * - Memory management presets
 * - Time management presets
 * - Network topology presets
 * - Integration with external systems
 * - Batch simulation presets
 */

#include "../core/Config/Config.hpp"
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

// Simulation presets for different use cases
enum class SimulationPreset {
    // Learning-focused presets
    LearningConverge,    // Fast convergence with strong learning
    LearningExplore,     // Balanced learning and exploration
    LearningStable,      // Stable, conservative learning
    
    // Exploration-focused presets
    ExplorationCuriosity, // High curiosity and exploration
    ExplorationNovelty,   // Novelty-driven exploration
    ExplorationRandom,    // Random exploration
    
    // Development-focused presets
    DevelopmentRapid,    // Rapid development
    DevelopmentBalanced, // Balanced development
    DevelopmentMature,    // Mature, stable development
    
    // Performance-focused presets
    PerformanceFast,     // Fast execution
    PerformanceQuality,   // High quality, thorough simulation
    PerformanceBalanced,  // Balanced performance
    
    // Resource-focused presets
    MemoryEfficient,     // Low memory usage
    CPUEfficient,        // Low CPU usage
    BalancedResource,    // Balanced resource usage
    
    // Custom presets
    Custom
};

// Advanced simulation modes
enum class SimulationMode {
    Standard,           // Standard simulation
    HighPrecision,       // High precision mode
    RealTime,           // Real-time mode
    BatchProcessing,    // Batch processing mode
    Parallel,          // Parallel processing mode
    Streaming,         // Streaming mode
    EventDriven,        // Event-driven mode
    Adaptive,          // Adaptive mode (adjusts based on progress)
    Learning,          // Learning-focused mode
    Development,       // Development-focused mode
    Exploration       // Exploration-focused mode
};

// Simulation template for custom configurations
struct SimulationTemplate {
    std::string name;                           // Template name
    std::string description;                    // Template description
    SimulationPreset preset;                   // Base preset
    std::map<std::string, std::string> parameters; // Custom parameters
    std::vector<std::string> enabledFeatures;   // Enabled features
    double estimatedTime;                       // Estimated simulation time
    size_t estimatedMemory;                     // Estimated memory usage (MB)
    std::function<void(std::shared_ptr<Config>)> configurationCallback; // Custom configuration
    
    SimulationTemplate(const std::string& name, const std::string& desc,
                      SimulationPreset preset) 
        : name(name), description(desc), preset(preset), 
          estimatedTime(60.0), estimatedMemory(512) {}
};

// Advanced simulation configuration
struct AdvancedSimulationConfig {
    SimulationMode mode;                         // Simulation mode
    SimulationPreset preset;                    // Simulation preset
    double timeStep;                             // Time step size
    size_t maxSteps;                             // Maximum simulation steps
    double timeLimit;                            // Time limit (seconds)
    size_t memoryLimit;                          // Memory limit (MB)
    bool enableProfiling;                        // Enable profiling
    bool enableLogging;                          // Enable detailed logging
    bool enableCheckpointing;                    // Enable checkpointing
    std::string checkpointPath;                  // Checkpoint path
    double checkpointInterval;                   // Checkpoint interval (steps)
    bool adaptiveTimeStep;                       // Adaptive time step
    double timeStepMin;                          // Minimum time step
    double timeStepMax;                          // Maximum time step
    std::map<std::string, double> parameterLimits; // Parameter limits
    std::function<void(double, double)> progressCallback; // Progress callback
    std::function<void(const std::string&)> statusCallback; // Status callback
    std::map<std::string, std::string> customSettings; // Custom settings
    
    AdvancedSimulationConfig() : mode(SimulationMode::Standard), preset(SimulationPreset::LearningConverge),
                                timeStep(0.001), maxSteps(10000), timeLimit(0.0), memoryLimit(1024),
                                enableProfiling(false), enableLogging(false), enableCheckpointing(false),
                                checkpointInterval(1000.0), adaptiveTimeStep(false), timeStepMin(0.0001),
                                timeStepMax(0.1) {}
};

// Performance optimization preset
struct PerformancePreset {
    std::string name;                           // Preset name
    std::string description;                    // Preset description
    double cpuPriority;                         // CPU priority (0-1)
    double memoryPriority;                      // Memory priority (0-1)
    double speedPriority;                        // Speed priority (0-1)
    bool enableOptimization;                   // Enable optimization
    std::vector<std::string> optimizationTargets; // Optimization targets
    std::map<std::string, double> optimizationParameters; // Optimization parameters
    
    PerformancePreset(const std::string& name, const std::string& desc,
                     double cpuP, double memP, double speedP)
        : name(name), description(desc), cpuPriority(cpuP), memoryPriority(memP),
          speedPriority(speedP), enableOptimization(true) {}
};

// Batch simulation configuration
struct BatchSimulationConfig {
    std::string batchName;                       // Batch name
    size_t numSimulations;                       // Number of simulations
    std::vector<SimulationPreset> presets;       // Presets for each simulation
    std::vector<std::string> parameterSets;      // Parameter sets for each simulation
    std::function<void(size_t, size_t)> progressCallback; // Progress callback
    std::function<void(const std::string&, const std::string&)> resultCallback; // Result callback
    bool enableParallel;                         // Enable parallel execution
    size_t maxParallelSimulations;                // Maximum parallel simulations
    double timeLimitPerSimulation;               // Time limit per simulation
    std::string outputDirectory;                 // Output directory
    bool saveResults;                            // Save results to files
    
    BatchSimulationConfig() : numSimulations(1), enableParallel(true),
                             maxParallelSimulations(4), timeLimitPerSimulation(3600.0),
                             saveResults(true) {}
};

// Simulation preset manager
class SimulationPresetManager {
public:
    static SimulationPresetManager& getInstance() {
        static SimulationPresetManager instance;
        return instance;
    }
    
    // Preset management
    void addPreset(SimulationPreset preset, const std::string& name, const std::string& description,
                   const std::map<std::string, std::string>& parameters);
    void removePreset(SimulationPreset preset);
    void updatePreset(SimulationPreset preset, const std::map<std::string, std::string>& parameters);
    const std::map<std::string, std::string>& getPresetParameters(SimulationPreset preset) const;
    std::string getPresetName(SimulationPreset preset) const;
    std::string getPresetDescription(SimulationPreset preset) const;
    
    // Preset application
    void applyPreset(std::shared_ptr<Config> config, SimulationPreset preset);
    void applyAdvancedPreset(std::shared_ptr<Config> config, SimulationPreset preset,
                            const AdvancedSimulationConfig& advancedConfig);
    
    // Preset search
    std::vector<SimulationPreset> searchPresets(const std::string& keyword) const;
    std::vector<SimulationPreset> getSimilarPresets(SimulationPreset preset, float similarity) const;
    
    // Preset comparison
    std::string comparePresets(SimulationPreset preset1, SimulationPreset preset2) const;
    
    // Batch preset operations
    void createBatchPreset(const std::string& name, const std::vector<SimulationPreset>& presets);
    void applyBatchPreset(std::shared_ptr<Config> config, const std::string& batchName);
    
    // Preset validation
    bool validatePreset(SimulationPreset preset) const;
    std::vector<std::string> getPresetIssues(SimulationPreset preset) const;
    
    // Preset export/import
    bool exportPreset(SimulationPreset preset, const std::string& filepath);
    bool importPreset(const std::string& filepath, SimulationPreset& preset);
    void exportAllPresets(const std::string& directory);
    bool importAllPresets(const std::string& directory);
    
    // Preset statistics
    size_t getPresetCount() const { return presets_.size(); }
    std::vector<SimulationPreset> getAllPresets() const;
    std::map<SimulationPreset, std::string> getPresetStats() const;
    
private:
    SimulationPresetManager() = default;
    ~SimulationPresetManager() = default;
    
    SimulationPresetManager(const SimulationPresetManager&) = delete;
    SimulationPresetManager& operator=(const SimulationPresetManager&) = delete;
    
    struct PresetInfo {
        SimulationPreset preset;
        std::string name;
        std::string description;
        std::map<std::string, std::string> parameters;
        double creationTime;
        std::string creator;
        size_t usageCount;
        double averageRating;
        
        PresetInfo(SimulationPreset p, const std::string& n, const std::string& desc,
                  const std::map<std::string, std::string>& params)
            : preset(p), name(n), description(desc), parameters(params),
              creationTime(0.0), creator(""), usageCount(0), averageRating(0.0) {}
    };
    
    std::vector<PresetInfo> presets_;
    std::map<std::string, std::vector<SimulationPreset>> presetCategories_;
};

// Advanced simulation controller
class AdvancedSimulationController {
public:
    static AdvancedSimulationController& getInstance() {
        static AdvancedSimulationController instance;
        return instance;
    }
    
    // Simulation mode management
    void setSimulationMode(SimulationMode mode);
    SimulationMode getSimulationMode() const { return currentMode_; }
    bool isSimulationModeEnabled(SimulationMode mode) const { return currentMode_ == mode; }
    
    // Advanced configuration management
    void setAdvancedConfig(const AdvancedSimulationConfig& config);
    AdvancedSimulationConfig getAdvancedConfig() const { return advancedConfig_; }
    void updateAdvancedConfig(const std::map<std::string, double>& updates);
    
    // Performance optimization
    void setPerformancePreset(const PerformancePreset& preset);
    PerformancePreset getCurrentPerformancePreset() const { return currentPerformancePreset_; }
    void optimizePerformance();
    
    // Simulation control
    void startSimulation(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                        std::shared_ptr<AgentBrain> agent);
    void pauseSimulation();
    void resumeSimulation();
    void stopSimulation();
    bool isSimulationRunning() const { return simulationRunning_; }
    
    // Dynamic configuration
    void adjustConfigurationDuringSimulation(const std::map<std::string, double>& adjustments);
    void applySimulationPreset(SimulationPreset preset);
    void applyPerformancePreset(const PerformancePreset& preset);
    
    // Progress tracking
    double getSimulationProgress() const;
    double getEstimatedTimeRemaining() const;
    std::string getCurrentStatus() const;
    
    // Event handling
    void addSimulationEvent(const std::string& eventType, std::function<void()> callback);
    void removeSimulationEvent(const std::string& eventType);
    void triggerSimulationEvent(const std::string& eventType);
    
    // Performance monitoring
    double getCurrentCPUUsage() const;
    double getCurrentMemoryUsage() const;
    double getCurrentFiringRate() const;
    size_t getCurrentStepCount() const { return currentStep_; }
    double getCurrentSimulationTime() const { return currentSimulationTime_; }
    
    // Batch simulation
    void startBatchSimulation(const BatchSimulationConfig& config);
    void pauseBatchSimulation();
    void resumeBatchSimulation();
    void stopBatchSimulation();
    bool isBatchSimulationRunning() const { return batchSimulationRunning_; }
    
    // Result management
    std::vector<std::string> getBatchResults() const;
    std::string getSimulationResult(const std::string& simulationId) const;
    
    // Configuration export
    std::string exportCurrentConfiguration() const;
    bool importConfiguration(const std::string& configString);
    
private:
    AdvancedSimulationController() : simulationRunning_(false), batchSimulationRunning_(false),
                                    currentStep_(0), currentSimulationTime_(0.0) {}
    ~AdvancedSimulationController() = default;
    
    AdvancedSimulationController(const AdvancedSimulationController&) = delete;
    AdvancedSimulationController& operator=(const AdvancedSimulationController&) = delete;
    
    void simulationLoop();
    void updatePerformanceMetrics();
    void checkProgress();
    void saveProgress();
    void restoreProgress();
    void handleExternalEvents();
    
    SimulationMode currentMode_;
    AdvancedSimulationConfig advancedConfig_;
    PerformancePreset currentPerformancePreset_;
    bool simulationRunning_;
    bool batchSimulationRunning_;
    size_t currentStep_;
    double currentSimulationTime_;
    std::map<std::string, std::function<void()>> simulationEvents_;
    std::vector<std::string> batchResults_;
};

// Simulation template manager
class SimulationTemplateManager {
public:
    static SimulationTemplateManager& getInstance() {
        static SimulationTemplateManager instance;
        return instance;
    }
    
    // Template management
    void addTemplate(const SimulationTemplate& template);
    void removeTemplate(const std::string& name);
    void updateTemplate(const std::string& name, const SimulationTemplate& template);
    const SimulationTemplate* getTemplate(const std::string& name) const;
    
    // Template search
    std::vector<SimulationTemplate> searchTemplates(const std::string& keyword) const;
    std::vector<SimulationTemplate> getTemplatesByCategory(const std::string& category) const;
    std::vector<SimulationTemplate> getSimilarTemplates(const SimulationTemplate& template,
                                                       float similarity) const;
    
    // Template application
    std::shared_ptr<Config> applyTemplate(const std::string& name,
                                         const std::map<std::string, std::string>& parameters = {});
    void applyTemplateToConfig(std::shared_ptr<Config> config, const std::string& name,
                              const std::map<std::string, std::string>& parameters = {});
    
    // Template categories
    void addTemplateCategory(const std::string& name, const std::vector<std::string>& templateNames);
    void removeTemplateCategory(const std::string& name);
    std::vector<std::string> getTemplateCategories() const;
    std::vector<std::string> getTemplatesInCategory(const std::string& category) const;
    
    // Template export/import
    bool exportTemplate(const std::string& name, const std::string& filepath);
    bool importTemplate(const std::string& filepath);
    void exportAllTemplates(const std::string& directory);
    bool importAllTemplates(const std::string& directory);
    
    // Template statistics
    size_t getTemplateCount() const { return templates_.size(); }
    std::vector<SimulationTemplate> getAllTemplates() const;
    std::map<std::string, std::string> getTemplateStats(const std::string& name) const;
    
private:
    SimulationTemplateManager() = default;
    ~SimulationTemplateManager() = default;
    
    SimulationTemplateManager(const SimulationTemplateManager&) = delete;
    SimulationTemplateManager& operator=(const SimulationTemplateManager&) = delete;
    
    struct TemplateInfo {
        SimulationTemplate template;
        std::string category;
        double creationTime;
        std::string creator;
        size_t usageCount;
        double averageRating;
        
        TemplateInfo(const SimulationTemplate& t, const std::string& cat)
            : template(t), category(cat), creationTime(0.0), creator(""), usageCount(0), averageRating(0.0) {}
    };
    
    std::vector<TemplateInfo> templates_;
    std::map<std::string, std::vector<std::string>> templateCategories_;
};

// Convenience functions
inline void applyLearningPreset(std::shared_ptr<Config> config) {
    SimulationPresetManager::getInstance().applyPreset(config, SimulationPreset::LearningConverge);
}

inline void applyExplorationPreset(std::shared_ptr<Config> config) {
    SimulationPresetManager::getInstance().applyPreset(config, SimulationPreset::ExplorationCuriosity);
}

inline void applyDevelopmentPreset(std::shared_ptr<Config> config) {
    SimulationPresetManager::getInstance().applyPreset(config, SimulationPreset::DevelopmentBalanced);
}

inline void applyPerformancePreset(std::shared_ptr<Config> config, const std::string& presetName) {
    // Find preset by name and apply
    for (const auto& presetInfo : SimulationPresetManager::getInstance().getAllPresets()) {
        if (presetInfo.second == presetName) {
            SimulationPresetManager::getInstance().applyPreset(config, presetInfo.first);
            break;
        }
    }
}

inline AdvancedSimulationConfig createAdvancedConfig(SimulationMode mode, SimulationPreset preset) {
    AdvancedSimulationConfig config;
    config.mode = mode;
    config.preset = preset;
    return config;
}

inline PerformancePreset createPerformancePreset(const std::string& name, const std::string& desc,
                                                 double cpuP, double memP, double speedP) {
    return PerformancePreset(name, desc, cpuP, memP, speedP);
}

inline BatchSimulationConfig createBatchConfig(const std::string& name, size_t numSimulations) {
    BatchSimulationConfig config;
    config.batchName = name;
    config.numSimulations = numSimulations;
    return config;
}

} // namespace nlm