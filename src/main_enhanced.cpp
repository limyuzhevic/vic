Better NLM (熙然) Main Program with Enhanced Command-Line Interface

**Phase 6: Final Integration Demo**

This program demonstrates the complete integrated artificial brain with:
- Real LIF neuron dynamics
- Event-driven spike propagation with delays
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)
- Memory systems (working memory, episodic memory)
- Neuromodulation (dopamine, curiosity, novelty)
- Prediction system
- Cognitive mechanisms (planning, attention, concept formation)
- Developmental plasticity
- Checkpoint save/load
- Replay and consolidation

## Usage:

```bash
# Run basic integration demo (default)
nlm

# Run with custom configuration
nlm --config my_config.cfg

# Run with performance profiling
nlm --profile

# Run with verbose logging
nlm --verbose

# Run specific test scenarios
nlm --test basic_connectivity
nlm --test plasticity_learning
nlm --test stdp_verification

# Run full experiment with custom parameters
nlm --neurons 2000 --max_steps 5000 --checkpoint

# Export brain state for analysis
nlm --export brain_state.bin

# Load previously saved brain state
nlm --load brain_state.bin

# Run integration verification only
nlm --verify-integration

# Run memory system demonstration
nlm --demo memory

# Run neuromodulation demonstration
nlm --demo neuromodulation

# Run development demonstration
nlm --demo development

# Run with custom random seed for reproducible experiments
nlm --seed 12345

# Run with detailed statistics output
nlm --stats

# Run with simulation speed control
nlm --speed 1.0

# Run with temperature control for development
nlm --temperature 0.8

# Create configuration template
nlm --create-config config_template.cfg

# Validate configuration file
nlm --validate config_file.cfg
```

## Command Line Options:

### Basic Options
- `--config <file>` - Load configuration from file
- `--seed <num>` - Set random seed (default: 42)
- `--verbose` - Enable verbose logging
- `--quiet` - Suppress non-essential output

### Performance Options
- `--profile` - Enable performance profiling
- `--speed <factor>` - Simulation speed multiplier (0.1-10.0, default: 1.0)
- `--parallel <num>` - Number of parallel processors (default: auto)

### Demonstration Options
- `--test <type>` - Run specific test suite:
  - `basic_connectivity` - Test neural connectivity
  - `plasticity_learning` - Demonstrate learning through experience
  - `stdp_verification` - Verify STDP mechanisms
  - `memory_integration` - Test memory system integration
  - `neuromodulation` - Show neuromodulation effects
  - `development` - Demonstrate developmental changes
- `--demo <feature>` - Run demonstration of specific feature:
  - `memory` - Working memory and episodic memory demo
  - `neuromodulation` - Dopamine, curiosity, novelty demo
  - `development` - Developmental stages demo
  - `prediction` - Prediction system demo

### Data Management Options
- `--save <file>` - Save brain state to checkpoint
- `--load <file>` - Load brain state from checkpoint
- `--export <file>` - Export data for analysis
- `--checkpoint` - Enable automatic checkpointing

### Analysis Options
- `--verify-integration` - Run integration verification only
- `--stats` - Show detailed statistics
- `--monitor` - Enable real-time monitoring
- `--benchmark` - Run performance benchmarks

### Configuration Options
- `--create-config <file>` - Create configuration template
- `--validate <file>` - Validate configuration file
- `--show-config` - Display current configuration

### Advanced Options
- `--temperature <float>` - Development temperature (plasticity rate, 0.1-2.0)
- `--curiosity <float>` - Curiosity drive strength (0.0-1.0)
- `--novelty <float>` - Novelty detection sensitivity (0.0-1.0)
- `--reward <float>` - Reward magnitude (0.0-10.0)

## Configuration File Format:

```ini
# NLM Configuration Template

# Basic settings
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 1
connection_probability = 0.1

# Learning parameters
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0

# Structural plasticity
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Memory system
working_memory_capacity = 100
max_episodes = 1000

# Neuromodulation
dopamine_level = 0.5
curiosity_drive = 0.3
novelty_threshold = 0.7

# Development
development_rate = 0.1
plasticity_decay = 0.95

# Performance
replay_interval = 100
consolidation_interval = 1000
checkpoint_enabled = true
checkpoint_path = ./checkpoints

# Output
log_level = info
save_stats = true
export_format = binary
```

## Advanced Usage Examples:

### 1. Learning Experiment with Custom Parameters:

```bash
nlm \
  --neurons 5000 \
  --max_steps 10000 \
  --config learning_config.cfg \
  --checkpoint \
  --stats \
  --demo memory
```

### 2. Development Study:

```bash
nlm \
  --temperature 2.0 \
  --curiosity 0.8 \
  --novelty 0.9 \
  --seed 12345 \
  --verify-integration
```

### 3. Performance Benchmarking:

```bash
nlm \
  --profile \
  --benchmark \
  --parallel 4 \
  --speed 2.0 \
  --test plasticity_learning
```

### 4. Data Analysis Pipeline:

```bash
# Create configuration template
nlm --create-config analysis_config.cfg

# Run experiment
nlm --config analysis_config.cfg --save experiment_data.bin

# Analyze saved data
nlm --load experiment_data.bin --export analysis_results.csv
```

## Integration Verification:

The program performs comprehensive integration verification:

1. **Memory Integration Test**
   - Working memory connects to neural processing
   - Episodic memory stores and retrieves experiences
   - Associative memory forms pattern associations

2. **Neuromodulation Integration Test**
   - Dopamine affects neural excitability and plasticity
   - Curiosity drives exploration behavior
   - Novelty detection integrates with sensory processing

3. **Prediction Integration Test**
   - Prediction system works with memory
   - Prediction error signals affect learning
   - Confidence tracking implemented

4. **Development Integration Test**
   - Developmental stages affect plasticity rates
   - Structural plasticity modulated by age
   - Neural excitability changes with development

5. **Persistence Test**
   - Checkpoint save/load works
   - Brain state serialization working
   - Can resume from saved checkpoints

6. **Replay and Consolidation Test**
   - Episodic memory replay during simulation
   - Memory consolidation for important episodes
   - Integration with sleep/rest cycle

## Output Formats:

### Console Output:
- **INFO**: General operation status and progress
- **WARNING**: Potential issues or deprecations
- **ERROR**: Critical failures or invalid operations

### Data Export:
- **Binary**: Optimized for machine reading (default)
- **JSON**: Human-readable structured data
- **CSV**: Tabular data for spreadsheet analysis
- **XML**: Document-oriented data exchange

### Logging:
- **INFO**: High-level operation status
- **DEBUG**: Detailed operational data
- **PERF**: Performance metrics and timing
- **BRAIN**: Neural state snapshots

## Tips for Advanced Users:

1. **Experiment Reproducibility**: Use `--seed` to ensure reproducible results
2. **Performance Optimization**: Use `--parallel` and `--speed` options
3. **Memory Management**: Enable checkpointing for long-running experiments
4. **Data Analysis**: Export results in appropriate format for analysis
5. **Configuration Management**: Use `--create-config` to generate templates
6. **Monitoring**: Use `--monitor` to track real-time performance
7. **Custom Experiments**: Combine multiple options for custom scenarios

This enhanced main program provides a comprehensive interface for running NLM experiments, from simple demos to complex research scenarios, with extensive configuration options and output formats for various use cases.
