# NLM Troubleshooting Guide

## Overview

This document provides comprehensive troubleshooting guidance for NLM (熙然) simulation issues. It covers common problems, their causes, and effective solutions to help you resolve issues quickly and efficiently.

## Common Issues and Solutions

### Initialization Failures

#### "Failed to initialize brain"

**Symptoms:**
- Brain initialization returns false
- Error messages about configuration validation
- Memory allocation failures

**Causes:**
1. **Invalid configuration values**
2. **Missing required settings**
3. **Insufficient system resources**
4. **Corrupted configuration files**

**Solutions:**
```bash
# 1. Check configuration file
cat config.json | grep -E "(maxNeurons|learningRate|simulation)"

# 2. Validate configuration programmatically
./validate_config config.json

# 3. Check system resources
free -h
ulimit -a

# 4. Use default configuration if needed
./nlm --config config.json --use-defaults
```

#### "Out of memory"

**Symptoms:**
- Memory allocation failures
- System swapping
- Slow performance

**Causes:**
1. **Configuration specifies too many neurons**
2. **Memory systems consuming too much memory**
3. **Memory fragmentation**
4. **Memory pool exhaustion**

**Solutions:**
```cpp
// Reduce brain size
config->set("brain.maxNeurons", 500);  // From 2000

// Disable memory-heavy features
config->set("brain.memory.episodicMemory.enabled", false);
config->set("brain.memory.workingMemory.enabled", false);
config->set("brain.development.enabled", false);

// Optimize memory usage
config->set("brain.memory.workingMemory.capacity", 100);
config->set("brain.memory.episodicMemory.maxEpisodes", 500);

// Use memory pools
config->set("brain.performance.useMemoryPools", true);

// Reduce simulation duration
config->set("brain.simulation.maxSteps", 1000);
```

### Simulation Issues

#### "Simulation step timeout"

**Symptoms:**
- Simulation steps taking too long
- Real-time deadline violations
- Performance degradation over time

**Causes:**
1. **Network size too large**
2. **Inefficient spike propagation**
3. **Memory bottleneck**
4. **Plasticity overload**

**Solutions:**
```cpp
// Reduce network size
config->set("brain.maxNeurons", 1000);  // From 5000

// Optimize spike processing
config->set("brain.spikeProcessing.useCompression", true);
config->set("brain.spikeProcessing.maxSpikeEvents", 10000);

// Optimize memory access
config->set("brain.memory.useCacheOptimization", true);

// Reduce plasticity frequency
config->set("brain.plasticity.updateFrequency", 10);
```

#### "Memory leak"

**Symptoms:**
- Memory usage increasing over time
- System running out of memory
- Performance degradation

**Causes:**
1. **Improper resource cleanup**
2. **Circular references**
3. **Memory pools not being released**
4. **Simulation objects not being destroyed**

**Solutions:**
```cpp
// Ensure proper cleanup in destructor
Brain::~Brain() {
    // Clean up memory systems
    if (workingMemory_) {
        workingMemory_->clear();
        delete workingMemory_;
        workingMemory_ = nullptr;
    }
    
    // Stop simulation
    stopSimulation();
    
    // Release other resources
    // ...
}

// Use smart pointers
auto brain = std::make_shared<Brain>(config);
// Shared pointers handle cleanup automatically

// Monitor memory usage
valgrind --tool=massif ./nlm_simulation
```

### Configuration Issues

#### "Invalid configuration value"

**Symptoms:**
- Error messages about configuration
- Invalid parameter values
- Configuration validation failures

**Causes:**
1. **Type mismatch**
2. **Range violations**
3. **Missing dependencies**
4. **Conflicting settings**

**Solutions:**
```cpp
// Fix type mismatches
config->set("brain.maxNeurons", "100");  // Wrong type
// Fix: config->set("brain.maxNeurons", 100);

// Fix range violations
config->set("brain.learningRate", 1.5);  // Out of range
// Fix: config->set("brain.learningRate", 0.01);

// Fix missing dependencies
config->set("brain.memory.workingMemory.enabled", true);
config->set("brain.maxNeurons", 100);  // Also set brain size

// Fix conflicting settings
config->set("brain.development.enabled", true);
config->set("brain.performance.optimization", true);
// These can work together
```

#### "Configuration file not found"

**Symptoms:**
- Error loading configuration
- Using default settings
- Missing custom settings

**Causes:**
1. **Wrong file path**
2. **File permissions**
3. **File corruption**
4. **Network issues**

**Solutions:**
```bash
# Check file exists
ls -la config.json

# Check file permissions
chmod 644 config.json

# Fix corrupted file
cp config.json config.json.backup
nano config.json  # Edit the file

# Use absolute path
./nlm --config /home/user/config.json

# Create config if missing
if [ ! -f config.json ]; then
    echo '{"brain": {"maxNeurons": 100}}' > config.json
fi
```

### Neuromodulation Issues

#### "Neuromodulator not responding"

**Symptoms:**
- Neuromodulation levels not changing
- Reward signals not affecting learning
- Curiosity not driving exploration

**Causes:**
1. **Neuromodulation disabled**
2. **Incorrect signal processing**
3. **Modulation range issues**
4. **Integration problems**

**Solutions:**
```cpp
// Ensure neuromodulation is enabled
config->set("brain.neuromodulation.dopamine.enabled", true);
config->set("brain.neuromodulation.curiosity.enabled", true);
config->set("brain.neuromodulation.novelty.enabled", true);

// Check neuromodulation levels
auto dopamine = brain->getDopamine();
float level = dopamine->getLevel();
std::cout << "Dopamine level: " << level << std::endl;

// Adjust neuromodulation parameters
config->set("brain.neuromodulation.dopamine.baselineLevel", 0.1);
config->set("brain.neuromodulation.dopamine.gain", 5.0);
```

#### "Prediction system not working"

**Symptoms:**
- Prediction errors not changing
- Poor prediction accuracy
- Learning not improving

**Causes:**
1. **Prediction system disabled**
2. **Incorrect prediction parameters**
3. **Integration with learning**
4. **Signal processing errors**

**Solutions:**
```cpp
// Enable prediction system
config->set("brain.prediction.enabled", true);
config->set("brain.prediction.errorSignal.enabled", true);

// Configure prediction accuracy
config->set("brain.prediction.threshold", 0.5);
config->set("brain.prediction.learningRate", 0.01);

// Check prediction error
auto error = brain->getPredictionErrorSignal();
float errorLevel = error->getLevel();
std::cout << "Prediction error: " << errorLevel << std::endl;
```

### Memory System Issues

#### "Memory system full"

**Symptoms:**
- Memory systems rejecting new data
- Errors about capacity limits
- Memory cleanup not working

**Causes:**
1. **Capacity exceeded**
2. **Memory cleanup not working**
3. **Memory fragmentation**
4. **Memory access patterns**

**Solutions:**
```cpp
// Reduce memory capacity
config->set("brain.memory.workingMemory.capacity", 100);
config->set("brain.memory.episodicMemory.maxEpisodes", 500);

// Implement memory cleanup
auto workingMemory = brain->getWorkingMemory();
workingMemory->clear();

// Optimize memory access patterns
config->set("brain.memory.accessPattern", "sequential");
```

#### "Episodic memory replay issues"

**Symptoms:**
- Memories not replaying correctly
- Replay timing issues
- Memory consolidation problems

**Causes:**
1. **Replay window too small**
2. **Memory encoding issues**
3. **Time synchronization problems**
4. **Memory corruption**

**Solutions:**
```cpp
// Adjust replay window
config->set("brain.memory.episodicMemory.replayWindow", 1000.0);

// Ensure memory encoding
auto episodicMemory = brain->getEpisodicMemory();
episodicMemory->encode(experience, currentTime);

// Check memory timestamps
auto replayed = episodicMemory->replay(startTime, endTime);
```

### Development System Issues

#### "Development system not progressing"

**Symptoms:**
- Brain not developing over time
- Plasticity not changing
- Age not affecting behavior

**Causes:**
1. **Development disabled**
2. **Plasticity decay issues**
3. **Age calculation problems**
4. **Development stage transitions**

**Solutions:**
```cpp
// Enable development system
config->set("brain.development.enabled", true);

// Configure development parameters
config->set("brain.development.plasticityDecayRate", 0.01);
config->set("brain.development.maturationRate", 0.1);

// Monitor development
auto developmentSystem = brain->getDevelopmentSystem();
float plasticityMultiplier = developmentSystem->getPlasticityMultiplier();
std::cout << "Plasticity multiplier: " << plasticityMultiplier << std::endl;
```

#### "Maturation not working"

**Symptoms:**
- Neurons not maturing
- Synaptic refinement not occurring
- Pruning not happening

**Causes:**
1. **Maturation system not working**
2. **Plasticity rules incorrect**
3. **Age tracking issues**
4. **Development stage problems**

**Solutions:**
```cpp
// Ensure maturation is enabled
config->set("brain.development.maturation.enabled", true);

// Configure maturation parameters
config->set("brain.development.maturation.threshold", 0.5);
config->set("brain.development.maturation.rate", 0.01);

// Check neuron maturity
auto maturation = brain->getMaturation();
float maturity = maturation->getMaturity(neuronId);
```

### Environment and World Issues

#### "Environment not updating"

**Symptoms:**
- Environment state not changing
- Rewards not being calculated
- Actions not affecting world

**Causes:**
1. **Environment interface not implemented**
2. **Action application issues**
3. **World physics problems**
4. **Environment configuration**

**Solutions:**
```cpp
// Implement environment interface
class MyEnvironment : public Environment {
public:
    void applyAction(const MotorCommand& action) override {
        // Update environment based on action
        // Calculate new state
        // Update rewards
    }
};

// Test environment
MyEnvironment env;
env.applyAction(action);
float reward = env.getReward();
```

#### "World simulation issues"

**Symptoms:**
- World state not updating
- Physics simulation errors
- Collision detection problems

**Causes:**
1. **World update logic errors**
2. **Physics engine issues**
3. **Coordinate system problems**
4. **Boundary condition errors**

**Solutions:**
```cpp
// Debug world state
std::cout << "Position: (" << position_.x << ", " << position_.y << ")" << std::endl;
std::cout << "Velocity: (" << velocity_.x << ", " << velocity_.y << ")" << std::endl;

// Check boundary conditions
if (position_.x < 0.0 || position_.x > width_) {
    std::cerr << "Error: Position out of bounds" << std::endl;
}
```

### Agent and World Interface Issues

#### "Agent not responding to world"

**Symptoms:**
- Agent not updating based on world state
- Sensory input not working
- Motor output not being applied

**Causes:**
1. **AgentBrain not initialized**
2. **World interface not connected**
3. **Sensory processing issues**
4. **Motor decoding problems**

**Solutions:**
```cpp
// Initialize agent
auto agent = std::make_shared<AgentBrain>(brain);
agent->initialize(world);

// Check sensory input
auto percept = agent->getSensoryInput(world);
size_t inputSize = agent->getSensoryInputSize();

// Check motor output
auto action = agent->decodeMotorCommand();
size_t outputSize = agent->getMotorOutputSize();
```

#### "Sensory input not working"

**Symptoms:**
- Sensor readings not changing
- Input size not matching world
- Percept processing errors

**Causes:**
1. **Sensor implementation errors**
2. **Input size mismatch**
3. **Percept processing issues**
4. **World sensor interface**

**Solutions:**
```cpp
// Implement sensors
class MySensors {
public:
    std::vector<float> getVision() const {
        // Return vision data
    }
    
    std::vector<float> getTouch() const {
        // Return touch data
    }
};

// Check input size
auto sensors = std::make_shared<MySensors>();
agent->processSensoryInput(sensors);

// Debug percept
for (size_t i = 0; i < percept.size(); ++i) {
    std::cout << "Sensor " << i << ": " << percept[i] << std::endl;
}
```

### Advanced Issues

#### "Spike system overload"

**Symptoms:**
- Too many spike events
- Spike processing slow
- Memory system overwhelmed

**Causes:**
1. **Spike generation too frequent**
2. **Spike propagation inefficient**
3. **Spike buffer overflow**
4. **Event processing bottleneck**

**Solutions:**
```cpp
// Reduce spike generation
config->set("brain.spikeGeneration.rate", 0.1);

// Optimize spike propagation
config->set("brain.spikeProcessing.useCompression", true);

// Increase spike buffer
config->set("brain.spikeProcessing.bufferSize", 1000000);

// Parallelize spike processing
config->set("brain.spikeProcessing.parallel", true);
```

#### "Plasticity computation too slow"

**Symptoms:**
- Plasticity updates taking too long
- Learning performance degraded
- Simulation step time increasing

**Causes:**
1. **Plasticity rules too complex**
2. **Too many neurons undergoing plasticity**
3. **Inefficient plasticity computation**
4. **Memory access patterns**

**Solutions:**
```cpp
// Simplify plasticity rules
config->set("brain.plasticity.rules", "basic");

// Reduce plasticity frequency
config->set("brain.plasticity.updateFrequency", 10);

// Optimize plasticity computation
config->set("brain.plasticity.useSIMD", true);

// Reduce plasticity range
config->set("brain.plasticity.maxChangesPerStep", 1000);
```

## Debugging Techniques

### 1. Enable Debug Logging
```cpp
// Enable debug logging
nlm::Logger::getInstance().setLevel(nlm::LogLevel::DEBUG);

// Log specific components
nlm::Logger::getInstance().enableComponent(nlm::LogComponent::Brain);
nlm::Logger::getInstance().enableComponent(nlm::LogComponent::Neuromodulation);
```

### 2. Use Debug Assertions
```cpp
// Add debug assertions
void Neuron::update(double timestep) {
    assert(timestep > 0.0);
    assert(timestep <= maxTimestep_);
    
    // Debug logging
    if (debugEnabled_) {
        std::cout << "Neuron " << id_ << " updating with timestep " << timestep << std::endl;
    }
    
    // Normal update
    // ...
}
```

### 3. Use Memory Debugging Tools
```bash
// Use Valgrind for memory debugging
valgrind --tool=massif --stacks=yes ./nlm_simulation

// Use AddressSanitizer for memory corruption
g++ -fsanitize=address -g main.cpp
./a.out
```

### 4. Performance Profiling
```cpp
// Profile simulation performance
class PerformanceProfiler {
public:
    void startProfile() {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    void stopProfile() {
        auto end = std::chrono::high_resolution_clock::now();
        double time = std::chrono::duration<double>(end - startTime_).count();
        std::cout << "Operation took " << time * 1000 << "ms" << std::endl;
    }
    
private:
    std::chrono::high_resolution_clock::time_point startTime_;
};
```

### 5. Use Debugging Checkpoints
```cpp
// Add debugging checkpoints
void Brain::step(SimulationStep step) {
    if (debugEnabled_ && step % 100 == 0) {
        std::cout << "=== Checkpoint Step " << step << " ===" << std::endl;
        std::cout << "Neurons: " << getTotalNeuronCount() << std::endl;
        std::cout << "Spikes: " << getTotalSpikeCount() << std::endl;
        std::cout << "Memory: " << getWorkingMemory()->getActiveCount() << std::endl;
    }
    
    // Normal step
    // ...
}
```

## Error Codes and Meanings

### Common Error Codes
```cpp
enum class ErrorCode {
    SUCCESS = 0,
    CONFIG_ERROR = 1,           // Configuration error
    MEMORY_ERROR = 2,           // Memory allocation error
    INITIALIZATION_ERROR = 3,   // Initialization failure
    SIMULATION_ERROR = 4,       // Simulation step error
    NEUROMODULATION_ERROR = 5,  // Neuromodulation error
    MEMORY_SYSTEM_ERROR = 6,    // Memory system error
    DEVELOPMENT_ERROR = 7,      // Development system error
    ENVIRONMENT_ERROR = 8,      // Environment error
    AGENT_ERROR = 9,            // Agent error
    PERFORMANCE_ERROR = 10,     // Performance error
    UNKNOWN_ERROR = 99          // Unknown error
};
```

### Error Handling
```cpp
ErrorCode Brain::initialize() {
    // Check configuration
    if (!config_) {
        return ErrorCode::CONFIG_ERROR;
    }
    
    // Allocate memory
    if (!allocateMemory()) {
        return ErrorCode::MEMORY_ERROR;
    }
    
    // Initialize components
    if (!initializeNeurons()) {
        return ErrorCode::INITIALIZATION_ERROR;
    }
    
    if (!initializeMemorySystems()) {
        return ErrorCode::MEMORY_SYSTEM_ERROR;
    }
    
    return ErrorCode::SUCCESS;
}
```

## Error Recovery Strategies

### 1. Graceful Degradation
```cpp
ErrorCode Brain::step(SimulationStep step) {
    // Try to continue even if some components fail
    try {
        updateNeurons(step);
    } catch (const std::exception& e) {
        // Log error and continue with degraded performance
        Logger::getInstance().logError(std::string("Neuron update failed: ") + e.what());
        useDegradedMode = true;
    }
    
    try {
        updatePlasticity();
    } catch (const std::exception& e) {
        Logger::getInstance().logError(std::string("Plasticity update failed: ") + e.what());
    }
    
    return ErrorCode::SUCCESS;
}
```

### 2. Checkpoint Recovery
```cpp
ErrorCode Brain::load(const std::string& filepath) {
    try {
        // Load brain state
        // ...
        
        // Validate loaded state
        if (!validateLoadedState()) {
            return ErrorCode::CORRUPTION_ERROR;
        }
        
        return ErrorCode::SUCCESS;
    } catch (const std::exception& e) {
        Logger::getInstance().logError(std::string("Failed to load brain: ") + e.what());
        
        // Try to recover with defaults
        reset();
        initialize();
        
        return ErrorCode::RECOVERY_ERROR;
    }
}
```

### 3. Resource Recovery
```cpp
ErrorCode Brain::step(SimulationStep step) {
    ErrorCode lastError = ErrorCode::SUCCESS;
    
    try {
        updateNeurons(step);
    } catch (const std::bad_alloc& e) {
        // Memory error - try to recover
        lastError = recoverFromMemoryError();
        if (lastError == ErrorCode::SUCCESS) {
            // Retry the operation
            updateNeurons(step);
        }
    }
    
    return lastError;
}

ErrorCode Brain::recoverFromMemoryError() {
    // Free unused memory
    freeUnusedMemory();
    
    // Reduce memory usage
    reduceMemoryUsage();
    
    // Try again
    if (!allocateMemory()) {
        return ErrorCode::MEMORY_ERROR;
    }
    
    return ErrorCode::SUCCESS;
}
```

## Support and Community Help

### Getting Help
1. **Check Documentation**: Review relevant documentation sections
2. **Search Issues**: Look for similar issues in the repository
3. **Enable Debug Logging**: Enable detailed logging for debugging
4. **Reproduce Issues**: Create minimal reproduction cases

### Reporting Issues
```bash
# Create a bug report
./report_bug --issue "Simulation step timeout"
./report_bug --description "Simulation step timeout when using 1000 neurons"
./report_bug --log --output bug.log
```

### Online Resources
- **Documentation**: https://docs.neural-learning-machine.org
- **Issue Tracker**: https://github.com/neural-learning-machine/nlm/issues
- **Forum**: https://forum.neural-learning-machine.org
- **Discord**: https://discord.gg/neural-learning-machine
- **Slack**: https://slack.neural-learning-machine.org

### Emergency Support
```bash
# Contact support
./support --emergency --issue "Critical simulation failure"
./support --email support@neural-learning-machine.org
./support --phone +1-555-123-4567
```

## Conclusion

Troubleshooting is an essential skill for working with NLM simulations. By understanding common issues and their solutions, you can quickly resolve problems and continue your research.

Key principles for effective troubleshooting:

1. **Identify the Problem**: Clearly understand what is failing
2. **Check Configuration**: Ensure all settings are correct
3. **Enable Logging**: Use detailed logging for debugging
4. **Reproduce the Issue**: Create minimal reproduction cases
5. **Use Systematic Approach**: Follow a step-by-step troubleshooting process
6. **Seek Help**: Use available documentation and community resources

By following these guidelines, you can efficiently diagnose and resolve NLM simulation issues.