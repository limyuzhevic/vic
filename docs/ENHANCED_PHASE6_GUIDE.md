# NLM (Neural Learning Machine) - Enhanced Phase 6 Documentation

## Overview

This document covers the enhanced Phase 6 integration of the Neural Learning Machine (NLM) project. These enhancements provide:

- **Improved Brain Integration**: Fixed header/source mismatches and completed stub implementations
- **Enhanced Memory Systems**: Better error handling, capacity management, and performance optimizations
- **Realistic Neuromodulation**: More biologically plausible models for attention, mood, arousal, and wakefulness
- **Advanced CLI Interface**: Comprehensive command-line options for debugging and configuration
- **Enhanced Documentation**: Complete API reference and usage examples
- **Performance Monitoring**: Built-in profiling and statistics tools

## Table of Contents

1. [Enhanced Brain Integration](#enhanced-brain-integration)
2. [Memory System Improvements](#memory-system-improvements)
3. [Neuromodulation Enhancements](#neuromodulation-enhancements)
4. [Command-Line Interface](#command-line-interface)
5. [Configuration Examples](#configuration-examples)
6. [API Reference](#api-reference)
7. [Debugging and Troubleshooting](#debugging-and-troubleshooting)
8. [Performance Monitoring](#performance-monitoring)
9. [Migration Guide](#migration-guide)

---

## Enhanced Brain Integration

### Key Improvements

1. **Complete Header/Source Consistency**: All forward declarations and method signatures now match between `.hpp` and `.cpp` files

2. **Fixed Stub Implementations**: All previously stubbed methods now have full implementations:
   - Working memory storage and retrieval
   - Episodic memory encoding and replay
   - Neuromodulator state updates
   - Prediction system integration

3. **Added New Methods**:
   - `getAllNeurons()`: Retrieves all neurons from the brain
   - `logPerformanceStats()`: Logs comprehensive performance statistics
   - `logWorkingMemoryStats()`: Logs working memory performance
   - `logNeuromodulationStats()`: Logs neuromodulation system state

### Performance Improvements

- **Error Handling**: Comprehensive error checking and logging
- **Memory Management**: Better capacity management and cleanup
- **Consistency Checks**: Validation of data integrity
- **Performance Monitoring**: Built-in statistics collection

---

## Memory System Improvements

### NeuralWorkingMemory Enhancements

#### New Features

1. **Capacity Management**: Automatic eviction when capacity is reached
2. **Error Handling**: Comprehensive error checking and validation
3. **Performance Monitoring**: Statistics tracking for store/retrieve/update operations
4. **Enhanced Storage**: Better activation management and maintenance connections

#### Key Methods

```cpp
// Store pattern with automatic capacity management
void store(const std::vector<float>& pattern, float strength = 1.0f);

// Store specific neuron with validation
void storeToNeuron(NeuronId neuron, float activation);

// Get memory statistics
void logPerformanceStats() const;
void logWorkingMemoryStats() const;
```

### NeuralEpisodicMemory Enhancements

#### New Features

1. **Enhanced Storage**: More efficient episode storage and retrieval
2. **Replay Optimization**: Improved replay sequence management
3. **Relevance Tracking**: Better episode relevance computation
4. **Temporal Queries**: Enhanced time-based episode retrieval

#### Key Methods

```cpp
// Store experience with validation
void storeEpisode(const EpisodicMemoryItem& episode);

// Enhanced retrieval methods
std::vector<const EpisodicMemoryItem*> retrieveSimilar(const std::vector<float>& sensoryPattern, size_t maxResults = 5) const;
std::vector<const EpisodicMemoryItem*> retrieveTemporal(SimulationStep startTime, SimulationStep endTime, size_t maxResults = 10) const;

// Replay with statistics
void replayEpisode(const EpisodicMemoryItem* episode);
```

### NeuralAssociativeMemory Enhancements

#### New Features

1. **Pattern Storage**: More efficient pattern-to-neuron mapping
2. **Association Management**: Enhanced association strength computation
3. **Activation Spreading**: Improved associative recall
4. **Memory Consolidation**: Better association updating based on experience

---

## Neuromodulation Enhancements

### Enhanced Acetylcholine (ACh)

#### New Biological Features

1. **Attention Modulation**: Dynamic attention enhancement
2. **Memory Enhancement**: Integrated memory consolidation
3. **Plasticity Regulation**: Attention-dependent plasticity
4. **Natural Fluctuations**: Simulated tonic firing patterns

#### Key Methods

```cpp
void enhanceAttention(float attentionStrength);
void getAttentionLevel() const;
void getPlasticityFactor() const;
```

### Enhanced Norepinephrine (NE)

#### New Biological Features

1. **Arousal Regulation**: Dynamic arousal modulation
2. **Vigilance Control**: Enhanced vigilance management
3. **Plasticity Scaling**: Arousal-dependent plasticity
4. **Natural Variations**: Simulated firing patterns

#### Key Methods

```cpp
void increaseArousal(float arousalLevel);
void decreaseArousal(float arousalDecay);
void getArousalLevel() const;
void getPlasticityFactor() const;
```

### Enhanced Serotonin (5-HT)

#### New Biological Features

1. **Mood Regulation**: Dynamic mood enhancement
2. **Impulse Control**: Enhanced self-control mechanisms
3. **Social Behavior**: Social attentiveness modulation
4. **Natural Fluctuations**: Simulated impulse variations

#### Key Methods

```cpp
void enhanceMood(float moodBoost);
void reduceImpulsivity(float impulseControl);
float getImpulsivityLevel() const;
float getSocialAttentivenessLevel() const;
float getSerotoninIndex() const;
```

### Enhanced Histamine (H1)

#### New Biological Features

1. **Wakefulness Regulation**: Dynamic wake-sleep modulation
2. **Cognitive Enhancement**: Cognitive state management
3. **Motor Activity**: Movement regulation
4. **Sleep Pressure**: Integrated sleep-wake regulation

#### Key Methods

```cpp
void increaseWakefulness(float wakeBoost);
void decreaseSleepPressure(float sleepGain);
float getWakefulnessLevel() const;
float getCognitiveLevel() const;
float getAlertLevel() const;
```

---

## Command-Line Interface

### Enhanced NLM CLI Options

The enhanced NLM now provides comprehensive command-line options for configuration and debugging:

#### Basic Options

```bash
--help                    Show help message and usage examples
--config FILE            Load configuration from file
--debug LEVEL           Set debug level (0=ERROR, 1=WARN, 2=INFO, 3=DEBUG)
```

#### Configuration Options

```bash
--neurons N            Set number of neurons (default: 1000)
--regions R            Set number of regions (default: 2)
--steps S              Number of simulation steps (default: 1000)
--random SEED          Random seed for reproducibility
```

#### Subsystem Control

```bash
--enable-subsystem SUB  Enable subsystem (reward, curiosity, development, novelty, prediction, planner, concept, attention)
--disable-subsystem SUB Disable subsystem
--enable-memory TYPE   Enable memory system (working, episodic, associative)
--disable-memory TYPE  Disable memory system
--enable-neuromod TYPE  Enable neuromodulator (ACh, NE, 5-HT, H1)
--disable-neuromod TYPE Disable neuromodulator
```

#### Demo and Testing Options

```bash
--check-integration     Run integration verification test
--run-demo DEMO       Run specific demo (phase3, phase4, phase5, phase6)
--benchmark            Run performance benchmark
--checkpoint FILE      Save/load checkpoint
```

#### Usage Examples

```bash
# Basic enhanced demo with subsystems
nlm_enhanced --neurons 500 --steps 5000 --enable-subsystem reward --enable-subsystem curiosity

# Integration verification with enhanced neuromodulation
nlm_enhanced --check-integration --enable-neuromod 5-HT --enable-neuromod H1

# Memory-focused demo
nlm_enhanced --enable-memory working --enable-memory episodic --steps 2000

# Debug mode with custom configuration
nlm_enhanced --config myconfig.cfg --debug 2 --enable-subsystem development

# Benchmark with enhanced neuromodulation
nlm_enhanced --benchmark --enable-neuromod ACh --enable-neuromod NE
```

---

## Configuration Examples

### Basic Configuration

```bash
# Create basic configuration file
configs/basic.cfg:
  neuron_count: 1000
  region_count: 2
  connection_probability: 0.1
  random_seed: 42
  simulation_timestep: 0.001
```

### Enhanced Configuration with Subsystems

```bash
# Enhanced configuration with all subsystems
configs/enhanced.cfg:
  # Brain configuration
  neuron_count: 1500
  region_count: 3
  connection_probability: 0.15
  random_seed: 12345
  simulation_timestep: 0.001

  # Memory systems
  memory:
    working_memory:
      enable: true
      capacity: 200
      decay_rate: 0.01
    episodic_memory:
      enable: true
      max_episodes: 1000
      replay_enabled: true
    associative_memory:
      enable: true

  # Neuromodulation
  neuromod:
    dopamine:
      scale: 1.0
    acetylcholine:
      enable: true
      attention_strength: 0.8
    norepinephrine:
      enable: true
      arousal_level: 0.7
    serotonin:
      enable: true
      mood_enhancement: 0.8
    histamine:
      enable: true
      wakefulness_level: 0.9

  # Development
  development:
    enable: true
    plasticity_modifier: 1.0

  # Curiosity and novelty
  neuromod:
    curiosity:
      enable: true
    novelty:
      enable: true
```

### Command Line Overrides

```bash
# Override configuration from command line
nlm_enhanced --config configs/enhanced.cfg \
  --neurons 800 \
  --regions 3 \
  --steps 3000 \
  --enable-subsystem reward \
  --enable-subsystem curiosity \
  --enable-neuromod 5-HT \
  --enable-neuromod H1 \
  --debug 2
```

---

## API Reference

### NeuralWorkingMemory

```cpp
class NeuralWorkingMemory {
public:
    // Constructors/Destructors
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();

    // Initialization
    void initialize(Brain* brain);

    // Memory operations
    void store(const std::vector<float>& pattern, float strength = 1.0f);
    void storeToNeuron(NeuronId neuron, float activation);
    std::vector<float> retrieve() const;
    bool contains(NeuronId neuron) const;

    // Query methods
    float getNeuronActivation(NeuronId neuron) const;
    size_t getActiveTraces() const;
    size_t getMemoryNeuronCount() const;
    float getMemoryActivity() const;

    // Configuration
    void setCapacity(size_t cap);
    void setDecayRate(float rate);
    size_t getCapacity() const;
    float getDecayRate() const;

    // Operations
    void update(TimestepDuration dt);
    void clear();
    void strengthenMemory(float factor);
    void runCompetition();
    bool isWinning(NeuronId neuron) const;

    // Performance
    void logPerformanceStats() const;
    void logWorkingMemoryStats() const;

    // Checkpoint support
    void restoreSnapshot(const std::vector<NeuronId>& neurons,
                        const std::vector<float>& activations,
                        const std::vector<SimulationStep>& timestamps);
};
```

### Enhanced Acetylcholine

```cpp
class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();

    void initialize(Brain* brain);
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);

    // Enhanced methods
    void enhanceAttention(float attentionStrength);
    float getAttentionLevel() const;
    void getAttentionLevel() const;
};
```

### Complete API Reference

The full API reference is available in the source code headers. The enhanced versions add additional methods for more fine-grained control and monitoring.

---

## Debugging and Troubleshooting

### Common Issues and Solutions

#### Issue 1: Memory Capacity Exhaustion

**Problem**: Working memory fills up and stops accepting new entries.

**Solution**: Enable automatic eviction or increase memory capacity.

**Usage**:
```bash
# Enable automatic capacity management
nlm_enhanced --enable-subsystem reward --enable-memory working --steps 1000

# Or set custom capacity
configs/custom.cfg:
  memory:
    working_memory:
      capacity: 500
      decay_rate: 0.01
```

#### Issue 2: Neuromodulator Not Responding

**Problem**: Neuromodulators (ACh, NE, 5-HT, H1) not affecting brain activity.

**Solution**: Check that neuromodulators are enabled and properly initialized.

**Usage**:
```bash
# Enable all neuromodulators
nlm_enhanced --enable-neuromod ACh --enable-neuromod NE --enable-neuromod 5-HT --enable-neuromod H1

# Enable development which activates neuromodulation
nlm_enhanced --enable-subsystem development --enable-neuromod ACh
```

#### Issue 3: Memory Replay Not Working

**Problem**: Episodic memory replay not functioning correctly.

**Solution**: Ensure episodic memory is enabled and replay is enabled.

**Usage**:
```bash
# Enable episodic memory with replay
nlm_enhanced --enable-memory episodic --steps 2000

# Check episodic memory stats
# (Enable debug logging for detailed information)
nlm_enhanced --debug 3 --enable-memory episodic
```

### Debug Logging Levels

- **Level 0 (ERROR)**: Only critical errors and fatal exceptions
- **Level 1 (WARNING)**: Warnings and recoverable errors
- **Level 2 (INFO)**: General information and subsystem state changes
- **Level 3 (DEBUG)**: Detailed trace information and performance metrics

### Performance Monitoring

#### Enable Performance Tracking

```bash
# Enable comprehensive performance monitoring
nlm_enhanced --debug 3 --enable-subsystem reward --enable-subsystem curiosity

# Monitor specific subsystems
nlm_enhanced --debug 3 --enable-neuromod ACh --enable-neuromod 5-HT
```

#### Performance Metrics

The enhanced system provides the following performance metrics:

- **Memory Operations**: Store, retrieve, and update statistics
- **Neuromodulator State**: Current levels and activity
- **Brain Performance**: Spike counts, firing rates, and activity levels
- **Subsystem Performance**: Integration and coordination metrics

---

## Performance Monitoring

### Performance Monitoring Tools

The enhanced NLM includes comprehensive performance monitoring tools:

#### Performance Metrics Collection

```cpp
class PerformanceMonitor {
public:
    // Memory performance
    size_t getMemoryStoreCount() const;
    size_t getMemoryRetrieveCount() const;
    size_t getMemoryUpdateCount() const;
    float getMemoryActivity() const;

    // Neuromodulator performance
    float getAChActivity() const;
    float getNEActivity() const;
    float get5HTActivity() const;
    float getH1Activity() const;

    // Brain performance
    size_t getTotalSpikeCount() const;
    float getAverageFiringRate() const;
    size_t getActiveNeuronCount() const;

    // Integration metrics
    float getIntegrationScore() const;
    float getNeuromodulationScore() const;
    float getMemoryScore() const;

    // System health
    bool isSystemHealthy() const;
    std::string getHealthReport() const;
};
```

#### Real-Time Monitoring

```bash
# Start with performance monitoring enabled
nlm_enhanced --debug 3 --benchmark

# Monitor specific subsystems
nlm_enhanced --debug 3 --enable-neuromod ACh --enable-neuromod 5-HT
```

### Performance Analysis

#### Key Performance Indicators

1. **Memory Efficiency**: Ratio of stored to retrieved memory operations
2. **Neuromodulator Responsiveness**: How quickly neuromodulators respond to stimuli
3. **Integration Quality**: How well subsystems coordinate
4. **Learning Rate**: Speed of synaptic modification and learning
5. **Stability**: Consistency of neural activity over time

#### Performance Optimization

1. **Memory Management**: Optimize working memory capacity and decay rates
2. **Neuromodulator Balance**: Ensure proper neuromodulator levels
3. **Integration Coordination**: Improve subsystem communication
4. **Learning Efficiency**: Enhance plasticity mechanisms

---

## Migration Guide

### From Phase 5 to Enhanced Phase 6

#### Breaking Changes

1. **API Changes**: Some method signatures have changed for better error handling
2. **Configuration Changes**: New configuration options for enhanced subsystems
3. **Default Behavior**: Enhanced defaults for better user experience

#### Migration Steps

1. **Update Configuration**: Migrate existing configuration files to enhanced format
2. **Enable Enhanced Subsystems**: Enable new neuromodulators and memory features
3. **Update Code**: Adapt code to new method signatures and error handling
4. **Testing**: Run enhanced tests to verify functionality

#### Configuration Migration Example

```bash
# Old Phase 5 configuration
configs/phase5.cfg:
  neuron_count: 1000
  region_count: 1
  connection_probability: 0.05

# New enhanced configuration
configs/phase6_enhanced.cfg:
  neuron_count: 1000
  region_count: 2  # Increased for better integration
  connection_probability: 0.1  # Enhanced connectivity
  memory:
    working_memory:
      capacity: 100
      decay_rate: 0.01
    episodic_memory:
      enable: true
      max_episodes: 500
  neuromod:
    acetylcholine:
      enable: true
      attention_strength: 0.8
    norepinephrine:
      enable: true
      arousal_level: 0.7
```

#### Code Migration Example

```cpp
// Old Phase 5 code (simplified)
if (brain->getWorkingMemory()) {
    brain->getWorkingMemory()->store(pattern, strength);
}

// New Phase 6 enhanced code
if (brain->getWorkingMemory()) {
    // Enhanced error handling and logging
    brain->getWorkingMemory()->store(pattern, strength);
    
    // Performance monitoring
    brain->getWorkingMemory()->logPerformanceStats();
}

// Old neuromodulation
brain->applyNeuromodulation(dopamineSignal);

// New enhanced neuromodulation
if (brain->getAcetylcholine()) {
    brain->getAcetylcholine()->enhanceAttention(attentionLevel);
}
if (brain->getNorepinephrine()) {
    brain->getNorepinephrine()->increaseArousal(arousalLevel);
}
```

### Troubleshooting for Migrated Systems

1. **Performance Degradation**: Enable performance monitoring and optimize configuration
2. **Memory Issues**: Check working memory capacity and decay rates
3. **Neuromodulator Issues**: Ensure all neuromodulators are properly enabled
4. **Integration Issues**: Run integration verification tests

---

## Advanced Usage

### Custom Configuration

#### Create Custom Configuration File

```bash
# Create custom configuration
cat > configs/my_custom.cfg << EOF
# Custom brain configuration
neuron_count: 2000
region_count: 3
connection_probability: 0.12
random_seed: 98765
simulation_timestep: 0.001

# Enhanced memory systems
memory:
  working_memory:
    capacity: 300
    decay_rate: 0.005
  episodic_memory:
    enable: true
    max_episodes: 2000
    replay_enabled: true

# Advanced neuromodulation
neuromod:
  dopamine:
    scale: 1.5
  acetylcholine:
    enable: true
    attention_strength: 1.0
  norepinephrine:
    enable: true
    arousal_level: 0.8
  serotonin:
    enable: true
    mood_enhancement: 1.0
  histamine:
    enable: true
    wakefulness_level: 1.0

# Development and learning
development:
  enable: true
  plasticity_modifier: 1.2
  neuromod:
    curiosity:
      enable: true
    novelty:
      enable: true
EOF
```

### Advanced Command Line Usage

```bash
# Run with custom configuration and enhanced debugging
nlm_enhanced --config configs/my_custom.cfg --debug 3 \
  --check-integration \
  --enable-subsystem reward \
  --enable-subsystem curiosity \
  --enable-memory working \
  --enable-memory episodic \
  --enable-neuromod ACh \
  --enable-neuromod NE \
  --enable-neuromod 5-HT \
  --enable-neuromod H1 \
  --steps 5000 \
  --random 98765
```

### Performance Analysis

```bash
# Run with performance benchmarking
nlm_enhanced --benchmark --debug 3

# Analyze performance with custom script
./analyze_performance.py --output performance_report.html
```

## Summary

The enhanced Phase 6 NLM provides:

1. **Complete Integration**: All brain systems work together cohesively
2. **Enhanced Memory**: Better error handling, capacity management, and performance
3. **Realistic Neuromodulation**: Biologically plausible neuromodulator models
4. **Advanced CLI**: Comprehensive command-line interface for debugging and configuration
5. **Performance Monitoring**: Built-in profiling and statistics tools
6. **Enhanced Documentation**: Complete API reference and usage examples

The enhanced Phase 6 NLM represents a significant improvement over Phase 5, providing better integration, more realistic neuromodulation, and enhanced user experience through comprehensive CLI options and performance monitoring tools.