## Advanced Features for Power Users

NLM now includes extensive advanced configuration and control options for power users. Use these features to:

- **Tune performance** with detailed configuration options
- **Debug and validate** your simulations with validation tools
- **Control learning** with precise on/off switches for each mechanism
- **Inspect networks** with detailed structure and activity information
- **Import/export configurations** for sharing and reproducibility

### Advanced Configuration

NLM supports two configuration file formats:

#### JSON Configuration (Recommended)
```json
{
  "neuron_count": 1000,
  "region_count": 2,
  "connection_probability": 0.1,
  "enable_learning": true,
  "enable_plasticity": true,
  "enable_development": true,
  "enable_neuromodulation": true,
  "debug_mode": false,
  "working_memory_capacity_fraction": 0.1,
  "episodic_memory_max_episodes": 1000,
  "memory_replay_interval": 100,
  "exploration_balance": 0.3,
  "curiosity_threshold": 0.7,
  "reward_pe_decay_rate": 0.99
}
```

#### Legacy Key=Value Format
```
neuron_count = 1000
region_count = 2
connection_probability = 0.1
enable_learning = true
enable_plasticity = true
enable_development = true
enable_neuromodulation = true
debug_mode = false
working_memory_capacity_fraction = 0.1
episodic_memory_max_episodes = 1000
memory_replay_interval = 100
exploration_balance = 0.3
curiosity_threshold = 0.7
reward_pe_decay_rate = 0.99
```

### Advanced Command Line Options

NLM supports extensive command line options for power users:

```bash
# Basic usage
nlm [OPTIONS]

# Debug and validation
nlm --debug --validate
nlm -d -v

# Configuration import/export
nlm --import=custom_config.json
nlm --export
nlm --import=legacy.cfg --export=advanced_config.json

# Learning control (independent mechanisms)
nlm --no-learning --no-plasticity
nlm --enable-learning --no-curiosity
nlm --enable-development --enable-neuromodulation

# Network inspection
nlm --inspect=neurons
nlm --inspect=synapses
nlm --inspect=regions
nlm --inspect=connectivity
nlm --inspect=weights
nlm --inspect=activity

# Performance and profiling
nlm --profile
nlm --no-learning --no-plasticity  # Faster testing

# Help and documentation
nlm --help
nlm -h
```

### Configuration Examples

#### Example 1: High-Performance Testing
```bash
nlm \
  --no-learning \
  --no-plasticity \
  --no-development \
  --no-neuromodulation \
  --neuron_count=100 \
  --max_simulation_steps=1000 \
  --debug
```

#### Example 2: Advanced Learning Configuration
```bash
nlm \
  --config=advanced_config.json \
  --enable-curiosity \
  --enable-novelty \
  --exploration-balance=0.7 \
  --curiosity-threshold=0.5 \
  --validate
```

#### Example 3: Network Inspection
```bash
# Create a small test network
nlm --neuron_count=50 --region_count=1 --connection_probability=0.2

# Inspect network structure
./nlm --inspect=neurons    # Show neuron counts and types
./nlm --inspect=synapses   # Show synapse statistics
./nlm --inspect=regions    # Show region connectivity
```

### Performance Tuning

NLM includes many performance tuning options:

#### Memory System Tuning
```
working_memory_capacity_fraction = 0.1      # Fraction of neurons for working memory
episodic_memory_max_episodes = 1000         # Maximum number of episodic memories
memory_replay_interval = 100                # Steps between memory replay
memory_consolidation_interval = 1000        # Steps between memory consolidation
```

#### Neuromodulation Tuning
```
dopamine_learning_rate = 0.05               # Dopamine-based learning rate
curiosity_bonus_weight = 0.2               # Curiosity exploration bonus
novelty_sensitivity = 0.3                 # Novelty detection sensitivity
prediction_error_sensitivity = 0.4       # Prediction error sensitivity
```

#### Computational Efficiency
```
synaptic_delivery_threads = 4               # Number of synaptic threads
enable_optimized_spike_delivery = true     # Use optimized spike delivery
spike_delivery_batch_size = 100             # Spike processing batch size
cache_connectivity = true                   # Use connectivity caching
connectivity_cache_refresh = 500           # Cache refresh interval
```

### Validation Tools

NLM includes configuration validation:

```bash
# Validate current configuration
nlm --validate

# Validate with custom configuration
nlm --config=my_config.json --validate

# Validation checks include:
# - Required parameter validation
# - Type validation
# - Range validation
# - Parameter consistency
# - Performance recommendation
```

### Debugging and Profiling

Enable comprehensive debugging and profiling:

```bash
# Basic debugging
nlm --debug --validate

# Performance profiling
nlm --profile --debug

# Detailed debugging (shows internal state)
nlm --debug --inspect=neurons --inspect=synapses
```

## Configuration Validation

NLM's configuration system includes comprehensive validation:

1. **Basic Parameter Validation**
   - Required parameters must be set
   - Parameter ranges must be valid
   - Consistency checks between related parameters

2. **Type Validation**
   - All configuration values must be valid types
   - Conversion between types supported
   - Array validation for list parameters

3. **Advanced Validation Features**
   - Performance recommendations based on available resources
   - Warning messages for potentially problematic settings
   - Parameter consistency checks

## Troubleshooting

### Common Issues and Solutions

#### Issue: Configuration File Not Found
```bash
# Solution: Create the config file or use defaults
nlm --config=configs/my_config.json
# or
nlm --config=/path/to/config.json
```

#### Issue: Invalid Configuration Values
```bash
# Solution: Use validation first
nlm --config=my_config.json --validate
# Fix any validation errors in the config file
```

#### Issue: Slow Simulation Performance
```bash
# Solution: Reduce complexity or optimize settings
nlm --neuron_count=50 --region_count=1 \
    --no-learning --no-plasticity \
    --max_simulation_steps=1000 --debug
```

#### Issue: Learning not Working
```bash
# Solution: Check configuration
nlm --config=my_config.json --debug --inspect=neurons
# Verify learning and plasticity are enabled
```

### Help and Support

For additional help or questions:
- Check the source code comments and headers
- Use `nlm --help` for comprehensive options
- Review the documentation in docs/ directory
- Experiment with small configurations first

## Additional Resources

- **GitHub Repository**: https://github.com/nlm-project/nlm
- **Scientific Papers**: Available in the project documentation
- **Development Logs**: Chronological development records
- **Experiment Documentation**: Detailed experiment descriptions
- **Phase 6 Audit**: Complete audit of final integration

NLM continues to evolve through research and experimentation. All code is subject to revision based on scientific investigation and experimental results.
