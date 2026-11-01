# NLM - Advanced User Guide

## Introduction

This guide covers advanced features and use cases for NLM (Neural Learning Machine). If you're new to NLM, start with the beginner guides (`easy_usage.md` and `HOW_TO_USE.md`).

## Command Line Interface

NLM provides a comprehensive command-line interface for advanced users:

### Basic Usage

```bash
# Run Phase 2 demo with custom configuration
nlm --config=my_config.cfg --neurons=2000 --steps=5000 --seed=123

# Run with debugging enabled
nlm --config=default.cfg --debug --verbose

# Print configuration
nlm --config=default.cfg --print-config

# Run benchmark
nlm --benchmark --neurons=1000 --steps=1000

# Enable profiling
nlm --config=test.cfg --profile --enable-benchmarking
```

### Advanced Command Line Options

```bash
# Multiple options in any order
nlm --neurons=5000 --steps=2000 --seed=456 --timestep=0.002 \
    --region-count=3 --connection-prob=0.2 --log-level=DEBUG \
    --enable-plasticity --enable-reward-modulation --enable-curiosity

# Configuration file with command line overrides
nlm --config=base_config.cfg \
    --neurons=10000 \
    --steps=50000 \
    --enable-plasticity=false \
    --enable-reward-modulation=true
```

## Configuration System

### Advanced Configuration Options

NLM supports advanced configuration options:

```python
# Using Python bindings
config = pynlm.createDefaultConfig()

# Advanced neural configuration
config.set("brain.neuron_count", 10000)
config.set("brain.synapse_density", 0.15)
config.set("brain.connection_probability", 0.2)
config.set("brain.v_thresh", -55.0)      # Higher threshold
config.set("brain.v_rest", -65.0)       # More negative resting potential
config.set("brain.v_reset", -70.0)      # Reset closer to threshold
config.set("brain.tau_mem", 25.0)       # Longer membrane time constant
config.set("brain.tau_ref", 3.0)        # Longer refractory period

# Advanced plasticity configuration
config.set("plasticity.stdp.enable", true)
config.set("plasticity.stdp.learning_rate", 0.005)  # Faster learning
config.set("plasticity.stdp.tau_plus", 25.0)        # Longer LTP time constant
config.set("plasticity.stdp.tau_minus", 30.0)       # Longer LTD time constant
config.set("plasticity.hebbian.enable", true)
config.set("plasticity.hebbian.strength", 0.1)      # Hebbian strength
config.set("plasticity.structural.enable", true)
config.set("plasticity.structural.synaptogenesis_rate", 0.002)  # Higher synaptogenesis
config.set("plasticity.structural.pruning_rate", 0.0001)          # Lower pruning

# Advanced neuromodulation configuration
config.set("neuromod.dopamine.scale", 1.5)            # Stronger dopamine effect
config.set("neuromod.curiosity.enable", true)
config.set("neuromod.curiosity.base_rate", 0.3)       # Higher base curiosity
config.set("neuromod.novelty.enable", true)
config.set("neuromod.novelty.sensitivity", 2.0)       # More sensitive to novelty

# Advanced developmental configuration
config.set("development.enable", true)
config.set("development.initial_plasticity", 2.0)      # High initial plasticity
config.set("development.critical_period_start", 100.0) # Early critical period
config.set("development.critical_period_end", 500.0)   # Longer critical period
config.set("development.maturation_rate", 0.8)          # Slower maturation

# Advanced simulation configuration
config.set("simulation.max_steps", 100000)
config.set("simulation.time_limit", 3600.0)  # 1 hour
config.set("simulation.checkpoint_interval", 1000)
config.set("simulation.save_state", true)

# Advanced logging configuration
config.set("logging.level", "DEBUG")
config.set("logging.to_file", true)
config.set("logging.file_path", "nlm_detailed.log")
config.set("logging.max_size_mb", 100)
config.set("logging.include_stack_traces", true)
```

## Advanced Simulation Features

### Custom Simulation Loops

```python
import pynlm

# Create a custom simulation with advanced features
def run_advanced_simulation():
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 5000)
    config.set("plasticity.stdp.learning_rate", 0.01)
    config.set("neuromod.curiosity.enable", true)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run custom simulation loop
    checkpoint_interval = 1000
    step = 0
    
    while step < 10000:
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Save checkpoint
        if step % checkpoint_interval == 0:
            brain.save(f"checkpoint_step_{step}.bin")
            NLM_LOG_INFO(f"Checkpoint saved at step {step}")
        
        # Custom learning logic
        if step % 500 == 0:
            # Adjust learning rates based on performance
            if brain.getFiringNeuronCount() < 100:
                config.set("plasticity.stdp.learning_rate", 0.02)
                NLM_LOG_INFO("Low activity detected, increasing learning rate")
            
            # Adjust curiosity based on novelty
            novelty = agent.getNoveltyLevel()
            if novelty > 0.8:
                config.set("neuromod.curiosity.base_rate", 0.5)
                NLM_LOG_INFO("High novelty detected, increasing curiosity")
        
        step += 1
    
    # Load last checkpoint for analysis
    brain.load("checkpoint_step_9500.bin")
    NLM_LOG_INFO("Loaded checkpoint for post-simulation analysis")
    
    return brain, world, agent
```

### Advanced Agent Configuration

```python
# Advanced agent configuration with custom behaviors
def create_advanced_agent(brain, world):
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Custom configuration
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Set custom thresholds for behavior
    agent.setActionThreshold(pynlm.ActionType.Interact, 0.7)  # Higher interact threshold
    agent.setExplorationThreshold(0.4)  # Lower exploration threshold
    
    # Configure neuromodulation
    agent.setDopamineSensitivity(1.2)
    agent.setCuriosityBaseRate(0.3)
    agent.setNoveltySensitivity(1.5)
    
    # Custom learning rules
    def custom_learning_rule(reward, prediction_error):
        # Implement custom learning based on reward and prediction error
        if reward > 0.5:
            return reward * 1.5  # Amplify positive rewards
        elif reward < -0.5:
            return reward * 0.8  # Attenuate negative rewards
        else:
            return reward * prediction_error * 0.5
    
    agent.setCustomLearningRule(custom_learning_rule)
    
    # Custom action selection
    def custom_action_selection(action_qualities):
        # Implement custom action selection strategy
        # Example: prefer exploration when curiosity is high
        if agent.getCuriosityLevel() > 0.7:
            # Bias towards exploration actions
            action_qualities[pynlm.ActionType.TurnLeft] *= 1.2
            action_qualities[pynlm.ActionType.TurnRight] *= 1.2
        
        return max(action_qualities.items(), key=lambda x: x[1])[0]
    
    agent.setCustomActionSelection(custom_action_selection)
    
    return agent
```

## Debugging and Analysis Tools

### Advanced Debugging Features

```python
# Advanced debugging and analysis tools
def run_advanced_debugging(brain, agent, world):
    # Enable comprehensive debugging
    import time
    
    # Performance profiling
    start_time = time.time()
    
    # Neural activity profiling
    neuron_stats = []
    for step in range(0, 1000, 100):
        # Run simulation
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect statistics
        stats = {
            'step': step,
            'firing_rate': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'development_stage': brain.getDevelopmentalStage(),
            'synapse_count': brain.getTotalSynapseCount(),
            'region_count': brain.getRegionCount()
        }
        neuron_stats.append(stats)
    
    end_time = time.time()
    
    # Performance analysis
    total_time = end_time - start_time
    steps_per_second = len(neuron_stats) * 10 / total_time  # 10Hz update rate
    
    print(f"Performance: {steps_per_second:.1f} steps/second")
    print(f"Average firing rate: {sum(s['avg_firing_rate'] for s in neuron_stats) / len(neuron_stats):.2f} Hz")
    print(f"Average curiosity: {sum(s['curiosity'] for s in neuron_stats) / len(neuron_stats):.3f}")
    
    # Identify key activity patterns
    peak_activity = max(neuron_stats, key=lambda s: s['firing_rate'])
    print(f"Peak activity at step {peak_activity['step']}: {peak_activity['firing_rate']} firing neurons")
    
    # Activity trends
    firing_trend = [s['firing_rate'] for s in neuron_stats]
    if firing_trend[-1] > firing_trend[0]:
        print("Activity trend: Increasing")
    elif firing_trend[-1] < firing_trend[0]:
        print("Activity trend: Decreasing")
    else:
        print("Activity trend: Stable")
    
    return neuron_stats
```

## Configuration File Templates

### Advanced Configuration File

```ini
# Advanced NLM Configuration File

# Basic Configuration
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 10000
region_count = 3
connection_probability = 0.15
log_level = INFO
log_to_file = true
log_filename = nlm_advanced.log

# Neural Configuration
v_thresh = -50.0
v_rest = -70.0
v_reset = -75.0
tau_mem = 20.0
tau_ref = 2.0

# Plasticity Configuration
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0
hebbian_strength = 0.1
enable_hebbian = true
enable_structural = true
synaptogenesis_rate = 0.001
pruning_rate = 0.0001

# Neuromodulation Configuration
dopamine_baseline = 0.1
dopamine_scale = 1.5
curiosity_base_rate = 0.3
curiosity_sensitivity = 1.0
novelty_sensitivity = 1.5
reward_discount_factor = 0.99

# Development Configuration
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001
plasticity_modifier_start = 1.0
plasticity_modifier_end = 0.2

# Simulation Configuration
max_simulation_steps = 100000
simulation_time_limit = 3600.0
checkpoint_interval = 1000
save_state = true
load_checkpoint = false

# World Configuration
environment_name = GridWorld
environment_width = 50
environment_height = 50
vision_width = 16
vision_height = 16
touch_range = 2.0
max_energy = 100.0

# Agent Configuration
enable_reward_modulation = true
enable_structural_plasticity = true
enable_development = true
enable_curiosity = true
reward_prediction_error_weight = 0.5
curiosity_decay_rate = 0.95
novelty_decay_rate = 0.99

# Learning Configuration
learning_rate_stdp = 0.01
learning_rate_hebbian = 0.005
learning_rate_reward = 0.1
learning_rate_curiosity = 0.05
learning_rate_novelty = 0.03

# Visualization Configuration
visualization_enabled = true
visualization_update_rate = 30.0
visualization_width = 800
visualization_height = 600
show_neurons = true
show_synapses = true
show_activity = true
show_brain_regions = true
```

## Performance Optimization

### Tips for Better Performance

```python
# Performance optimization tips

def optimize_performance(brain, config):
    # Increase parallelization
    config.set("parallel.enabled", True)
    config.set("parallel.num_threads", 4)
    
    # Optimize memory usage
    config.set("memory.pool_size", 1024 * 1024)  # 1MB pool
    config.set("memory.enable_gc", True)
    config.set("memory.gc_interval", 10000)
    
    # Optimize neural computation
    config.set("neuron.use_vectorized", True)
    config.set("neuron.use_avx", True)
    config.set("synapse.use_sparse_matrix", True)
    
    # Optimize simulation
    config.set("simulation.use_fixed_time", True)
    config.set("simulation.use_async_update", True)
    config.set("simulation.batch_size", 100)
    
    # Optimize plasticity
    config.set("plasticity.enable_batch_update", True)
    config.set("plasticity.update_interval", 10)
    config.set("plasticity.use_delta_encoding", True)
    
    # Re-initialize brain with new optimizations
    brain.reset()
    brain.initialize()
    
    return brain
```

## Troubleshooting

### Common Issues and Solutions

#### Issue: High Memory Usage
```bash
# Solution: Reduce memory footprint
nlm --config=memory_efficient.cfg --neurons=2000 --steps=5000

# memory_efficient.cfg:
neuron_count = 2000
simulation.max_steps = 5000
plasticity.enable_batch_update = true
synapse.use_sparse_matrix = true
```

#### Issue: Slow Performance
```bash
# Solution: Enable optimizations
nlm --config=optimized.cfg --neurons=1000 --steps=1000 --enable-benchmarking

# optimized.cfg:
neuron_count = 1000
parallel.enabled = true
parallel.num_threads = 4
neuron.use_vectorized = true
neuron.use_avx = true
```

#### Issue: Unstable Learning
```bash
# Solution: Adjust learning rates
nlm --config=stabled.cfg --neurons=5000 --steps=2000

# stabled.cfg:
plasticity_learning_rate = 0.005
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.015
neuromod.curiosity.base_rate = 0.2
reward_discount_factor = 0.9
```

### Advanced Logging

For detailed debugging, enable comprehensive logging:

```bash
nlm --config=debug.cfg --debug --verbose --log-level=DEBUG

# debug.cfg:
log_level = DEBUG
log_to_file = true
log_filename = nlm_debug.log
logging.include_stack_traces = true
logging.max_size_mb = 100
```

## Migration Guide

### From Basic to Advanced Configuration

If you're migrating from basic to advanced NLM configuration:

1. **Start with advanced configuration file**
2. **Enable logging and debugging**
3. **Test with smaller networks first**
4. **Gradually increase complexity**
5. **Monitor performance and stability**

### Common Configuration Changes

| Change | Effect | Recommended For |
|--------|--------|-----------------|
| Increase neuron_count | More complex behavior | Complex tasks |
| Enable structural_plasticity | Dynamic network growth | Long-term learning |
| Increase curiosity | More exploration | Novel environments |
| Adjust learning rates | Control learning speed | Fine-tuning |
| Enable development | Age-dependent behavior | Developmental studies |
| Configure neuromodulation | Realistic brain dynamics | Biological realism |

## Future Enhancements

### Planned Features for Advanced Users

1. **Distributed Computing**: Run simulations across multiple machines
2. **GPU Acceleration**: Offload neural computation to GPU
3. **Real-time Visualization**: Live monitoring of neural activity
4. **Import/Export**: Load/save brains in standard formats
5. **Multi-environment**: Run across different simulated worlds
6. **Advanced Learning**: Custom learning algorithms
7. **Hierarchical Models**: Multiple levels of abstraction
8. **Social Learning**: Learn from other agents
9. **Transfer Learning**: Apply knowledge across tasks
10. **Meta-learning**: Learn how to learn

## Resources

### Additional Reading

- `docs/ARCHITECTURE.md` - System architecture overview
- `docs/SCIENCE.md` - Scientific background
- `docs/EXPERIMENTS.md` - Experiment descriptions
- `docs/PHASE6_FINAL_AUDIT.md` - Phase 6 integration results
- `docs/PHASE6_ARCHITECTURE.md` - Phase 6 architecture

### Support

For advanced issues and support:
- Check the [GitHub Issues](https://github.com/nlm-project/nlm/issues)
- Join the [NLM Community](https://community.nlm-project.org)
- Consult the [Architecture Documentation](docs/ARCHITECTURE.md)
- Review [Phase 6 Final Results](docs/PHASE6_FINAL_RESULTS.md)

## Conclusion

NLM's advanced features provide a powerful platform for exploring artificial brain development, learning, and behavior. Whether you're conducting research, developing applications, or simply interested in the science of artificial intelligence, NLM offers the tools you need to create sophisticated neural systems.

Remember: Start simple, understand the basics, then gradually explore advanced features as you become more comfortable with the system.
