# NLM Configuration and Command Line Interface Improvements

## Introduction

This document describes improvements to NLM's configuration system and command-line interface to make it more robust, user-friendly, and suitable for both beginners and advanced users.

## Current Configuration System

NLM currently uses a simple key=value text file format for configuration:

```ini
# Example config file
brain.neuron_count = 1000
brain.v_thresh = -50.0
brain.tau_mem = 20.0
```

**Limitations:**
- No data type validation
- Basic file format
- No nested configuration sections
- Limited error reporting

## Configuration Improvements

### 1. Enhanced Config.hpp

```cpp
// Add advanced configuration features

class Config {
public:
    // JSON/YAML support
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    
    // Advanced validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Type safety
    template<typename T>
    T getWithValidation(const std::string& key) const;
    
    // Configuration introspection
    std::vector<std::string> getSections() const;
    std::vector<std::string> getKeys(const std::string& section) const;
    
    // Configuration comparison
    bool hasChanges(const Config& other) const;
    double similarity(const Config& other) const;
    
    // Serialization
    std::string toJSON() const;
    std::string toYAML() const;
    
    // Configuration statistics
    size_t getEntryCount() const;
    std::string getSummary() const;
};
```

### 2. Configuration Types

```cpp
enum class ConfigType {
    Int,
    Float,
    String,
    Bool,
    Vector3,
    Matrix,
    Range
};

struct ConfigOption {
    std::string key;
    ConfigType type;
    ConfigValue defaultValue;
    std::string description;
    bool required;
    std::vector<ConfigValue> allowedValues;
    
    // Validation
    bool validate(const ConfigValue& value) const;
    std::string getErrorMessage(const ConfigValue& value) const;
};
```

### 3. Configuration Validation

```cpp
// Create a configuration schema
void createConfigSchema(Config& config) {
    config.addOption("brain.neuron_count", ConfigType::Int, 1000,
                     "Total number of neurons", {10, 10000});
    
    config.addOption("brain.v_thresh", ConfigType::Float, -50.0f,
                     "Neuron threshold potential (mV)", {-100.0f, -10.0f});
    
    config.addOption("brain.enable_plasticity", ConfigType::Bool, true,
                     "Enable synaptic plasticity", {});
    
    config.addOption("brain.plasticity_rate", ConfigType::Float, 0.001f,
                     "Plasticity learning rate", {0.0001f, 0.01f});
}

// Validate configuration
bool validateConfig(const Config& config) {
    auto errors = config.getValidationErrors();
    if (!errors.empty()) {
        std::cout << "Configuration errors:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "  " << error << std::endl;
        }
        return false;
    }
    return true;
}
```

### 4. Configuration Presets

```cpp
// Predefined configuration profiles
enum class ConfigPreset {
    Minimal,      // Smallest viable brain
    Standard,     // Default configuration
    HighPerformance, // Optimized for speed
    Research,     // For research experiments
    Custom       // User-customized
};

// Apply preset
void applyPreset(Config& config, ConfigPreset preset) {
    switch (preset) {
        case ConfigPreset::Minimal:
            config.set("brain.neuron_count", 100);
            config.set("brain.synapse_density", 0.05f);
            config.set("plasticity.stdp.enable", true);
            break;
            
        case ConfigPreset::Standard:
            config.set("brain.neuron_count", 1000);
            config.set("brain.synapse_density", 0.1f);
            config.set("plasticity.stdp.enable", true);
            config.set("plasticity.hebbian.enable", true);
            break;
            
        case ConfigPreset::HighPerformance:
            config.set("brain.neuron_count", 500);
            config.set("brain.synapse_density", 0.2f);
            config.set("plasticity.stdp.enable", true);
            config.set("performance.parallel_enabled", true);
            break;
    }
}
```

## Enhanced Command Line Interface

### 1. Command Line Parser

```cpp
// Advanced command line interface
class CommandLineInterface {
public:
    struct Options {
        std::string configFile;
        std::string outputFile;
        std::string checkpointFile;
        uint64_t steps = 1000;
        size_t neurons = 1000;
        float v_thresh = -50.0f;
        std::string preset;
        std::vector<std::string> customArgs;
        bool verbose = false;
        bool quiet = false;
        std::string logLevel = "info";
        std::string logFile;
        bool enableValidation = true;
        bool saveConfig = false;
    };
    
    Options parse(int argc, char** argv);
    void printHelp() const;
    void printVersion() const;
    bool validateOptions(const Options& options) const;
};
```

### 2. Command Line Options

```bash
# Basic usage
./nlm --config myconfig.cfg

# Advanced options
./nlm 
    --config experiment.cfg 
    --steps 5000 
    --neurons 2000 
    --v_thresh -60.0 
    --preset standard 
    --enable-validation 
    --log-level debug 
    --log-file simulation.log 
    --save-config myconfig.json

# Subcommands for different operations
./nlm verify --config test.cfg    # Validate configuration
./nlm benchmark --config bench.cfg --steps 10000  # Performance test
./nlm save --config config.cfg --output checkpoint.bin  # Save checkpoint
./nlm load --checkpoint checkpoint.bin  # Load checkpoint
./nlm explore --config config.cfg --random-seed 42  # Random exploration
./nlm analyze --config config.cfg --output results.json  # Analysis and export
```

### 3. Configuration File Format Support

```bash
# JSON configuration (--config file.json)
{
    "brain": {
        "neuron_count": 1000,
        "v_thresh": -50.0,
        "enable_plasticity": true
    },
    "plasticity": {
        "stdp": {"enable": true, "learning_rate": 0.001},
        "hebbian": {"enable": true}
    }
}

# YAML configuration (--config file.yaml)
brain:
  neuron_count: 1000
  v_thresh: -50.0
  enable_plasticity: true

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
  hebbian:
    enable: true

# Advanced features
advanced:
  parallel_processing: true
  checkpoint_interval: 1000
  replay_enabled: true
```

### 4. Interactive Configuration

```cpp
// Interactive configuration mode
void interactiveConfiguration(Config& config) {
    std::cout << "=== NLM Configuration Wizard ===" << std::endl;
    std::cout << "Welcome to the configuration wizard!" << std::endl << std::endl;
    
    // Step 1: Select preset
    std::cout << "Select configuration preset:" << std::endl;
    std::cout << "  1. Minimal (fast, small)" << std::endl;
    std::cout << "  2. Standard (balanced)" << std::endl;
    std::cout << "  3. High Performance (speed)" << std::endl;
    std::cout << "  4. Research (customizable)" << std::endl;
    std::cout << "  5. Custom (manual)" << std::endl;
    
    int choice;
    std::cin >> choice;
    
    // Step 2: Customize settings
    if (choice == 5) {
        customizeConfig(config);
    }
    
    // Step 3: Validate and save
    if (validateConfig(config)) {
        std::cout << "Configuration validated successfully!" << std::endl;
        std::cout << "Save to file? (y/n): ";
        char save;
        std::cin >> save;
        if (save == 'y' || save == 'Y') {
            std::string filename;
            std::cout << "Filename: ";
            std::cin >> filename;
            config.saveToFile(filename);
        }
    }
    
    std::cout << "=== Configuration Complete ===" << std::endl;
}

void customizeConfig(Config& config) {
    std::cout << "Current settings:" << std::endl;
    for (const auto& key : config.getKeys()) {
        std::cout << "  " << key << " = " << config.getOr<std::string>(key, "") << std::endl;
    }
    
    std::cout << "Enter new values (or leave blank to keep):" << std::endl;
    
    // Example customization
    std::cout << "brain.neuron_count: ";
    std::string input;
    std::getline(std::cin >> std::ws, input);
    if (!input.empty()) {
        config.set("brain.neuron_count", std::stoi(input));
    }
    
    std::cout << "brain.v_thresh: ";
    std::getline(std::cin >> std::ws, input);
    if (!input.empty()) {
        config.set("brain.v_thresh", std::stof(input));
    }
}
```

## Logging Enhancements

### 1. Structured Logging

```cpp
// Enhanced logging system
class AdvancedLogger : public ILogger {
public:
    struct LogEntry {
        std::chrono::system_clock::time_point timestamp;
        LogLevel level;
        std::string module;
        std::string message;
        std::unordered_map<std::string, std::string> metadata;
        std::string file;
        int line;
        std::string function;
    };
    
    void logStructured(LogLevel level, const std::string& module, 
                       const std::string& message, 
                       const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Log filtering and routing
    void addLogFilter(std::function<bool(const LogEntry&)> filter);
    void addLogSink(std::shared_ptr<ILogSink> sink);
};
```

### 2. Performance Logging

```cpp
// Performance monitoring
class PerformanceLogger {
public:
    struct PerformanceMetrics {
        double cpuUsage;
        double memoryUsage;
        size_t neuronCount;
        size_t synapseCount;
        double simulationTime;
        double wallClockTime;
        size_t totalSpikes;
        double avgFiringRate;
    };
    
    void logPerformance(const PerformanceMetrics& metrics);
    std::vector<PerformanceMetrics> getPerformanceHistory() const;
    PerformanceMetrics getLatestMetrics() const;
};
```

### 3. Debug Mode

```cpp
// Enhanced debugging capabilities
void enableDebugMode() {
    // Set log level to debug
    Logger::getGlobal().setLevel(LogLevel::Debug);
    
    // Enable performance logging
    performanceLogger.enable(true);
    
    // Enable memory tracking
    MemoryTracker::enable(true);
    
    // Enable validation
    Config::enableValidation(true);
    
    // Set up debug output file
    DebugOutput::setup("debug.log");
    
    std::cout << "Debug mode enabled. Detailed logs will be written to debug.log" 
              << std::endl;
}
```

## Error Handling and Validation

### 1. Configuration Validation

```cpp
// Configuration validation system
class ConfigValidator {
public:
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        std::string getSummary() const;
        bool hasErrors() const;
        bool hasWarnings() const;
    };
    
    ValidationResult validate(const Config& config) const;
    
    // Custom validators
    void addValidator(std::string key, 
                     std::function<bool(const ConfigValue&)> validator,
                     std::string errorMessage);
};
```

### 2. Simulation Validation

```cpp
// Simulation state validation
class SimulationValidator {
public:
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        std::string getSummary() const;
    };
    
    ValidationResult validate(const Brain& brain, const AgentBrain& agent,
                             const SimpleWorld& world) const;
    
    // Component-specific validation
    bool validateBrain(const Brain& brain) const;
    bool validateAgent(const AgentBrain& agent) const;
    bool validateWorld(const SimpleWorld& world) const;
};
```

## Testing and Benchmarking

### 1. Configuration Testing

```cpp
// Automated configuration testing
class ConfigTester {
public:
    struct TestResult {
        std::string testName;
        bool passed;
        std::string message;
        double executionTime;
    };
    
    std::vector<TestResult> runAllTests(const Config& config);
    
    // Specific test suites
    std::vector<TestResult> runConfigValidationTests(const Config& config);
    std::vector<TestResult> runPerformanceTests(const Config& config);
    std::vector<TestResult> runCompatibilityTests(const Config& config);
};
```

### 2. Performance Benchmarking

```cpp
// Performance benchmarking
class Benchmark {
public:
    struct BenchmarkResult {
        std::string benchmarkName;
        double timePerStep;       // milliseconds
        double memoryUsage;       // MB
        double spikesPerSecond;
        double neuronsPerSecond;
        size_t maxNeurons;
        size_t maxSynapses;
    };
    
    BenchmarkResult run(const Config& config, uint64_t steps);
    
    // Specialized benchmarks
    BenchmarkResult benchmarkNeuronDynamics(const Config& config);
    BenchmarkResult benchmarkPlasticity(const Config& config);
    BenchmarkResult benchmarkMemorySystems(const Config& config);
};
```

## Configuration File Examples

### 1. Minimal Configuration

```ini
# minimal.ini - Smallest viable configuration
brain.neuron_count = 100
brain.synapse_density = 0.05
brain.v_thresh = -55.0
brain.tau_mem = 20.0

plasticity.stdp.enable = true
plasticity.hebbian.enable = true

neuromod.dopamine.scale = 1.0
neuromod.curiosity.enable = true

performance.parallel_enabled = false
```

### 2. Standard Configuration

```json
// config.json - Balanced configuration
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "v_thresh": -50.0,
        "tau_mem": 20.0,
        "enable_plasticity": true
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.0001f
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.0001f,
            "pruning_rate": 0.00001f
        }
    },
    "neuromodulation": {
        "dopamine": {
            "scale": 1.0,
            "learning_rate": 0.001
        },
        "curiosity": {
            "enable": true,
            "scale": 1.0
        },
        "novelty": {
            "enable": true,
            "decay_rate": 0.99
        }
    },
    "performance": {
        "parallel_enabled": true,
        "vectorization_enabled": true,
        "sparse_connectivity": true
    },
    "memory": {
        "working_memory_size": 1000,
        "episodic_memory_capacity": 100,
        "semantic_memory_enabled": true
    },
    "development": {
        "enable": true,
        "critical_period_enabled": true,
        "plasticity_decay_rate": 0.1
    }
}
```

### 3. Research Configuration

```yaml
# research.yaml - Advanced research configuration
simulation:
    timestep: 0.001
    max_steps: 100000
    parallel: true

brain:
    neuron_count: 5000
    region_count: 3
    connection_probability: 0.15
    
    neuron_parameters:
        v_thresh: -55.0
        v_rest: -70.0
        tau_mem: 20.0
        tau_ref: 2.0
        
plasticity:
    stdp:
        enable: true
        learning_rate: 0.002
        tau_plus: 25.0
        tau_minus: 25.0
        a_plus: 0.01
        a_minus: 0.012
        
    hebbian:
        enable: true
        learning_rate: 0.0005f
        eligibility_window: 100
        
    structural:
        enable: true
        synaptogenesis_rate: 0.0002f
        pruning_rate: 0.00002f
        pruning_threshold: 0.001f
        
neuromodulation:
    dopamine:
        enable: true
        scale: 1.5
        learning_rate: 0.001
        prediction_error_weight: 0.8
        
    curiosity:
        enable: true
        scale: 2.0
        novelty_threshold: 0.3
        exploration_rate: 0.2
        
    novelty:
        enable: true
        detection_threshold: 0.1
        decay_rate: 0.95
        memory_window: 1000
        
prediction:
    enable: true
    prediction_horizon: 50
    prediction_error_decay: 0.9
    confidence_threshold: 0.7
    
    transformer_enabled: false  # For research comparison

cognition:
    concept_formation:
        enable: true
        pattern_threshold: 0.8
        consolidation_interval: 1000
        
    neural_planner:
        enable: true
        planning_horizon: 20
        action_selection_method: "softmax"
        
    attention:
        enable: true
        competition_strength: 1.0
        focus_decay_rate: 0.98
        
    self_model:
        enable: true
        updating_rate: 0.01
        confidence_threshold: 0.5
        
memory:
    working_memory:
        enable: true
        capacity: 2000
        trace_decay: 0.99
        consolidation_threshold: 0.8
        
    episodic_memory:
        enable: true
        capacity: 500
        encoding_rate: 0.01
        replay_interval: 100
        replay_duration: 50
        
    semantic_memory:
        enable: true
        learning_rate: 0.001
        pattern_similarity_threshold: 0.7
        
    procedural_memory:
        enable: true
        reinforcement_learning_rate: 0.01
        policy_gradient_enabled: true
        
    associative_memory:
        enable: true
        association_strength_threshold: 0.3
        pattern_completion_enabled: true
        
development:
    enable: true
    developmental_stages: true
    critical_period_enabled: true
    
    stage_parameters:
        initial:
            plasticity_modifier: 1.0
            synaptogenesis_rate: 0.0003f
            pruning_rate: 0.00001f
            curiosity_weight: 0.5
            
        critical_period:
            plasticity_modifier: 0.8
            synaptogenesis_rate: 0.0002f
            pruning_rate: 0.00002f
            curiosity_weight: 1.0
            
        maturation:
            plasticity_modifier: 0.6
            synaptogenesis_rate: 0.0001f
            pruning_rate: 0.00003f
            curiosity_weight: 0.8
            
        adult:
            plasticity_modifier: 0.4
            synaptogenesis_rate: 0.00005f
            pruning_rate: 0.00005f
            curiosity_weight: 0.3
            
        aging:
            plasticity_modifier: 0.2
            synaptogenesis_rate: 0.00001f
            pruning_rate: 0.00007f
            curiosity_weight: 0.1
            
experiments:
    ablations:
        dopamine_blocked: false
        curiosity_blocked: false
        novelty_blocked: false
        working_memory_blocked: false
        episodic_memory_blocked: false
        
    metrics:
        recording_interval: 100
        save_to_file: true
        output_format: "json"
        
performance:
    parallel_processing:
        enable: true
        thread_count: "auto"
        work_queue_size: 1000
        
    memory_pools:
        enable: true
        neuron_pool_size: 10000
        synapse_pool_size: 50000
        trace_pool_size: 5000
        
    vectorization:
        enable: true
        simd_level: "AVX2"
        optimization_level: "aggressive"
        
    event_queues:
        enable: true
        spike_queue_size: 100000
        input_queue_size: 10000
        
        event_processing:
            mode: "parallel"
            priority_levels: 3
            
output:
    visualization:
        enable: true
        format: "json"
        fps: 30
        max_history: 1000
        
    checkpointing:
        enable: true
        interval: 10000
        compression: "gzip"
        path: "./checkpoints/"
        
    replay:
        enable: true
        method: "graduated_newest_first"
        consolidation_interval: 1000
        replay_ratio: 0.1
        
    logging:
        level: "info"
        file: "simulation.log"
        format: "json"
        include_performance: true
        include_validation: true
```

## Next Steps

1. **Immediate Improvements**:
   - Add JSON/YAML configuration support
   - Implement configuration validation
   - Enhance command-line interface
   - Add interactive configuration wizard

2. **Advanced Features**:
   - Performance monitoring and benchmarking
   - Configuration comparison and similarity analysis
   - Automated configuration testing
   - Configuration versioning and diff

3. **Integration with Python Bindings**:
   - Expose configuration system to Python API
   - Add Python configuration validation
   - Integrate with Python profiling tools

These enhancements will make NLM much more user-friendly, robust, and suitable for both beginners and advanced users, while maintaining backward compatibility with existing configurations.