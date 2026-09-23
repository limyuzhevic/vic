# NLM Neural Simulation Framework - Configuration Guide

## Overview

This document provides comprehensive configuration documentation for the NLM (Neural Learning Machine) neural simulation framework. It covers all configuration options, settings, and parameters that control the behavior, performance, and characteristics of neural simulations.

## Configuration System Overview

The NLM framework uses a hierarchical configuration system managed by the `Config` class. Configuration is organized into several categories:

### 1. Core Configuration
- **Simulation Parameters**: Basic simulation settings
- **Network Configuration**: Neuron and synapse settings
- **Plasticity Configuration**: Learning rule parameters
- **Memory Configuration**: Memory system settings
- **Development Configuration**: Developmental process parameters
- **Neuromodulation Configuration**: Neuromodulatory signal settings
- **Output Configuration**: Logging and result options

### 2. Configuration Hierarchy

```
Global Configuration
├── Simulation Settings
│   ├── Time Parameters
│   ├── Algorithm Selection
│   └── Output Configuration
├── Network Configuration
│   ├── Neuron Configuration
│   ├── Synapse Configuration
│   └── Connectivity Settings
├── Plasticity Configuration
│   ├── STDP Settings
│   ├── Hebbian Settings
│   └── Structural Plasticity Settings
├── Memory Configuration
│   ├── Working Memory Settings
│   ├── Episodic Memory Settings
│   └── Associative Memory Settings
├── Development Configuration
│   ├── Developmental Stages
│   ├── Plasticity Modifiers
│   └── Critical Period Settings
├── Neuromodulation Configuration
│   ├── Dopamine Settings
│   ├── Novelty Settings
│   └── Curiosity Settings
└── Output Configuration
    ├── Logging Settings
    ├── Result Output
    └── Visualization Settings
```

## Configuration Categories

### 1. Simulation Configuration

#### Time Management
```cpp
class SimulationConfig {
public:
    // Time step and duration
    double getSimulationTimestep() const;           // Default: 0.001
    void setSimulationTimestep(double timestep);   // Minimum: 0.0001
    
    double getMaxSimulationTime() const;           // Default: 1000.0
    void setMaxSimulationTime(double time);       // Minimum: 10.0
    
    size_t getMaxSteps() const;                   // Default: 1000000
    void setMaxSteps(size_t steps);
    
    // Time measurement
    bool getEnablePreciseTiming() const;           // Default: false
    void setEnablePreciseTiming(bool enable);
    
    double getTimeTolerance() const;              // Default: 1e-6
    void setTimeTolerance(double tolerance);
};
```

#### Algorithm Selection
```cpp
class AlgorithmConfig {
public:
    // Neuron dynamics
    NeuronModel getNeuronModel() const;           // Default: NeuronModel::LIF
    void setNeuronModel(NeuronModel model);
    
    bool getEnableAdaptiveExponential() const;    // Default: false
    void setEnableAdaptiveExponential(bool enable);
    
    // Plasticity rules
    PlasticityRules getPlasticityRules() const;    // Default: STDP + Hebbian
    void setPlasticityRules(PlasticityRules rules);
    
    bool getEnableSTDP() const;                    // Default: true
    void setEnableSTDP(bool enable);
    
    bool getEnableHebbian() const;                 // Default: true
    void setEnableHebbian(bool enable);
    
    bool getEnableRewardModulated() const;        // Default: false
    void setEnableRewardModulated(bool enable);
    
    // Memory systems
    MemorySystemMode getMemorySystemMode() const;  // Default: WorkingMemory
    void setMemorySystemMode(MemorySystemMode mode);
    
    bool getEnableEpisodicMemory() const;          // Default: false
    void setEnableEpisodicMemory(bool enable);
    
    bool getEnableAssociativeMemory() const;       // Default: false
    void setEnableAssociativeMemory(bool enable);
};
```

#### Output Configuration
```cpp
class OutputConfig {
public:
    // Logging levels
    LogLevel getLogLevel() const;                 // Default: LogLevel::Info
    void setLogLevel(LogLevel level);
    
    std::string getLogFile() const;               // Default: "nlm_simulation.log"
    void setLogFile(const std::string& file);
    
    bool getEnableVerboseOutput() const;          // Default: false
    void setEnableVerboseOutput(bool enable);
    
    // Result output
    OutputFormat getResultFormat() const;         // Default: OutputFormat::Binary
    void setResultFormat(OutputFormat format);
    
    std::string getResultDirectory() const;       // Default: "results"
    void setResultDirectory(const std::string& dir);
    
    // Visualization
    bool getEnableVisualization() const;          // Default: false
    void setEnableVisualization(bool enable);
    
    VisualizationType getVisualizationType() const; // Default: VisualizationType::None
    void setVisualizationType(VisualizationType type);
};
```

### 2. Network Configuration

#### Neuron Configuration
```cpp
class NeuronConfig {
public:
    // Neuron count and distribution
    size_t getNeuronCount() const;                // Default: 1000
    void setNeuronCount(size_t count);
    
    NeuronTypeDistribution getNeuronTypeDistribution() const; // Default: Mixed
    void setNeuronTypeDistribution(NeuronTypeDistribution dist);
    
    // Neuron parameters
    double getDefaultThreshold() const;           // Default: -55.0
    void setDefaultThreshold(double threshold);
    
    double getDefaultLeakConductance() const;      // Default: 10.0
    void setDefaultLeakConductance(double conductance);
    
    double getDefaultRestingPotential() const;    // Default: -70.0
    void setDefaultRestingPotential(double potential);
    
    double getDefaultResetPotential() const;      // Default: -70.0
    void setDefaultResetPotential(double potential);
    
    // Refractory period
    size_t getDefaultRefractoryPeriod() const;    // Default: 5
    void setDefaultRefractoryPeriod(size_t period);
    
    // Firing rate parameters
    double getMaxFiringRate() const;              // Default: 1000.0
    void setMaxFiringRate(double rate);
    
    double getAdaptationTimeConstant() const;     // Default: 10.0
    void setAdaptationTimeConstant(double tau);
};
```

#### Synapse Configuration
```cpp
class SynapseConfig {
public:
    // Synapse count and density
    size_t getMaxSynapseCount() const;            // Default: 100000
    void setMaxSynapseCount(size_t count);
    
    double getSynapseDensity() const;             // Default: 0.1 (10%)
    void setSynapseDensity(double density);
    
    // Synapse properties
    SynapseType getDefaultSynapseType() const;   // Default: SynapseType::Excitatory
    void setDefaultSynapseType(SynapseType type);
    
    double getDefaultSynapseWeight() const;       // Default: 1.0
    void setDefaultSynapseWeight(double weight);
    
    size_t getDefaultSynapseDelay() const;        // Default: 1
    void setDefaultSynapseDelay(size_t delay);
    
    // Short-term plasticity
    bool getEnableShortTermPlasticity() const;    // Default: true
    void setEnableShortTermPlasticity(bool enable);
    
    double getFacilitationTimeConstant() const;  // Default: 100.0
    void setFacilitationTimeConstant(double tau);
    
    double getDepressionTimeConstant() const;    // Default: 1000.0
    void setDepressionTimeConstant(double tau);
    
    double getUtilizationParameter() const;      // Default: 0.3
    void setUtilizationParameter(double u);
};
```

#### Connectivity Configuration
```cpp
class ConnectivityConfig {
public:
    // Connectivity patterns
    ConnectivityPattern getConnectivityPattern() const; // Default: Random
    void setConnectivityPattern(ConnectivityPattern pattern);
    
    double getConnectionProbability() const;     // Default: 0.1
    void setConnectionProbability(double prob);
    
    // Distance-dependent connectivity
    bool getEnableDistanceDependentConnectivity() const; // Default: false
    void setEnableDistanceDependentConnectivity(bool enable);
    
    double getDistanceDecayExponent() const;     // Default: -2.0
    void setDistanceDecayExponent(double exponent);
    
    // Cluster connectivity
    bool getEnableClusterConnectivity() const;   // Default: false
    void setEnableClusterConnectivity(bool enable);
    
    size_t getClusterSize() const;               // Default: 10
    void setClusterSize(size_t size);
    
    // Directional connectivity
    bool getDirectedConnectivity() const;        // Default: false
    void setDirectedConnectivity(bool directed);
};
```

### 3. Plasticity Configuration

#### STDP Configuration
```cpp
class STDPConfig {
public:
    // STDP parameters
    double getLTPWeight() const;                  // Default: 0.01
    void setLTPWeight(double weight);
    
    double getLTDWeight() const;                  // Default: 0.01
    void setLTDWeight(double weight);
    
    double getSTDPTimeConstant() const;          // Default: 20.0
    void setSTDPTimeConstant(double tau);
    
    double getSTDPTimeConstantPlus() const;      // Default: 20.0
    void setSTDPTimeConstantPlus(double tau);
    
    double getSTDPTimeConstantMinus() const;     // Default: 40.0
    void setSTDPTimeConstantMinus(double tau);
    
    // Activity-dependent STDP
    bool getEnableActivityDependentSTDP() const; // Default: true
    void setEnableActivityDependentSTDP(bool enable);
    
    double getMaximumWeightChange() const;       // Default: 0.1
    void setMaximumWeightChange(double maxChange);
};
```

#### Hebbian Configuration
```cpp
class HebbianConfig {
public:
    // Hebbian parameters
    double getLearningRate() const;              // Default: 0.005
    void setLearningRate(double rate);
    
    double getMaximumWeight() const;             // Default: 10.0
    void setMaximumWeight(double max);
    
    double getMinimumWeight() const;             // Default: -10.0
    void setMinimumWeight(double min);
    
    // Stability mechanisms
    double getWeightNormalizationFactor() const; // Default: 1.0
    void setWeightNormalizationFactor(double factor);
    
    double getHomeostaticLearningRate() const;   // Default: 0.001
    void setHomeostaticLearningRate(double rate);
};
```

#### Structural Plasticity Configuration
```cpp
class StructuralPlasticityConfig {
public:
    // Synapse formation
    bool getEnableSynaptogenesis() const;       // Default: true
    void setEnableSynaptogenesis(bool enable);
    
    double getSynaptogenesisRate() const;        // Default: 0.001
    void setSynaptogenesisRate(double rate);
    
    // Synapse elimination
    bool getEnablePruning() const;               // Default: true
    void setEnablePruning(bool enable);
    
    double getPruningRate() const;              // Default: 0.001
    void setPruningRate(double rate);
    
    // Homeostatic mechanisms
    double getTargetFiringRate() const;         // Default: 10.0
    void setTargetFiringRate(double rate);
    
    double getHomeostaticPlasticityRate() const; // Default: 0.0001
    void setHomeostaticPlasticityRate(double rate);
};
```

### 4. Memory Configuration

#### Working Memory Configuration
```cpp
class WorkingMemoryConfig {
public:
    // Memory capacity
    size_t getWorkingMemoryCapacity() const;     // Default: 100
    void setWorkingMemoryCapacity(size_t capacity);
    
    double getWorkingMemoryDecayRate() const;    // Default: 0.1
    void setWorkingMemoryDecayRate(double rate);
    
    double getWorkingMemoryStrength() const;    // Default: 1.0
    void setWorkingMemoryStrength(double strength);
    
    // Competition parameters
    CompetitionMode getCompetitionMode() const;  // Default: WinnerTakeAll
    void setCompetitionMode(CompetitionMode mode);
    
    double getCompetitionThreshold() const;     // Default: 0.5
    void setCompetitionThreshold(double threshold);
    
    // Replay mechanisms
    bool getEnableReplay() const;                // Default: true
    void setEnableReplay(bool enable);
    
    double getReplayInterval() const;            // Default: 100.0
    void setReplayInterval(double interval);
};
```

#### Episodic Memory Configuration
```cpp
class EpisodicMemoryConfig {
public:
    // Memory encoding
    double getEncodingStrength() const;         // Default: 1.0
    void setEncodingStrength(double strength);
    
    double getEncodingPrecision() const;        // Default: 0.8
    void setEncodingPrecision(double precision);
    
    // Memory storage
    size_t getEpisodicMemoryCapacity() const;    // Default: 1000
    void setEpisodicMemoryCapacity(size_t capacity);
    
    double getEpisodicMemoryDecayRate() const;   // Default: 0.01
    void setEpisodicMemoryDecayRate(double rate);
    
    // Pattern completion
    double getCompletionThreshold() const;      // Default: 0.7
    void setCompletionThreshold(double threshold);
    
    CompletionMode getCompletionMode() const;   // Default: PatternCompletion
    void setCompletionMode(CompletionMode mode);
};
```

#### Associative Memory Configuration
```cpp
class AssociativeMemoryConfig {
public:
    // Association strength
    double getAssociationStrength() const;       // Default: 1.0
    void setAssociationStrength(double strength);
    
    double getAssociationDecayRate() const;      // Default: 0.05
    void setAssociationDecayRate(double rate);
    
    // Generalization
    double getGeneralizationFactor() const;      // Default: 0.5
    void setGeneralizationFactor(double factor);
    
    GeneralizationMode getGeneralizationMode() const; // Default: SimilarityBased
    void setGeneralizationMode(GeneralizationMode mode);
    
    // Retrieval
    double getRetrievalThreshold() const;       // Default: 0.6
    void setRetrievalThreshold(double threshold);
    
    RetrievalMode getRetrievalMode() const;     // Default: VectorSimilarity
    void setRetrievalMode(RetrievalMode mode);
};
```

### 5. Development Configuration

#### Developmental Stages
```cpp
class DevelopmentConfig {
public:
    // Stage durations (in simulation time units)
    double getInitialStageDuration() const;      // Default: 100.0
    void setInitialStageDuration(double duration);
    
    double getCriticalStageDuration() const;     // Default: 200.0
    void setCriticalStageDuration(double duration);
    
    double getMaturationStageDuration() const;   // Default: 300.0
    void setMaturationStageDuration(double duration);
    
    double getAdultStageDuration() const;        // Default: 400.0
    void setAdultStageDuration(double duration);
    
    // Plasticity modifiers
    double getInitialPlasticityModifier() const; // Default: 1.0
    void setInitialPlasticityModifier(double modifier);
    
    double getCriticalPeriodModifier() const;   // Default: 2.0
    void setCriticalPeriodModifier(double modifier);
    
    double getMaturationModifier() const;       // Default: 0.8
    void setMaturationModifier(double modifier);
    
    double getAdultModifier() const;            // Default: 0.5
    void setAdultModifier(double modifier);
};
```

#### Critical Period Configuration
```cpp
class CriticalPeriodConfig {
public:
    // Critical period dynamics
    double getCriticalPeriodOnset() const;      // Default: 50.0
    void setCriticalPeriodOnset(double time);
    
    double getCriticalPeriodOffset() const;     // Default: 250.0
    void setCriticalPeriodOffset(double time);
    
    double getCriticalPeriodDecayRate() const;  // Default: 0.01
    void setCriticalPeriodDecayRate(double rate);
    
    // Sensitive period effects
    double getSensitivePeriodThreshold() const; // Default: 0.9
    void setSensitivePeriodThreshold(double threshold);
    
    CriticalPeriodEffect getCriticalPeriodEffect() const; // Default: EnhancedLearning
    void setCriticalPeriodEffect(CriticalPeriodEffect effect);
};
```

### 6. Neuromodulation Configuration

#### Dopamine Configuration
```cpp
class DopamineConfig {
public:
    // Reward prediction error
    double getRPEScaling() const;                // Default: 1.0
    void setRPEScaling(double scaling);
    
    double getRPEDecayRate() const;              // Default: 0.1
    void setRPEDecayRate(double rate);
    
    // Dopamine dynamics
    double getDopamineTimeConstant() const;     // Default: 100.0
    void setDopamineTimeConstant(double tau);
    
    double getDopamineReleaseRate() const;      // Default: 0.5
    void setDopamineReleaseRate(double rate);
    
    double getDopamineReuptakeRate() const;     // Default: 0.1
    void setDopamineReuptakeRate(double rate);
};
```

#### Novelty Configuration
```cpp
class NoveltyConfig {
public:
    // Novelty detection
    double getNoveltySensitivity() const;       // Default: 0.5
    void setNoveltySensitivity(double sensitivity);
    
    double getNoveltyDecayRate() const;         // Default: 0.05
    void setNoveltyDecayRate(double rate);
    
    // Attention modulation
    double getNoveltyAttentionStrength() const; // Default: 2.0
    void setNoveltyAttentionStrength(double strength);
    
    double getNoveltyAttenuationRate() const;   // Default: 0.9
    void setNoveltyAttenuationRate(double rate);
};
```

#### Curiosity Configuration
```cpp
class CuriosityConfig {
public:
    // Curiosity generation
    double getCuriosityBaseRate() const;         // Default: 0.01
    void setCuriosityBaseRate(double rate);
    
    double getCuriosityAmplification() const;    // Default: 1.0
    void setCuriosityAmplification(double amplification);
    
    // Exploration behavior
    double getExplorationRate() const;         // Default: 0.3
    void setExplorationRate(double rate);
    
    double getExploitationRate() const;         // Default: 0.7
    void setExploitationRate(double rate);
};
```

### 7. Output Configuration

#### Logging Configuration
```cpp
class LoggingConfig {
public:
    // Log levels
    LogLevel getSystemLogLevel() const;          // Default: LogLevel::Info
    void setSystemLogLevel(LogLevel level);
    
    LogLevel getNeuralLogLevel() const;          // Default: LogLevel::Warning
    void setNeuralLogLevel(LogLevel level);
    
    LogLevel getPlasticityLogLevel() const;      // Default: LogLevel::Debug
    void setPlasticityLogLevel(LogLevel level);
    
    // Log formats
    LogFormat getLogFormat() const;             // Default: LogFormat::Text
    void setLogFormat(LogFormat format);
    
    bool getEnableStructuredLogging() const;     // Default: true
    void setEnableStructuredLogging(bool enable);
    
    // Log rotation
    size_t getMaxLogFileSize() const;           // Default: 100 (MB)
    void setMaxLogFileSize(size_t size);
    
    size_t getMaxLogFiles() const;               // Default: 10
    void setMaxLogFiles(size_t count);
};
```

#### Result Output Configuration
```cpp
class ResultOutputConfig {
public:
    // Output formats
    OutputFormat getNeuralStateFormat() const;   // Default: OutputFormat::Binary
    void setNeuralStateFormat(OutputFormat format);
    
    OutputFormat getPlasticityFormat() const;    // Default: OutputFormat::Binary
    void setPlasticityFormat(OutputFormat format);
    
    OutputFormat getMemoryFormat() const;        // Default: OutputFormat::Binary
    void setMemoryFormat(OutputFormat format);
    
    // Output frequency
    size_t getStateOutputInterval() const;       // Default: 100
    void setStateOutputInterval(size_t interval);
    
    size_t getPlasticityOutputInterval() const;  // Default: 10
    void setPlasticityOutputInterval(size_t interval);
    
    // Output compression
    bool getEnableCompression() const;          // Default: true
    void setEnableCompression(bool enable);
    
    CompressionType getCompressionType() const; // Default: CompressionType::LZ4
    void setCompressionType(CompressionType type);
};
```

## Configuration File Formats

### 1. JSON Configuration
```json
{
    "simulation": {
        "timestep": 0.001,
        "max_time": 1000.0,
        "max_steps": 1000000
    },
    "network": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "neuron_type_distribution": "mixed"
    },
    "plasticity": {
        "stdp": {
            "weight": 0.01,
            "time_constant": 20.0
        },
        "hebbian": {
            "learning_rate": 0.005,
            "max_weight": 10.0
        }
    },
    "output": {
        "log_level": "info",
        "result_format": "binary",
        "compression": true
    }
}
```

### 2. XML Configuration
```xml
<?xml version="1.0" encoding="UTF-8"?>
<NLM_Configuration>
    <Simulation>
        <TimeStep>0.001</TimeStep>
        <MaxTime>1000.0</MaxTime>
        <MaxSteps>1000000</MaxSteps>
    </Simulation>
    
    <Network>
        <NeuronCount>1000</NeuronCount>
        <SynapseDensity>0.1</SynapseDensity>
        <NeuronTypeDistribution>mixed</NeuronTypeDistribution>
    </Network>
    
    <Plasticity>
        <STDP>
            <Weight>0.01</Weight>
            <TimeConstant>20.0</TimeConstant>
        </STDP>
        <Hebbian>
            <LearningRate>0.005</LearningRate>
            <MaxWeight>10.0</MaxWeight>
        </Hebbian>
    </Plasticity>
    
    <Output>
        <LogLevel>info</LogLevel>
        <ResultFormat>binary</ResultFormat>
        <Compression>true</Compression>
    </Output>
</NLM_Configuration>
```

### 3. Command Line Arguments
```bash
# Command line interface
nlm_simulation \
    --timestep 0.001 \
    --max-time 1000.0 \
    --neuron-count 1000 \
    --synapse-density 0.1 \
    --log-level info \
    --output-format binary \
    --enable-plasticity true \
    --plasticity-rules "STDP,Hebbian" \
    --config-file config.json
```

## Configuration Validation

### 1. Schema Validation
```cpp
class ConfigurationValidator {
public:
    bool validateConfiguration(const Config& config) {
        // Check for required settings
        if (!config.hasRequiredSettings()) {
            return false;
        }
        
        // Validate type consistency
        if (!validateTypes(config)) {
            return false;
        }
        
        // Check parameter ranges
        if (!validateRanges(config)) {
            return false;
        }
        
        // Validate compatibility
        if (!validateCompatibility(config)) {
            return false;
        }
        
        return true;
    }
    
private:
    bool validateTypes(const Config& config) {
        // Implementation
        return true;
    }
    
    bool validateRanges(const Config& config) {
        // Implementation
        return true;
    }
    
    bool validateCompatibility(const Config& config) {
        // Implementation
        return true;
    }
};
```

### 2. Consistency Validation
```cpp
class ConsistencyValidator {
public:
    bool validateConfiguration(const Config& config) {
        // Check for conflicting settings
        if (config.getPlasticityRules() == PlasticityRules::STDP && 
            config.getEnableHebbian()) {
            // This combination might be inconsistent
            return false;
        }
        
        // Check for sufficient resources
        if (!checkResourceAvailability(config)) {
            return false;
        }
        
        // Check for parameter interactions
        if (!checkParameterInteractions(config)) {
            return false;
        }
        
        return true;
    }
    
private:
    bool checkResourceAvailability(const Config& config) {
        // Implementation
        return true;
    }
    
    bool checkParameterInteractions(const Config& config) {
        // Implementation
        return true;
    }
};
```

## Configuration Management

### 1. Default Configuration
```cpp
class DefaultConfiguration {
public:
    static Config getDefaultConfig() {
        Config config;
        
        // Set all default values
        config.setSimulationTimestep(0.001);
        config.setMaxSimulationTime(1000.0);
        config.setNeuronCount(1000);
        config.setSynapseDensity(0.1);
        config.setEnableSTDP(true);
        config.setEnableHebbian(true);
        config.setEnableRewardModulated(false);
        config.setEnableVisualization(false);
        
        // Return default config
        return config;
    }
    
    static Config getDevelopmentConfig() {
        Config config = getDefaultConfig();
        
        // Override for development
        config.setEnableVerboseOutput(true);
        config.setLogLevel(LogLevel::Debug);
        config.setResultFormat(OutputFormat::Text);
        
        return config;
    }
    
    static Config getProductionConfig() {
        Config config = getDefaultConfig();
        
        // Optimize for production
        config.setEnableVerboseOutput(false);
        config.setLogLevel(LogLevel::Info);
        config.setResultFormat(OutputFormat::Binary);
        config.setEnableCompression(true);
        
        return config;
    }
};
```

### 2. Configuration Updates
```cpp
class ConfigurationManager {
public:
    void updateConfiguration(const Config& newConfig) {
        // Validate new configuration
        if (!validator_.validateConfiguration(newConfig)) {
            throw ConfigurationValidationError("Invalid configuration");
        }
        
        // Apply configuration updates
        applyConfigurationUpdates(newConfig);
        
        // Save configuration
        saveConfiguration(newConfig);
        
        // Notify observers
        notifyObservers(newConfig);
    }
    
    void loadConfiguration(const std::string& filename) {
        // Parse configuration file
        Config config = parser_.parseFile(filename);
        
        // Validate and apply
        updateConfiguration(config);
    }
    
    void saveConfiguration(const Config& config) {
        // Save to file
        stdwriter_.write("config.json", config.toJSON());
    }
    
    Config getCurrentConfiguration() const {
        return currentConfig_;
    }
    
private:
    ConfigurationValidator validator_;
    ConfigurationParser parser_;
    ConfigurationWriter stdwriter_;
    Config currentConfig_;
    std::vector<std::function<void(const Config&)>> observers_;
};
```

## Configuration Examples

### 1. Simple Configuration
```cpp
Config simpleConfig;
simpleConfig.setSimulationTimestep(0.001);
simpleConfig.setNeuronCount(1000);
simpleConfig.setSynapseDensity(0.1);
simpleConfig.setEnableSTDP(true);
simpleConfig.setEnableHebbian(true);
```

### 2. Complex Configuration
```cpp
Config complexConfig;
simpleConfig.setSimulationTimestep(0.0001);
simpleConfig.setMaxSimulationTime(10000.0);
simpleConfig.setNeuronCount(10000);
simpleConfig.setSynapseDensity(0.05);
simpleConfig.setEnableSTDP(true);
simpleConfig.setEnableHebbian(true);
simpleConfig.setEnableRewardModulated(true);
simpleConfig.setEnableNovelty(true);
simpleConfig.setEnableCuriosity(true);
simpleConfig.setEnableEpisodicMemory(true);
simpleConfig.setEnableAssociativeMemory(true);
simpleConfig.setEnableVisualization(true);
simpleConfig.setLogLevel(LogLevel::Debug);
simpleConfig.setResultFormat(OutputFormat::Text);
```

### 3. Performance-Oriented Configuration
```cpp
Config performanceConfig;
simpleConfig.setSimulationTimestep(0.001);
simpleConfig.setNeuronCount(50000);
simpleConfig.setSynapseDensity(0.01);
simpleConfig.setEnableSTDP(true);
simpleConfig.setEnableHebbian(false);  // Disable Hebbian for performance
simpleConfig.setEnableRewardModulated(false);
simpleConfig.setEnableCompression(true);
simpleConfig.setMaxLogFileSize(500);
simpleConfig.setMaxLogFiles(5);
```

## Conclusion

The NLM framework provides a comprehensive configuration system that supports a wide range of neural simulation scenarios. The hierarchical configuration structure allows users to easily customize simulations for different research questions and applications.

Key features of the NLM configuration system include:

1. **Hierarchical Organization**: Clear separation of concerns
2. **Flexible Configuration**: Support for multiple configuration formats
3. **Validation**: Comprehensive validation and consistency checking
4. **Extensibility**: Easy addition of new configuration options
5. **Optimization**: Built-in performance considerations

Users can start with simple configurations and gradually increase complexity as needed, or use the provided presets for common simulation scenarios. The framework supports both programmatic and file-based configuration, making it suitable for research, development, and production environments.

Configuration management in NLM enables:

- **Reproducibility**: Save and reload simulation configurations
- **Flexibility**: Quick switching between different simulation parameters
- **Validation**: Ensure configuration consistency and completeness
- **Documentation**: Clear specification of simulation parameters
- **Collaboration**: Shared configuration files for team research
