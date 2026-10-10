// Configuration defaults with descriptions
// This file contains all the default configuration values for the NLM system
// organized by functional areas

#pragma once

#include <string>
#include "core/Config/Config.hpp"

namespace nlm {

// Core System Configuration
struct CoreDefaults {
    static constexpr const char* RandomSeed = "random_seed";
    static constexpr int64_t RandomSeedDefault = 42;
    static constexpr const char* SimulationTimestep = "simulation_timestep";
    static constexpr double SimulationTimestepDefault = 0.001;
    static constexpr const char* NeuronCount = "neuron_count";
    static constexpr int64_t NeuronCountDefault = 1000;
    static constexpr const char* RegionCount = "region_count";
    static constexpr int64_t RegionCountDefault = 2;
    static constexpr const char* ConnectionProbability = "connection_probability";
    static constexpr double ConnectionProbabilityDefault = 0.1;
    static constexpr const char* MaxSimulationSteps = "max_simulation_steps";
    static constexpr int64_t MaxSimulationStepsDefault = 10000;
    static constexpr const char* SimulationTimeLimit = "simulation_time_limit";
    static constexpr double SimulationTimeLimitDefault = 0.0;
};

// Plasticity Configuration
struct PlasticityDefaults {
    static constexpr const char* PlasticityLearningRate = "plasticity_learning_rate";
    static constexpr double PlasticityLearningRateDefault = 0.01;
    static constexpr const char* STDP LTPWeight = "stdp_ltp_weight";
    static constexpr double STDP LTPWeightDefault = 0.01;
    static constexpr const char* STDP LTDWeight = "stdp_ltd_weight";
    static constexpr double STDP LTDWeightDefault = 0.012;
    static constexpr const char* STDPTimeConstant = "stdp_time_constant";
    static constexpr double STDPTimeConstantDefault = 20.0;
    static constexpr const char* STDPWindowSize = "stdp_window_size";
    static constexpr double STDPWindowSizeDefault = 20.0;
};

// Development Configuration
struct DevelopmentDefaults {
    static constexpr const char* DevelopmentSynaptogenesisRate = "development_synaptogenesis_rate";
    static constexpr double DevelopmentSynaptogenesisRateDefault = 0.001;
    static constexpr const char* DevelopmentPruningRate = "development_pruning_rate";
    static constexpr double DevelopmentPruningRateDefault = 0.0001;
    static constexpr const char* DevelopmentPruningThreshold = "development_pruning_threshold";
    static constexpr double DevelopmentPruningThresholdDefault = 0.1;
    static constexpr const char* DevelopmentSynaptogenesisThreshold = "development_synaptogenesis_threshold";
    static constexpr double DevelopmentSynaptogenesisThresholdDefault = 0.01;
};

// Neuromodulation Configuration
struct NeuromodulationDefaults {
    static constexpr const char* DopamineBaseline = "dopamine_baseline";
    static constexpr double DopamineBaselineDefault = 0.1;
    static constexpr const char* SerotoninBaseline = "serotonin_baseline";
    static constexpr double SerotoninBaselineDefault = 0.1;
    static constexpr const char* AcetylcholineBaseline = "acetylcholine_baseline";
    static constexpr double AcetylcholineBaselineDefault = 0.1;
    static constexpr const char* NorepinephrineBaseline = "norepinephrine_baseline";
    static constexpr double NorepinephrineBaselineDefault = 0.1;
};

// Reward Configuration
struct RewardDefaults {
    static constexpr const char* RewardDiscountFactor = "reward_discount_factor";
    static constexpr double RewardDiscountFactorDefault = 0.99;
    static constexpr const char* RewardLearningRate = "reward_learning_rate";
    static constexpr double RewardLearningRateDefault = 0.01;
    static constexpr const char* RewardScale = "reward_scale";
    static constexpr double RewardScaleDefault = 1.0;
};

// Environment Configuration
struct EnvironmentDefaults {
    static constexpr const char* EnvironmentName = "environment_name";
    static constexpr std::string EnvironmentNameDefault = "GridWorld";
    static constexpr const char* EnvironmentWidth = "environment_width";
    static constexpr int64_t EnvironmentWidthDefault = 10;
    static constexpr const char* EnvironmentHeight = "environment_height";
    static constexpr int64_t EnvironmentHeightDefault = 10;
    static constexpr const char* EnvironmentDepth = "environment_depth";
    static constexpr int64_t EnvironmentDepthDefault = 1;
    static constexpr const char* BoundaryCondition = "boundary_condition";
    static constexpr std::string BoundaryConditionDefault = "wrap";
};

// Simulation Configuration
struct SimulationDefaults {
    static constexpr const char* SimulationTimestep = "simulation_timestep";
    static constexpr double SimulationTimestepDefault = 0.001;
    static constexpr const char* MaxSimulationSteps = "max_simulation_steps";
    static constexpr int64_t MaxSimulationStepsDefault = 10000;
    static constexpr const char* SimulationTimeLimit = "simulation_time_limit";
    static constexpr double SimulationTimeLimitDefault = 0.0;
    static constexpr const char* SimulationMaxMemory = "simulation_max_memory";
    static constexpr int64_t SimulationMaxMemoryDefault = 1000000;
    static constexpr const char* SimulationSnapshotInterval = "simulation_snapshot_interval";
    static constexpr int64_t SimulationSnapshotIntervalDefault = 100;
};

// Logging Configuration
struct LoggingDefaults {
    static constexpr const char* LogLevel = "log_level";
    static constexpr int LogLevelDefault = 0;
    static constexpr const char* LogToFile = "log_to_file";
    static constexpr bool LogToFileDefault = false;
    static constexpr const char* LogFilename = "log_filename";
    static constexpr std::string LogFilenameDefault = "nlm.log";
    static constexpr const char* LogMaxFileSize = "log_max_file_size";
    static constexpr int64_t LogMaxFileSizeDefault = 10485760;
    static constexpr const char* LogFlushInterval = "log_flush_interval";
    static constexpr int64_t LogFlushIntervalDefault = 1000;
};

// Visualization Configuration
struct VisualizationDefaults {
    static constexpr const char* VisualizationEnabled = "visualization_enabled";
    static constexpr bool VisualizationEnabledDefault = false;
    static constexpr const char* VisualizationUpdateRate = "visualization_update_rate";
    static constexpr double VisualizationUpdateRateDefault = 30.0;
    static constexpr const char* VisualizationBufferSize = "visualization_buffer_size";
    static constexpr int64_t VisualizationBufferSizeDefault = 1000;
    static constexpr const char* VisualizationBackend = "visualization_backend";
    static constexpr std::string VisualizationBackendDefault = "OpenGL";
};

// Hebbian Configuration
struct HebbianDefaults {
    static constexpr const char* HebbianLearningRate = "hebbian_learning_rate";
    static constexpr double HebbianLearningRateDefault = 0.01;
    static constexpr const char* HebbianWindowSize = "hebbian_window_size";
    static constexpr int64_t HebbianWindowSizeDefault = 100;
    static constexpr const char* HebbianThreshold = "hebbian_threshold";
    static constexpr double HebbianThresholdDefault = 0.1;
};

// Configuration descriptions for user reference
inline std::string GetDescription(const std::string& key) {
    if (key == CoreDefaults::RandomSeed) return "Random seed for reproducible simulations";
    if (key == CoreDefaults::SimulationTimestep) return "Time step for simulation updates (seconds)";
    if (key == CoreDefaults::NeuronCount) return "Number of neurons in the network";
    if (key == CoreDefaults::RegionCount) return "Number of brain regions";
    if (key == CoreDefaults::ConnectionProbability) return "Probability of creating connections between neurons";
    if (key == CoreDefaults::MaxSimulationSteps) return "Maximum number of simulation steps to run";
    if (key == CoreDefaults::SimulationTimeLimit) return "Time limit for simulation (seconds)";
    
    if (key == PlasticityDefaults::PlasticityLearningRate) return "Global plasticity learning rate";
    if (key == PlasticityDefaults::STDP LTPWeight) return "STDP long-term potentiation weight";
    if (key == PlasticityDefaults::STDP LTDWeight) return "STDP long-term depression weight";
    if (key == PlasticityDefaults::STDPTimeConstant) return "STDP time constant for synaptic modifications";
    if (key == PlasticityDefaults::STDPWindowSize) return "STDP time window size (ms)";
    
    if (key == DevelopmentDefaults::DevelopmentSynaptogenesisRate) return "Rate of new synaptic formation during development";
    if (key == DevelopmentDefaults::DevelopmentPruningRate) return "Rate of synaptic elimination during development";
    if (key == DevelopmentDefaults::DevelopmentPruningThreshold) return "Threshold for pruning weak synapses";
    if (key == DevelopmentDefaults::DevelopmentSynaptogenesisThreshold) return "Threshold for forming new synapses";
    
    if (key == NeuromodulationDefaults::DopamineBaseline) return "Baseline dopamine level";
    if (key == NeuromodulationDefaults::SerotoninBaseline) return "Baseline serotonin level";
    if (key == NeuromodulationDefaults::AcetylcholineBaseline) return "Baseline acetylcholine level";
    if (key == NeuromodulationDefaults::NorepinephrineBaseline) return "Baseline norepinephrine level";
    
    if (key == RewardDefaults::RewardDiscountFactor) return "Discount factor for future rewards";
    if (key == RewardDefaults::RewardLearningRate) return "Rate of reward prediction error learning";
    if (key == RewardDefaults::RewardScale) return "Scale factor for reward values";
    
    if (key == EnvironmentDefaults::EnvironmentName) return "Name of the environment";
    if (key == EnvironmentDefaults::EnvironmentWidth) return "Width of the environment grid";
    if (key == EnvironmentDefaults::EnvironmentHeight) return "Height of the environment grid";
    if (key == EnvironmentDefaults::EnvironmentDepth) return "Depth of the environment grid";
    if (key == EnvironmentDefaults::BoundaryCondition) return "Boundary condition type (wrap, reflect, periodic)";
    
    if (key == SimulationDefaults::SimulationTimestep) return "Time step for simulation updates (seconds)";
    if (key == SimulationDefaults::MaxSimulationSteps) return "Maximum number of simulation steps to run";
    if (key == SimulationDefaults::SimulationTimeLimit) return "Time limit for simulation (seconds)";
    if (key == SimulationDefaults::SimulationMaxMemory) return "Maximum memory usage for simulation (bytes)";
    if (key == SimulationDefaults::SimulationSnapshotInterval) return "Interval between simulation snapshots";
    
    if (key == LoggingDefaults::LogLevel) return "Logging level (0=DEBUG, 1=INFO, 2=WARNING, 3=ERROR)";
    if (key == LoggingDefaults::LogToFile) return "Whether to log to file";
    if (key == LoggingDefaults::LogFilename) return "Filename for log output";
    if (key == LoggingDefaults::LogMaxFileSize) return "Maximum log file size before rotation (bytes)";
    if (key == LoggingDefaults::LogFlushInterval) return "Flush interval for log file (ms)";
    
    if (key == VisualizationDefaults::VisualizationEnabled) return "Whether visualization is enabled";
    if (key == VisualizationDefaults::VisualizationUpdateRate) return "Visualization update rate (Hz)";
    if (key == VisualizationDefaults::VisualizationBufferSize) return "Size of visualization buffer";
    if (key == VisualizationDefaults::VisualizationBackend) return "Visualization backend (OpenGL, Vulkan, etc.)";
    
    if (key == HebbianDefaults::HebbianLearningRate) return "Hebbian learning rate";
    if (key == HebbianDefaults::HebbianWindowSize) return "Hebbian learning window size";
    if (key == HebbianDefaults::HebbianThreshold) return "Hebbian learning threshold";
    
    return "";
}

// Function to get all default configuration entries
inline std::vector<ConfigEntry> GetAllDefaultEntries() {
    std::vector<ConfigEntry> entries;
    
    // Core System
    entries.emplace_back(CoreDefaults::RandomSeed, ConfigValue(CoreDefaults::RandomSeedDefault), ConfigSource::Default, GetDescription(CoreDefaults::RandomSeed));
    entries.emplace_back(CoreDefaults::SimulationTimestep, ConfigValue(CoreDefaults::SimulationTimestepDefault), ConfigSource::Default, GetDescription(CoreDefaults::SimulationTimestep));
    entries.emplace_back(CoreDefaults::NeuronCount, ConfigValue(CoreDefaults::NeuronCountDefault), ConfigSource::Default, GetDescription(CoreDefaults::NeuronCount));
    entries.emplace_back(CoreDefaults::RegionCount, ConfigValue(CoreDefaults::RegionCountDefault), ConfigSource::Default, GetDescription(CoreDefaults::RegionCount));
    entries.emplace_back(CoreDefaults::ConnectionProbability, ConfigValue(CoreDefaults::ConnectionProbabilityDefault), ConfigSource::Default, GetDescription(CoreDefaults::ConnectionProbability));
    entries.emplace_back(CoreDefaults::MaxSimulationSteps, ConfigValue(CoreDefaults::MaxSimulationStepsDefault), ConfigSource::Default, GetDescription(CoreDefaults::MaxSimulationSteps));
    entries.emplace_back(CoreDefaults::SimulationTimeLimit, ConfigValue(CoreDefaults::SimulationTimeLimitDefault), ConfigSource::Default, GetDescription(CoreDefaults::SimulationTimeLimit));
    
    // Plasticity
    entries.emplace_back(PlasticityDefaults::PlasticityLearningRate, ConfigValue(PlasticityDefaults::PlasticityLearningRateDefault), ConfigSource::Default, GetDescription(PlasticityDefaults::PlasticityLearningRate));
    entries.emplace_back(PlasticityDefaults::STDP LTPWeight, ConfigValue(PlasticityDefaults::STDP LTPWeightDefault), ConfigSource::Default, GetDescription(PlasticityDefaults::STDP LTPWeight));
    entries.emplace_back(PlasticityDefaults::STDP LTDWeight, ConfigValue(PlasticityDefaults::STDP LTDWeightDefault), ConfigSource::Default, GetDescription(PlasticityDefaults::STDP LTDWeight));
    entries.emplace_back(PlasticityDefaults::STDPTimeConstant, ConfigValue(PlasticityDefaults::STDPTimeConstantDefault), ConfigSource::Default, GetDescription(PlasticityDefaults::STDPTimeConstant));
    entries.emplace_back(PlasticityDefaults::STDPWindowSize, ConfigValue(PlasticityDefaults::STDPWindowSizeDefault), ConfigSource::Default, GetDescription(PlasticityDefaults::STDPWindowSize));
    
    // Development
    entries.emplace_back(DevelopmentDefaults::DevelopmentSynaptogenesisRate, ConfigValue(DevelopmentDefaults::DevelopmentSynaptogenesisRateDefault), ConfigSource::Default, GetDescription(DevelopmentDefaults::DevelopmentSynaptogenesisRate));
    entries.emplace_back(DevelopmentDefaults::DevelopmentPruningRate, ConfigValue(DevelopmentDefaults::DevelopmentPruningRateDefault), ConfigSource::Default, GetDescription(DevelopmentDefaults::DevelopmentPruningRate));
    entries.emplace_back(DevelopmentDefaults::DevelopmentPruningThreshold, ConfigValue(DevelopmentDefaults::DevelopmentPruningThresholdDefault), ConfigSource::Default, GetDescription(DevelopmentDefaults::DevelopmentPruningThreshold));
    entries.emplace_back(DevelopmentDefaults::DevelopmentSynaptogenesisThreshold, ConfigValue(DevelopmentDefaults::DevelopmentSynaptogenesisThresholdDefault), ConfigSource::Default, GetDescription(DevelopmentDefaults::DevelopmentSynaptogenesisThreshold));
    
    // Neuromodulation
    entries.emplace_back(NeuromodulationDefaults::DopamineBaseline, ConfigValue(NeuromodulationDefaults::DopamineBaselineDefault), ConfigSource::Default, GetDescription(NeuromodulationDefaults::DopamineBaseline));
    entries.emplace_back(NeuromodulationDefaults::SerotoninBaseline, ConfigValue(NeuromodulationDefaults::SerotoninBaselineDefault), ConfigSource::Default, GetDescription(NeuromodulationDefaults::SerotoninBaseline));
    entries.emplace_back(NeuromodulationDefaults::AcetylcholineBaseline, ConfigValue(NeuromodulationDefaults::AcetylcholineBaselineDefault), ConfigSource::Default, GetDescription(NeuromodulationDefaults::AcetylcholineBaseline));
    entries.emplace_back(NeuromodulationDefaults::NorepinephrineBaseline, ConfigValue(NeuromodulationDefaults::NorepinephrineBaselineDefault), ConfigSource::Default, GetDescription(NeuromodulationDefaults::NorepinephrineBaseline));
    
    // Reward
    entries.emplace_back(RewardDefaults::RewardDiscountFactor, ConfigValue(RewardDefaults::RewardDiscountFactorDefault), ConfigSource::Default, GetDescription(RewardDefaults::RewardDiscountFactor));
    entries.emplace_back(RewardDefaults::RewardLearningRate, ConfigValue(RewardDefaults::RewardLearningRateDefault), ConfigSource::Default, GetDescription(RewardDefaults::RewardLearningRate));
    entries.emplace_back(RewardDefaults::RewardScale, ConfigValue(RewardDefaults::RewardScaleDefault), ConfigSource::Default, GetDescription(RewardDefaults::RewardScale));
    
    // Environment
    entries.emplace_back(EnvironmentDefaults::EnvironmentName, ConfigValue(EnvironmentDefaults::EnvironmentNameDefault), ConfigSource::Default, GetDescription(EnvironmentDefaults::EnvironmentName));
    entries.emplace_back(EnvironmentDefaults::EnvironmentWidth, ConfigValue(EnvironmentDefaults::EnvironmentWidthDefault), ConfigSource::Default, GetDescription(EnvironmentDefaults::EnvironmentWidth));
    entries.emplace_back(EnvironmentDefaults::EnvironmentHeight, ConfigValue(EnvironmentDefaults::EnvironmentHeightDefault), ConfigSource::Default, GetDescription(EnvironmentDefaults::EnvironmentHeight));
    entries.emplace_back(EnvironmentDefaults::EnvironmentDepth, ConfigValue(EnvironmentDefaults::EnvironmentDepthDefault), ConfigSource::Default, GetDescription(EnvironmentDefaults::EnvironmentDepth));
    entries.emplace_back(EnvironmentDefaults::BoundaryCondition, ConfigValue(EnvironmentDefaults::BoundaryConditionDefault), ConfigSource::Default, GetDescription(EnvironmentDefaults::BoundaryCondition));
    
    // Simulation
    entries.emplace_back(SimulationDefaults::SimulationTimestep, ConfigValue(SimulationDefaults::SimulationTimestepDefault), ConfigSource::Default, GetDescription(SimulationDefaults::SimulationTimestep));
    entries.emplace_back(SimulationDefaults::MaxSimulationSteps, ConfigValue(SimulationDefaults::MaxSimulationStepsDefault), ConfigSource::Default, GetDescription(SimulationDefaults::MaxSimulationSteps));
    entries.emplace_back(SimulationDefaults::SimulationTimeLimit, ConfigValue(SimulationDefaults::SimulationTimeLimitDefault), ConfigSource::Default, GetDescription(SimulationDefaults::SimulationTimeLimit));
    entries.emplace_back(SimulationDefaults::SimulationMaxMemory, ConfigValue(SimulationDefaults::SimulationMaxMemoryDefault), ConfigSource::Default, GetDescription(SimulationDefaults::SimulationMaxMemory));
    entries.emplace_back(SimulationDefaults::SimulationSnapshotInterval, ConfigValue(SimulationDefaults::SimulationSnapshotIntervalDefault), ConfigSource::Default, GetDescription(SimulationDefaults::SimulationSnapshotInterval));
    
    // Logging
    entries.emplace_back(LoggingDefaults::LogLevel, ConfigValue(LoggingDefaults::LogLevelDefault), ConfigSource::Default, GetDescription(LoggingDefaults::LogLevel));
    entries.emplace_back(LoggingDefaults::LogToFile, ConfigValue(LoggingDefaults::LogToFileDefault), ConfigSource::Default, GetDescription(LoggingDefaults::LogToFile));
    entries.emplace_back(LoggingDefaults::LogFilename, ConfigValue(LoggingDefaults::LogFilenameDefault), ConfigSource::Default, GetDescription(LoggingDefaults::LogFilename));
    entries.emplace_back(LoggingDefaults::LogMaxFileSize, ConfigValue(LoggingDefaults::LogMaxFileSizeDefault), ConfigSource::Default, GetDescription(LoggingDefaults::LogMaxFileSize));
    entries.emplace_back(LoggingDefaults::LogFlushInterval, ConfigValue(LoggingDefaults::LogFlushIntervalDefault), ConfigSource::Default, GetDescription(LoggingDefaults::LogFlushInterval));
    
    // Visualization
    entries.emplace_back(VisualizationDefaults::VisualizationEnabled, ConfigValue(VisualizationDefaults::VisualizationEnabledDefault), ConfigSource::Default, GetDescription(VisualizationDefaults::VisualizationEnabled));
    entries.emplace_back(VisualizationDefaults::VisualizationUpdateRate, ConfigValue(VisualizationDefaults::VisualizationUpdateRateDefault), ConfigSource::Default, GetDescription(VisualizationDefaults::VisualizationUpdateRate));
    entries.emplace_back(VisualizationDefaults::VisualizationBufferSize, ConfigValue(VisualizationDefaults::VisualizationBufferSizeDefault), ConfigSource::Default, GetDescription(VisualizationDefaults::VisualizationBufferSize));
    entries.emplace_back(VisualizationDefaults::VisualizationBackend, ConfigValue(VisualizationDefaults::VisualizationBackendDefault), ConfigSource::Default, GetDescription(VisualizationDefaults::VisualizationBackend));
    
    // Hebbian
    entries.emplace_back(HebbianDefaults::HebbianLearningRate, ConfigValue(HebbianDefaults::HebbianLearningRateDefault), ConfigSource::Default, GetDescription(HebbianDefaults::HebbianLearningRate));
    entries.emplace_back(HebbianDefaults::HebbianWindowSize, ConfigValue(HebbianDefaults::HebbianWindowSizeDefault), ConfigSource::Default, GetDescription(HebbianDefaults::HebbianWindowSize));
    entries.emplace_back(HebbianDefaults::HebbianThreshold, ConfigValue(HebbianDefaults::HebbianThresholdDefault), ConfigSource::Default, GetDescription(HebbianDefaults::HebbianThreshold));
    
    return entries;
}
