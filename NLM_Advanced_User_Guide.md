# NLM Advanced Features README

# NLM Advanced Features Documentation

## Overview

This document provides comprehensive information about the advanced features available in the NLM (Neural Learning Machine) Phase 6 implementation. The NLM is now a complete, coherent artificial brain system with all cognitive functions properly integrated and functional.

## Contents

1. [Quick Start Guide](#quick-start-guide)
2. [Advanced Configuration](#advanced-configuration)
3. [Command Line Interface](#command-line-interface)
4. [Performance Monitoring](#performance-monitoring)
5. [Debugging Tools](#debugging-tools)
6. [Advanced Usage Patterns](#advanced-usage-patterns)
7. [API Reference](#api-reference)
8. [Troubleshooting](#troubleshooting)

## Quick Start Guide

### Phase 6 Advanced Demo

The NLM Phase 6 now includes comprehensive advanced features for research and development:

```bash
# Build the advanced version
./build.sh

# Run Phase 6 integration demo with advanced features
./build/nlm_phase6_demo --monitor

# Run with custom advanced configuration
./build/nlm_phase6_demo --config configs/advanced.cfg
```

### Advanced Configuration

Create an advanced configuration file (`configs/advanced.cfg`) to customize brain behavior:

```bash
# Create advanced configuration
cat > configs/advanced.cfg << EOF
# NLM Advanced Configuration
neuron_count: 5000
region_count: 2
stdp_ltp_weight: 0.025f
stdp_ltd_weight: 0.018f
synaptogenesis_rate: 0.00005f
pruning_rate: 0.000005f

# Neuromodulation parameters
dopamine_baseline: 0.5f
curiosity_threshold: 0.3f
novelty_sensitivity: 0.8f

# Memory parameters
working_memory_capacity: 500
episodic_memory_max_episodes: 5000
replay_interval: 100

# Development parameters
development_enabled: true
synaptogenesis_enabled: true
maturation_enabled: true
EOF
```

## Advanced Configuration

### Configuration File Format

The NLM advanced configuration uses YAML format with the following structure:

```yaml
# Neural parameters
neuron_count: 5000
region_count: 2
connection_probability: 0.15f

# Plasticity parameters
stdp_ltp_weight: 0.025f
stdp_ltd_weight: 0.018f
stdp_tau: 25.0f

# Development parameters
development_enabled: true
initial_plasticity: 1.0f
maturation_plasticity: 0.5f
adult_plasticity: 0.2f

# Neuromodulation parameters
dopamine_enabled: true
dopamine_baseline: 0.5f
acetylcholine_enabled: true
ach_baseline: 0.3f

# Memory parameters
working_memory_enabled: true
working_memory_capacity: 500
episodic_memory_enabled: true
episodic_memory_max_episodes: 5000

# Performance parameters
performance_monitoring_enabled: true
checkpointing_enabled: true
checkpoint_path: "./checkpoints/advanced/"
```

### Configuration Sources

The NLM supports multiple configuration sources with the following precedence (highest to lowest):

1. **Command line arguments** (`--key value`)
2. **Configuration files** (YAML format)
3. **Default configuration** (hard-coded values)
4. **Environment variables** (for certain settings)

## Command Line Interface

### Available Commands

#### Phase 6 Demo Commands

```bash
# Standard Phase 6 demo
./nlm_phase6_demo [options]

Options:
  --help                    Show help message
  --config <file>           Load configuration from file
  --verbose                 Enable verbose logging
  --monitor                 Enable performance monitoring
  --benchmark               Run performance benchmarks
  --validate                Validate configuration
  --checkpoint <path>       Save checkpoint to specified path
  --load <path>             Load checkpoint from specified path
  --timelimit <seconds>     Limit simulation time
  --step <steps>            Limit number of simulation steps
  --seed <seed>             Set random seed for reproducible experiments
  --debug                   Enable debug output
  --system <name>           Debug specific system (memory, neuromodulation, prediction)
  --neuron <id>             Debug specific neuron
  --region <id>             Debug specific region
```

#### Advanced Demo Commands

```bash
# Advanced Phase 6 demo with all features
./nlm_phase6_advanced_demo [options]

Additional options:
  --stress-test              Run stress test with extreme parameters
  --continual-learning        Run continual learning simulation
  --multi-environment        Run multi-environment testing
  --performance-benchmark    Run comprehensive performance benchmarks
  --detailed-metrics         Output detailed metrics to file
  --visualization            Enable visualization output
```

### Command Line Examples

#### Custom Configuration

```bash
# Run with custom advanced configuration
./build/nlm_phase6_demo \
  --config configs/custom_experiment.cfg \
  --verbose \
  --monitor \
  --checkpoint ./results/experiment_01.checkpoint \
  --timelimit 300
```

#### Advanced Configuration with Monitoring

```bash
# Run advanced demo with comprehensive monitoring
./build/nlm_phase6_demo \
  --config configs/advanced.cfg \
  --monitor \
  --benchmark \
  --detailed-metrics \
  --visualization \
  --debug \
  --verbose
```

#### Stress Testing

```bash
# Run stress test with extreme parameters
./build/nlm_phase6_advanced_demo \
  --stress-test \
  --neuron_count 10000 \
  --region_count 10 \
  --connection_probability 0.01f \
  --timelimit 600
```

#### Continual Learning

```bash
# Run continual learning simulation
./build/nlm_phase6_advanced_demo \
  --continual-learning \
  --learning_rate 0.001f \
  --exploration_rate 0.3f \
  --exploitation_rate 0.7f \
  --replay_enabled true
```

## Performance Monitoring

### Performance Metrics

The NLM provides comprehensive performance monitoring:

```cpp
// Enable performance monitoring
brain->enablePerformanceMonitoring(true);

// Get performance metrics
PerformanceMetrics metrics = brain->getPerformanceMetrics();

std::cout << "=== Performance Metrics ===" << std::endl;
std::cout << "Spikes per second: " << metrics.spikesPerSecond << std::endl;
std::cout << "Average firing rate: " << metrics.averageFiringRate << std::endl;
std::cout << "Memory usage: " << metrics.memoryUsage << " MB" << std::endl;
std::cout << "CPU usage: " << metrics.cpuUsage << "%" << std::endl;
std::cout << "Wall clock time: " << metrics.wallClockTime << "s" << std::endl;
std::cout << "Simulations per second: " << metrics.simulationsPerSecond << std::endl;
```

### Performance Monitoring Commands

```bash
# Enable performance monitoring
./build/nlm_phase6_demo --monitor

# Generate performance report
./build/nlm_phase6_demo --monitor --report ./performance_report.html

# Track performance over multiple runs
./build/nlm_phase6_demo --monitor --benchmark --benchmark_file ./benchmark_results.csv

# Real-time monitoring
./build/nlm_phase6_demo --monitor --verbose --debug
```

## Debugging Tools

### Debug Output

```bash
# Enable comprehensive debug output
./build/nlm_phase6_demo --debug

# Debug specific systems
./build/nlm_phase6_demo --debug --system memory
./build/nlm_phase6_demo --debug --system neuromodulation
./build/nlm_phase6_demo --debug --system prediction
./build/nlm_phase6_demo --debug --system cognition

# Debug specific components
./build/nlm_phase6_demo --debug --neuron 42
./build/nlm_phase6_demo --debug --region 1 --neuron_range 0-100
./build/nlm_phase6_demo --debug --synapse 123
```

### Debug API

```cpp
// Enable debug logging
brain->enableDebugLogging(true);

// Set debug level
brain->setDebugLevel(DebugLevel::Info);     // Normal output
brain->setDebugLevel(DebugLevel::Warning);   // Warnings only
brain->setDebugLevel(DebugLevel::Error);     // Errors only
brain->setDebugLevel(DebugLevel::Verbose);   // Detailed output

// Log debug messages
NLM_LOG_DEBUG("Debug message with value: " + std::to_string(value));
NLM_LOG_WARNING("Warning message: " + message);
NLM_LOG_ERROR("Error message: " + errorMessage);
```

## Advanced Usage Patterns

### Pattern 1: Multi-Environment Testing

```cpp
void runMultiEnvironmentTest() {
    std::vector<std::string> environments = {
        "configs/simple_world.cfg",
        "configs/complex_world.cfg", 
        "configs/stochastic_world.cfg"
    };
    
    std::ofstream resultsFile("./results/multi_environment_results.csv");
    resultsFile << "Environment,Neurons,Regions,FinalReward,FinalFiringRate,TimeElapsed" << std::endl;
    
    for (const auto& envConfig : environments) {
        std::cout << "=== Testing Environment: " << envConfig << " ===" << std::endl;
        
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(envConfig)) {
            auto brain = std::make_shared<Brain>(config);
            brain->enablePerformanceMonitoring(true);
            brain->enableDebugLogging(true);
            brain->setDebugLevel(DebugLevel::Info);
            
            brain->initialize();
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            for (int step = 0; step < 1000; ++step) {
                brain->step(step);
                
                if (step % 100 == 0) {
                    brain->logStatus();
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                endTime - startTime).count() / 1000.0f;
            
            PerformanceMetrics metrics = brain->getPerformanceMetrics();
            
            resultsFile << envConfig << ","
                       << config->getOr<size_t>("neuron_count", 1000) << ","
                       << config->getOr<size_t>("region_count", 1) << ","
                       << metrics.totalReward << ","
                       << metrics.avgFiringRate << ","
                       << elapsed << std::endl;
            
            std::string checkpointPath = "./checkpoints/env_" + 
                                         std::to_string(environments.size()) + 
                                         "_" + std::to_string(step) + ".bin";
            brain->save(checkpointPath);
        }
    }
    
    resultsFile.close();
    std::cout << "=== Multi-Environment Test Complete ===" << std::endl;
    std::cout << "Results saved to ./results/multi_environment_results.csv" << std::endl;
}
```

### Pattern 2: Continual Learning with Replay

```cpp
void runContinualLearningWithReplay() {
    auto config = std::make_shared<Config>();
    config->set("random_seed", 42, ConfigSource::Default);
    config->set("neuron_count", 1000, ConfigSource::Default);
    config->set("region_count", 2, ConfigSource::Default);
    config->set("enable_replay", true, ConfigSource::User);
    config->set("replay_interval", 50, ConfigSource::User);
    
    auto brain = std::make_shared<Brain>(config);
    brain->enablePerformanceMonitoring(true);
    
    SimpleWorld world;
    world.initialize(20, 20);
    
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    
    std::cout << "=== Continual Learning with Replay ===" << std::endl;
    
    for (int episode = 0; episode < 50; ++episode) {
        std::cout << "Episode " << episode << ": ";
        
        float episodeReward = 0.0f;
        size_t episodeSteps = 0;
        
        for (int step = 0; step < 200; ++step) {
            SensoryPercept percept = world.observe(agent.getBrain()->getRegions()[0].get());
            agent.processSensoryInput(percept);
            
            brain->step(step);
            
            MotorCommand cmd = agent.decodeMotorCommand();
            world.applyAction(agent.getBrain()->getRegions()[0].get(), cmd);
            
            float reward = world.computeReward(agent.getBrain()->getRegions()[0].get());
            episodeReward += reward;
            episodeSteps++;
            
            agent.applyRewardModulation(reward, 0.0f);
            
            if (step % 50 == 0) {
                std::cout << ".";
            }
        }
        
        float avgReward = episodeReward / episodeSteps;
        std::cout << " Reward: " << avgReward << std::endl;
        
        world.reset();
        agent.enableCuriosity(true);
        
        if (episode % 10 == 0) {
            PerformanceMetrics metrics = brain->getPerformanceMetrics();
            std::cout << "  Metrics: Reward=" << metrics.totalReward 
                      << ", Firing=" << metrics.avgFiringRate 
                      << ", Time=" << metrics.wallClockTime << "s" << std::endl;
        }
    }
    
    std::cout << "=== Continual Learning Complete ===" << std::endl;
}
```

### Pattern 3: Advanced Configuration Management

```cpp
void runAdvancedConfigurationManagement() {
    // Load base configuration
    auto baseConfig = std::make_shared<Config>();
    baseConfig->loadFromFile("configs/base.cfg");
    
    // Modify configuration for specific experiment
    auto experimentConfig = std::make_shared<Config>(*baseConfig);
    
    // Override specific parameters
    experimentConfig->set("neuron_count", 2000, ConfigSource::User);
    experimentConfig->set("stdp_ltp_weight", 0.05f, ConfigSource::User);
    experimentConfig->set("stdp_ltd_weight", 0.03f, ConfigSource::User);
    experimentConfig->set("synaptogenesis_rate", 0.0001f, ConfigSource::User);
    
    // Create brain with experiment configuration
    auto brain = std::make_shared<Brain>(experimentConfig);
    brain->initialize();
    
    // Run experiment with specific configuration
    std::cout << "=== Advanced Configuration Management ===" << std::endl;
    std::cout << "Neuron count: " << experimentConfig->getOr<size_t>("neuron_count", 1000) << std::endl;
    std::cout << "STDP LTP weight: " << experimentConfig->getOr<float>("stdp_ltp_weight", 0.01f) << std::endl;
    std::cout << "Development enabled: " << (experimentConfig->getOr<bool>("enable_development", false) ? "true" : "false") << std::endl;
    
    // Save configuration for reproducibility
    experimentConfig->saveToFile("./results/experiment_config.cfg");
    
    // Run simulation
    for (int step = 0; step < 500; ++step) {
        brain->step(step);
        
        if (step % 100 == 0) {
            brain->logStatus();
        }
    }
    
    // Load configuration for next experiment
    auto nextExperimentConfig = std::make_shared<Config>();
    nextExperimentConfig->loadFromFile("./results/experiment_config.cfg");
    
    std::cout << "=== Configuration Management Complete ===" << std::endl;
}
```

## API Reference

### Core Brain API

#### Brain Class

```cpp
class Brain {
public:
    // Construction and initialization
    Brain(std::shared_ptr<Config> config);
    ~Brain();
    bool initialize();
    
    // Main simulation loop
    void step(SimulationStep step);
    void step(SimulationStep step, Timestamp time);
    
    // I/O operations
    void receiveSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    std::unique_ptr<Action> produceAction();
    
    // Memory system access
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    SemanticMemory* getSemanticMemory();
    ProceduralMemory* getProceduralMemory();
    
    // Prediction system
    PredictionSystem* getPredictionSystem();
    
    // Cognitive systems
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    SelfModel* getSelfModel();
    SocialLearning* getSocialLearning();
    
    // Attention system
    AttentionalSelection* getAttention();
    
    // Development system
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Neuromodulation systems
    Dopamine* getDopamine();
    Acetylcholine* getAcetylcholine();
    Norepinephrine* getNorepinephrine();
    Serotonin* getSerotonin();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Configuration access
    std::shared_ptr<const Config> getConfig() const;
    
    // Performance and monitoring
    void enablePerformanceMonitoring(bool enable);
    PerformanceMetrics getPerformanceMetrics() const;
    void enableDebugLogging(bool enable);
    void setDebugLevel(DebugLevel level);
    float getMemoryUsage() const;
    
    // Persistence
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Utilities
    void reset();
    void logStatus() const;
    
    // Advanced features
    void enableReplay(bool enable);
    void enableCheckpointing(bool enable);
    void setCheckpointPath(const std::string& path);
};
```

#### Config Class

```cpp
class Config {
public:
    // Configuration sources
    enum class ConfigSource {
        Default,
        User,
        CommandLine
    };
    
    // Set configuration values
    void set(const std::string& key, bool value, ConfigSource source);
    void set(const std::string& key, int64_t value, ConfigSource source);
    void set(const std::string& key, double value, ConfigSource source);
    void set(const std::string& key, float value, ConfigSource source);
    void set(const std::string& key, const std::string& value, ConfigSource source);
    
    // Get configuration values
    template<typename T>
    T get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, T defaultValue) const;
    
    // Load configuration
    bool loadFromFile(const std::string& filename);
    void loadFromArgs(int argc, char** argv);
    
    // Save configuration
    bool saveToFile(const std::string& filename) const;
};
```

#### AgentBrain Class

```cpp
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialization
    void initialize(const SimpleWorld& world);
    
    // Processing
    void processSensoryInput(const SensoryPercept& percept);
    MotorCommand decodeMotorCommand();
    
    // Neuromodulation
    void applyRewardModulation(float reward, float predictedReward);
    void applyCuriosity(float curiosity);
    
    // Enable features
    void enableRewardModulation(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableDevelopment(bool enable);
    void enableCuriosity(bool enable);
    void enableSocialLearning(bool enable);
    
    // Accessors
    std::shared_ptr<Brain> getBrain() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getNeuromodulationLevel() const;
};
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: Configuration File Not Found

**Problem**: Configuration file cannot be found.

**Solution**:

1. Check if the file path is correct
2. Create the configuration directory if it doesn't exist
3. Use absolute paths for configuration files

```bash
# Create directory
mkdir -p configs

# Check file exists
ls -la configs/

# Use absolute path
./build/nlm_phase6_demo --config /home/user/project/configs/advanced.cfg
```

#### Issue 2: Performance Too Slow

**Problem**: Simulation runs too slowly.

**Solution**:

1. Reduce neuron count
2. Decrease region count
3. Use sparser connectivity
4. Monitor performance during run

```cpp
// Reduce performance parameters
config->set("neuron_count", 500, ConfigSource::User);
config->set("region_count", 1, ConfigSource::User);
config->set("connection_probability", 0.05f, ConfigSource::User);
```

#### Issue 3: Memory Usage Too High

**Problem**: Memory usage exceeds system limits.

**Solution**:

1. Reduce memory-intensive features
2. Use checkpointing to save memory
3. Monitor memory usage
4. Increase system memory if possible

```cpp
// Enable checkpointing
brain->enableCheckpointing(true);
brain->setCheckpointPath("./checkpoints/");
```

#### Issue 4: Debug Output Too Verbose

**Problem**: Debug output is overwhelming.

**Solution**:

1. Set appropriate debug level
2. Filter debug output
3. Use different log levels

```cpp
// Set debug level to Info
brain->setDebugLevel(DebugLevel::Info);
```

#### Issue 5: Checkpoint Issues

**Problem**: Checkpoint save/load fails.

**Solution**:

1. Check file permissions
2. Ensure directory exists
3. Use absolute paths
4. Check disk space

```bash
# Ensure directory exists
mkdir -p ./checkpoints

# Use absolute path
./build/nlm_phase6_demo --checkpoint /home/user/checkpoints/experiment.bin
```

#### Issue 6: Integration Test Failures

**Problem**: Phase 6 integration test fails.

**Solution**:

1. Check configuration files
2. Verify all systems are properly initialized
3. Run with debug output
4. Check system logs

```bash
# Run with debug output
./build/nlm_phase6_demo --debug --verbose
```

### Getting Help

If you encounter issues not covered in this documentation:

1. **Check the documentation**: Review `NLM_Advanced_User_Guide.md`, `docs/ARCHITECTURE.md`
2. **Run with debug output**: Use `--debug` flag for detailed information
3. **Check logs**: Look for log files in `./logs/`
4. **File bug reports**: Check for known issues in the repository
5. **Contact support**: For commercial support, contact the NLM team

## Documentation Resources

- **Advanced User Guide**: `NLM_Advanced_User_Guide.md`
- **Main Documentation**: `README.md`, `easy_usage.md`, `HOW_TO_USE.md`
- **Architecture**: `docs/ARCHITECTURE.md`, `docs/PHASE6_FINAL_AUDIT.md`
- **Science**: `docs/SCIENCE.md`, `docs/RESEARCH_METHODS.md`
- **Experiments**: `docs/EXPERIMENTS.md`, `docs/PHASE6_FINAL_RESULTS.md`
- **Limitations**: `docs/LIMITATIONS.md`, `docs/FINAL_LIMITATIONS.md`

## Conclusion

The NLM Phase 6 implementation now includes comprehensive advanced features for research and development. All cognitive functions are properly integrated, and the system provides extensive configuration options, performance monitoring, debugging tools, and advanced usage patterns.

This advanced user guide provides the necessary information to work with the full capabilities of the NLM artificial brain system. For the latest documentation and support, visit the NLM project website or contact the development team.

## Version Information

- **Version**: Phase 6 Advanced Edition
- **Build Date**: $(date)
- **Documentation**: $(date)
- **Features**: Complete cognitive integration, advanced configuration, performance monitoring, debugging tools

---
*This documentation is continuously updated. For the latest information, visit the NLM documentation website.*
