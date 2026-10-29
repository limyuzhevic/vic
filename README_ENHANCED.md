# NLM Enhanced Version 2.0.0

## Overview

This is the enhanced version of NLM (熙然) - Neural Learning Machine, featuring advanced command-line interfaces, sophisticated agent systems, comprehensive configuration management, and powerful debugging tools.

## Key Improvements

### 1. Enhanced Command-Line Interface
- **Multiple Execution Modes**: normal, debug, agent, benchmark, educational
- **Comprehensive Options**: Over 20 new command-line options for advanced control
- **Power-User Shortcuts**: 7 quick command shortcuts for common operations
- **Advanced Configuration**: --set option for runtime configuration changes

### 2. Advanced Agent System
- **Enhanced Sensory Processing**: Adaptive current injection with novelty-based amplification
- **Improved Motor Decoding**: Multi-factor decision making with confidence weighting
- **Sophisticated Neuromodulation**: Advanced reward prediction and eligibility traces
- **Developmental Dynamics**: Age-dependent plasticity and behavior modulation

### 3. Configuration Management
- **Multiple Configuration Sources**: File, command-line, and runtime settings
- **Advanced Config File**: Enhanced default.cfg with comprehensive options
- **Runtime Configuration**: Dynamic parameter adjustment without restarting
- **Configuration Validation**: Built-in configuration checking and validation

### 4. Advanced Testing and Debugging
- **Debug Mode**: Enhanced logging with detailed step-by-step analysis
- **Performance Analysis**: Comprehensive benchmarking and profiling tools
- **Memory Analysis**: Advanced memory monitoring and optimization
- **Neural Activity Tracking**: Real-time neural activity visualization

### 5. Power-User Features
- **Checkpoint System**: Save and load simulation states
- **Statistics Export**: Multiple output formats (CSV, JSON, etc.)
- **Learning Curve Tracking**: Monitor learning progress over time
- **Scenario Testing**: Pre-defined experimental scenarios

## Quick Start

### Basic Usage (Enhanced)

```bash
# Standard execution with all tests
./nlm

# Debug mode with enhanced logging
./nlm --mode=debug

# Agent-only mode with exploration
./nlm --agent-only --agent-mode=balanced

# Performance benchmarking
./nlm --benchmark --steps=5000

# Advanced configuration
./nlm --mode=debug --benchmark --set neuron_count=3000 --set plasticity_learning_rate=0.03

# Power-user shortcuts
-fast                          # Debug + benchmark (100 steps)
-agent-only                    # Agent-only mode (5000 steps)
-perf                         # Performance analysis
-plot                         # Plotting mode
```

### Advanced Usage

```bash
# Create and configure advanced brain
./nlm --config=production.cfg \
  --mode=debug \
  --benchmark \
  --set simulation_timestep=0.0005 \
  --set neuron_count=3000 \
  --enable-feature meta_learning \
  --enable-feature structural_plasticity \
  --export-stats=production_metrics.csv \
  --save-checkpoint=final_checkpoint.bin

# Agent training with advanced exploration
./nlm --agent-only \
  --agent-mode=balanced \
  --steps=10000 \
  --enable-feature curiosity \
  --enable-feature reward_modulation \
  --learning-curve \
  --performance-analysis
```

### Python API (Enhanced)

```python
import pynlm

# Create advanced configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
config.set("plasticity.stdp.learning_rate", 0.025)
config.set("neuromod.dopamine.scale", 1.5)

# Create advanced brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create advanced agent
agent = pynlm.createAgentBrain(brain)

# Run comprehensive simulation
metrics = run_advanced_training(brain, agent, world)
```

## New Features

### 1. Command-Line Interface

#### Core Options
- `--help`: Show comprehensive help with all options
- `--version`: Show version information
- `--config=<file>`: Load configuration from file
- `--mode=<mode>`: Set execution mode (normal, debug, agent, benchmark, educational)
- `--steps=<n>`: Number of simulation steps
- `--duration=<t>`: Simulation duration in seconds
- `--neurons=<n>`: Number of neurons
- `--seed=<n>`: Random seed

#### Advanced Options
- `--verbose`: Enable verbose logging
- `--benchmark`: Run benchmark mode
- `--agent-only`: Run only agent interface tests
- `--test-plasticity`: Run only plasticity tests
- `--test-stdp`: Run only STDP tests
- `--test-connectivity`: Run only connectivity tests
- `--export-stats=<file>`: Export statistics to file
- `--save-checkpoint=<file>`: Save brain state to checkpoint
- `--load-checkpoint=<file>`: Load brain state from checkpoint
- `--performance-analysis`: Enable performance analysis
- `--learning-curve`: Track learning curve
- `--agent-mode=<name>`: Specify agent mode
- `--enable-feature=<feature>`: Enable specific feature
- `--disable-feature=<feature>`: Disable specific feature

#### Power-User Shortcuts
- `-fast`: `--mode=debug --benchmark --steps=100`
- `-agent-only`: `--agent-only --steps=5000`
- `-perf`: `--mode=debug --performance-analysis`
- `-plot`: `--plot-mode --export-stats=stats.csv`

### 2. Agent System Enhancements

#### Sensory Processing
- **Novelty-Based Amplification**: Increased sensitivity to novel stimuli
- **Prediction Error Modulation**: Enhanced learning based on prediction errors
- **Developmental Modulation**: Age-dependent sensory processing
- **Attention Gating**: Dynamic attention control based on curiosity

#### Motor Control
- **Multi-Factor Decision Making**: Confidence-weighted action selection
- **Exploration-Exploitation Balance**: Adaptive behavioral strategies
- **Temporal Discounting**: Consideration of future rewards
- **Neuromodulation Integration**: Dopamine-based action modulation

#### Neuromodulation
- **Enhanced Reward Processing**: Sophisticated prediction error calculation
- **Eligibility Traces**: Temporal credit assignment for learning
- **Developmental Plasticity**: Age-dependent learning rates
- **Structural Plasticity**: Synaptogenesis and pruning with neuromodulation

### 3. Configuration Management

#### Configuration Sources
1. **Default Configuration**: Built-in defaults
2. **Configuration File**: Load from file (configs/default.cfg)
3. **Command Line**: Override with --set option
4. **Runtime**: Dynamic parameter adjustment

#### Advanced Configuration Options
- **Neural Parameters**: neuron_count, region_count, connection_probability
- **Plasticity Parameters**: stdp_ltp_weight, stdp_ltd_weight, synaptogenesis_rate
- **Neuromodulation Parameters**: dopamine_baseline, curiosity_threshold
- **Development Parameters**: developmental stages, plasticity_modifier
- **Performance Parameters**: simulation_timestep, max_simulation_steps

### 4. Testing and Debugging

#### Debug Mode
- **Enhanced Logging**: Detailed step-by-step output
- **Neural Activity Tracking**: Real-time neural monitoring
- **Performance Analysis**: Benchmarking and profiling
- **Memory Analysis**: Memory usage monitoring

#### Testing Modes
- **Normal Mode**: All three tests (connectivity, plasticity, STDP)
- **Agent Mode**: Focus on agent interface functionality
- **Educational Mode**: Step-by-step learning demonstrations
- **Benchmark Mode**: Performance testing and optimization

## Configuration Files

### Enhanced Default Configuration (configs/default.cfg)

```ini
# Enhanced configuration with comprehensive options

# Neural network parameters
neuron_count = 2000
region_count = 3
connection_probability = 0.15

# Plasticity settings
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.018
stdp_time_constant = 25.0
plasticity_learning_rate = 0.02

# Developmental settings
development_synaptogenesis_rate = 0.002
development_pruning_rate = 0.0005

# Neuromodulation settings
dopamine_baseline = 0.3
dopamine_sensitivity = 2.0
curiosity_threshold = 0.15
novelty_decay_rate = 0.985

# Performance settings
log_level = DEBUG
log_to_file = true
log_filename = nlm_advanced.log

# Simulation settings
simulation_time_limit = 3600.0
checkpoint_interval = 300.0
```

## Python API Enhancements

### Advanced Configuration

```python
import pynlm

# Create advanced configuration
config = pynlm.createDefaultConfig()

# Configure for advanced agent behavior
config.set("brain.neuron_count", 3000)
config.set("brain.synapse_density", 0.2)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Neuromodulation configuration
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

# Developmental configuration
config.set("development.synaptogenesis_rate", 0.002)
config.set("development.pruning_rate", 0.0005)

# Advanced brain creation
brain = pynlm.createBrain(config)
brain.initialize()
```

### Advanced Agent Creation

```python
# Create world with advanced settings
world = pynlm.createSimpleWorld()
world.configure(
    width=30.0,
    height=30.0,
    visionWidth=12,
    visionHeight=12
)

# Create agent with advanced capabilities
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Configure advanced agent behavior
agent.enableCuriosity(True)
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
```

## Performance Optimization

### Recommended Settings for Advanced Use

```bash
# High-performance configuration
./nlm \
  --mode=debug \
  --benchmark \
  --set simulation_timestep=0.0005 \
  --set neuron_count=5000 \
  --set plasticity_learning_rate=0.03 \
  --memory-analysis \
  --performance-analysis
```

### System Requirements

#### Minimum Requirements
- CPU: 4 cores or equivalent
- RAM: 8 GB minimum
- Storage: 1 GB free space

#### Recommended for Advanced Use
- CPU: 8+ cores
- RAM: 16+ GB
- Storage: 10+ GB free space

## Examples

### Advanced Agent Training

```python
def run_advanced_training(brain, agent, world, steps=5000):
    """Run comprehensive agent training with advanced monitoring."""
    
    metrics = {
        'rewards': [],
        'curiosity': [],
        'novelty': [],
        'prediction_errors': [],
        'behavioral_patterns': []
    }
    
    for step in range(steps):
        # Update world with advanced physics
        world.update(0.1)
        
        # Enhanced sensory processing with noise
        percept = world.getSensoryPercept()
        
        # Process with advanced agent
        agent.processSensoryInput(percept)
        
        # Run brain step with adaptive timing
        brain.step(step)
        
        # Decode motor command with curiosity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply reward modulation
        agent.applyRewardModulation(percept.getInternal()[0], 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect metrics
        metrics['rewards'].append(percept.getInternal()[0])
        metrics['curiosity'].append(agent.getCuriosityLevel())
        metrics['novelty'].append(agent.getNoveltyLevel())
        metrics['prediction_errors'].append(agent.getPredictionError())
        metrics['behavioral_patterns'].append(int(motor_cmd))
    
    return metrics
```

### Advanced Configuration Management

```python
# Load advanced configuration
config = pynlm.createConfig()
config.loadFromFile("production.cfg")

# Runtime configuration changes
config.set("brain.neuron_count", 5000)
config.set("neuromod.dopamine.scale", 2.0)
config.set("plasticity.stdp.learning_rate", 0.03)

# Save configuration
config.saveToFile("optimized_config.cfg")
```

## Error Handling and Troubleshooting

### Common Issues

#### Memory Issues
```bash
# Solution 1: Reduce network size
./nlm --neurons=1000

# Solution 2: Enable memory-efficient mode
./nlm --config=memory_efficient.cfg
```

#### Performance Issues
```bash
# Solution 1: Increase timestep
./nlm --set simulation_timestep=0.01

# Solution 2: Reduce network size
./nlm --neurons=500
```

#### Configuration Issues
```bash
# Solution: Fix configuration syntax
# Check for duplicate entries, missing values, and format errors
./nlm --config=your_config.cfg --validate
```

## Conclusion

This enhanced version of NLM provides comprehensive tools for advanced users, researchers, and production deployments. The power-user shortcuts, advanced configuration options, and extensive monitoring capabilities make it suitable for a wide range of applications from research to production environments.

For the most up-to-date documentation and examples, visit the NLM documentation website or check the release notes in the repository.

---

**Version**: 2.0.0  
**Build Date**: $(date +'%Y-%m-%d')  
**Author**: NLM Development Team  
**License**: MIT