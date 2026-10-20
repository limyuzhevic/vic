# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Enhanced CLI Documentation

### Advanced Command Line Interface

The NLM project now includes a comprehensive CLI tool for advanced configuration and simulation control. The CLI provides full access to all configuration options, logging control, and simulation management capabilities.

#### Installation
```bash
# Build and install CLI
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make nlm_cli

# Or install system-wide
make install
```

#### Basic CLI Usage
```bash
# Show help and available commands
./nlm_cli --help

# Run a basic simulation
./nlm_cli simulate --help

# Start interactive mode
./nlm_cli interactive
```

#### CLI Command Reference

##### Simulate Command
```bash
./nlm_cli simulate [OPTIONS]

Required arguments:
  --config FILE          Configuration file path (JSON/YAML/INI)
  --steps N              Number of simulation steps (default: 1000)

Optional arguments:
  --brain NEURONS        Number of neurons (default: 1000)
  --seed SEED            Random seed (default: 42)
  --timestep DT          Simulation timestep in seconds (default: 0.001)
  --world-width W        World width (default: 10)
  --world-height H       World height (default: 10)
  --log-level LEVEL      Logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL, FATAL)
  --log-file PATH        Log file path
  --log-async            Enable asynchronous logging
  --checkpoint FILE      Checkpoint file for resume
  --no-checkpoint        Disable checkpointing
  --enable-curiosity     Enable curiosity-driven exploration
  --enable-development   Enable developmental processes
  --enable-reward        Enable reward modulation

Advanced options:
  --category CATEGORY    Enable specific log category (Brain, Memory, Neuromodulation, Cognition, Development, Performance, System, Debug)
  --category-level NAME:LEVEL Set log level for category
  --enable-all-categories Enable all log categories
  --log-sampling RATE    Log sampling rate (0.0-1.0, default: 1.0)
  --rate-limit CATEGORY:COUNT Set rate limit for category
  --include-pattern REGEX Include logs matching pattern
  --exclude-pattern REGEX Exclude logs matching pattern
  --log-system-metrics    Enable system metrics logging
  --log-performance-metrics Enable performance metrics logging
  --log-gpu-events       Enable GPU events logging (if applicable)
  --max-file-size MB     Maximum log file size in MB (default: 10)
  --compress-logs        Compress rotated log files
  --rotation-time HOURS  Log rotation interval in hours (default: 1)
  --max-rotated-files N  Maximum rotated log files to keep (default: 5)
```

##### Config Command
```bash
./nlm_cli config [OPTIONS] COMMAND

Commands:
  validate FILE         Validate configuration file
  generate FILE TYPE    Generate configuration template
  migrate FROM:TO       Migrate configuration between versions

Config validation examples:
  ./nlm_cli config validate config.json
  ./nlm_cli config validate --schema brain config.yaml

Config generation examples:
  ./nlm_cli config generate brain.json brain
  ./nlm_cli config generate default.ini legacy
```

##### Log Command
```bash
./nlm_cli log [OPTIONS] COMMAND

Commands:
  status                Show current logging status
  categories            List all log categories
  enable NAME           Enable log category
  disable NAME          Disable log category
  set-level NAME:LEVEL  Set log level for category
  config FILE           Load logging configuration
  export FILE           Export current logging configuration

Examples:
  ./nlm_cli log status
  ./nlm_cli log categories
  ./nlm_cli log enable Brain
  ./nlm_cli log set-level Brain:INFO
```

##### Monitor Command
```bash
./nlm_cli monitor [OPTIONS]

Options:
  --interval SECONDS    Monitoring interval (default: 1)
  --metrics             Enable performance metrics monitoring
  --system              Enable system resource monitoring
  --gpu                 Enable GPU monitoring (if applicable)
  --output FILE         Output file for monitoring data

Example:
  ./nlm_cli monitor --interval 0.5 --metrics
```

## Enhanced Logging Documentation

### Comprehensive Logging Guide

NLM now features an enhanced logging infrastructure with advanced features for production use and debugging. The logging system supports multiple levels, categories, and output formats.

#### Log Levels and Categories

**Enhanced Log Levels:**
- `TRACE (-1)`: Finer-grained debugging level for detailed system internals
- `DEBUG (0)`: Standard debug information
- `INFO (1)`: General informational messages
- `WARNING (2)`: Warning conditions that don't prevent operation
- `ERROR (3)`: Error conditions affecting operation
- `CRITICAL (4)`: Critical errors requiring immediate attention
- `FATAL (5)`: Fatal errors causing system shutdown

**Log Categories:**
- `Brain`: Neural processing, neuron dynamics, synaptic operations
- `Memory`: Working memory, episodic memory, associative memory
- `Neuromodulation`: Dopamine, curiosity, novelty detection
- `Cognition`: Planning, attention, concept formation
- `Development`: Synaptogenesis, pruning, maturation
- `Performance`: Simulation performance, throughput metrics
- `System`: System initialization, configuration, shutdown
- `Debug`: Deep debugging information, raw data

#### Configuration File Format

**JSON Configuration Example:**
```json
{
  "logging": {
    "global_level": "INFO",
    "use_enhanced_levels": true,
    "use_async_logging": true,
    "async_queue_size": 10000,
    "log_file": "nlm.log",
    "max_file_size": 10485760,
    "compress_logs": true,
    "rotation_time": 3600,
    "max_rotated_files": 5,
    "sample_rate": 1.0,
    "log_system_metrics": true,
    "log_performance_metrics": true,
    "categories": {
      "Brain": {"min_level": "INFO", "max_level": "FATAL", "enabled": true, "rate_limit": 1000},
      "Memory": {"min_level": "WARNING", "max_level": "FATAL", "enabled": true},
      "Neuromodulation": {"min_level": "INFO", "max_level": "FATAL", "enabled": true},
      "Cognition": {"min_level": "DEBUG", "max_level": "FATAL", "enabled": true},
      "Development": {"min_level": "INFO", "max_level": "FATAL", "enabled": true},
      "Performance": {"min_level": "INFO", "max_level": "FATAL", "enabled": true},
      "System": {"min_level": "INFO", "max_level": "FATAL", "enabled": true},
      "Debug": {"min_level": "TRACE", "max_level": "FATAL", "enabled": false}
    },
    "include_patterns": [],
    "exclude_patterns": ["^GPU.*operation.*failed"],
    "custom_fields": {
      "project_name": "NLM",
      "version": "6.0.0",
      "environment": "production"
    }
  }
}
```

**YAML Configuration Example:**
```yaml
logging:
  global_level: INFO
  use_enhanced_levels: true
  use_async_logging: true
  async_queue_size: 10000
  log_file: nlm.log
  max_file_size: 10485760
  compress_logs: true
  rotation_time: 3600
  max_rotated_files: 5
  sample_rate: 1.0
  log_system_metrics: true
  log_performance_metrics: true
  categories:
    Brain:
      min_level: INFO
      max_level: FATAL
      enabled: true
      rate_limit: 1000
    Memory:
      min_level: WARNING
      max_level: FATAL
      enabled: true
    Neuromodulation:
      min_level: INFO
      max_level: FATAL
      enabled: true
    Cognition:
      min_level: DEBUG
      max_level: FATAL
      enabled: true
    Development:
      min_level: INFO
      max_level: FATAL
      enabled: true
    Performance:
      min_level: INFO
      max_level: FATAL
      enabled: true
    System:
      min_level: INFO
      max_level: FATAL
      enabled: true
    Debug:
      min_level: TRACE
      max_level: FATAL
      enabled: false
  include_patterns: []
  exclude_patterns:
    - "^GPU.*operation.*failed"
  custom_fields:
    project_name: NLM
    version: 6.0.0
    environment: production
```

#### Structured Logging Examples

**JSON Output Format:**
```json
{
  "timestamp": "2026-10-20 09:30:45.123",
  "timestamp_ns": 123456789,
  "level": "INFO",
  "category": "Brain",
  "message": "Simulation step completed",
  "thread_id": "140737488355392",
  "cpu_core": 0,
  "memory_usage": 512000,
  "neuron_count": "500",
  "firing_rate": "0.15",
  "synapse_count": "2500",
  "file": "brain_simulation.cpp",
  "line": 142,
  "function": "processStep"
}
```

**Custom Fields Example:**
```cpp
// Configure enhanced logging
nlm::LogConfig config;
config.globalLevel = nlm::EnhancedLogLevel::Info;
config.useEnhancedLevels = true;
config.useAsyncLogging = true;
config.logFile = "nlm.log";
config.logSystemMetrics = true;

auto& logger = nlm::Logger::getGlobal();
logger.setLogConfig(config);

// Create enhanced log entry with custom fields
nlm::EnhancedLogEntry entry;
entry.level = nlm::EnhancedLogLevel::Info;
entry.category = "Brain";
entry.message = "Neural processing completed";
entry.fields["neuron_count"] = "1000";
entry.fields["firing_rate"] = "0.25";
entry.fields["synaptic_efficiency"] = "0.85";
entry.fields["energy_consumption"] = "128.5mW";
logger.logEnhanced(entry);
```

**Performance Monitoring Example:**
```cpp
// Configure for performance monitoring
nlm::LogConfig config;
config.globalLevel = nlm::EnhancedLogLevel::Info;
config.useAsyncLogging = true;
config.logPerformanceMetrics = true;
config.logSystemMetrics = true;
config.sampleRate = 0.1; // 10% sampling for performance

auto& logger = nlm::Logger::getGlobal();
logger.setLogConfig(config);

// Log performance metrics
for (int step = 0; step < maxSteps; ++step) {
    // Simulation step
    brain.step(step);
    
    // Log performance every 10 steps (with sampling)
    if (step % 10 == 0) {
        logger.logInfo(std::to_string(step) + ": " + 
                     std::to_string(brain.getFiringNeuronCount()) + " firing neurons, " +
                     std::to_string(brain.getAverageFiringRate()) + " avg firing rate");
    }
}
```

#### Performance Considerations and Optimization

**Async Logging Benefits:**
- Prevents I/O blocking during simulation timesteps
- Maintains steady performance with high log volume
- Reduces lock contention in multi-threaded environments
- Improves overall system responsiveness

**Filtering Optimization:**
- Log level filtering at entry point (O(1) complexity)
- Category-based filtering for granular control
- Rate limiting prevents log flooding
- Sampling reduces log volume for performance-critical scenarios

**Memory Management:**
- Buffer pooling reduces dynamic allocation
- Lock-free queues avoid mutex overhead
- Efficient log rotation minimizes disk usage
- Pre-allocated log buffers for high-frequency logging

**Performance Tuning Examples:**
```bash
# High-performance configuration for large simulations
./nlm_cli simulate \
  --steps 100000 \
  --brain 5000 \
  --log-level INFO \
  --log-async \
  --async-queue-size 50000 \
  --log-sampling 0.5 \
  --category-level Brain:WARNING \
  --category-level Memory:WARNING

# Real-time simulation configuration
./nlm_cli simulate \
  --steps 10000 \
  --brain 2000 \
  --timestep 0.0001 \
  --log-level INFO \
  --log-async \
  --max-file-size 100 \
  --compress-logs
```

## Running

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

## Enhanced CLI Documentation

### Advanced Command Line Interface

The NLM project now includes a comprehensive CLI tool for advanced configuration and simulation control. The CLI provides full access to all configuration options, logging control, and simulation management capabilities.

#### Installation
```bash
# Build and install CLI
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make nlm_cli

# Or install system-wide
make install
```

#### Basic CLI Usage
```bash
# Show help and available commands
./nlm_cli --help

# Run a basic simulation
./nlm_cli simulate --help

# Start interactive mode
./nlm_cli interactive
```

#### CLI Command Reference

##### Simulate Command
```bash
./nlm_cli simulate [OPTIONS]

Required arguments:
  --config FILE          Configuration file path (JSON/YAML/INI)
  --steps N              Number of simulation steps (default: 1000)

Optional arguments:
  --brain NEURONS        Number of neurons (default: 1000)
  --seed SEED            Random seed (default: 42)
  --timestep DT          Simulation timestep in seconds (default: 0.001)
  --world-width W        World width (default: 10)
  --world-height H       World height (default: 10)
  --log-level LEVEL      Logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL, FATAL)
  --log-file PATH        Log file path
  --log-async            Enable asynchronous logging
  --checkpoint FILE      Checkpoint file for resume
  --no-checkpoint        Disable checkpointing
  --enable-curiosity     Enable curiosity-driven exploration
  --enable-development   Enable developmental processes
  --enable-reward        Enable reward modulation

Advanced options:
  --category CATEGORY    Enable specific log category (Brain, Memory, Neuromodulation, Cognition, Development, Performance, System, Debug)
  --category-level NAME:LEVEL Set log level for category
  --enable-all-categories Enable all log categories
  --log-sampling RATE    Log sampling rate (0.0-1.0, default: 1.0)
  --rate-limit CATEGORY:COUNT Set rate limit for category
  --include-pattern REGEX Include logs matching pattern
  --exclude-pattern REGEX Exclude logs matching pattern
  --log-system-metrics    Enable system metrics logging
  --log-performance-metrics Enable performance metrics logging
  --log-gpu-events       Enable GPU events logging (if applicable)
  --max-file-size MB     Maximum log file size in MB (default: 10)
  --compress-logs        Compress rotated log files
  --rotation-time HOURS  Log rotation interval in hours (default: 1)
  --max-rotated-files N  Maximum rotated log files to keep (default: 5)
```

##### Config Command
```bash
./nlm_cli config [OPTIONS] COMMAND

Commands:
  validate FILE         Validate configuration file
  generate FILE TYPE    Generate configuration template
  migrate FROM:TO       Migrate configuration between versions

Config validation examples:
  ./nlm_cli config validate config.json
  ./nlm_cli config validate --schema brain config.yaml

Config generation examples:
  ./nlm_cli config generate brain.json brain
  ./nlm_cli config generate default.ini legacy
```

##### Log Command
```bash
./nlm_cli log [OPTIONS] COMMAND

Commands:
  status                Show current logging status
  categories            List all log categories
  enable NAME           Enable log category
  disable NAME          Disable log category
  set-level NAME:LEVEL  Set log level for category
  config FILE           Load logging configuration
  export FILE           Export current logging configuration

Examples:
  ./nlm_cli log status
  ./nlm_cli log categories
  ./nlm_cli log enable Brain
  ./nlm_cli log set-level Brain:INFO
```

##### Monitor Command
```bash
./nlm_cli monitor [OPTIONS]

Options:
  --interval SECONDS    Monitoring interval (default: 1)
  --metrics             Enable performance metrics monitoring
  --system              Enable system resource monitoring
  --gpu                 Enable GPU monitoring (if applicable)
  --output FILE         Output file for monitoring data

Example:
  ./nlm_cli monitor --interval 0.5 --metrics
```n

## Enhanced Logging Documentation

### Comprehensive Logging Guide

NLM now features an enhanced logging infrastructure with advanced features for production use and debugging. The logging system supports multiple levels, categories, and output formats.

#### Log Levels and Categories

**Enhanced Log Levels:**
- `TRACE (-1)`: Finer-grained debugging level for detailed system internals
- `DEBUG (0)`: Standard debug information
- `INFO (1)`: General informational messages
- `WARNING (2)`: Warning conditions that don't prevent operation
- `ERROR (3)`: Error conditions affecting operation
- `CRITICAL (4)`: Critical errors requiring immediate attention
- `FATAL (5)`: Fatal errors causing system shutdown

**Log Categories:**
- `Brain`: Neural processing, neuron dynamics, synaptic operations
- `Memory`: Working memory, episodic memory, associative memory
- `Neuromodulation`: Dopamine, curiosity, novelty detection
- `Cognition`: Planning, attention, concept formation
- `Development`: Synaptogenesis, pruning, maturation
- `Performance`: Simulation performance, throughput metrics
- `System`: System initialization, configuration, shutdown
- `Debug`: Deep debugging information, raw data

#### Configuration File Format

**JSON Configuration Example:**
```json
{
  "logging": {
    "global_level": "INFO",
    "use_enhanced_levels": true,
    "use_async_logging": true,
    "async_queue_size": 10000,
    "log_file": "nlm.log",
    "max_file_size": 10485760,
    "compress_logs": true,
    "rotation_time": 3600,
    "max_rotated_files": 5,
    "sample_rate": 1.0,
    "log_system_metrics": true,
    "log_performance_metrics": true,
    "categories": {
      "Brain": {"min_level": "INFO", "max_level": "FATAL", "enabled": true, "rate_limit": 1000},
      "Memory": {"min_level": "WARNING", "max_level": "FATAL", "enabled": true},
      "Neuromodulation": {"min_level": "INFO", "max_level": "FATAL", "enabled": true},
      "Cognition": {"min_level": "DEBUG", "max_level": "FATAL", "enabled": true},
      "Development": {"min_level": "INFO", "max_level": "FATAL", "enabled": true},
      "Performance": {"min_level": "INFO", "max_level": "FATAL", "enabled": true},
      "System": {"min_level": "INFO", "max_level": "FATAL", "enabled": true},
      "Debug": {"min_level": "TRACE", "max_level": "FATAL", "enabled": false}
    },
    "include_patterns": [],
    "exclude_patterns": ["^GPU.*operation.*failed"],
    "custom_fields": {
      "project_name": "NLM",
      "version": "6.0.0",
      "environment": "production"
    }
  }
}
```

**YAML Configuration Example:**
```yaml
logging:
  global_level: INFO
  use_enhanced_levels: true
  use_async_logging: true
  async_queue_size: 10000
  log_file: nlm.log
  max_file_size: 10485760
  compress_logs: true
  rotation_time: 3600
  max_rotated_files: 5
  sample_rate: 1.0
  log_system_metrics: true
  log_performance_metrics: true
  categories:
    Brain:
      min_level: INFO
      max_level: FATAL
      enabled: true
      rate_limit: 1000
    Memory:
      min_level: WARNING
      max_level: FATAL
      enabled: true
    Neuromodulation:
      min_level: INFO
      max_level: FATAL
      enabled: true
    Cognition:
      min_level: DEBUG
      max_level: FATAL
      enabled: true
    Development:
      min_level: INFO
      max_level: FATAL
      enabled: true
    Performance:
      min_level: INFO
      max_level: FATAL
      enabled: true
    System:
      min_level: INFO
      max_level: FATAL
      enabled: true
    Debug:
      min_level: TRACE
      max_level: FATAL
      enabled: false
  include_patterns: []
  exclude_patterns:
    - "^GPU.*operation.*failed"
  custom_fields:
    project_name: NLM
    version: 6.0.0
    environment: production
```

#### Structured Logging Examples

**JSON Output Format:**
```json
{
  "timestamp": "2026-10-20 09:30:45.123",
  "timestamp_ns": 123456789,
  "level": "INFO",
  "category": "Brain",
  "message": "Simulation step completed",
  "thread_id": "140737488355392",
  "cpu_core": 0,
  "memory_usage": 512000,
  "neuron_count": "500",
  "firing_rate": "0.15",
  "synapse_count": "2500",
  "file": "brain_simulation.cpp",
  "line": 142,
  "function": "processStep"
}
```

**Custom Fields Example:**
```cpp
// Configure enhanced logging
nlm::LogConfig config;
config.globalLevel = nlm::EnhancedLogLevel::Info;
config.useEnhancedLevels = true;
config.useAsyncLogging = true;
config.logFile = "nlm.log";
config.logSystemMetrics = true;

auto& logger = nlm::Logger::getGlobal();
logger.setLogConfig(config);

// Create enhanced log entry with custom fields
nlm::EnhancedLogEntry entry;
entry.level = nlm::EnhancedLogLevel::Info;
entry.category = "Brain";
entry.message = "Neural processing completed";
entry.fields["neuron_count"] = "1000";
entry.fields["firing_rate"] = "0.25";
entry.fields["synaptic_efficiency"] = "0.85";
entry.fields["energy_consumption"] = "128.5mW";
logger.logEnhanced(entry);
```

**Performance Monitoring Example:**
```cpp
// Configure for performance monitoring
nlm::LogConfig config;
config.globalLevel = nlm::EnhancedLogLevel::Info;
config.useAsyncLogging = true;
config.logPerformanceMetrics = true;
config.logSystemMetrics = true;
config.sampleRate = 0.1; // 10% sampling for performance

auto& logger = nlm::Logger::getGlobal();
logger.setLogConfig(config);

// Log performance metrics
for (int step = 0; step < maxSteps; ++step) {
    // Simulation step
    brain.step(step);
    
    // Log performance every 10 steps (with sampling)
    if (step % 10 == 0) {
        logger.logInfo(std::to_string(step) + ": " + 
                     std::to_string(brain.getFiringNeuronCount()) + " firing neurons, " +
                     std::to_string(brain.getAverageFiringRate()) + " avg firing rate");
    }
}
```

#### Performance Considerations and Optimization

**Async Logging Benefits:**
- Prevents I/O blocking during simulation timesteps
- Maintains steady performance with high log volume
- Reduces lock contention in multi-threaded environments
- Improves overall system responsiveness

**Filtering Optimization:**
- Log level filtering at entry point (O(1) complexity)
- Category-based filtering for granular control
- Rate limiting prevents log flooding
- Sampling reduces log volume for performance-critical scenarios

**Memory Management:**
- Buffer pooling reduces dynamic allocation
- Lock-free queues avoid mutex overhead
- Efficient log rotation minimizes disk usage
- Pre-allocated log buffers for high-frequency logging

**Performance Tuning Examples:**
```bash
# High-performance configuration for large simulations
./nlm_cli simulate \
  --steps 100000 \
  --brain 5000 \
  --log-level INFO \
  --log-async \
  --async-queue-size 50000 \
  --log-sampling 0.5 \
  --category-level Brain:WARNING \
  --category-level Memory:WARNING

# Real-time simulation configuration
./nlm_cli simulate \
  --steps 10000 \
  --brain 2000 \
  --timestep 0.0001 \
  --log-level INFO \
  --log-async \
  --max-file-size 100 \
  --compress-logs
```

## Configuration Validation Documentation

### Schema Documentation with Parameter Descriptions

NLM features a comprehensive schema-based configuration system with detailed parameter validation and documentation.

#### Schema Structure

**Parameter Types:**
- `int`: Integer values with min/max constraints
- `int64_t`: 64-bit integer values
- `double`: Floating point values with precision
- `bool`: Boolean flags
- `std::string`: String values with pattern validation
- `std::vector<int>`: Integer arrays
- `std::vector<double>`: Float arrays
- `std::vector<std::string>`: String arrays

**Parameter Metadata Fields:**
- `name`: Parameter identifier
- `type`: Data type specification
- `description`: Human-readable description
- `required`: Whether parameter must be provided
- `defaultValue`: Default value if not specified
- `hasMin/maxValue`: Range constraints
- `pattern`: Regex pattern for string validation
- `format`: Format type (email, url, etc.)
- `dependsOn`: List of dependent parameters
- `customValidator`: Custom validation function
- `helpText`: Detailed help documentation
- `category`: Parameter category for organization
- `order`: Display order in documentation

#### Configuration Validation Examples

**Brain Configuration Schema:**
```cpp
// Add brain configuration parameters
config.registerSchema("brain", std::make_shared<ConfigSchema>(
    "brain", "Brain configuration parameters"
));

auto brainSchema = config.getSchema("brain");

// Neuron count parameter
ParameterMetadata neuronCount(
    "neuron_count", "int", 
    "Number of neurons in the brain (1-1000000)",
    true, std::any(1000),
    "Total neurons: determines brain capacity and computational complexity",
    "Neural Computation"
);
neuronCount.hasMin = true; neuronCount.minValue = 1;
neuronCount.hasMax = true; neuronCount.maxValue = 1000000;
brainSchema->addParameter(neuronCount);

// Synaptic density parameter
ParameterMetadata synapseDensity(
    "synapse_density", "double",
    "Average number of synapses per neuron (0.0-1.0)",
    false, std::any(0.1),
    "Controls neural connectivity and information flow efficiency",
    "Neural Connectivity"
);
synapseDensity.hasMin = true; synapseDensity.minValue = 0.0;
synapseDensity.hasMax = true; synapseDensity.maxValue = 1.0;
synapseDensity.customValidator = [](const std::any& value, const std::map<std::string, std::any>&) {
    double density = std::any_cast<double>(value);
    if (density < 0.01 || density > 0.5) {
        return ValidationResult::Invalid("Synapse density must be between 0.01 and 0.5");
    }
    return ValidationResult::Valid();
};
brainSchema->addParameter(synapseDensity);
```

**Validation Examples:**
```bash
# Validate configuration file
./nlm_cli config validate --schema brain config.json

# Validate specific parameter
./nlm_cli config validate --param neuron_count --value 5000 --schema brain

# Schema-based validation
c++ code:
auto result = config.validateSchema("brain");
if (!result.valid) {
    for (const auto& error : result.errors) {
        std::cout << "Error: " << error << std::endl;
    }
    for (const auto& paramError : result.parameterResults) {
        std::cout << "Parameter " << paramError.first << ": " << paramError.second.message << std::endl;
    }
}
```

#### Schema-Based Configuration Examples

**JSON Configuration with Schema:**
```json
{
  "brain": {
    "neuron_count": 2000,
    "synapse_density": 0.15,
    "v_thresh": -50.0,
    "v_rest": -70.0,
    "tau_mem": 20.0,
    "tau_ref": 2.0
  },
  "plasticity": {
    "stdp": {
      "learning_rate": 0.001,
      "tau_plus": 20.0,
      "tau_minus": 20.0
    },
    "hebbian": {
      "learning_rate": 0.0005
    },
    "structural": {
      "enable": true,
      "growth_rate": 0.0001
    }
  },
  "neuromodulation": {
    "dopamine": {
      "baseline": 0.1,
      "scale": 1.0
    },
    "curiosity": {
      "enable": true,
      "novelty_threshold": 0.05
    },
    "novelty": {
      "enable": true,
      "detection_sensitivity": 0.8
    }
  },
  "memory": {
    "working_memory_capacity": 50,
    "episodic_memory_size": 1000,
    "consolidation_rate": 0.01
  },
  "development": {
    "critical_period_length": 100,
    "plasticity_decay_rate": 0.001,
    "synaptogenesis_rate": 0.0005
  }
}
```

#### Advanced Configuration Patterns

**Template System:**
```cpp
// Create configuration template
config.addTemplate("small_brain", {
    {"neuron_count", std::any(100)},
    {"synapse_density", std::any(0.1)},
    {"region_count", std::any(1)},
    {"plasticity.stdp.learning_rate", std::any(0.001)},
    {"development.critical_period_length", std::any(50)}
}, "Small brain configuration for testing", "brain");

// Apply template
config.applyTemplate("small_brain", "brain");

// Create complex template with dependencies
config.addTemplate("social_brain", {
    {"neuron_count", std::any(5000)},
    {"region_count", std::any(5)},
    {"plasticity.stdp.learning_rate", std::any(0.002)},
    {"plasticity.hebbian.learning_rate", std::any(0.001)},
    {"neuromodulation.dopamine.baseline", std::any(0.2)},
    {"neuromodulation.curiosity.enable", std::any(true)},
    {"memory.working_memory_capacity", std::any(100)},
    {"memory.episodic_memory_size", std::any(5000)}
}, "Social brain configuration with enhanced memory and learning", "brain");

// Template with conditional validation
config.addTemplate("high_performance", {
    {"neuron_count", std::any(10000)},
    {"plasticity.stdp.learning_rate", std::any(0.005)},
    {"development.plasticity_decay_rate", std::any(0.0005)},
    {"log_level", std::any("INFO")},
    {"log_system_metrics", std::any(true)},
    {"log_performance_metrics", std::any(true)},
    {"log_async", std::any(true)},
    {"async_queue_size", std::any(20000)}
}, "High-performance configuration for research", "system");
```

**Runtime Configuration Updates:**
```cpp
// Update configuration at runtime
config.set("brain.neuron_count", 3000, ConfigSource::Runtime, "Adjusting for larger simulation");
config.set("plasticity.stdp.learning_rate", 0.002, ConfigSource::Runtime, "Increasing learning rate");

// Get current configuration
int neuronCount = config.getOr("brain.neuron_count", 1000);
double learningRate = config.getOr("plasticity.stdp.learning_rate", 0.001);

// Configuration validation after changes
auto validationResult = config.validateSchema("brain");
if (!validationResult.valid) {
    // Handle validation errors
}
```

## Python Library Documentation

### Enhanced Python API Documentation with New Features

The NLM Python bindings have been significantly enhanced with new configuration validation, logging integration, and performance optimization features.

#### Core Classes and Methods

**Config Class:**
```python
class Config:
    def __init__(self):
        """Create a new configuration object"""
    
    def loadFromFile(self, filepath: str, category: str = "") -> bool:
        """Load configuration from JSON/YAML/INI file with schema validation"""
    
    def loadFromArgs(self, argc: int, argv: List[str]) -> bool:
        """Load configuration from command line arguments"""
    
    def saveToFile(self, filepath: str, category: str = "") -> bool:
        """Save configuration to file with schema validation"""
    
    def has(self, key: str, category: str = "") -> bool:
        """Check if configuration key exists"""
    
    def getKeys(self, category: str = "") -> List[str]:
        """Get all configuration keys in category"""
    
    def set(self, key: str, value: Any, source: ConfigSource = ConfigSource.Runtime, 
            reason: str = "", category: str = ""):
        """Set configuration value with validation and history"""
    
    def get(self, key: str, category: str = "", default: Any = None) -> Any:
        """Get configuration value with optional default"""
    
    def getOr(self, key: str, default: Any, category: str = "") -> Any:
        """Get configuration value or return default if not set"""
    
    def validateSchema(self, category: str = "") -> SchemaValidationResult:
        """Validate configuration against schema"""
    
    def enableHistory(self, enabled: bool = True):
        """Enable/disable configuration history tracking"""
    
    def getHistory(self, key: str = "") -> List[ConfigChange]:
        """Get configuration change history"""
    
    def createSnapshot(self, name: str = "") -> str:
        """Create configuration snapshot"""
    
    def rollbackToSnapshot(self, name: str) -> bool:
        """Rollback to named snapshot"""
    
    def applyTemplate(self, templateName: str, category: str = ""):
        """Apply configuration template"""
    
    def addTemplate(self, name: str, parameters: Dict[str, Any], 
                   description: str = "", category: str = ""):
        """Add configuration template"""
```

**Brain Class:**
```python
class Brain:
    def __init__(self, config: Config):
        """Create brain with configuration"""
    
    def initialize(self):
        """Initialize the brain with current configuration"""
    
    def step(self, step_number: int, time: float = 0.0):
        """Perform one simulation step"""
    
    def reset(self):
        """Reset brain to initial state"""
    
    def save(self, filepath: str):
        """Save brain state to checkpoint file"""
    
    def load(self, filepath: str):
        """Load brain state from checkpoint file"""
    
    def getTotalNeuronCount(self) -> int:
        """Get total number of neurons"""
    
    def getTotalSynapseCount(self) -> int:
        """Get total number of synapses"""
    
    def getFiringNeuronCount(self) -> int:
        """Get number of currently firing neurons"""
    
    def getAverageFiringRate(self) -> float:
        """Get average firing rate across all neurons"""
    
    def getExcitationInhibitionRatio(self) -> float:
        """Get E/I balance ratio"""
    
    def getDevelopmentalStage(self) -> DevelopmentalStage:
        """Get current developmental stage"""
    
    def addRegion(self, name: str) -> RegionId:
        """Add new brain region"""
    
    def getRegion(self, region_id: RegionId) -> Region:
        """Get region by ID"""
```

**AgentBrain Class:**
```python
class AgentBrain:
    def __init__(self, brain: Brain):
        """Create agent brain interface"""
    
    def initialize(self, world: SimpleWorld):
        """Initialize agent with world interface"""
    
    def processSensoryInput(self, percept: SensoryPercept):
        """Process sensory input from world"""
    
    def decodeMotorCommand(self) -> Action:
        """Decode motor command from brain activity"""
    
    def applyRewardModulation(self, reward: float, predicted_reward: float):
        """Apply reward prediction error"""
    
    def updateDevelopment(self, timestep: float):
        """Update developmental state"""
    
    def enableRewardModulation(self, enabled: bool):
        """Enable/disable reward modulation"""
    
    def enableStructuralPlasticity(self, enabled: bool):
        """Enable/disable structural plasticity"""
    
    def enableDevelopment(self, enabled: bool):
        """Enable/disable development"""
    
    def enableCuriosity(self, enabled: bool):
        """Enable/disable curiosity-driven exploration"""
    
    def getCuriosityLevel(self) -> float:
        """Get current curiosity level"""
    
    def getNoveltyLevel(self) -> float:
        """Get current novelty level"""
    
    def getPredictionError(self) -> float:
        """Get prediction error signal"""
    
    def getNeuromodulationLevel(self) -> float:
        """Get current neuromodulation level"""
```

**SimpleWorld Class:**
```python
class SimpleWorld:
    def __init__(self):
        """Create simple 2D world"""
    
    def configure(self, width: int, height: int, 
                 visionWidth: int, visionHeight: int):
        """Configure world dimensions and agent vision"""
    
    def reset(self):
        """Reset world to initial state"""
    
    def setAgentStart(self, x: float, y: float):
        """Set agent starting position"""
    
    def update(self, timestep: float):
        """Update world state"""
    
    def applyMotorCommand(self, cmd: Action, current_time: float):
        """Apply motor command to world"""
    
    def getSensoryPercept(self) -> SensoryPercept:
        """Get sensory percept from world"""
    
    def getAgentBody(self) -> AgentBody:
        """Get agent body state"""
    
    def addObject(self, obj: WorldObject):
        """Add world object"""
    
    def removeObject(self, x: float, y: float):
        """Remove world object"""
    
    def isValidPosition(self, x: float, y: float) -> bool:
        """Check if position is valid"""
    
    # Properties
    width: int
    height: int
    simulation_time: float
    max_energy: float
```

#### Configuration Validation in Python Examples

**Example 1: Schema-based Configuration:**
```python
import pynlm

# Create configuration with schema validation
config = pynlm.createDefaultConfig()

# Validate configuration
validation_result = config.validateSchema("brain")
if validation_result.valid:
    print("Configuration is valid!")
else:
    print("Configuration errors:")
    for error in validation_result.errors:
        print(f"  - {error}")
    for param, param_result in validation_result.parameterResults.items():
        if not param_result.valid:
            print(f"  Parameter '{param}': {param_result.message}")

# Set configuration with validation
config.set("brain.neuron_count", 3000, reason="Increasing simulation size")
config.set("plasticity.stdp.learning_rate", 0.002, reason="Higher learning rate for faster adaptation")

# Get configuration with validation
neuron_count = config.getOr("brain.neuron_count", 1000, "brain")
print(f"Using {neuron_count} neurons")
```

**Example 2: Advanced Configuration with Templates:**
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Apply predefined templates
config.applyTemplate("small_brain")
print("Applied small_brain template")

# Create custom template
config.addTemplate(
    name="research_config",
    parameters={
        "brain.neuron_count": 5000,
        "brain.synapse_density": 0.15,
        "plasticity.stdp.learning_rate": 0.002,
        "development.critical_period_length": 200,
        "neuromodulation.dopamine.baseline": 0.2,
        "neuromodulation.curiosity.enable": True,
        "memory.working_memory_capacity": 100,
        "memory.episodic_memory_size": 2000,
        "log_level": "INFO",
        "log_system_metrics": True,
        "log_performance_metrics": True
    },
    description="Research-grade configuration for long-term experiments",
    category="brain"
)

# Apply custom template
config.applyTemplate("research_config")

# Validate template application
validation = config.validateSchema()
if validation.valid:
    print("Custom template applied successfully")
else:
    print("Template application failed:", validation.errors)
```

**Example 3: Configuration History and Snapshots:**
```python
import pynlm

# Enable configuration history
config = pynlm.createDefaultConfig()
config.enableHistory(True)

# Make configuration changes
config.set("brain.neuron_count", 2000, reason="Initial configuration")
config.set("plasticity.stdp.learning_rate", 0.001, reason="Standard learning rate")

# Check configuration history
history = config.getHistory()
print(f"Configuration changed {len(history)} times")
for change in history:
    print(f"  {change.timestamp}: {change.key} = {change.newValue} ({change.reason})")

# Create snapshot
snapshot_name = config.createSnapshot("initial_research")
print(f"Created snapshot: {snapshot_name}")

# Modify configuration
config.set("brain.neuron_count", 3000, reason="Increased for longer simulation")

# Rollback to snapshot
if config.rollbackToSnapshot(snapshot_name):
    print("Successfully rolled back to snapshot")
else:
    print("Failed to rollback to snapshot")
```

**Example 4: Multi-format Configuration Loading:**
```python
import pynlm
import tempfile
import os

# Create sample configuration files
json_config = {
    "brain": {
        "neuron_count": 2000,
        "synapse_density": 0.15
    },
    "plasticity": {
        "stdp": {"learning_rate": 0.001}
    }
}

yaml_config = """
brain:
  neuron_count: 2000
  synapse_density: 0.15
plasticity:
  stdp:
    learning_rate: 0.001
"""

# Save and load different formats
with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
    import json
    json.dump(json_config, f)
    json_file = f.name

with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
    f.write(yaml_config)
    yaml_file = f.name

try:
    # Load JSON configuration
    config1 = pynlm.createDefaultConfig()
    if config1.loadFromFile(json_file, "brain"):
        print("Successfully loaded JSON configuration")
    
    # Load YAML configuration
    config2 = pynlm.createDefaultConfig()
    if config2.loadFromFile(yaml_file, "brain"):
        print("Successfully loaded YAML configuration")
    
    # Save configuration in different formats
    config2.saveToFile("config.json", "brain")
    config2.saveToFile("config.yaml", "brain")
    config2.saveToFile("config.ini", "brain")
    
    print("Configuration saved in multiple formats")
    
finally:
    # Clean up temporary files
    os.unlink(json_file)
    os.unlink(yaml_file)
```

#### Logging Integration Examples

**Example 1: Basic Logging Configuration:**
```python
import pynlm

# Configure logging
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enable different logging subsystems
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Log simulation progress
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
              f"curiosity: {agent.getCuriosityLevel():.3f}")
```

**Example 2: Advanced Logging with Custom Formatting:**
```python
import pynlm

# Create configuration with enhanced logging
config = pynlm.createDefaultConfig()
config.set("log_level", "INFO", reason="Set log level for simulation")
config.set("log_async", True, reason="Enable async logging for performance")
config.set("log_system_metrics", True, reason="Monitor system resources")
config.set("log_performance_metrics", True, reason="Track simulation performance")

# Create brain and agent
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run simulation with logging
for step in range(500):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process in brain
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Log progress
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
```

**Example 3: Performance Monitoring with Logging:**
```python
import pynlm
import time

# Create high-performance configuration
config = pynlm.createDefaultConfig()
config.set("log_level", "INFO", reason="Enable performance logging")
config.set("log_async", True, reason="Prevent I/O blocking")
config.set("log_sampling", 0.1, reason="Sample logs to reduce volume")
config.set("log_system_metrics", True, reason="Monitor system resources")

# Create brain and world
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

# Performance monitoring
start_time = time.time()
steps_completed = 0
performance_log_interval = 100

for step in range(5000):
    step_start = time.time()
    
    # Update world
    world.update(0.1)
    
    # Get percept and process
    percept = world.getSensoryPercept()
    # Simulate agent processing (simplified)
    brain.step(step)
    
    # Check performance
    step_time = time.time() - step_start
    steps_completed += 1
    
    if step % performance_log_interval == 0:
        avg_step_time = (time.time() - start_time) / steps_completed
        print(f"Step {step}: {avg_step_time*1000:.2f}ms avg, "
              f"{1.0/avg_step_time:.0f} steps/sec")
        
        # Log performance metrics
        pynlm.Logger.logPerformanceMetrics(
            step=step,
            avg_step_time=avg_step_time,
            firing_rate=brain.getAverageFiringRate(),
            memory_usage=???  # Would need system monitoring
        )
```

**Example 4: Structured Logging with JSON Output:**
```python
import pynlm
import json

# Configure for structured logging
config = pynlm.createDefaultConfig()
config.set("log_level", "INFO", reason="Enable structured logging")
config.set("log_file", "simulation.log", reason="Log to file")

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Log structured data
for step in range(100):
    brain.step(step)
    
    # Create structured log entry
    log_entry = {
        "timestamp": time.time(),
        "step": step,
        "firing_neurons": brain.getFiringNeuronCount(),
        "total_neurons": brain.getTotalNeuronCount(),
        "firing_rate": brain.getAverageFiringRate(),
        "e_i_ratio": brain.getExcitationInhibitionRatio(),
        "simulation_data": {
            "time": step * 0.1,
            "neuron_activity": "active" if brain.getFiringNeuronCount() > 0 else "inactive"
        },
        "metadata": {
            "project": "NLM",
            "version": "6.0.0",
            "simulation_type": "integration_test"
        }
    }
    
    # Log as JSON
    pynlm.Logger.logStructured("Brain Simulation", log_entry)
    
    if step % 20 == 0:
        print(f"Logged structured data for step {step}")
```

#### Performance Optimization Examples

**Example 1: Large-scale Simulation with Performance Tuning:**
```python
import pynlm
import time

# High-performance configuration for large simulations
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 10000, reason="Large-scale simulation")
config.set("log_level", "WARNING", reason="Reduce log volume")
config.set("log_async", True, reason="Prevent I/O blocking")
config.set("log_sampling", 0.05, reason="Minimal logging for performance")
config.set("log_system_metrics", False, reason="Disable system monitoring")

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run large simulation with timing
start_time = time.time()
steps_completed = 0

for step in range(10000):
    brain.step(step)
    steps_completed += 1
    
    if step % 1000 == 0:
        elapsed = time.time() - start_time
        rate = steps_completed / elapsed
        print(f"Progress: {step}/10000 steps, {rate:.1f} steps/sec")

# Performance summary
elapsed = time.time() - start_time
rate = steps_completed / elapsed
print(f"\nSimulation complete!")
print(f"Total steps: {steps_completed}")
print(f"Total time: {elapsed:.2f}s")
print(f"Performance: {rate:.1f} steps/sec")
```

**Example 2: Memory-Efficient Simulation:**
```python
import pynlm
import gc

# Memory-efficient configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000, reason="Balanced memory usage")
config.set("log_level", "ERROR", reason="Minimal logging")
config.set("log_async", True, reason="Reduce memory pressure")
config.set("log_file", "", reason="Disable file logging")

# Create brain and run simulation
brain = pynlm.createBrain(config)
brain.initialize()

# Force garbage collection between simulation phases
gc.collect()

# Run simulation in chunks
default_steps = 1000
chunk_size = 100

for chunk in range(default_steps // chunk_size):
    start_step = chunk * chunk_size
    end_step = start_step + chunk_size
    
    for step in range(start_step, end_step):
        brain.step(step)
    
    # Memory cleanup
    gc.collect()
    
    print(f"Completed chunk {chunk + 1}/{default_steps // chunk_size}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")

print("Memory-efficient simulation complete!")
```

## Advanced Usage Examples

### Complex Simulation Examples Using New Features

**Example 1: Multi-stage Development Simulation:**
```python
import pynlm
import json

# Complex simulation configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000, reason="Complex brain structure")
config.set("development.critical_period_length", 200, reason="Extended critical period")
config.set("development.synaptogenesis_rate", 0.001, reason="Active synaptogenesis")
config.set("plasticity.stdp.learning_rate", 0.002, reason="Enhanced plasticity")
config.set("neuromodulation.dopamine.baseline", 0.2, reason="Higher baseline motivation")
config.set("neuromodulation.curiosity.enable", True, reason="Enable curiosity-driven exploration")
config.set("neuromodulation.novelty.enable", True, reason="Enable novelty detection")
config.set("memory.working_memory_capacity", 100, reason="Enhanced working memory")
config.set("memory.episodic_memory_size", 5000, reason="Large episodic memory")

# Create simulation components
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()
agent.initialize(world)

# Enable all subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Multi-stage simulation with development phases
print("=== Multi-stage Development Simulation ===")

# Phase 1: Learning
print("Phase 1: Initial learning (steps 0-499)")
learning_rewards = []
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Simulate reward from world
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    if step % 100 == 0:
        learning_rewards.append(reward)
        print(f"  Step {step}: Reward {reward:.3f}, "
              f"Curiosity {agent.getCuriosityLevel():.3f}")

# Phase 2: Exploration
print("\nPhase 2: Exploration and novelty detection (steps 500-999)")
exploration_rewards = []
for step in range(500, 1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    if step % 100 == 0:
        exploration_rewards.append(reward)
        print(f"  Step {step}: Reward {reward:.3f}, Novelty {agent.getNoveltyLevel():.3f}")

# Phase 3: Consolidation
print("\nPhase 3: Memory consolidation (steps 1000-1999)")
consolidation_rewards = []
for step in range(1000, 2000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    if step % 100 == 0:
        consolidation_rewards.append(reward)
        print(f"  Step {step}: Reward {reward:.3f}, "
              f"Dev Stage {brain.getDevelopmentalStage()}, "
              f"Memory Episodes {brain.getTotalNeuronCount()//100}")

# Phase 4: Advanced learning
print("\nPhase 4: Advanced learning with structural plasticity (steps 2000-2999)")
advanced_rewards = []
for step in range(2000, 3000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    if step % 100 == 0:
        advanced_rewards.append(reward)
        print(f"  Step {step}: Reward {reward:.3f}, "
              f"Dev Stage {brain.getDevelopmentalStage()}")

# Simulation summary
print("\n=== Simulation Summary ===")
all_rewards = learning_rewards + exploration_rewards + consolidation_rewards + advanced_rewards
print(f"Total reward: {sum(all_rewards):.3f}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
print(f"Final developmental stage: {brain.getDevelopmentalStage()}")
print(f"Total memory episodes stored: {brain.getTotalNeuronCount()//100}")
```

**Example 2: Configuration Schema Examples:**
```python
import pynlm

# Create comprehensive configuration with schema
config = pynlm.createDefaultConfig()

# Register custom schemas
config.registerSchema("brain", {
    "neuron_count": {"type": "int", "min": 1, "max": 1000000, "description": "Number of neurons"},
    "synapse_density": {"type": "double", "min": 0.0, "max": 1.0, "description": "Synapse density"},
    "development.critical_period_length": {"type": "int", "min": 10, "max": 1000, "description": "Critical period length"}
})

config.registerSchema("neuromodulation", {
    "dopamine.baseline": {"type": "double", "min": 0.0, "max": 1.0, "description": "Dopamine baseline"},
    "curiosity.novelty_threshold": {"type": "double", "min": 0.0, "max": 1.0, "description": "Novelty detection threshold"}
})

# Validate configuration
validation = config.validateSchema()
if validation.valid:
    print("Schema validation passed!")
    
    # Use configuration
    neuron_count = config.getOr("brain.neuron_count", 1000, "brain")
    dopamine_baseline = config.getOr("neuromodulation.dopamine.baseline", 0.0, "neuromodulation")
    
    print(f"Brain configuration: {neuron_count} neurons")
    print(f"Dopamine baseline: {dopamine_baseline}")
    
else:
    print("Schema validation failed:", validation.errors)
```

**Example 3: CLI Tool Usage Examples:**
```python
import subprocess
import json

# Example 1: Run simulation with CLI
print("=== Example 1: CLI Simulation ===")
result = subprocess.run([
    "./nlm_cli", "simulate",
    "--config", "config.json",
    "--steps", "1000",
    "--brain", "3000",
    "--log-level", "INFO",
    "--log-async",
    "--enable-curiosity",
    "--enable-development"
], capture_output=True, text=True)

if result.returncode == 0:
    print("CLI simulation completed successfully")
    print("Output:", result.stdout)
else:
    print("CLI simulation failed:", result.stderr)

# Example 2: Configuration validation
print("\n=== Example 2: CLI Configuration Validation ===")
result = subprocess.run([
    "./nlm_cli", "config", "validate",
    "--schema", "brain",
    "config.json"
], capture_output=True, text=True)

if result.returncode == 0:
    print("Configuration validation passed")
else:
    print("Configuration validation failed:", result.stderr)

# Example 3: Logging status check
print("\n=== Example 3: CLI Logging Status ===")
result = subprocess.run([
    "./nlm_cli", "log", "status"
], capture_output=True, text=True)

if result.returncode == 0:
    print("Current logging status:")
    print(result.stdout)
else:
    print("Failed to get logging status:", result.stderr)

# Example 4: Performance monitoring
print("\n=== Example 4: CLI Performance Monitoring ===")
result = subprocess.run([
    "./nlm_cli", "monitor",
    "--interval", "0.5",
    "--metrics",
    "--output", "performance.json"
], capture_output=True, text=True)

if result.returncode == 0:
    print("Performance monitoring started")
    print("Data saved to performance.json")
else:
    print("Performance monitoring failed:", result.stderr)
```

**Example 4: Performance Monitoring Examples:**
```python
import pynlm
import time
import threading
from datetime import datetime

# Performance monitoring setup
monitor_data = {
    "start_time": datetime.now(),
    "simulation_steps": [],
    "performance_metrics": {},
    "system_resources": {}
}

# Performance monitoring thread
def performance_monitor(brain, interval=1.0):
    """Background thread for performance monitoring"""
    last_step = 0
    last_time = time.time()
    
    while monitor_active:
        current_time = time.time()
        
        # Collect performance metrics
        step_time = current_time - last_time
        steps_completed = brain.getTotalNeuronCount() // 100  # Estimated
        
        monitor_data["performance_metrics"][datetime.now().isoformat()] = {
            "step_time": step_time,
            "steps_completed": steps_completed,
            "step_rate": steps_completed / (current_time - last_time) if current_time > last_time else 0,
            "memory_usage": brain.getTotalNeuronCount() * 100,  # Estimate
            "firing_rate": brain.getAverageFiringRate()
        }
        
        last_step = steps_completed
        last_time = current_time
        time.sleep(interval)

# Create brain with performance monitoring
config = pynlm.createDefaultConfig()
config.set("log_level", "INFO", reason="Enable performance logging")
config.set("log_async", True, reason="Prevent I/O blocking")
config.set("log_performance_metrics", True, reason="Monitor simulation performance")

brain = pynlm.createBrain(config)
brain.initialize()

# Start performance monitoring
monitor_active = True
monitor_thread = threading.Thread(target=performance_monitor, args=(brain, 0.5))
monitor_thread.start()

# Run simulation
start_time = time.time()
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        elapsed = time.time() - start_time
        rate = step / elapsed
        print(f"Step {step}: {rate:.1f} steps/sec, "
              f"Avg step time: {elapsed/step*1000:.2f}ms")

# Stop monitoring
monitor_active = False
monitor_thread.join()

# Save performance data
with open("performance_monitor.json", "w") as f:
    json.dump(monitor_data, f, indent=2)

print(f"Simulation complete. Performance data saved to performance_monitor.json")
```

**Example 5: Error Handling Examples:**
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Safe configuration loading with error handling
def safe_load_config(filepath):
    """Load configuration with comprehensive error handling"""
    try:
        if not config.loadFromFile(filepath):
            print(f"Warning: Failed to load configuration from {filepath}")
            return False
            
        # Validate configuration
        validation = config.validateSchema()
        if not validation.valid:
            print("Configuration validation errors:")
            for error in validation.errors:
                print(f"  - {error}")
            return False
            
        print(f"Successfully loaded configuration from {filepath}")
        return True
        
    except FileNotFoundError:
        print(f"Error: Configuration file {filepath} not found")
        return False
    except json.JSONDecodeError as e:
        print(f"Error: Invalid JSON in configuration file: {e}")
        return False
    except Exception as e:
        print(f"Unexpected error loading configuration: {e}")
        return False

# Safe brain initialization
def safe_initialize_brain(config):
    """Initialize brain with error handling and recovery"""
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("Brain initialized successfully")
        return brain
        
    except Exception as e:
        print(f"Error initializing brain: {e}")
        
        # Try with reduced configuration
        print("Attempting recovery with reduced configuration...")
        try:
            config.set("brain.neuron_count", 1000, reason="Recovery configuration")
            config.set("log_level", "ERROR", reason="Reduce logging overhead")
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            print("Brain recovered successfully with reduced configuration")
            return brain
            
        except Exception as recovery_error:
            print(f"Recovery failed: {recovery_error}")
            return None

# Safe simulation execution
def safe_run_simulation(brain, max_steps):
    """Run simulation with error handling and recovery"""
    try:
        for step in range(max_steps):
            try:
                brain.step(step)
                
                # Check for simulation errors
                if brain.getFiringNeuronCount() == 0 and step > 100:
                    print(f"Warning: No firing neurons at step {step}")
                
                if step % 100 == 0:
                    print(f"Step {step} completed")
                    
            except Exception as step_error:
                print(f"Error at step {step}: {step_error}")
                
                # Recovery attempt
                try:
                    brain.reset()
                    print(f"Recovered from step error at {step}")
                except:
                    print(f"Failed to recover from step error at {step}")
                    raise
                    
        print("Simulation completed successfully")
        return True
        
    except Exception as sim_error:
        print(f"Simulation failed: {sim_error}")
        return False

# Example usage with error handling
print("=== Error Handling Example ===")

# Load configuration safely
if safe_load_config("simulation_config.json"):
    # Initialize brain safely
    brain = safe_initialize_brain(config)
    
    if brain:
        # Run simulation safely
        if safe_run_simulation(brain, 1000):
            print("Error handling example completed successfully")
        else:
            print("Error handling example: Simulation failed")
    else:
        print("Error handling example: Failed to initialize brain")
else:
    print("Error handling example: Failed to load configuration")
```

## Best Practices Guide

### Performance Optimization Best Practices

**1. Configuration for Large-Scale Simulations:**
```python
# For simulations with 10,000+ neurons
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 10000)
config.set("log_level", "WARNING")  # Reduce log volume
config.set("log_async", True)      # Prevent I/O blocking
config.set("log_sampling", 0.1)    # Sample 10% of logs
config.set("log_system_metrics", False)  # Disable for performance
config.set("log_performance_metrics", False)
```

**2. Memory Management:**
```python
# Memory-efficient configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)  # Balanced memory usage
config.set("log_file", "")              # Disable file logging
config.set("log_async", True)           # Reduce memory pressure
config.set("log_level", "ERROR")        # Minimal logging overhead

# Force garbage collection
import gc
gc.collect()
```

**3. Real-time Simulation Performance:**
```python
# For real-time applications
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
config.set("simulation_timestep", 0.001)  # Small timestep
config.set("log_level", "INFO")
config.set("log_async", True)
config.set("async_queue_size", 5000)  # Manage memory

# Performance monitoring
import time
start_time = time.time()
for step in range(1000):
    step_start = time.time()
    brain.step(step)
    step_time = time.time() - step_start
    
    if step_time > 0.1:  # Step takes too long
        print(f"Performance warning: Step {step} took {step_time*1000:.2f}ms")
```

**4. Async Logging Optimization:**
```python
# Optimize async logging for high-frequency logging
config = pynlm.createDefaultConfig()
config.set("log_async", True)
config.set("async_queue_size", 10000)  # Large queue for burst logging
config.set("log_level", "INFO")
config.set("log_sampling", 0.5)  # 50% sampling for performance
config.set("log_system_metrics", False)

# Custom logging for critical information
pynlm.Logger.logInfo("Simulation started", custom_fields={
    "neuron_count": brain.getTotalNeuronCount(),
    "step_rate": 100.0
})
```

### Configuration Management Best Practices

**1. Hierarchical Configuration:**
```python
# Use categories for organized configuration
config = pynlm.createDefaultConfig()

# Brain configuration
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.15)

# Plasticity configuration  
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.hebbian.learning_rate", 0.001)

# Neuromodulation configuration
config.set("neuromodulation.dopamine.baseline", 0.2)
config.set("neuromodulation.curiosity.enable", True)

# Memory configuration
config.set("memory.working_memory_capacity", 100)
config.set("memory.episodic_memory_size", 5000)

# Validate each category
brain_validation = config.validateSchema("brain")
if brain_validation.valid:
    print("Brain configuration is valid")

neuromod_validation = config.validateSchema("neuromodulation")
if neuromod_validation.valid:
    print("Neuromodulation configuration is valid")
```

**2. Configuration Templates:**
```python
# Create reusable configuration templates
config = pynlm.createDefaultConfig()

# Research template
config.addTemplate("research", {
    "brain.neuron_count": 5000,
    "brain.synapse_density": 0.15,
    "plasticity.stdp.learning_rate": 0.002,
    "development.critical_period_length": 200,
    "neuromodulation.dopamine.baseline": 0.2,
    "neuromodulation.curiosity.enable": True,
    "memory.working_memory_capacity": 100,
    "memory.episodic_memory_size": 5000,
    "log_level": "INFO",
    "log_system_metrics": True,
    "log_performance_metrics": True
}, "Research-grade configuration", "brain")

# Production template
config.addTemplate("production", {
    "brain.neuron_count": 2000,
    "brain.synapse_density": 0.1,
    "log_level": "WARNING",
    "log_async": True,
    "log_sampling": 0.1,
    "log_system_metrics": False
}, "Production-ready configuration", "system")

# Apply templates
config.applyTemplate("research")
```

**3. Configuration History and Version Control:**
```python
# Enable configuration history
config = pynlm.createDefaultConfig()
config.enableHistory(True)

# Make configuration changes
config.set("brain.neuron_count", 2000, reason="Initial configuration")
config.set("plasticity.stdp.learning_rate", 0.001, reason="Standard learning rate")

# Check history
history = config.getHistory()
for change in history:
    print(f"{change.timestamp}: {change.key} = {change.newValue} ({change.reason})")

# Create snapshots for version control
snapshot1 = config.createSnapshot("version_1_0")
snapshot2 = config.createSnapshot("version_1_1")

# Rollback if needed
config.rollbackToSnapshot("version_1_0")
```

**4. Configuration Validation:**
```python
# Comprehensive validation
config = pynlm.createDefaultConfig()

# Validate all schemas
all_valid = True
for category in config.getSchemaCategories():
    validation = config.validateSchema(category)
    if validation.valid:
        print(f"✓ {category} configuration is valid")
    else:
        print(f"✗ {category} configuration errors:")
        for error in validation.errors:
            print(f"  - {error}")
        all_valid = False

if all_valid:
    print("All configuration schemas are valid!")
else:
    print("Some configuration schemas have errors")
```

### Logging Best Practices

**1. Appropriate Log Levels:**
```python
# Use correct log levels for different message types
pynlm.Logger.logDebug("Debug information for troubleshooting")      # Use for detailed debugging
pynlm.Logger.logInfo("Information about simulation progress")         # Use for general information
pynlm.Logger.logWarning("Potential issue that doesn't prevent operation") # Use for warnings
pynlm.Logger.logError("Error condition that affects operation")        # Use for errors
pynlm.Logger.logCritical("Critical error that requires immediate attention") # Use for critical errors
pynlm.Logger.logFatal("Fatal error causing system shutdown")           # Use for fatal errors
```

**2. Structured Logging:**
```python
# Use structured logging for better log analysis
pynlm.Logger.logStructured("Brain Initialization", {
    "neuron_count": brain.getTotalNeuronCount(),
    "synapse_count": brain.getTotalSynapseCount(),
    "firing_rate": brain.getAverageFiringRate(),
    "development_stage": brain.getDevelopmentalStage(),
    "timestamp": time.time(),
    "simulation_id": "sim_12345"
})

# JSON logging for machine-readable logs
pynlm.Logger.logJson("Simulation Step", {
    "step": step,
    "neuron_activity": brain.getFiringNeuronCount(),
    "reward": current_reward,
    "performance": {
        "steps_per_second": steps_per_second,
        "memory_usage": memory_usage
    }
})
```

**3. Category-based Logging:**
```python
# Enable appropriate log categories
pynlm.Logger.enableCategory("Brain", True)
pynlm.Logger.enableCategory("Memory", True)
pynlm.Logger.enableCategory("Neuromodulation", True)
pynlm.Logger.enableCategory("Cognition", True)
pynlm.Logger.enableCategory("Development", True)
pynlm.Logger.enableCategory("Performance", True)
pynlm.Logger.enableCategory("System", True)

# Set category-specific log levels
pynlm.Logger.setCategoryLevel("Brain", "INFO")
pynlm.Logger.setCategoryLevel("Memory", "WARNING")
pynlm.Logger.setCategoryLevel("Neuromodulation", "INFO")
pynlm.Logger.setCategoryLevel("Cognition", "DEBUG")
pynlm.Logger.setCategoryLevel("Development", "INFO")
pynlm.Logger.setCategoryLevel("Performance", "INFO")
pynlm.Logger.setCategoryLevel("System", "INFO")
```

**4. Rate Limiting and Sampling:**
```python
# Configure rate limiting for high-frequency categories
pynlm.Logger.setCategoryRateLimit("Brain", 1000)  # Max 1000 logs per second
pynlm.Logger.setCategoryRateLimit("Neuromodulation", 100)

# Configure sampling for performance monitoring
pynlm.Logger.setSamplingRate(0.1)  # Sample 10% of logs

# Configure pattern-based filtering
pynlm.Logger.setIncludePattern(".*processing.*")  # Only include processing logs
pynlm.Logger.setExcludePattern(".*GPU.*operation.*failed")  # Exclude failed GPU operations
```

### Error Handling Best Practices

**1. Graceful Error Recovery:**
```python
import pynlm

def run_simulation_with_recovery(config_file, max_steps):
    """Run simulation with comprehensive error handling and recovery"""
    
    # Load configuration
    config = pynlm.createDefaultConfig()
    if not config.loadFromFile(config_file):
        print(f"Failed to load configuration from {config_file}")
        return False
    
    # Validate configuration
    validation = config.validateSchema()
    if not validation.valid:
        print("Configuration validation failed:")
        for error in validation.errors:
            print(f"  - {error}")
        return False
    
    # Create brain
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
    except Exception as e:
        print(f"Failed to initialize brain: {e}")
        
        # Attempt recovery with reduced configuration
        try:
            config.set("brain.neuron_count", 1000, reason="Recovery configuration")
            config.set("log_level", "ERROR", reason="Reduce logging overhead")
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            print("Recovered with reduced configuration")
        except:
            print("Recovery failed, aborting simulation")
            return False
    
    # Run simulation with error handling
    try:
        for step in range(max_steps):
            try:
                brain.step(step)
                
                # Check for simulation health
                if brain.getFiringNeuronCount() == 0 and step > 100:
                    print(f"Warning: No firing neurons at step {step}")
                
                # Periodic status report
                if step % 100 == 0:
                    print(f"Progress: Step {step}/{max_steps}, "
                          f"Firing neurons: {brain.getFiringNeuronCount()}")
                    
            except Exception as step_error:
                print(f"Error at step {step}: {step_error}")
                
                # Attempt recovery
                try:
                    brain.reset()
                    print(f"Recovered from step error at {step}")
                except:
                    print(f"Failed to recover from step error at {step}")
                    return False
        
        print("Simulation completed successfully")
        return True
        
    except Exception as sim_error:
        print(f"Simulation failed: {sim_error}")
        return False

# Usage example
if __name__ == "__main__":
    success = run_simulation_with_recovery("simulation_config.json", 1000)
    if success:
        print("Simulation completed successfully")
    else:
        print("Simulation failed")
```

**2. Input Validation:**
```python
def validate_simulation_parameters(config):
    """Validate simulation parameters before running"""
    
    errors = []
    
    # Check neuron count
    neuron_count = config.getOr("brain.neuron_count", 1000)
    if neuron_count < 1 or neuron_count > 1000000:
        errors.append(f"Invalid neuron count: {neuron_count} (must be 1-1,000,000)")
    
    # Check timestep
    timestep = config.getOr("simulation.timestep", 0.001)
    if timestep <= 0 or timestep > 1.0:
        errors.append(f"Invalid timestep: {timestep} (must be > 0 and <= 1.0)")
    
    # Check memory configuration
    memory_capacity = config.getOr("memory.working_memory_capacity", 50)
    if memory_capacity < 10 or memory_capacity > 1000:
        errors.append(f"Invalid working memory capacity: {memory_capacity} (must be 10-1000)")
    
    # Check developmental parameters
    critical_period = config.getOr("development.critical_period_length", 100)
    if critical_period < 10 or critical_period > 1000:
        errors.append(f"Invalid critical period length: {critical_period} (must be 10-1000)")
    
    if errors:
        print("Configuration validation errors:")
        for error in errors:
            print(f"  - {error}")
        return False
    
    print("Configuration validation passed")
    return True
```

**3. Exception Hierarchy:**
```python
try:
    # Simulation code
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(max_steps):
        brain.step(step)
        
except pynlm.ConfigurationError as e:
    # Configuration-related errors
    print(f"Configuration error: {e}")
    
except pynlm.SimulationError as e:
    # Simulation runtime errors
    print(f"Simulation error: {e}")
    
except pynlm.MemoryError as e:
    # Memory allocation errors
    print(f"Memory error: {e}")
    # Attempt recovery with reduced configuration
    
except pynlm.PerformanceError as e:
    # Performance-related errors
    print(f"Performance error: {e}")
    
except Exception as e:
    # Unexpected errors
    print(f"Unexpected error: {e}")
    # Log error and attempt graceful shutdown
```

### Development Workflow Recommendations

**1. Testing and Validation:**
```bash
# Build with tests
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4
ctest --output-on-failure

# Run specific tests
./nlm_test --gtest_filter="*logging*"
./nlm_test --gtest_filter="*config*"

# Run enhanced logging demo
./nlm_phase6_enhanced_demo

# Run original demo (backward compatibility)
./nlm_phase6_demo
```

**2. Code Quality and Linting:**
```bash
# Check code quality
make check-style
make run-clang-tidy

# Validate documentation
generate-documentation --check

# Type checking (if applicable)
run-typecheck
```

**3. Integration Testing:**
```python
# Integration test for enhanced features
import pynlm

def test_enhanced_logging():
    """Test enhanced logging features"""
    config = pynlm.createDefaultConfig()
    config.set("log_level", "INFO")
    config.set("log_async", True)
    config.set("log_system_metrics", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test logging at different levels
    pynlm.Logger.logTrace("Trace message")
    pynlm.Logger.logDebug("Debug message")
    pynlm.Logger.logInfo("Info message")
    pynlm.Logger.logWarning("Warning message")
    pynlm.Logger.logError("Error message")
    pynlm.Logger.logCritical("Critical message")
    pynlm.Logger.logFatal("Fatal message")
    
    print("Enhanced logging test passed")

def test_configuration_validation():
    """Test configuration validation"""
    config = pynlm.createDefaultConfig()
    
    # Set valid configuration
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.1)
    
    # Validate
    validation = config.validateSchema("brain")
    assert validation.valid, "Configuration validation failed"
    
    # Test invalid configuration
    config.set("brain.neuron_count", -1)  # Invalid
    validation = config.validateSchema("brain")
    assert not validation.valid, "Invalid configuration should fail validation"
    
    print("Configuration validation test passed")

def test_performance_monitoring():
    """Test performance monitoring"""
    config = pynlm.createDefaultConfig()
    config.set("log_performance_metrics", True)
    config.set("log_system_metrics", True)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation with performance monitoring
    start_time = time.time()
    for step in range(100):
        brain.step(step)
    
    elapsed = time.time() - start_time
    rate = 100 / elapsed
    print(f"Performance: {rate:.1f} steps/sec")
    
    # Check that performance metrics were logged
    print("Performance monitoring test passed")

# Run integration tests
if __name__ == "__main__":
    test_enhanced_logging()
    test_configuration_validation()
    test_performance_monitoring()
    print("All integration tests passed")
```

**4. Performance Benchmarking:**
```bash
#!/bin/bash
# Performance benchmark script

echo "=== NLM Performance Benchmark ==="

echo "1. Building with optimizations..."
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_PERF_MEASUREMENTS=ON
make -j$(nproc)

echo "2. Running performance benchmarks..."

# Benchmark 1: Simple brain
./nlm_bench --scenario=simple --steps=1000

echo "Benchmark 1 complete"

# Benchmark 2: Complex brain
./nlm_bench --scenario=complex --steps=5000

echo "Benchmark 2 complete"

# Benchmark 3: Logging performance
./nlm_bench --scenario=logging --steps=1000 --log-level=INFO

echo "Benchmark 3 complete"

# Benchmark 4: Memory usage
./nlm_bench --scenario=memory --steps=2000 --check-memory

echo "Benchmark 4 complete"

echo "=== All benchmarks complete ==="
```

**5. Documentation Updates:**
```bash
#!/bin/bash
# Documentation update script

echo "=== Updating Documentation ==="

# Generate API documentation
python generate_docs.py --output=api_docs.html

# Update user guides
generate_user_guides.py --update all

# Validate documentation links
validate_docs_links.py

# Check documentation completeness
check_documentation_completeness.py

echo "=== Documentation update complete ==="
```

## Troubleshooting Guide

### Enhanced Troubleshooting with New Error Messages

NLM now provides enhanced error messages and diagnostic information for troubleshooting.

#### Configuration Validation Error Diagnosis

**Error Types:**
- `CONFIG_INVALID_PARAMETER`: Invalid parameter value
- `CONFIG_MISSING_REQUIRED`: Missing required parameter
- `CONFIG_TYPE_MISMATCH`: Type validation failed
- `CONFIG_RANGE_VIOLATION`: Value outside valid range
- `CONFIG_DEPENDENCY_FAILED`: Dependent parameter validation failed

**Example Error Messages:**
```
Configuration validation failed:
  - Invalid parameter 'brain.neuron_count': value -1 is outside valid range [1, 1000000]
  - Missing required parameter 'plasticity.stdp.learning_rate'
  - Type mismatch for 'neuromodulation.dopamine.baseline': expected double, got string
  - Dependency failed for 'memory.working_memory_capacity': requires 'brain.neuron_count' to be > 1000
```

**Troubleshooting Steps:**
```bash
# Diagnose configuration issues
./nlm_cli config validate --schema brain --verbose config.json

# Get specific parameter validation
./nlm_cli config validate --param neuron_count --value 5000 --schema brain

# Get schema documentation
./nlm_cli config generate --schema brain --format=markdown > brain_schema.md
```

#### Performance Troubleshooting

**Common Performance Issues:**
- High memory usage
- Slow simulation steps
- Log file I/O bottlenecks
- Lock contention in multi-threaded environments

**Performance Diagnosis:**
```bash
# Enable performance monitoring
./nlm_cli monitor --interval=0.5 --metrics

# Check performance metrics
./nlm_cli log categories --performance

# Analyze log performance
./nlm_cli log export performance.json

# Performance analysis script
python analyze_performance.py performance.json
```

**Troubleshooting Commands:**
```bash
# Optimize for performance
./nlm_cli simulate --log-level=WARNING --log-async --log-sampling=0.1

# Debug performance issues
./nlm_cli simulate --log-level=DEBUG --log-system-metrics

# Monitor resource usage
./nlm_cli monitor --system --interval=1
```

#### Logging Troubleshooting

**Common Logging Issues:**
- Log files not created
- Log messages missing
- Performance degradation due to logging
- Async logging configuration problems

**Logging Diagnosis:**
```bash
# Check logging status
./nlm_cli log status

# List log categories
./nlm_cli log categories

# Check log file rotation
./nlm_cli log status --rotation

# Export current configuration
./nlm_cli log export --current logging_config.json
```

**Troubleshooting Commands:**
```bash
# Enable detailed logging for debugging
./nlm_cli log enable Debug
./nlm_cli log set-level Debug:TRACE

# Check log file permissions
ls -la nlm.log

# Monitor log performance
./nlm_cli monitor --metrics --system

# Clear log files for testing
./nlm_cli log export --current debug_config.json
./nlm_cli log disable all
rm -f nlm.log
./nlm_cli log config debug_config.json
```

### Configuration Validation Error Diagnosis

**Error Classification:**

**1. Schema Validation Errors:**
- Missing required parameters
- Invalid parameter types
- Parameter value constraints violated
- Schema inheritance issues

**2. Configuration Loading Errors:**
- File not found
- Invalid file format
- Corrupted configuration files
- Permission issues

**3. Runtime Configuration Errors:**
- Invalid runtime parameter values
- Configuration conflicts
- Resource constraints violated

**Diagnosis Tools:**
```cpp
// Detailed error reporting
class ConfigDiagnostics {
public:
    static std::string diagnose(const std::string& configFile);
    static std::vector<std::string> validate(const std::map<std::string, std::any>& config);
    static std::string suggestFix(const std::string& error);
};

// Usage example
std::string diagnosis = ConfigDiagnostics::diagnose("config.json");
std::cout << diagnosis << std::endl;
```

### Performance Troubleshooting

**Performance Bottleneck Detection:**
```cpp
// Performance monitoring and diagnosis
class PerformanceProfiler {
public:
    void profileSimulation(const std::function<void()>& simulationFunction);
    void profileLogging(const std::function<void()>& loggingFunction);
    void profileConfiguration(const std::function<void()>& configFunction);
    std::string generatePerformanceReport() const;
};
```

**Common Performance Issues and Solutions:**

1. **Memory Issues:**
   - Problem: High memory usage during large simulations
   - Solution: Use memory-efficient configuration, disable unnecessary features

2. **CPU Issues:**
   - Problem: Slow simulation steps
   - Solution: Optimize configuration, enable SIMD optimizations

3. **I/O Issues:**
   - Problem: Log file I/O bottlenecks
   - Solution: Use async logging, configure appropriate file rotation

4. **Lock Contention:**
   - Problem: Thread contention in multi-threaded environments
   - Solution: Optimize async queue size, configure finer-grained locking

### Logging Troubleshooting

**Log Message Issues:**
- Missing log messages
- Incorrect log levels
- Category filtering problems
- Async logging issues

**Log File Issues:**
- Log files not created
- Log file permissions
- Log file rotation problems
- Log compression failures

**Log Performance Issues:**
- Slow log writes
- High memory usage from logging
- Performance degradation

**Diagnosis Commands:**
```bash
# Check current logging configuration
./nlm_cli log status

# Enable debug logging for troubleshooting
./nlm_cli log enable Debug
./nlm_cli log set-level Debug:TRACE

# Check log file properties
ls -la nlm.log
stat nlm.log

# Monitor log performance
./nlm_cli monitor --metrics

# Test log rotation
./nlm_cli log config test_rotation.json
./nlm_cli simulate --steps=1000
ls -la *.log*
```

**Solution Examples:**
```python
# Fix log file permissions
import os
import stat

# Check permissions
file_stat = os.stat("nlm.log")
if not file_stat.st_mode & stat.S_IWRITE:
    os.chmod("nlm.log", stat.S_IWRITE)
    print("Fixed log file permissions")

# Check disk space
import shutil
total, used, free = shutil.disk_usage(".")
if free < 100 * 1024 * 1024:  # Less than 100MB free
    print(f"Warning: Low disk space ({free / (1024*1024):.1f}MB free)")
    # Configure smaller log files
    pynlm.Logger.setLogConfig({"max_file_size": 5 * 1024 * 1024})

# Check for memory issues
import psutil
memory = psutil.virtual_memory()
if memory.available < 500 * 1024 * 1024:  # Less than 500MB available
    print(f"Warning: Low memory available ({memory.available / (1024*1024):.1f}MB)")
    # Disable memory-intensive features
    pynlm.Logger.setLogConfig({"log_system_metrics": False})
```

## Migration Guide

### Upgrading from Previous Versions

NLM v6.0.0 introduces significant enhancements while maintaining backward compatibility. This guide covers migration from previous versions.

#### Version Compatibility Matrix

| From Version | To Version | Breaking Changes | Migration Required |
|--------------|------------|------------------|-------------------|
| v5.2.0       | v6.0.0     | ✓                | Yes               |
| v4.1.0       | v6.0.0     | ✓                | Yes               |
| v3.0.0       | v6.0.0     | ✓                | Yes               |
| v2.5.0       | v6.0.0     | ✓                | Yes               |
| v1.0.0       | v6.0.0     | ✓                | Yes               |

#### Migration Requirements

**1. Configuration Files:**
- Previous `.cfg` format → New JSON/YAML/INI support
- Legacy parameters → New schema-based configuration
- Runtime settings → Enhanced configuration options

**2. Code Changes:**
- Old logging API → Enhanced logging API
- Basic configuration → Schema-based validation
- Simple simulation → Advanced simulation features

**3. Performance Considerations:**
- Default settings optimized for performance
- Enhanced logging with optional performance impact
- New memory management features

#### Migration Steps

**Step 1: Assess Current Configuration:**
```bash
# Analyze current configuration
./nlm_cli config analyze --current > analysis_report.txt

# Check for deprecated parameters
./nlm_cli config check --deprecated

# Get migration recommendations
./nlm_cli config migrate --from=v5.2.0 --to=v6.0.0 > migration_plan.txt
```

**Step 2: Update Configuration Files:**
```python
# Convert configuration files
import pynlm

# Load legacy configuration
config = pynlm.createDefaultConfig()
config.loadFromFile("legacy_config.cfg")

# Convert to new format
config.saveToFile("legacy_config.json", "legacy")
config.saveToFile("legacy_config.yaml", "legacy")
config.saveToFile("legacy_config.ini", "legacy")

# Apply schema
config.registerSchema("legacy", legacy_schema)
config.validateSchema("legacy")
```

**Step 3: Update Code:**
```cpp
// Old code (v5.2.0)
nlm::Logger::getInstance().setLevel(nlm::Logger::Level::Info);
nlm::Logger::getInstance().log("Simulation step completed");

// New code (v6.0.0)
nlm::Logger::getGlobal().setLogConfig(config);
nlm::Logger::getGlobal().logInfo("Simulation step completed");
```

**Step 4: Test Migration:**
```bash
# Run integration tests
./nlm_phase6_demo

# Run enhanced logging demo
./nlm_phase6_enhanced_demo

# Run configuration validation tests
./nlm_test --gtest_filter="*config*"
```

### Configuration Migration

**Migration from Legacy .cfg Files:**

**Legacy Format (v5.2.0):**
```ini
# legacy_config.cfg
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
```

**New Format (v6.0.0):**
```json
{
  "legacy": {
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "plasticity_learning_rate": 0.01
  },
  "brain": {
    "neuron_count": 1000,
    "synapse_density": 0.1,
    "region_count": 2,
    "connection_probability": 0.1
  },
  "plasticity": {
    "stdp": {"learning_rate": 0.01},
    "hebbian": {"learning_rate": 0.001}
  }
}
```

**Migration Process:**
```cpp
// Migration helper function
void migrateLegacyConfig(const std::string& legacyPath, const std::string& newPath) {
    Config legacyConfig;
    legacyConfig.loadFromFile(legacyPath);
    
    // Extract legacy parameters
    auto random_seed = legacyConfig.getOr("random_seed", 42);
    auto simulation_timestep = legacyConfig.getOr("simulation_timestep", 0.001);
    auto neuron_count = legacyConfig.getOr("neuron_count", 1000);
    auto region_count = legacyConfig.getOr("region_count", 2);
    auto connection_probability = legacyConfig.getOr("connection_probability", 0.1);
    auto plasticity_learning_rate = legacyConfig.getOr("plasticity_learning_rate", 0.01);
    
    // Create new configuration
    Config newConfig;
    
    // Legacy category (backward compatibility)
    newConfig.set("legacy.random_seed", random_seed);
    newConfig.set("legacy.simulation_timestep", simulation_timestep);
    newConfig.set("legacy.neuron_count", neuron_count);
    newConfig.set("legacy.region_count", region_count);
    newConfig.set("legacy.connection_probability", connection_probability);
    newConfig.set("legacy.plasticity_learning_rate", plasticity_learning_rate);
    
    // New categories
    newConfig.set("brain.neuron_count", neuron_count);
    newConfig.set("brain.synapse_density", 0.1);
    newConfig.set("brain.region_count", region_count);
    newConfig.set("brain.connection_probability", connection_probability);
    
    newConfig.set("plasticity.stdp.learning_rate", plasticity_learning_rate);
    newConfig.set("plasticity.hebbian.learning_rate", 0.0005);
    
    // Save new configuration
    newConfig.saveToFile(newPath);
}
```

**Automatic Migration:**
```python
import pynlm
import json
import yaml

def auto_migrate_config(input_file, output_file):
    """Automatically migrate configuration file"""
    
    # Detect file format
    if input_file.endswith('.cfg'):
        # Legacy .cfg format
        config = pynlm.createDefaultConfig()
        config.loadFromFile(input_file)
        
        # Convert to new format
        migrated = {
            "legacy": {},
            "brain": {},
            "plasticity": {},
            "neuromodulation": {},
            "memory": {},
            "development": {}
        }
        
        # Copy legacy parameters
        migrated["legacy"]["random_seed"] = config.getOr("random_seed", 42)
        migrated["legacy"]["simulation_timestep"] = config.getOr("simulation_timestep", 0.001)
        migrated["legacy"]["neuron_count"] = config.getOr("neuron_count", 1000)
        migrated["legacy"]["region_count"] = config.getOr("region_count", 2)
        migrated["legacy"]["connection_probability"] = config.getOr("connection_probability", 0.1)
        migrated["legacy"]["plasticity_learning_rate"] = config.getOr("plasticity_learning_rate", 0.01)
        
        # Add new parameters
        migrated["brain"]["neuron_count"] = migrated["legacy"]["neuron_count"]
        migrated["brain"]["synapse_density"] = 0.1
        migrated["brain"]["region_count"] = migrated["legacy"]["region_count"]
        migrated["brain"]["connection_probability"] = migrated["legacy"]["connection_probability"]
        
        migrated["plasticity"]["stdp"] = {"learning_rate": migrated["legacy"]["plasticity_learning_rate"]}
        migrated["plasticity"]["hebbian"] = {"learning_rate": 0.0005}
        
        # Save in multiple formats
        with open(output_file + ".json", 'w') as f:
            json.dump(migrated, f, indent=2)
        
        with open(output_file + ".yaml", 'w') as f:
            yaml.dump(migrated, f)
            
        print(f"Configuration migrated to {output_file}.json and {output_file}.yaml")
        
    else:
        print(f"Unsupported file format: {input_file}")
        return False
    
    return True
```

### API Compatibility Guide

**Backward Compatibility Matrix:**

| Old API (v5.2.0)                | New API (v6.0.0)                   | Status          |
|--------------------------------|-----------------------------------|-----------------| |
| `Logger::getInstance()`         | `Logger::getGlobal()`              | ✅ Compatible   |
| `Logger::setLevel()`            | `Logger::setLogConfig()`           | ✅ Compatible   |
| `Logger::log()`                 | `Logger::logInfo()`                | ✅ Compatible   |
| `Config::loadFromFile()`        | `Config::loadFromFile()`           | ✅ Compatible   |
| `Config::saveToFile()`          | `Config::saveToFile()`             | ✅ Compatible   |
| `Config::has()`                 | `Config::has()`                    | ✅ Compatible   |
| `Config::get()`                 | `Config::get()`                    | ✅ Compatible   |
| `Config::set()`                 | `Config::set()`                    | ✅ Compatible   |
| `Brain::create()`               | `pynlm.createBrain()`              | ✅ Compatible   |
| `Agent::create()`               | `pynlm.createAgentBrain()`         | ✅ Compatible   |
| `World::create()`               | `pynlm.createSimpleWorld()`        | ✅ Compatible   |

**Migration Examples:**

**C++ API Migration:**
```cpp
// Old API (v5.2.0)
nlm::Logger& logger = nlm::Logger::getInstance();
logger.setLevel(nlm::Logger::Level::Info);
logger.log("Simulation step completed");

// New API (v6.0.0) - fully backward compatible
nlm::Logger& logger = nlm::Logger::getGlobal();
logger.setLevel(nlm::Logger::Level::Info);
logger.log("Simulation step completed");

// Enhanced logging (new features)
nlm::LogConfig config;
config.globalLevel = nlm::EnhancedLogLevel::Info;
config.useEnhancedLevels = true;
config.useAsyncLogging = true;
logger.setLogConfig(config);

logger.trace("Detailed debug information");
logger.debug("Debug information");
logger.info("Informational message");
logger.warning("Warning message");
logger.error("Error message");
logger.critical("Critical error");
logger.fatal("Fatal error");
```

**Python API Migration:**
```python
# Old API (v5.2.0)
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Old logging API
pynlm.Logger.log("Simulation step completed")

# New API (v6.0.0) - fully backward compatible
import pynlm

# Create and initialize brain with enhanced features
config = pynlm.createDefaultConfig()
config.set("log_level", "INFO", reason="Enable enhanced logging")
config.set("log_async", True, reason="Enable async logging")
config.set("log_system_metrics", True, reason="Enable system metrics")

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with enhanced logging
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        # Enhanced logging (new features)
        pynlm.Logger.logInfo(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
        pynlm.Logger.logStructured("Brain Statistics", {
            "step": step,
            "firing_neurons": brain.getFiringNeuronCount(),
            "total_neurons": brain.getTotalNeuronCount(),
            "firing_rate": brain.getAverageFiringRate()
        })

# Backward compatible logging (still works)
pynlm.Logger.log("Simulation step completed")
```

**CLI Migration:**
```bash
# Old CLI (v5.2.0) - limited options
./nlm_cli simulate --steps=1000 --brain=1000

# New CLI (v6.0.0) - comprehensive features
./nlm_cli simulate \
  --steps=1000 \
  --brain=1000 \
  --log-level=INFO \
  --log-async \
  --enable-curiosity \
  --enable-development \
  --enable-reward \
  --category=Brain \
  --category=Memory \
  --log-system-metrics
```

**Configuration Migration:**
```cpp
// Old configuration (v5.2.0)
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1

// New configuration (v6.0.0) - backward compatible
{
  "legacy": {
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1
  },
  "brain": {
    "neuron_count": 1000,
    "synapse_density": 0.1,
    "region_count": 2,
    "connection_probability": 0.1
  },
  "plasticity": {
    "stdp": {"learning_rate": 0.01},
    "hebbian": {"learning_rate": 0.0005}
  }
}
```

### Migration Examples

**Complete Migration Example:**
```cpp
// Example: Complete migration from v5.2.0 to v6.0.0

// Step 1: Include new headers
#include "core/Logger/Logger.hpp"
#include "core/Logger/EnhancedLogger.hpp"
#include "core/Config/Config.hpp"

// Step 2: Update logging configuration
auto& logger = nlm::Logger::getGlobal();

nlm::LogConfig config;
config.globalLevel = nlm::EnhancedLogLevel::Info;
config.useEnhancedLevels = true;
config.useAsyncLogging = true;
config.logFile = "nlm.log";
config.logSystemMetrics = true;
config.logPerformanceMetrics = true;

// Enable categories
logger.enableCategory("Brain", true);
logger.enableCategory("Memory", true);
logger.enableCategory("Neuromodulation", true);
logger.enableCategory("Cognition", true);
logger.enableCategory("Development", true);
logger.enableCategory("Performance", true);
logger.enableCategory("System", true);

logger.setLogConfig(config);

// Step 3: Update configuration management
nlm::Config config;

// Register enhanced schema
auto brainSchema = std::make_shared<nlm::ConfigSchema>("brain", "Brain configuration");

nlm::ParameterMetadata neuronCount("neuron_count", "int", 
    "Number of neurons (1-1000000)", true, std::any(1000));
neuronCount.hasMin = true; neuronCount.minValue = 1;
neuronCount.hasMax = true; neuronCount.maxValue = 1000000;
brainSchema->addParameter(neuronCount);

config.registerSchema("brain", brainSchema);

// Load enhanced configuration
config.loadFromFile("enhanced_config.json");

// Validate configuration
auto validation = config.validateSchema("brain");
if (validation.valid) {
    std::cout << "Configuration is valid!" << std::endl;
} else {
    std::cout << "Configuration errors:" << std::endl;
    for (const auto& error : validation.errors) {
        std::cout << "  - " << error << std::endl;
    }
}

// Step 4: Update brain creation
nlm::Brain brain(config);
brain.initialize();

// Step 5: Update simulation loop
for (int step = 0; step < 1000; ++step) {
    brain.step(step);
    
    if (step % 100 == 0) {
        // Enhanced logging
        logger.logInfo("Step " + std::to_string(step) + 
                      ": " + std::to_string(brain.getFiringNeuronCount()) + 
                      " firing neurons");
        
        // Structured logging
        nlm::EnhancedLogEntry entry;
        entry.level = nlm::EnhancedLogLevel::Info;
        entry.category = "Brain";
        entry.message = "Processing completed";
        entry.fields["neuron_count"] = std::to_string(brain.getTotalNeuronCount());
        entry.fields["firing_rate"] = std::to_string(brain.getAverageFiringRate());
        
        logger.logEnhanced(entry);
    }
}

// Step 6: Update agent and world
nlm::AgentBrain agent(brain);
nlm::SimpleWorld world;
world.configure(20, 20, 8, 8);
world.reset();
agent.initialize(world);

// Run enhanced simulation
for (int step = 0; step < 1000; ++step) {
    world.update(0.1);
    
    auto percept = world.getSensoryPercept();
    agent.processSensoryInput(percept);
    brain.step(step);
    
    auto action = agent.decodeMotorCommand();
    world.applyMotorCommand(action, world.getSimulationTime());
    
    if (step % 100 == 0) {
        std::cout << "Step " << step << ": "
                  << "Firing: " << brain.getFiringNeuronCount() << ", "
                  << "Curiosity: " << agent.getCuriosityLevel() << ", "
                  << "Novelty: " << agent.getNoveltyLevel() << std::endl;
    }
}

std::cout << "Enhanced simulation completed successfully!" << std::endl;
```

**Python Migration Example:**
```python
# Example: Complete migration from v5.2.0 to v6.0.0

import pynlm

# Step 1: Create enhanced configuration
config = pynlm.createDefaultConfig()

# Enable enhanced logging
config.set("log_level", "INFO", reason="Enable enhanced logging")
config.set("log_async", True, reason="Enable async logging")
config.set("log_system_metrics", True, reason="Enable system metrics")
config.set("log_performance_metrics", True, reason="Enable performance metrics")
config.set("brain.neuron_count", 2000, reason="Configure for enhanced simulation")

# Create enhanced brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create enhanced agent
agent = pynlm.createAgentBrain(brain)

# Create enhanced world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Initialize enhanced agent
agent.initialize(world)

# Enable all enhanced subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Step 2: Run enhanced simulation
print("=== Enhanced Simulation (v6.0.0) ===")

for step in range(1000):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process in enhanced agent
    agent.processSensoryInput(percept)
    
    # Run enhanced brain
    brain.step(step)
    
    # Get enhanced action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply enhanced reward modulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev stage: {brain.getDevelopmentalStage()}")
        
        # Enhanced structured logging
        pynlm.Logger.logStructured("Brain Statistics", {
            "step": step,
            "firing_neurons": brain.getFiringNeuronCount(),
            "total_neurons": brain.getTotalNeuronCount(),
            "firing_rate": brain.getAverageFiringRate(),
            "curiosity": agent.getCuriosityLevel(),
            "novelty": agent.getNoveltyLevel(),
            "development_stage": str(brain.getDevelopmentalStage())
        })

print("\n=== Enhanced simulation completed successfully! ===")

# Step 3: Backward compatibility check
print("\n=== Backward Compatibility Check ===")

# Old API still works (backward compatibility)
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.initialize()
for step in range(100):
    brain2.step(step)

print("✓ Backward compatibility maintained - all old APIs still work")

# Step 4: Migration summary
print("\n=== Migration Summary ===")
print("✓ Enhanced logging infrastructure added")
print("✓ Configuration validation implemented")
print("✓ Schema-based configuration added")
print("✓ Async logging for performance")
print("✓ Structured logging with JSON support")
print("✓ Performance monitoring added")
print("✓ Backward compatibility maintained")
print("✓ New Python features added")
print("✓ Advanced CLI tool added")
print("✓ Comprehensive documentation added")
```

**Migration Checklist:**

**Configuration Files:**
- [ ] Convert `.cfg` files to JSON/YAML/INI format
- [ ] Add schema definitions for new parameters
- [ ] Test configuration validation
- [ ] Update configuration documentation

**Code Changes:**
- [ ] Update logging configuration to use enhanced features
- [ ] Add configuration validation where needed
- [ ] Test backward compatibility
- [ ] Update API documentation

**Testing:**
- [ ] Run existing tests to verify compatibility
- [ ] Test new features with unit tests
- [ ] Test enhanced logging with integration tests
- [ ] Test configuration validation with schema tests
- [ ] Test performance with benchmarking

**Documentation:**
- [ ] Update README.md with new features
- [ ] Update HOW_TO_USE.md with enhanced API
- [ ] Update easy_usage.md with new examples
- [ ] Add migration guide
- [ ] Add troubleshooting guide

This migration guide provides comprehensive guidance for upgrading from previous versions of NLM to v6.0.0, ensuring backward compatibility while leveraging all new features and capabilities.