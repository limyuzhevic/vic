#pragma once

#include "../core/Config/Config.hpp"
#include <string>

namespace nlm {

// Advanced configuration options for NLM
class AdvancedConfig {
public:
    // Performance optimization settings
    struct PerformanceSettings {
        // Threading
        int numThreads;
        bool enableSIMD;
        bool enableParallelProcessing;
        
        // Memory management
        size_t memoryPoolSize;
        bool enableMemoryPools;
        size_t maxEvents;
        
        // Caching
        bool enableEventCache;
        size_t cacheSize;
        
        // Simulation optimization
        bool enableLazyEvaluation;
        bool enableDeltaCompression;
        
        PerformanceSettings()
            : numThreads(1)
            , enableSIMD(true)
            , enableParallelProcessing(false)
            , memoryPoolSize(1024)
            , enableMemoryPools(true)
            , maxEvents(100000)
            , enableEventCache(true)
            , cacheSize(1024)
            , enableLazyEvaluation(false)
            , enableDeltaCompression(true) {}
    };
    
    // Advanced plasticity rules
    struct AdvancedPlasticitySettings {
        // STDP parameters
        float stdpLearningRate;
        float stdpTimeConstant;
        float stdpLTPWeight;
        float stdpLTDWeight;
        
        // Hebbian parameters
        float hebbianThreshold;
        float hebbianDecayRate;
        
        // Structural plasticity
        float synaptogenesisRate;
        float pruningRate;
        float maxSynapsesPerNeuron;
        
        // Neuromodulation effects
        float dopaminePlasticityModulator;
        float curiosityPlasticityModulator;
        
        AdvancedPlasticitySettings()
            : stdpLearningRate(0.01f)
            , stdpTimeConstant(20.0f)
            , stdpLTPWeight(0.01f)
            , stdpLTDWeight(0.012f)
            , hebbianThreshold(0.5f)
            , hebbianDecayRate(0.95f)
            , synaptogenesisRate(0.0001f)
            , pruningRate(0.00001f)
            , maxSynapsesPerNeuron(1000)
            , dopaminePlasticityModulator(1.0f)
            , curiosityPlasticityModulator(0.5f) {}
    };
    
    // Memory management settings
    struct MemorySettings {
        // Working memory
        size_t workingMemoryCapacity;
        float workingMemoryDecayRate;
        bool enableWorkingMemoryCompression;
        
        // Episodic memory
        size_t maxEpisodicEpisodes;
        float consolidationThreshold;
        size_t replayInterval;
        
        // Associative memory
        size_t associativeMemorySize;
        float associationDecayRate;
        
        // Memory optimization
        bool enableMemoryPooling;
        bool enableMemoryCompression;
        
        MemorySettings()
            : workingMemoryCapacity(1000)
            , workingMemoryDecayRate(0.99f)
            , enableWorkingMemoryCompression(true)
            , maxEpisodicEpisodes(1000)
            , consolidationThreshold(0.3f)
            , replayInterval(100)
            , associativeMemorySize(500)
            , associationDecayRate(0.98f)
            , enableMemoryPooling(true)
            , enableMemoryCompression(true) {}
    };
    
    // Visualization and debugging settings
    struct VisualizationSettings {
        // Output formats
        std::string defaultOutputFormat;
        bool enableProgressReporting;
        bool enableDetailedLogging;
        
        // Visualization options
        bool enableVisualPlots;
        std::string plotDirectory;
        bool generateStatistics;
        
        // Debugging
        bool enableDebugMode;
        std::string debugOutputPath;
        bool trackPerformanceMetrics;
        
        VisualizationSettings()
            : defaultOutputFormat("text")
            , enableProgressReporting(true)
            , enableDetailedLogging(false)
            , enableVisualPlots(false)
            , plotDirectory("./plots")
            , generateStatistics(true)
            , enableDebugMode(false)
            , debugOutputPath("./debug")
            , trackPerformanceMetrics(true) {}
    };
    
    // Export and import settings
    struct ExportImportSettings {
        // File formats
        bool enableJSONExport;
        bool enableBinaryExport;
        bool enableTextExport;
        
        // Compression
        bool enableCompression;
        std::string compressionAlgorithm;
        int compressionLevel;
        
        // Validation
        bool validateOnImport;
        bool verifyChecksums;
        
        ExportImportSettings()
            : enableJSONExport(true)
            , enableBinaryExport(true)
            , enableTextExport(true)
            , enableCompression(true)
            , compressionAlgorithm("gzip")
            , compressionLevel(6)
            , validateOnImport(true)
            , verifyChecksums(true) {}
    };
    
    // Neural network topology settings
    struct NetworkTopologySettings {
        // Region configuration
        size_t defaultRegionCount;
        size_t minNeuronsPerRegion;
        size_t maxNeuronsPerRegion;
        
        // Population configuration
        size_t maxPopulationsPerRegion;
        bool enablePopulationBalancing;
        
        // Connectivity
        float defaultConnectionProbability;
        bool enableSmallWorldTopology;
        float smallWorldClustering;
        float smallWorldPathLength;
        
        NetworkTopologySettings()
            : defaultRegionCount(1)
            , minNeuronsPerRegion(100)
            , maxNeuronsPerRegion(5000)
            , maxPopulationsPerRegion(10)
            , enablePopulationBalancing(true)
            , defaultConnectionProbability(0.1f)
            , enableSmallWorldTopology(false)
            , smallWorldClustering(0.3f)
            , smallWorldPathLength(4.0f) {}
    };
    
    // Learning and adaptation settings
    struct LearningSettings {
        // Developmental stages
        double developmentalStageDuration;
        bool enableNeuromodulation;
        bool enableCuriosityDrivenLearning;
        bool enableRewardBasedLearning;
        
        // Adaptation mechanisms
        bool enableStructuralAdaptation;
        bool enableFunctionalAdaptation;
        float adaptationRate;
        
        // Learning algorithms
        bool enableSTDP;
        bool enableHebbian;
        bool enableRandom;
        float randomLearningRate;
        
        LearningSettings()
            : developmentalStageDuration(100.0)
            , enableNeuromodulation(true)
            , enableCuriosityDrivenLearning(true)
            , enableRewardBasedLearning(true)
            , enableStructuralAdaptation(true)
            , enableFunctionalAdaptation(true)
            , adaptationRate(0.01f)
            , enableSTDP(true)
            , enableHebbian(true)
            , enableRandom(false)
            , randomLearningRate(0.001f) {}
    };
    
    // Integration with AdvancedConfig
    void applyToConfig(Config& config) const;
    static AdvancedConfig createDefault();
    static AdvancedConfig createOptimized();
    
    // Getters and setters
    const PerformanceSettings& getPerformanceSettings() const { return performanceSettings_; }
    PerformanceSettings& getPerformanceSettings() { return performanceSettings_; }
    
    const AdvancedPlasticitySettings& getPlasticitySettings() const { return plasticitySettings_; }
    AdvancedPlasticitySettings& getPlasticitySettings() { return plasticitySettings_; }
    
    const MemorySettings& getMemorySettings() const { return memorySettings_; }
    MemorySettings& getMemorySettings() { return memorySettings_; }
    
    const VisualizationSettings& getVisualizationSettings() const { return visualizationSettings_; }
    VisualizationSettings& getVisualizationSettings() { return visualizationSettings_; }
    
    const ExportImportSettings& getExportImportSettings() const { return exportImportSettings_; }
    ExportImportSettings& getExportImportSettings() { return exportImportSettings_; }
    
    const NetworkTopologySettings& getNetworkTopologySettings() const { return networkTopologySettings_; }
    NetworkTopologySettings& getNetworkTopologySettings() { return networkTopologySettings_; }
    
    const LearningSettings& getLearningSettings() const { return learningSettings_; }
    LearningSettings& getLearningSettings() { return learningSettings_; }
    
private:
    PerformanceSettings performanceSettings_;
    AdvancedPlasticitySettings plasticitySettings_;
    MemorySettings memorySettings_;
    VisualizationSettings visualizationSettings_;
    ExportImportSettings exportImportSettings_;
    NetworkTopologySettings networkTopologySettings_;
    LearningSettings learningSettings_;
};

} // namespace nlm