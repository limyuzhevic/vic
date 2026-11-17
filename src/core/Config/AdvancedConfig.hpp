#pragma once

// Advanced NLM Configuration for Power Users
// This file provides extended configuration options for advanced users,
// including profiling, serialization, and specialized testing features.

#include <string>
#include <vector>

namespace nlm {
namespace config {

// Advanced configuration constants

// Performance and profiling
constexpr bool DEFAULT_ENABLE_PERFORMANCE_PROFILING = false;
constexpr bool DEFAULT_ENABLE_TIMING_PROFILE = false;
constexpr bool DEFAULT_ENABLE_MEMORY_PROFILE = false;
constexpr const char* DEFAULT_PERF_OUTPUT_FILE = "performance.prof";
constexpr double DEFAULT_PERF_SAMPLING_INTERVAL = 0.1;  // seconds
constexpr size_t DEFAULT_MAX_PERF_EVENTS = 100000;

// Serialization and checkpointing
constexpr const char* DEFAULT_CHECKPOINT_FILE = "brain_checkpoint.nml";
constexpr const char* DEFAULT_SNAPSHOT_FILE = "brain_snapshot.nml";
constexpr bool DEFAULT_ENABLE_CHECKPOINT_RESTORE = true;
constexpr bool DEFAULT_ENABLE_AUTO_CHECKPOINT = false;
constexpr double DEFAULT_CHECKPOINT_INTERVAL = 10.0;  // seconds
constexpr size_t DEFAULT_MAX_CHECKPOINT_FILES = 10;

// Advanced plasticity settings
constexpr float DEFAULT_ADAPTIVE_LEARNING_RATE = 0.005f;
constexpr float DEFAULT_METAplasticity_THRESHOLD = 0.5f;
constexpr float DEFAULT_FORGETTING_RATE = 0.00001f;
constexpr bool DEFAULT_ENABLE_METAPLASTICITY = false;

// Advanced neuromodulation
constexpr bool DEFAULT_ENABLE_DOPAMINE_TRACKING = false;
constexpr bool DEFAULT_ENABLE_NOVELTY_DETECTION = false;
constexpr bool DEFAULT_ENABLE_CURIOUSITY_MODULE = false;
constexpr float DEFAULT_NOVELTY_THRESHOLD = 0.1f;

// Advanced sensory processing
constexpr bool DEFAULT_ENABLE_PREDICTION_ERROR_SIGNALING = false;
constexpr float DEFAULT_PREDICTION_WEIGHT = 0.8f;
constexpr size_t DEFAULT_PREDICTION_HISTORY_SIZE = 100;

// Advanced memory systems
constexpr bool DEFAULT_ENABLE_EPISODIC_MEMORY = true;
constexpr bool DEFAULT_ENABLE_WORKING_MEMORY = true;
constexpr bool DEFAULT_ENABLE_ASSOCIATIVE_MEMORY = true;
constexpr size_t DEFAULT_EPISODIC_CAPACITY = 1000;
constexpr double DEFAULT_MEMORY_CONSOLIDATION_INTERVAL = 1.0;  // seconds

// Advanced development
constexpr bool DEFAULT_ENABLE_DEVELOPMENT_PHASES = true;
constexpr float DEFAULT_MATURATION_RATE = 0.01f;
constexpr float DEFAULT_SYNAPTOGENESIS_BURST = 0.01f;
constexpr float DEFAULT_PRUNING_INTENSITY = 0.005f;
constexpr size_t DEFAULT_DEVELOPMENT_PHASES = 5;

// Advanced cognitive functions
constexpr bool DEFAULT_ENABLE_PLANNING_MODULE = false;
constexpr bool DEFAULT_ENABLE_CONCEPT_FORMATION = false;
constexpr bool DEFAULT_ENABLE_ATTENTIONAL_SELECTION = false;
constexpr float DEFAULT_PLANNING_HORIZON = 5.0;  // seconds
constexpr size_t DEFAULT_CONCEPT_CAPACITY = 500;

// Visualization and output
constexpr const char* DEFAULT_ADVANCED_VIZ_OUTPUT = "visualization.json";
constexpr bool DEFAULT_ENABLE_ADVANCED_VIZ = false;
constexpr const char* DEFAULT_STATISTICS_OUTPUT = "brain_stats.json";
constexpr const char* DEFAULT_ACTIVITY_OUTPUT = "activity.nml";

// Debugging and development
constexpr bool DEFAULT_ENABLE_DEBUG_MODE = false;
constexpr bool DEFAULT_ENABLE_STEP_BY_STEP_MODE = false;
constexpr bool DEFAULT_ENABLE_VALIDATION_CHECKS = true;
constexpr size_t DEFAULT_VALIDATION_BATCH_SIZE = 100;
constexpr const char* DEFAULT_DEBUG_LOG_FILE = "debug.log";
constexpr const char* DEFAULT_ERROR_LOG_FILE = "errors.log";

// Network topology and structure
constexpr bool DEFAULT_ENABLE_DYNAMIC_TOPOLOGY = false;
constexpr float DEFAULT_TOPOLOGY_CHANGE_RATE = 0.001f;
constexpr size_t DEFAULT_MIN_REGIONS = 1;
constexpr size_t DEFAULT_MAX_REGIONS = 10;
constexpr bool DEFAULT_ENABLE_HIERARCHICAL_STRUCTURE = false;

// Experimental features
constexpr bool DEFAULT_ENABLE_EXPERIMENTAL_FEATURES = false;
constexpr const char* DEFAULT_EXPERIMENT_RESULTS_DIR = "experiments/";
constexpr bool DEFAULT_ENABLE_DATA_COLLECTION = true;
constexpr bool DEFAULT_ENABLE_STATISTICAL_ANALYSIS = false;

// System integration
constexpr bool DEFAULT_ENABLE_PLUGIN_SYSTEM = false;
constexpr const char* DEFAULT_PLUGIN_DIRECTORY = "plugins/";
constexpr bool DEFAULT_ENABLE_SCRIPTING = false;
constexpr const char* DEFAULT_SCRIPT_LANGUAGE = "python";

// Output formats
enum class OutputFormat {
    BINARY,
    JSON,
    XML,
    CSV,
    PLAIN_TEXT,
    PROTOBUF,
    MSG_PACK
};

// Checkpoint compression
enum class CheckpointCompression {
    NONE,
    ZIP,
    LZ4,
    ZSTD,
    GZIP
};

// Advanced configuration structure for power users
struct AdvancedConfig {
    // Performance and profiling
    bool enablePerformanceProfiling;
    bool enableTimingProfile;
    bool enableMemoryProfile;
    std::string perfOutputFile;
    double perfSamplingInterval;
    size_t maxPerfEvents;
    
    // Serialization and checkpointing
    std::string checkpointFile;
    std::string snapshotFile;
    bool enableCheckpointRestore;
    bool enableAutoCheckpoint;
    double checkpointInterval;
    size_t maxCheckpointFiles;
    CheckpointCompression checkpointCompression;
    
    // Advanced plasticity
    float adaptiveLearningRate;
    float metaplasticityThreshold;
    float forgettingRate;
    bool enableMetaplasticity;
    
    // Neuromodulation
    bool enableDopamineTracking;
    bool enableNoveltyDetection;
    bool enableCuriosityModule;
    float noveltyThreshold;
    
    // Sensory processing
    bool enablePredictionErrorSignaling;
    float predictionWeight;
    size_t predictionHistorySize;
    
    // Memory systems
    bool enableEpisodicMemory;
    bool enableWorkingMemory;
    bool enableAssociativeMemory;
    size_t episodicCapacity;
    double memoryConsolidationInterval;
    
    // Development
    bool enableDevelopmentPhases;
    float maturationRate;
    float synaptogenesisBurst;
    float pruningIntensity;
    size_t developmentPhases;
    
    // Cognitive functions
    bool enablePlanningModule;
    bool enableConceptFormation;
    bool enableAttentionalSelection;
    float planningHorizon;
    size_t conceptCapacity;
    
    // Visualization and output
    std::string advancedVizOutput;
    bool enableAdvancedViz;
    std::string statisticsOutput;
    std::string activityOutput;
    
    // Debugging and development
    bool enableDebugMode;
    bool enableStepByStepMode;
    bool enableValidationChecks;
    size_t validationBatchSize;
    std::string debugLogFile;
    std::string errorLogFile;
    
    // Network structure
    bool enableDynamicTopology;
    float topologyChangeRate;
    size_t minRegions;
    size_t maxRegions;
    bool enableHierarchicalStructure;
    
    // Experimental features
    bool enableExperimentalFeatures;
    std::string experimentResultsDir;
    bool enableDataCollection;
    bool enableStatisticalAnalysis;
    
    // System integration
    bool enablePluginSystem;
    std::string pluginDirectory;
    bool enableScripting;
    std::string scriptLanguage;
    
    // Default constructor
    AdvancedConfig()
        : enablePerformanceProfiling(DEFAULT_ENABLE_PERFORMANCE_PROFILING),
          enableTimingProfile(DEFAULT_ENABLE_TIMING_PROFILE),
          enableMemoryProfile(DEFAULT_ENABLE_MEMORY_PROFILE),
          perfOutputFile(DEFAULT_PERF_OUTPUT_FILE),
          perfSamplingInterval(DEFAULT_PERF_SAMPLING_INTERVAL),
          maxPerfEvents(DEFAULT_MAX_PERF_EVENTS),
          
          checkpointFile(DEFAULT_CHECKPOINT_FILE),
          snapshotFile(DEFAULT_SNAPSHOT_FILE),
          enableCheckpointRestore(DEFAULT_ENABLE_CHECKPOINT_RESTORE),
          enableAutoCheckpoint(DEFAULT_ENABLE_AUTO_CHECKPOINT),
          checkpointInterval(DEFAULT_CHECKPOINT_INTERVAL),
          maxCheckpointFiles(DEFAULT_MAX_CHECKPOINT_FILES),
          checkpointCompression(CheckpointCompression::ZIP),
          
          adaptiveLearningRate(DEFAULT_ADAPTIVE_LEARNING_RATE),
          metaplasticityThreshold(DEFAULT_METAplasticity_THRESHOLD),
          forgettingRate(DEFAULT_FORGETTING_RATE),
          enableMetaplasticity(DEFAULT_ENABLE_METAPLASTICITY),
          
          enableDopamineTracking(DEFAULT_ENABLE_DOPAMINE_TRACKING),
          enableNoveltyDetection(DEFAULT_ENABLE_NOVELTY_DETECTION),
          enableCuriosityModule(DEFAULT_ENABLE_CURIOUSITY_MODULE),
          noveltyThreshold(DEFAULT_NOVELTY_THRESHOLD),
          
          enablePredictionErrorSignaling(DEFAULT_ENABLE_PREDICTION_ERROR_SIGNALING),
          predictionWeight(DEFAULT_PREDICTION_WEIGHT),
          predictionHistorySize(DEFAULT_PREDICTION_HISTORY_SIZE),
          
          enableEpisodicMemory(DEFAULT_ENABLE_EPISODIC_MEMORY),
          enableWorkingMemory(DEFAULT_ENABLE_WORKING_MEMORY),
          enableAssociativeMemory(DEFAULT_ENABLE_ASSOCIATIVE_MEMORY),
          episodicCapacity(DEFAULT_EPISODIC_CAPACITY),
          memoryConsolidationInterval(DEFAULT_MEMORY_CONSOLIDATION_INTERVAL),
          
          enableDevelopmentPhases(DEFAULT_ENABLE_DEVELOPMENT_PHASES),
          maturationRate(DEFAULT_MATURATION_RATE),
          synaptogenesisBurst(DEFAULT_SYNAPTOGENESIS_BURST),
          pruningIntensity(DEFAULT_PRUNING_INTENSITY),
          developmentPhases(DEFAULT_DEVELOPMENT_PHASES),
          
          enablePlanningModule(DEFAULT_ENABLE_PLANNING_MODULE),
          enableConceptFormation(DEFAULT_ENABLE_CONCEPT_FORMATION),
          enableAttentionalSelection(DEFAULT_ENABLE_ATTENTIONAL_SELECTION),
          planningHorizon(DEFAULT_PLANNING_HORIZON),
          conceptCapacity(DEFAULT_CONCEPT_CAPACITY),
          
          enableAdvancedViz(DEFAULT_ENABLE_ADVANCED_VIZ),
          advancedVizOutput(DEFAULT_ADVANCED_VIZ_OUTPUT),
          statisticsOutput(DEFAULT_STATISTICS_OUTPUT),
          activityOutput(DEFAULT_ACTIVITY_OUTPUT),
          
          enableDebugMode(DEFAULT_ENABLE_DEBUG_MODE),
          enableStepByStepMode(DEFAULT_ENABLE_STEP_BY_STEP_MODE),
          enableValidationChecks(DEFAULT_ENABLE_VALIDATION_CHECKS),
          validationBatchSize(DEFAULT_VALIDATION_BATCH_SIZE),
          debugLogFile(DEFAULT_DEBUG_LOG_FILE),
          errorLogFile(DEFAULT_ERROR_LOG_FILE),
          
          enableDynamicTopology(DEFAULT_ENABLE_DYNAMIC_TOPOLOGY),
          topologyChangeRate(DEFAULT_TOPOLOGY_CHANGE_RATE),
          minRegions(DEFAULT_MIN_REGIONS),
          maxRegions(DEFAULT_MAX_REGIONS),
          enableHierarchicalStructure(DEFAULT_ENABLE_HIERARCHICAL_STRUCTURE),
          
          enableExperimentalFeatures(DEFAULT_ENABLE_EXPERIMENTAL_FEATURES),
          experimentResultsDir(DEFAULT_EXPERIMENT_RESULTS_DIR),
          enableDataCollection(DEFAULT_ENABLE_DATA_COLLECTION),
          enableStatisticalAnalysis(DEFAULT_ENABLE_STATISTICAL_ANALYSIS),
          
          enablePluginSystem(DEFAULT_ENABLE_PLUGIN_SYSTEM),
          pluginDirectory(DEFAULT_PLUGIN_DIRECTORY),
          enableScripting(DEFAULT_ENABLE_SCRIPTING),
          scriptLanguage(DEFAULT_SCRIPT_LANGUAGE) {}
};

} // namespace config
} // namespace nlm