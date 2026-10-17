#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

// Advanced debugging and monitoring options
    class DebugOptions {
    public:
        bool enableDebugLogging;
        bool enablePerformanceMonitoring;
        bool enableMemoryTracking;
        bool enableNeuriteTracing;
        bool enableSpikeAnalysis;
        bool enablePlasticityAnalysis;
        bool enableNeuromodulationAnalysis;
        std::string logFilePath;
        int debugLevel;  // 0=Minimal, 1=Normal, 2=Verbose, 3=Debug
        
        DebugOptions() : enableDebugLogging(false), enablePerformanceMonitoring(false),
                        enableMemoryTracking(false), enableNeuriteTracing(false),
                        enableSpikeAnalysis(false), enablePlasticityAnalysis(false),
                        enableNeuromodulationAnalysis(false), logFilePath(""), debugLevel(1) {}
    };
    
    // Advanced brain architecture options
    class BrainArchitectureOptions {
    public:
        size_t customNeuronCount;
        size_t customRegionCount;
        std::vector<std::string> neuronTypes;
        std::vector<std::string> populationTypes;
        bool enableCustomConnectivity;
        float customConnectionProbability;
        std::string customTopology;
        
        BrainArchitectureOptions() : customNeuronCount(0), customRegionCount(0),
                                   enableCustomConnectivity(false), customConnectionProbability(0.1f),
                                   customTopology("random") {}
    };
    
    // Advanced plasticity options
    class PlasticityOptions {
    public:
        bool enableSTDP;
        bool enableHebbian;
        bool enableRewardModulated;
        bool enableHomeostatic;
        float stdpLearningRate;
        float hebbianLearningRate;
        float homeostaticTarget;
        float homeostaticTimeConstant;
        float neuromodulationFactor;
        
        PlasticityOptions() : enableSTDP(true), enableHebbian(true), enableRewardModulated(true),
                            enableHomeostatic(true), stdpLearningRate(0.01f), hebbianLearningRate(0.02f),
                            homeostaticTarget(1.0f), homeostaticTimeConstant(100.0f), neuromodulationFactor(1.0f) {}
    };
    
    // Advanced neuromodulation options
    class NeuromodulationOptions {
    public:
        bool enableDopamine;
        bool enableAcetylcholine;
        bool enableNorepinephrine;
        bool enableSerotonin;
        bool enableCuriosity;
        bool enableNovelty;
        bool enablePredictionError;
        
        // Neuromodulator parameters
        float dopamineBaseline;
        float acetylcholineBaseline;
        float norepinephrineBaseline;
        float serotoninBaseline;
        float curiosityBaseline;
        float noveltyBaseline;
        float predictionErrorBaseline;
        
        NeuromodulationOptions() : enableDopamine(true), enableAcetylcholine(true), 
                                 enableNorepinephrine(true), enableSerotonin(true),
                                 enableCuriosity(true), enableNovelty(true), enablePredictionError(true),
                                 dopamineBaseline(0.1f), acetylcholineBaseline(0.1f),
                                 norepinephrineBaseline(0.1f), serotoninBaseline(0.1f),
                                 curiosityBaseline(0.1f), noveltyBaseline(0.1f), predictionErrorBaseline(0.1f) {}
    };
    
    // Advanced memory system options
    class MemoryOptions {
    public:
        // Working memory options
        bool enableWorkingMemory;
        size_t workingMemoryCapacity;
        float workingMemoryDecayRate;
        float workingMemoryConsolidationThreshold;
        bool enableWorkingMemoryReplay;
        
        // Episodic memory options
        bool enableEpisodicMemory;
        size_t maxEpisodicEpisodes;
        float episodicMemoryConsolidationRate;
        bool enableEpisodicMemoryReplay;
        
        // Associative memory options
        bool enableAssociativeMemory;
        float associativeMemoryStrength;
        bool enableAssociativeMemoryReplay;
        
        MemoryOptions() : enableWorkingMemory(true), workingMemoryCapacity(1000), 
                         workingMemoryDecayRate(0.01f), workingMemoryConsolidationThreshold(0.5f),
                         enableWorkingMemoryReplay(true), enableEpisodicMemory(true),
                         maxEpisodicEpisodes(10000), episodicMemoryConsolidationRate(0.1f),
                         enableEpisodicMemoryReplay(true), enableAssociativeMemory(true),
                         associativeMemoryStrength(0.8f), enableAssociativeMemoryReplay(true) {}
    };
    
    // Advanced simulation options
    class SimulationOptions {
    public:
        double timeStep;
        bool enableParallelProcessing;
        bool enableSIMD;
        bool enableEventDriven;
        std::string integrationMethod;  // "fixed", "adaptive", "adaptive_timestep"
        double stabilityCheckInterval;
        double energyBudget;
        
        SimulationOptions() : timeStep(0.001), enableParallelProcessing(true), enableSIMD(true),
                             enableEventDriven(true), integrationMethod("fixed"), 
                             stabilityCheckInterval(100.0), energyBudget(1e12) {}
    };
    
    // Performance optimization options
    class PerformanceOptions {
    public:
        bool enableMemoryPools;
        bool enableEventQueues;
        bool enableCacheFriendly;
        bool enableThreading;
        size_t maxThreads;
        size_t cpuAffinity;
        bool enableProfiling;
        
        PerformanceOptions() : enableMemoryPools(true), enableEventQueues(true), enableCacheFriendly(true),
                              enableThreading(true), maxThreads(4), cpuAffinity(0), enableProfiling(false) {}
    };
    
    // Expert mode options
    class ExpertOptions {
    public:
        bool enableExpertMode;
        bool enableExperimentalFeatures;
        bool enableBetaFeatures;
        bool enableDebugModes;
        std::string customScriptPath;
        bool enableCustomPlugins;
        bool enablePerformanceOverrides;
        
        ExpertOptions() : enableExpertMode(true), enableExperimentalFeatures(true), enableBetaFeatures(true),
                         enableDebugModes(true), customScriptPath(""), enableCustomPlugins(false),
                         enablePerformanceOverrides(false) {}
    };

    // ========== ADVANCED CONFIGURATION OPTIONS ==========

    // Debug and monitoring
    const DebugOptions& getDebugOptions() const { return debugOptions_; }
    DebugOptions& getDebugOptions() { return debugOptions_; }
    
    // Brain architecture
    const BrainArchitectureOptions& getBrainArchitectureOptions() const { return brainArchOptions_; }
    BrainArchitectureOptions& getBrainArchitectureOptions() { return brainArchOptions_; }
    
    // Plasticity
    const PlasticityOptions& getPlasticityOptions() const { return plasticityOptions_; }
    PlasticityOptions& getPlasticityOptions() { return plasticityOptions_; }
    
    // Neuromodulation
    const NeuromodulationOptions& getNeuromodulationOptions() const { return neuromodOptions_; }
    NeuromodulationOptions& getNeuromodulationOptions() { return neuromodOptions_; }
    
    // Memory systems
    const MemoryOptions& getMemoryOptions() const { return memoryOptions_; }
    MemoryOptions& getMemoryOptions() { return memoryOptions_; }
    
    // Simulation
    const SimulationOptions& getSimulationOptions() const { return simulationOptions_; }
    SimulationOptions& getSimulationOptions() { return simulationOptions_; }
    
    // Performance
    const PerformanceOptions& getPerformanceOptions() const { return performanceOptions_; }
    PerformanceOptions& getPerformanceOptions() { return performanceOptions_; }
    
    // Expert mode
    const ExpertOptions& getExpertOptions() const { return expertOptions_; }
    ExpertOptions& getExpertOptions() { return expertOptions_; }
    
    // Set all expert options
    void setExpertOptions(const ExpertOptions& options) { expertOptions_ = options; }
    
    // Enable expert mode
    void enableExpertMode(bool enable = true) { expertOptions_.enableExpertMode = enable; }
    
    // Set custom configuration
    bool loadCustomConfiguration(const std::string& filepath);
    bool saveCustomConfiguration(const std::string& filepath) const;
    
    // Export configuration to JSON
    std::string exportToJSON() const;
    
    // Import configuration from JSON
    bool importFromJSON(const std::string& json);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Advanced configuration options
    DebugOptions debugOptions_;
    BrainArchitectureOptions brainArchOptions_;
    PlasticityOptions plasticityOptions_;
    NeuromodulationOptions neuromodOptions_;
    MemoryOptions memoryOptions_;
    SimulationOptions simulationOptions_;
    PerformanceOptions performanceOptions_;
    ExpertOptions expertOptions_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
