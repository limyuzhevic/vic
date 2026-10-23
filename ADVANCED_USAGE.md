# NLM - Advanced Usage Guide

## Overview

This guide covers advanced features and best practices for using NLM (Neural Learning Machine). It includes:

- Command-line interface for advanced usage
- Configuration validation and error checking
- Performance profiling and debugging
- Integration with different NLM phases
- Advanced examples and patterns

## 1. Command-Line Interface (CLI)

NLM now has a comprehensive CLI for running demos, tests, and experiments without writing Python code.

### Basic CLI Usage

```bash
# Show help
./nlm --help
./nlm -h

# Show version
./nlm --version

# Basic simulation with verbose output
./nlm --verbose --neurons 1000 --steps 2000

# Debug mode with performance profiling
./nlm --debug --profile

# Load checkpoint and continue simulation
./nlm --load-checkpoint brain_state.bin --continue

# Save checkpoint after simulation
./nlm --save-checkpoint final_state.bin --steps 5000
```

### Available Commands for Different Phases

#### Phase 2 (Real Neural Computation)
```bash
# Phase 2 demo with all tests
./nlm_phase2_demo --verbose --test-mode all --neurons 1000

# Just STDP verification test
./nlm_phase2_demo --test-mode stdp

# Connectivity and learning test
./nlm_phase2_demo --test-mode connectivity
./nlm_phase2_demo --test-mode plasticity
```

#### Phase 3 (World Interaction)
```bash
# Phase 3 demo with episodes and steps
./nlm_phase3_demo 10 200 --verbose --debug

# Using the new CLI interface
./nlm_phase3_demo --help
```

#### Phase 4 (Emergent Cognition)
```bash
# Phase 4 experiments
./nlm_phase4_demo 50 --profile --verbose

# Detailed experimental output
./nlm_phase4_demo 100 --verbose --debug
```

#### Phase 6 (Integration)
```bash
# Full integration test
./nlm_phase6_demo --debug --profile --neurons 500 --steps 2000

# Integration with checkpointing
./nlm_phase6_demo --checkpoint test.bin --load-checkpoint test.bin

# Run integration tests only
./nlm_phase6_demo --no-checkpoint --no-replay --no-development
```

## 2. Configuration Validation

NLM now includes comprehensive configuration validation to catch errors early.

### Configuration Validation

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Validate configuration
validation = config.validateFull()

if validation.isValid:
    print("Configuration is valid!")
else:
    print("Configuration errors:")
    for error in validation.errors:
        print(f"  ERROR: {error}")
    
    print("\nWarnings:")
    for warning in validation.warnings:
        print(f"  WARNING: {warning}")
```

### Common Configuration Issues

**Missing Required Keys:**
```yaml
# Missing: random_seed, simulation_timestep, neuron_count, region_count
# Result: Configuration validation will fail
```

**Invalid Value Ranges:**
```yaml
# neuron_count: 50000 (too high, max is 10000)
# connection_probability: 1.5 (invalid, max is 1.0)
# simulation_timestep: 0.0 (invalid, must be > 0)
```

**Conflicting Settings:**
```yaml
# plasticity_stdp_enable: false
# plasticity_hebbian_enable: false
# Result: Warning - no learning possible!

# neuromod_curiosity_enable: false
# neuromod_novelty_enable: false
# Result: Warning - reduced exploration behavior!
```

### Configuration File Format

NLM configuration files support the following types:

```yaml
# Boolean values
neuromod_curiosity_enable: true
plasticity_stdp_enable: false

# Numeric values
neuron_count: 1000
region_count: 2
simulation_timestep: 0.001
sudp_ltp_weight: 0.01

# String values
environment_name: "GridWorld"
log_level: "INFO"

# Comments
# This is a configuration file
environment_width: 20  # Grid width
```

## 3. Performance Profiling

Add profiling to identify performance bottlenecks and optimize your NLM systems.

### Performance Profiler API

```python
import pynlm

# Enable performance profiling
profiler = pynlm.PerformanceProfiler()
profiler.enable()

# Run simulation code
brain.initialize()
for step in range(1000):
    brain.step(step)
    profiler.logStep(step, 1000)

# Performance profiling disabled
profiler.disable()

# Get total execution time
duration = profiler.getElapsedTime()
print(f"Simulation took {duration:.2f} seconds")
print(f"Speed: {1000/duration:.1f} steps/second")
```

### Performance Profiler Command Line

```bash
# Enable profiling in CLI
./nlm --profile --verbose
./nlm_phase3_demo --profile
./nlm_phase4_demo --profile

# Performance output example:
# Performance profiling enabled.
# Progress: Step 100/1000 (10.0%)
#   Elapsed: 0.12s, Estimated total: 1.20s, Remaining: 1.08s
# Performance profiler disabled. Total time: 1.15s
```

### Performance Monitoring Tips

1. **Monitor step timing:** Use `profiler.logStep()` periodically to track progress
2. **Identify bottlenecks:** Look for inconsistent step times
3. **Optimize configurations:** Large networks with high connectivity may need optimization
4. **Profile with real workloads:** Test with actual use cases, not just synthetic benchmarks

## 4. Error Handling and Logging

NLM provides comprehensive error handling and logging for debugging.

### Logging Levels

```python
# Set logging level
config.set("log_level", "DEBUG")  # All messages
# OR
config.set("log_level", "INFO")   # Important events
# OR
config.set("log_level", "WARNING")  # Only warnings and errors
# OR
config.set("log_level", "ERROR")   # Only errors
```

### Error Handling Best Practices

```python
def run_simulation_safely(config):
    try:
        brain = pynlm.createBrain(config)
        brain.initialize()
        print("Brain initialized successfully")
        
        for step in range(1000):
            brain.step(step)
            print(f"Step {step} completed")
            
    except std::runtime_error as e:
        print(f"Runtime error: {e}")
        # Handle specific runtime errors
    except std::exception as e:
        print(f"Unexpected error: {e}")
        # Handle generic exceptions
    except ...:
        print("Unknown error occurred")
        # Handle other exception types
```

### Error Message Categories

1. **Configuration Errors:** Invalid values, missing keys, conflicting settings
2. **Initialization Errors:** Failed to create brain, load files, etc.
3. **Simulation Errors:** Runtime errors during execution
4. **Performance Warnings:** Potential issues that may affect performance
5. **Debug Information:** Detailed debugging information (debug mode)

## 5. Advanced Integration Examples

### Example 1: Complete Agent with Full Features

```python
import pynlm

def advanced_agent_simulation(config):
    """Run an advanced agent simulation with all features enabled"""
    
    # Create brain with configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # Setup world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=30, 
        height=30, 
        visionWidth=16, 
        visionHeight=16
    )
    world.setRandomSeed(12345)
    world.reset()
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Track comprehensive metrics
    metrics = {
        'total_reward': 0.0,
        'episodes_stored': 0,
        'neurons_firing': 0,
        'synapses_changed': 0,
        'max_energy_reached': 0.0
    }
    
    # Run simulation loop
    for step in range(2000):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process and think
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action and apply to world
        action = agent.decodeMotorCommand()
        result = world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        agent.applyRewardModulation(result.reward, result.predicted_reward)
        
        # Update development
        agent.updateDevelopment(0.01)
        
        # Track metrics
        metrics['total_reward'] += result.reward
        metrics['max_energy_reached'] = max(
            metrics['max_energy_reached'], 
            world.getAgentBody().energy
        )
        
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Reward: {result.reward:.3f}")
            print(f"  Action: {result.action_type}")
            print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Energy: {world.getAgentBody().energy:.1f}")
            print(f"  Development: {brain.getDevelopmentalStage()}")
            print()
    
    return brain, world, agent, metrics
```

### Example 2: Multi-Phase Integration

```python
import pynlm

def multi_phase_integration():
    """Integrate across multiple NLM phases"""
    
    # Phase 2: Basic neural computation
    print("=== Phase 2: Neural Computation ===")
    phase2_config = pynlm.createDefaultConfig()
    phase2_config.set("neuron_count", 500)
    phase2_config.set("stdp_ltp_weight", 0.01)
    phase2_config.set("stdp_ltd_weight", 0.012)
    
    brain2 = pynlm.createBrain(phase2_config)
    brain2.initialize()
    
    # Run plasticity tests
    for step in range(1000):
        brain2.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0)
        brain2.step(step, step * 0.001)
    
    print("Phase 2 completed successfully!")
    
    # Phase 3: World interaction
    print("\n=== Phase 3: World Interaction ===")
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 16, 16)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain2)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run world interaction
    for step in range(500):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain2.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        agent.applyRewardModulation(1.0, 0.0)
    
    print("Phase 3 completed successfully!")
    
    # Phase 4: Emergent cognition
    print("\n=== Phase 4: Emergent Cognition ===")
    # Would involve running Phase 4 experiments
    # This is conceptual - actual implementation would use Phase4Experiment classes
    
    print("Phase 4 concept demonstrated!")
    
    # Phase 6: Final integration
    print("\n=== Phase 6: Final Integration ===")
    # Would use Phase6IntegratedExperiment class
    
    print("Integration completed successfully!")
    
    return brain2, world, agent
```

### Example 3: Checkpoint and Recovery

```python
import pynlm

def simulation_with_checkpointing(config, checkpoint_path):
    """Run simulation with checkpoint and recovery support"""
    
    # Try to load existing checkpoint
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    if os.path.exists(checkpoint_path):
        print(f"Loading checkpoint from {checkpoint_path}")
        if brain.load(checkpoint_path):
            print("Checkpoint loaded successfully")
        else:
            print("Failed to load checkpoint, starting from scratch")
    
    # Run simulation
    for step in range(config.getOr("max_steps", 1000)):
        # Your simulation code here
        brain.step(step)
        
        # Save checkpoint periodically
        if step % 500 == 0:
            brain.save(f"{checkpoint_path}_step_{step}.bin")
            print(f"Checkpoint saved at step {step}")
    
    # Save final state
    brain.save(checkpoint_path)
    print(f"Final state saved to {checkpoint_path}")
    
    return brain
```

## 6. Advanced Configuration Examples

### Example 1: Learning-Focused Configuration

```python
# Configuration optimized for learning performance
learning_config = pynlm.createDefaultConfig()

# Learning parameters
learning_config.set("plasticity_stdp_enable", True)
learning_config.set("plasticity_hebbian_enable", True)
learning_config.set("plasticity_stdp_learning_rate", 0.005)
learning_config.set("plasticity_hebbian_rate", 0.01)

# Neuromodulation for exploration
learning_config.set("neuromod_curiosity_enable", True)
learning_config.set("neuromod_curiosity_threshold", 0.05)
learning_config.set("neuromod_novelty_enable", True)
learning_config.set("neuromod_novelty_sensitivity", 0.3)

# Development for adaptability
learning_config.set("development_enable", True)
learning_config.set("development_synaptogenesis_rate", 0.001)
learning_config.set("development_pruning_rate", 0.0001)
learning_config.set("development_maturation_rate", 0.005)

# Environment for rich learning
learning_config.set("environment_width", 50)
learning_config.set("environment_height", 50)
learning_config.set("vision_width", 16)
learning_config.set("vision_height", 16)

# Resource management
learning_config.set("max_energy", 100.0)
learning_config.set("energy_decay_rate", 0.01)

print("Learning-focused configuration created")
```

### Example 2: Performance-Optimized Configuration

```python
# Configuration optimized for performance
performance_config = pynlm.createDefaultConfig()

# Moderate size for good performance
performance_config.set("neuron_count", 1000)
performance_config.set("region_count", 2)
performance_config.set("connection_probability", 0.1)

# Efficient learning
performance_config.set("plasticity_stdp_enable", True)
performance_config.set("plasticity_hebbian_enable", False)

# Fast simulation
performance_config.set("simulation_timestep", 0.001)
performance_config.set("max_steps", 5000)

# Minimal neuromodulation (performance cost)
performance_config.set("neuromod_curiosity_enable", False)
performance_config.set("neuromod_novelty_enable", False)

print("Performance-optimized configuration created")
```

## 7. Troubleshooting Common Issues

### Performance Issues

**Problem:** Simulation is slow
**Solutions:**
1. Reduce neuron count or connection probability
2. Disable neuromodulation (curiosity, novelty)
3. Reduce simulation timestep
4. Disable development processes

**Problem:** Memory usage is high
**Solutions:**
1. Use smaller network sizes
2. Disable episodic memory if not needed
3. Use periodic checkpointing instead of continuous saving

### Learning Issues

**Problem:** Brain doesn't learn from experience
**Solutions:**
1. Enable plasticity (STDP, Hebbian)
2. Check reward modulation settings
3. Verify curiosity/novelty detection
4. Ensure developmental processes are enabled

**Problem:** Brain forgets too quickly
**Solutions:**
1. Increase synaptogenesis rate
2. Reduce pruning rate
3. Enable stronger neuromodulation
4. Adjust learning rates

### Configuration Issues

**Problem:** Invalid configuration file
**Solutions:**
1. Check syntax: key=value pairs
2. Use quotes for string values
3. Verify numeric ranges
4. Use the configuration validation function

**Problem:** Missing required keys
**Solutions:**
1. Check for: random_seed, simulation_timestep, neuron_count, region_count
2. Use validateFull() to check for missing keys
3. Set reasonable defaults

## 8. Best Practices

### 1. Configuration Management

- Always validate configuration before use
- Use environment variables for sensitive data
- Document configuration settings
- Test configurations with different parameter values

### 2. Performance Monitoring

- Enable profiling for production runs
- Monitor step times and detect performance degradation
- Use checkpointing for long-running simulations
- Profile with realistic workloads

### 3. Error Handling

- Use try-catch blocks for critical operations
- Provide meaningful error messages
- Log important events for debugging
- Implement graceful shutdown on errors

### 4. Memory Management

- Be aware of memory requirements for large networks
- Use checkpointing for long simulations
- Monitor memory usage during execution
- Consider network size based on available memory

### 5. Development and Testing

- Write unit tests for individual components
- Test with different configuration settings
- Profile performance with realistic workloads
- Document best practices and common issues

## 9. Migration Guide

### From Phase 1/2 to Advanced Usage

If you're migrating from older versions of NLM:

1. **Configuration:** Use the new configuration validation functions
2. **Error Handling:** Replace basic error checking with comprehensive try-catch blocks
3. **Logging:** Enable logging for debugging and monitoring
4. **Performance:** Add profiling to identify bottlenecks
5. **Integration:** Use the new Phase 3-6 integration classes

### Upgrading to CLI

Replace Python scripts with CLI commands:

**Old (Python):**
```python
import pynlm
brain = pynlm.createBrain(config)
brain.initialize()
for step in range(1000):
    brain.step(step)
```

**New (CLI):**
```bash
./nlm --verbose --neurons 1000 --steps 1000
```

## 10. Resources

### Documentation

- `README.md` - Project overview and basic usage
- `HOW_TO_USE.md` - Detailed Python usage guide
- `docs/ARCHITECTURE.md` - System architecture
- `docs/SCIENCE.md` - Scientific background

### Examples

- `src/experiments/Phase6Demo.cpp` - Integration test
- `src/experiments/Phase3Demo.cpp` - World interaction demo
- `src/experiments/Phase4Demo.cpp` - Emergence of cognition demo
- `src/main.cpp` - Phase 2 demo with CLI

### Testing

- `tests/` - Unit tests for all components
- Configuration validation tests
- Performance benchmarking
- Integration tests

### Community

- GitHub repository for issues and contributions
- Documentation and API reference
- Discussion forums for users
- Example repositories and templates

---

## Conclusion

This advanced usage guide provides comprehensive coverage of NLM's advanced features, including:

1. **Command-line interface** for running demos and experiments
2. **Configuration validation** to catch errors early
3. **Performance profiling** to optimize simulations
4. **Comprehensive error handling** and logging
5. **Integration examples** across multiple phases
6. **Best practices** for production usage

NLM is a powerful framework for creating artificial neural systems that can learn, remember, and make decisions. With these advanced features, you can build complex, realistic simulations that closely mirror biological brain function.

Start with the basic examples, then gradually incorporate more advanced features as you become familiar with the system.
