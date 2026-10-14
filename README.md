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

## Advanced Configuration Options

NLM Phase 6 supports extensive configuration options for advanced users. Key advanced settings include:

### Advanced STDP Configuration
- **stdp_ltp_weight**: Long-term potentiation weight (default: 0.02)
- **stdp_ltd_weight**: Long-term depression weight (default: 0.015)  
- **stdp_time_constant**: STDP time constant in ms (default: 20.0)
- **stdp_max_weight_change**: Maximum weight change per STDP update (default: 0.05)
- **stdp_weight_history_size**: Size of spike history buffer (default: 100)

### Reward-Modulated STDP
- **reward_prediction_error_weight**: Weight for reward prediction error (default: 0.1)
- **reward_modulation_tau**: Time constant for reward modulation (default: 100.0)
- **reward_baseline**: Baseline reward level (default: 0.0)

### Triplet STDP
- **pre_trace_time_constant**: Time constant for pre-synaptic trace (default: 20.0)
- **post_trace_time_constant**: Time constant for post-synaptic trace (default: 20.0)
- **triplet_learning_rate**: Learning rate for triplet interactions (default: 0.01)
- **third_order_learning_rate**: Learning rate for third-order interactions (default: 0.001)

### Neuromodulation
- **dopamine_baseline**: Baseline dopamine level (default: 0.0)
- **dopamine_gain**: Dopamine gain factor (default: 1.0)
- **curiosity_baseline**: Baseline curiosity level (default: 0.0)
- **novelty_threshold**: Threshold for novelty detection (default: 0.5)

### Memory Systems
- **working_memory_capacity**: Capacity of working memory (default: 100)
- **episodic_memory_max_episodes**: Maximum number of episodic memory episodes (default: 1000)
- **associative_memory_capacity**: Capacity of associative memory (default: 50)

### Performance Tuning
- **spike_queue_size**: Size of spike event queue (default: 10000)
- **neuron_processing_batch_size**: Batch size for neuron processing (default: 50)
- **plasticity_update_frequency**: Frequency of plasticity updates (default: 1.0)

## Advanced Usage Examples

### Example 1: Custom STDP Configuration
```bash
# Create custom configuration file
cat > custom_config.cfg << EOF
# Override default STDP parameters
stdp_ltp_weight = 0.05
stdp_ltd_weight = 0.06
stdp_time_constant = 25.0
stdp_max_weight_change = 0.08

# Add reward-modulated STDP
reward_prediction_error_weight = 0.15
reward_modulation_tau = 150.0

# Configure triplet STDP
pre_trace_time_constant = 25.0
post_trace_time_constant = 25.0
EOF

# Run with custom configuration
./nlm --config custom_config.cfg
```

### Example 2: High-Performance Configuration
```bash
# High-performance configuration for large networks
cat > perf_config.cfg << EOF
# Performance tuning
spike_queue_size = 50000
neuron_processing_batch_size = 200
plasticity_update_frequency = 0.8

# Memory optimization
working_memory_capacity = 200
episodic_memory_max_episodes = 5000

# Fast learning
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.035
EOF

# Run with performance configuration
./nlm --config perf_config.cfg
```

### Example 3: Neuromodulation Experiment
```bash
# Configure for neuromodulation experiments
cat > neuromod_config.cfg << EOF
# Enable neuromodulation
dopamine_baseline = 0.1
curiosity_baseline = 0.05
novelty_threshold = 0.3

# Memory with neuromodulation
working_memory_capacity = 150
episodic_memory_max_episodes = 2000

# Advanced STDP with reward
reward_prediction_error_weight = 0.2
reward_modulation_tau = 200.0
EOF

# Run neuromodulation experiment
./nlm --config neuromod_config.cfg
```

## Command Line Interface

NLM Phase 6 provides enhanced command line options:

### Basic Options
```bash
./nlm [options]

Options:
  --help                    Show help information
  --version                 Show version information
  --config <file>           Load configuration from file
  --log-level <level>       Set log level (DEBUG, INFO, WARNING, ERROR)
  --checkpoint-dir <dir>    Set checkpoint directory
  --random-seed <seed>      Set random seed for reproducible experiments
```

### Advanced Options
```bash
  --neuron-count <count>    Number of neurons (default: 1000)
  --region-count <count>    Number of brain regions (default: 2)
  --connection-prob <prob>  Connection probability (default: 0.1)
  --max-steps <steps>       Maximum simulation steps (default: 10000)
  --enable-visualization    Enable visualization (default: false)
  --visualization-update-rate <rate> Visualization update rate in Hz (default: 30.0)
  --save-interval <steps>   Save checkpoint interval (default: 1000)
  --load-checkpoint <file>  Load brain state from checkpoint
```

## Configuration File Format

The configuration file uses a simple key=value format:

```
# Comments start with #
key = value
string_key = "quoted value"
bool_key = true
float_key = 1.23
int_key = 42
```

### Available Configuration Categories

1. **Simulation Settings**: neuron_count, region_count, simulation_timestep
2. **Plasticity Settings**: stdp_ltp_weight, stdp_ltd_weight, stdp_time_constant
3. **Neuromodulation Settings**: dopamine_baseline, curiosity_baseline, novelty_threshold
4. **Memory Settings**: working_memory_capacity, episodic_memory_max_episodes
5. **Performance Settings**: spike_queue_size, neuron_processing_batch_size
6. **Development Settings**: development_synaptogenesis_rate, development_pruning_rate
7. **Replay Settings**: replay_interval, consolidation_interval
8. **Logging Settings**: log_level, log_to_file, log_filename

## Advanced Usage Patterns

### 1. Multi-Stage Learning
```bash
# Phase 1: High plasticity for rapid learning
./nlm --config phase1_config.cfg

# Phase 2: Normal learning
./nlm --config phase2_config.cfg

# Phase 3: Low plasticity for consolidation
./nlm --config phase3_config.cfg
```

### 2. Environment Adaptation
```bash
# Start with simple environment
./nlm --config simple_env.cfg

# Adapt to complex environment
./nlm --config complex_env.cfg

# Monitor adaptation performance
./nlm --log-performance --log-file adaptation.log
```

### 3. Reward-Based Learning
```bash
# Configure reward-based learning
cat > reward_config.cfg << EOF
reward_prediction_error_weight = 0.2
reward_modulation_tau = 200.0
dopamine_baseline = 0.05
EOF

# Run reward-based learning experiment
./nlm --config reward_config.cfg --log-file reward_experiment.log
```

## Troubleshooting

### Common Issues and Solutions

1. **Configuration file not found**
   ```bash
   # Check if file exists
   ls -la configs/
   
   # Run with absolute path
   ./nlm --config /workspace/app/configs/custom.cfg
   ```

2. **Invalid configuration values**
   ```bash
   # Check log files for errors
   tail -f nlm.log
   
   
   # Use default configuration
   ./nlm
   ```

3. **Memory usage too high**
   ```bash
   # Reduce memory settings
   # In configs/default.cfg:
   working_memory_capacity = 50
   episodic_memory_max_episodes = 500
   ```

4. **Simulation too slow**
   ```bash
   # Optimize performance settings
   # In configs/default.cfg:
   neuron_processing_batch_size = 100
   spike_queue_size = 5000
   ```

## Advanced Topics

### 1. Checkpoint System
NLM Phase 6 includes a robust checkpoint system for saving and loading brain states:

```bash
# Save checkpoint
./nlm --save-interval 1000 --save-checkpoint

# Load checkpoint
./nlm --load-checkpoint checkpoints/snapshot_1000.bin
```

### 2. Visualization
Enable visualization for interactive monitoring:

```bash
# Enable with high update rate
./nlm --enable-visualization --visualization-update-rate 60.0

# Use with specific configuration
./nlm --config vis_config.cfg --visualization-window-size 1024
```

### 3. Parallel Processing
Configure for parallel processing on multi-core systems:

```bash
# Configure for parallel processing
# In configs/default.cfg:
neuron_processing_batch_size = 200
spike_queue_size = 20000
```

## Performance Monitoring

NLM Phase 6 includes comprehensive performance monitoring:

```bash
# Enable performance logging
./nlm --log-performance --performance-sampling-interval 1000

# Monitor resource usage
./nlm --monitor-memory --monitor-cpu --log-file perf.log
```

## Troubleshooting Script

Create a troubleshooting script for common issues:

```bash
#!/bin/bash
# troubleshoot_nlm.sh

echo "NLM Troubleshooting Script"
echo "==========================="

echo "Checking configuration..."
if [ ! -f "configs/default.cfg" ]; then
    echo "Error: Configuration file not found!"
    exit 1
fi

echo "Checking build directory..."
if [ ! -f "build/nlm" ]; then
    echo "Error: Binary not found!"
    echo "Please run: mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make"
    exit 1
fi

echo "Checking available memory..."
MEM_INFO=$(free -h | grep "^Mem:" | awk '{print $2}')
if [[ "$MEM_INFO" < "1G" ]]; then
    echo "Warning: Low memory ($MEM_INFO)"
    echo "Consider reducing memory settings in configuration"
fi

echo "Troubleshooting completed."
```

## Conclusion

NLM Phase 6 provides a comprehensive, extensible platform for neural network simulation with advanced configuration options, performance optimizations, and extensive documentation. The improvements include:

- **Enhanced STDP Rules**: Standard, reward-modulated, and triplet STDP
- **Advanced Configuration**: Extensive configuration options for all system components
- **Performance Optimizations**: Efficient algorithms and memory management
- **Comprehensive Documentation**: Detailed examples and troubleshooting guidance
- **Robust Error Handling**: Improved error handling and validation
- **Advanced Usage Patterns**: Multiple usage patterns for different scenarios

These improvements make NLM Phase 6 suitable for both introductory research and advanced production use.
