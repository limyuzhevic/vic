# NLM Advanced Configuration and Command-Line Interface

This document provides comprehensive information about the enhanced configuration system and command-line interface for the NLM (Neural Learning Machine) project.

## Overview

The NLM project now features an advanced configuration system with the following improvements:

1. **Enhanced Configuration Parser**: Supports JSON, YAML, and legacy key=value formats
2. **Advanced Command-Line Interface**: Provides extensive control over brain simulation
3. **Neuromodulator Enhancements**: Biological accuracy in dopamine, acetylcholine, norepinephrine, and serotonin systems
4. **Configuration Validation**: Schema validation and consistency checks
5. **Backup and Restore**: Checkpoint management for configuration files

## Configuration System

### Supported File Formats

#### JSON Configuration
```json
{
  "experiment": "phase6_integration",
  "brain": {
    "neuron_count": 500,
    "region_count": 1,
    "connection_probability": 0.15
  },
  "simulation": {
    "timestep": 0.001,
    "max_steps": 2000
  }
}
```

#### YAML Configuration
```yaml
experiment: phase6_integration
brain:
  neuron_count: 500
  region_count: 1
  connection_probability: 0.15
simulation:
  timestep: 0.001
  max_steps: 2000
```

#### Legacy Configuration
```
# neuron_count=1000
# region_count=1
# connection_probability=0.1
# timestep=0.001
# max_steps=1000
```

### Configuration Sections

#### Brain Configuration
- `neuron_count`: Number of neurons in the brain
- `region_count`: Number of brain regions
- `connection_probability`: Probability of synaptic connections
- `random_seed`: Random seed for reproducible simulations

#### Simulation Configuration
- `timestep`: Simulation timestep in seconds
- `max_steps`: Maximum simulation steps
- `duration_seconds`: Target simulation duration

#### Memory Configuration
- `working_memory_capacity`: Size of working memory
- `episodic_memory_max_episodes`: Maximum episodic memories
- `replay_interval`: Memory replay frequency
- `consolidation_interval`: Memory consolidation frequency

#### Neuromodulation Configuration
- `enable_dopamine`: Enable dopamine system
- `enable_curiosity`: Enable curiosity system
- `enable_novelty`: Enable novelty detection
- `dopamine_baseline`, `dopamine_peak`: Dopamine parameters
- `curiosity_novelty_weight`: Curiosity sensitivity

#### Plasticity Configuration
- `stdp_enabled`: Enable STDP learning
- `hebbian_enabled`: Enable Hebbian learning
- `structural_enabled`: Enable structural plasticity
- `synaptogenesis_rate`: New synapse formation rate
- `pruning_rate`: Synapse elimination rate

#### Development Configuration
- `enable`: Enable developmental processes
- `stage`: Current developmental stage
- `initial_plasticity`: Initial plasticity level

## Command-Line Interface

### Basic Usage

```bash
# Run with default configuration
./nlm_phase6_demo

# Run with JSON configuration
./nlm_phase6_demo --config=experiment_config.json

# Run with specific parameters
./nlm_phase6_demo --neuron-count=5000 --max-steps=10000 --experiment-mode=learning
```

### Advanced Options

#### Configuration Options
```bash
--config=<file>              Load configuration from JSON/YAML file
--config-format=<format>    Config file format: json, yaml, or legacy
--validate-config            Validate configuration file and exit
--show-config-schema         Show configuration schema and exit
```

#### Brain Configuration
```bash
--neuron-count=<n>          Number of neurons (default: 1000)
--region-count=<n>          Number of brain regions (default: 1)
--connection-prob=<p>       Connection probability (0.0-1.0, default: 0.1)
--random-seed=<seed>        Random seed (default: 42)
```

#### Simulation Configuration
```bash
--simulation-timestep=<t>   Simulation timestep in seconds (default: 0.001)
--duration=<steps>          Run simulation for N steps (default: 100)
--output=<file>             Save results to file
--analysis=<mode>           Analysis mode: neural, synaptic, memory, all
```

#### Neuromodulation Options
```bash
--enable-neuromodulation     Enable neuromodulation systems
--dopamine-baseline=<b>      Dopamine baseline level (0.0-1.0, default: 0.1)
--dopamine-peak=<p>          Dopamine peak level (0.0-2.0, default: 1.0)
--curiosity-novelty-weight=<w> Novelty weight for curiosity (default: 0.5)
--curiosity-prediction-weight=<w> Prediction error weight (default: 0.5)
```

#### Memory Options
```bash
--limit-memory=<n>          Limit memory capacity to N entries
--replay-interval=<steps>   Memory replay interval (default: 100)
--consolidation-interval=<steps> Memory consolidation interval (default: 1000)
```

#### Plasticity Options
```bash
--enable-structural-plasticity Enable structural plasticity
--plasticity-rate=<r>        Structural plasticity rate (default: 0.0001)
--pruning-rate=<r>           Synaptic pruning rate (default: 0.00001)
```

#### Development Options
```bash
--development-stage=<stage>  Development stage: initial, critical, mature, adult
--developmental-interval=<steps> Development update interval (default: 1000)
```

#### Experimental Options
```bash
--noise-level=<n>           Add synaptic noise (0.0-1.0, default: 0.01)
--temperature=<t>           Temperature for stochastic processes (default: 1.0)
--learning-rate=<r>          General learning rate (default: 0.01)
--reward-function=<mode>     Reward calculation mode: constant, temporal, sparse
```

#### Checkpoint and Export Options
```bash
--checkpoint-interval=<steps> Save checkpoint every N steps (default: 1000)
--checkpoint-dir=<dir>       Checkpoint directory (default: ./checkpoints)
--export-synaptic-matrix      Export synaptic connectivity matrix
--export-neuron-activity       Export neuron activity data
--export-memory-content        Export memory system contents
--export-neuromodulator-levels  Export neuromodulator levels
--export-all=<dir>            Export all data to directory
```

### Experiment Modes

#### Basic Mode
- Standard simulation with minimal neuromodulation
- Fast execution, suitable for testing

#### Learning Mode
- Enhanced learning with neuromodulation
- Optimized for plasticity exploration

#### STDP Mode
- Focused on spike-timing dependent plasticity
- Precise timing control

#### Full Integration Mode
- Complete integration of all brain systems
- Maximum feature set enabled

```bash
--experiment-mode=<mode>     Experiment mode: basic, learning, stdp, full (default: basic)
```

### Debugging and Validation

#### Debug Options
```bash
--log-level=<level>          Logging level: trace, debug, info, warn, error, fatal
--profile                     Enable performance profiling
--debug                      Enable debug output and validation
```

#### Validation
```bash
--validate-config              Validate configuration file
--show-config-schema           Show configuration schema
```

## Configuration Files

### Enhanced Configuration Examples

#### Enhanced JSON Configuration
- File: `configs/enhanced_config.json`
- Contains all sections with detailed parameters

#### Enhanced YAML Configuration
- File: `configs/enhanced_config.yaml`
- Human-readable YAML format

#### Legacy Configuration
- File: `configs/default.cfg`
- Simple key=value format for backward compatibility

### Configuration File Backup

```bash
# Backup current configuration
./nlm_phase6_demo --save=backup_config.json

# Restore configuration
./nlm_phase6_demo --config=backup_config.json
```

## Advanced Features

### Neuromodulator System

The enhanced neuromodulator system includes:

1. **Dopamine**: Reward prediction error, reinforcement learning
2. **Acetylcholine**: Attention, memory consolidation
3. **Norepinephrine**: Arousal, vigilance
4. **Serotonin**: Mood, impulsivity, social behavior

Each neuromodulator implements biologically realistic dynamics:
- Baseline levels and peak responses
- Decay and release rates
- Plasticity modulation factors
- Activity-dependent adaptation

### Memory Systems

#### Working Memory
- Capacity-limited storage of recent neural activity
- Competitive dynamics for winner-take-all processing
- Integration with sensory input

#### Episodic Memory
- Storage of complete experience episodes
- Replay mechanism for memory consolidation
- Temporal sequencing of events

#### Associative Memory
- Pattern completion and recognition
- Hebbian learning mechanisms
- Cross-region associations

### Developmental Processes

#### Developmental Stages
1. **Initial**: High plasticity, rapid learning
2. **Critical Period**: Optimized for specific learning
3. **Maturation**: Reduced plasticity, stabilization
4. **Adult**: Low plasticity, maintenance

#### Structural Plasticity
- Synaptogenesis (new synapse formation)
- Pruning (synapse elimination)
- Developmental modulation of plasticity rates

### Prediction and Cognition

#### Prediction System
- Forward models for expected sensory input
- Prediction error computation
- Confidence-based decision making

#### Cognitive Systems
- Neural planner for action sequences
- Concept formation from patterns
- Attentional selection mechanisms

## Performance Monitoring

### Metrics Collection
```bash
--profile                     Enable performance profiling
--benchmark-mode              Optimize for benchmarking
--report-performance          Generate detailed performance reports
```

### Resource Monitoring
- CPU usage tracking
- Memory consumption monitoring
- Network activity analysis
- Simulation progress reporting

## File Export and Analysis

### Export Options
```bash
--export-synaptic-matrix      Export synaptic connectivity matrix
--export-neuron-activity       Export neuron activity data
--export-memory-content        Export memory system contents
--export-neuromodulator-levels  Export neuromodulator concentration levels
--export-all=<dir>            Export all data to directory
```

### Analysis Options
```bash
--analysis=<mode>           Analysis mode: neural, synaptic, memory, all
--export-format=<format>     Export format: json, binary, text (default: json)
--analysis-interval=<steps>  Analysis output frequency
```

## Troubleshooting

### Common Issues

#### Configuration Validation Errors
```bash
# Validate configuration file
./nlm_phase6_demo --validate-config --config=my_config.json
```

#### Missing Dependencies
```bash
# Check for required libraries
./nlm_phase6_demo --check-dependencies
```

#### Performance Issues
```bash
# Run with optimized settings
./nlm_phase6_demo --benchmark-mode --noise-level=0.001
```

### Debugging Tips

1. **Configuration Errors**: Use `--validate-config` to check syntax
2. **Memory Issues**: Limit memory with `--limit-memory=<n>`
3. **Performance Issues**: Use `--benchmark-mode` for speed
4. **Neuromodulation Issues**: Check with `--export-neuromodulator-levels`

## Advanced Usage Examples

### Example 1: Complex Integration Experiment
```bash
./nlm_phase6_demo \
  --config=experiment_config.json \
  --neuron-count=5000 \
  --max-steps=10000 \
  --experiment-mode=learning \
  --enable-neuromodulation \
  --checkpoint-interval=500 \
  --export-synaptic-matrix \
  --export-memory-content \
  --report-performance
```

### Example 2: Developmental Study
```bash
./nlm_phase6_demo \
  --development-stage=critical \
  --plasticity-rate=0.001 \
  --pruning-rate=0.0001 \
  --duration=5000 \
  --enable-structural-plasticity \
  --enable-neuromodulation \
  --export-all=developmental_study/
```

### Example 3: Benchmark Testing
```bash
./nlm_phase6_demo \
  --benchmark-mode \
  --noise-level=0.001 \
  --temperature=0.5 \
  --learning-rate=0.1 \
  --max-steps=50000 \
  --checkpoint-interval=5000
```

## Configuration Schema

The configuration system supports the following schema:

```json
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "NLM Configuration",
  "type": "object",
  "required": ["brain", "simulation"],
  "properties": {
    "brain": {
      "type": "object",
      "properties": {
        "neuron_count": {"type": "integer", "minimum": 1},
        "region_count": {"type": "integer", "minimum": 1},
        "connection_probability": {"type": "number", "minimum": 0.0, "maximum": 1.0}
      }
    },
    "simulation": {
      "type": "object",
      "properties": {
        "timestep": {"type": "number", "minimum": 0.0},
        "max_steps": {"type": "integer", "minimum": 1}
      }
    }
  }
}
```

## Future Enhancements

### Planned Features

1. **Dynamic Configuration Updates**: Runtime configuration modification
2. **Distributed Configuration**: Multi-machine configuration management
3. **Configuration Templates**: Preset configurations for different use cases
4. **Configuration History**: Version control for configuration files
5. **Machine Learning Integration**: Automated configuration optimization

## Support and Documentation

For more information, visit the NLM documentation:
- Website: https://nlm.readthedocs.io
- GitHub: https://github.com/nlm-project/nlm
- Examples: https://github.com/nlm-project/nlm/tree/main/examples

## License

MIT License - See LICENSE file for details.

---

*Last updated: 2026-11-08*
*NLM Project Team*