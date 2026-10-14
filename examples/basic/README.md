Phase 6 Basic Usage Examples
===========================

This directory contains examples demonstrating various usage patterns for NLM Phase 6.

## Basic Configuration Example

### Run NLM with default settings:

```bash
./nlm
```

This will run a comprehensive integration test with default configuration.

## Custom Configuration Example

### Create a custom configuration file:

```bash
# Create a new configuration file
cat > custom_config.cfg << 'EOF'
# Custom settings for a specific experiment
neuron_count = 500
region_count = 3
connection_probability = 0.15
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.035
reward_prediction_error_weight = 0.2
working_memory_capacity = 150
EOF

# Run with custom configuration
./nlm --config custom_config.cfg
```

## Performance Configuration Example

### Create a high-performance configuration:

```bash
cat > perf_config.cfg << 'EOF'
# Performance-optimized settings
spike_queue_size = 20000
neuron_processing_batch_size = 200
plasticity_update_frequency = 0.9
working_memory_capacity = 200
episodic_memory_max_episodes = 2000
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.03
EOF

# Run with performance configuration
./nlm --config perf_config.cfg
```

## Neuromodulation Example

### Configure for neuromodulation experiments:

```bash
cat > neuromod_config.cfg << 'EOF'
# Neuromodulation settings
dopamine_baseline = 0.05
curiosity_baseline = 0.02
novelty_threshold = 0.4
reward_prediction_error_weight = 0.25
reward_modulation_tau = 150.0

# Memory with neuromodulation
working_memory_capacity = 180
episodic_memory_max_episodes = 1500

# Triplet STDP for complex learning
pre_trace_time_constant = 25.0
post_trace_time_constant = 25.0
EOF

# Run neuromodulation experiment
./nlm --config neuromod_config.cfg --log-file neuromod_experiment.log
```

## Checkpoint Example

### Save and load checkpoints:

```bash
# Run for a while to generate checkpoints
./nlm --config perf_config.cfg --save-interval 500 --log-file run.log

# Load checkpoint after 1000 steps
./nlm --config perf_config.cfg --load-checkpoint checkpoints/snapshot_1000.bin
```

## Help and Documentation

### View help information:

```bash
./nlm --help
```

### View version information:

```bash
./nlm --version
```

## Configuration File Reference

See README.md in the main directory for a complete list of configuration options.

## Troubleshooting

### Common Issues:

1. **Configuration file not found**

   ```bash
   # Check if file exists
   ls -la configs/
   
   # Run with absolute path
   ./nlm --config /workspace/app/configs/custom_config.cfg
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
   # Reduce memory settings in configs/default.cfg:
   working_memory_capacity = 50
   episodic_memory_max_episodes = 500
   ```

4. **Simulation too slow**

   ```bash
   # Optimize performance settings in configs/default.cfg:
   neuron_processing_batch_size = 100
   spike_queue_size = 5000
   ```

## Performance Tips

### For larger networks:

- Use the performance configuration (perf_config.cfg)
- Monitor memory usage with: `--monitor-memory`
- Enable visualization with: `--enable-visualization`
- Save checkpoints regularly with: `--save-interval`

### For faster simulation:

- Reduce `neuron_count` and `region_count`
- Increase `neuron_processing_batch_size`
- Decrease `working_memory_capacity`

## Advanced Usage Patterns

### 1. Multi-Stage Learning:

```bash
# Phase 1: High plasticity for rapid learning
./nlm --config phase1_config.cfg

# Phase 2: Normal learning
./nlm --config phase2_config.cfg

# Phase 3: Low plasticity for consolidation
./nlm --config phase3_config.cfg
```

### 2. Environment Adaptation:

```bash
# Start with simple environment
./nlm --config simple_env.cfg

# Adapt to complex environment
./nlm --config complex_env.cfg

# Monitor adaptation performance
./nlm --log-performance --log-file adaptation.log
```

### 3. Reward-Based Learning:

```bash
# Configure reward-based learning
cat > reward_config.cfg << 'EOF'
reward_prediction_error_weight = 0.2
reward_modulation_tau = 200.0
dopamine_baseline = 0.05
EOF

# Run reward-based learning experiment
./nlm --config reward_config.cfg --log-file reward_experiment.log
```

## Configuration Categories

1. **Simulation Settings**: neuron_count, region_count, simulation_timestep
2. **Plasticity Settings**: stdp_ltp_weight, stdp_ltd_weight, stdp_time_constant
3. **Neuromodulation Settings**: dopamine_baseline, curiosity_baseline, novelty_threshold
4. **Memory Settings**: working_memory_capacity, episodic_memory_max_episodes
5. **Performance Settings**: spike_queue_size, neuron_processing_batch_size
6. **Development Settings**: development_synaptogenesis_rate, development_pruning_rate
7. **Replay Settings**: replay_interval, consolidation_interval
8. **Logging Settings**: log_level, log_to_file, log_filename

## Quick Start Guide

### 1. Installation:

```bash
# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### 2. First Run:

```bash
# Run with default settings
./nlm
```

### 3. Custom Configuration:

```bash
# Create a custom configuration
./nlm --config your_config.cfg
```

### 4. Performance Monitoring:

```bash
# Enable performance logging
./nlm --log-performance --performance-sampling-interval 1000
```

### 5. Visualization:

```bash
# Enable visualization (if available)
./nlm --enable-visualization --visualization-update-rate 30.0
```

## Command Line Reference

### Basic Options:

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

### Advanced Options:

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

### Example Configuration File:

```bash
cat > example_config.cfg << 'EOF'
# Simulation settings
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1

# Plasticity settings
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0

# Neuromodulation settings
dopamine_baseline = 0.0
curiosity_baseline = 0.0
novelty_threshold = 0.5

# Memory settings
working_memory_capacity = 100
episodic_memory_max_episodes = 1000

# Performance settings
spike_queue_size = 10000
neuron_processing_batch_size = 50

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log
EOF

# Use the configuration
./nlm --config example_config.cfg
```

## File Structure

The NLM project is organized as follows:

```
workspace/app/
├── README.md                    # Main documentation
├── create_examples.sh           # Script to create examples
├── configs/                     # Configuration files
│   ├── default.cfg              # Default configuration
│   └── custom_config.cfg        # Custom configuration example
├── examples/                    # Usage examples
│   └── basic/                   # Basic usage examples
│       └── README.md
├── docs/                        # Documentation
│   ├── README.md                # Documentation overview
│   ├── ARCHITECTURE.md         # Architecture documentation
│   ├── SCIENCE.md              # Scientific background
│   └── ...                     # Additional documentation
├── src/                         # Source code
│   ├── core/                    # Core utilities
│   ├── brain/                   # Neural components
│   ├── plasticity/              # Plasticity rules
│   └── ...                      # Other components
└── build/                       # Build directory (created during build)
```

## Support and Community

For support and community resources:

1. **Documentation**: Check the README.md and docs/ directory
2. **Examples**: Run the examples in examples/
3. **Configuration**: Customize configs/default.cfg for your needs
4. **Issues**: Report issues on GitHub
5. **Discussion**: Join the NLM discussion forum

## Contributing

If you want to contribute to NLM Phase 6:

1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Test your changes
5. Submit a pull request

See CONTRIBUTING.md in the main repository for more information.

## Conclusion

NLM Phase 6 provides a comprehensive platform for neural network simulation with:

- **Advanced Configuration Options**: Extensive configuration for all system components
- **Performance Optimizations**: Efficient algorithms and memory management
- **Comprehensive Documentation**: Detailed examples and troubleshooting guidance
- **Robust Error Handling**: Improved error handling and validation
- **Advanced Usage Patterns**: Multiple patterns for different scenarios
- **Community Resources**: Extensive examples and documentation

This makes NLM suitable for both introductory research and advanced production use.
