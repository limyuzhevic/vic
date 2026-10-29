# Advanced NLM Configuration and Usage Guide

## Power User Configuration Management

This guide covers advanced configuration options and power-user features for NLM.

## Advanced Configuration File Format

### Configuration File Structure

```ini
# NLM Configuration File
# Advanced Configuration for Production Use

# Random seed for reproducible experiments
random_seed = 12345

# Simulation timestep in seconds (fine-grained control)
simulation_timestep = 0.0005  # 0.5 ms for high precision

# Neural network architecture
neuron_count = 2000           # Larger network for complex tasks
region_count = 3              # Multiple regions for specialization
connection_probability = 0.15  # Higher connectivity for robustness

# Enhanced plasticity settings
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.018
stdp_time_constant = 25.0  # Slower STDP dynamics

# Advanced developmental settings
development_synaptogenesis_rate = 0.002
development_pruning_rate = 0.0005
development_maturity_threshold = 500.0

# Neuromodulation settings with advanced options
dopamine_baseline = 0.3
dopamine_sensitivity = 2.0
curiosity_threshold = 0.15
novelty_decay_rate = 0.985

# Reward and motivation settings
reward_discount_factor = 0.995
discount_window = 100.0

# Environment configuration
environment_name = GridWorld
environment_width = 20
environment_height = 20
vision_width = 16
vision_height = 16

# Advanced energy and homeostasis
max_energy = 500.0
energy_decay_rate = 0.9995
energy_threshold = 50.0

# Logging and monitoring
log_level = DEBUG
log_to_file = true
log_filename = nlm_advanced.log
log_rotation_mb = 100
log_max_files = 10

# Performance and optimization
simulation_time_limit = 3600.0  # 1 hour max
checkpoint_interval = 300.0     # Save every 5 minutes
max_memory_usage_mb = 2048
enable_profiling = true

# Advanced visualization
visualization_enabled = true
visualization_update_rate = 60.0
visualization_quality = "high"
render_path_trajectories = true
render_neuron_activities = true

# Learning and adaptation parameters
learning_rate = 0.001
adaptation_rate = 0.0001
meta_learning_enabled = true

# Structural plasticity advanced settings
synaptogenesis_probability = 0.001
pruning_threshold = 0.1
structural_plasticity_enabled = true

# Reward prediction and prediction error
target_prediction_error = 0.01
prediction_error_tolerance = 0.05

# Agent behavior parameters
agent_mode = "balanced"
curiosity_enabled = true
reward_modulation_enabled = true
structural_plasticity_enabled = true
development_enabled = true
exploration_probability = 0.2
exploitation_probability = 0.8

# Advanced experimental parameters
experiment_type = "continuous"
record_every_step = true
export_metrics = true
metrics_format = "json"
metrics_prefix = "exp_"

# Network topology
network_topology = "small_world"
community_structure = true
node_degree = 6
rewiring_probability = 0.15

# Noise and stochasticity
additive_noise_level = 0.01
multiplicative_noise_level = 0.02
stochastic_plasticity = true
noise_seed = 42

# Monitoring and analysis
tracking_period = 1000
metric_window = 100
outlier_detection = true
performance_baseline = 0.8
```

## Power User Command Shortcuts

### Command Line Aliases

These shortcuts provide quick access to common configurations:

```bash
# Fast test mode (debug + benchmark)
-fast

# Agent-only mode with maximum exploration
-agent-only

# Performance analysis mode
-perf

# Professional plotting and analysis
-plot

# Memory-efficient debugging
-mem-debug

# High-precision simulation
-high-precision

# Cloud production deployment
-cloud-deploy

# Development with auto-restart
-dev-watch

# Research mode with continuous learning
-research-mode

# Production validation
-prod-validate

# Quick validation test
-quick-test
```

### Advanced Command Examples

```bash
# Run with custom configuration and advanced settings
./nlm \
  --config=production.cfg \
  --mode=debug \
  --benchmark \
  --set simulation_timestep=0.0005 \
  --set neuron_count=3000 \
  --set plasticity_learning_rate=0.03 \
  --enable-feature meta_learning \
  --enable-feature structural_plasticity \
  --export-stats=production_metrics.csv \
  --save-checkpoint=final_checkpoint.bin

# Agent training with advanced exploration
./nlm \
  --agent-only \
  --agent-mode=balanced \
  --steps=10000 \
  --enable-feature curiosity \
  --enable-feature reward_modulation \
  --enable-feature development \
  --learning-curve \
  --performance-analysis \
  --export-stats=training_metrics.json

# Production benchmark with analysis
./nlm \
  --mode=benchmark \
  --benchmark \
  --steps=5000 \
  --neurons=5000 \
  --performance-analysis \
  --memory-analysis \
  --neural-activity \
  --plasticity-analysis \
  --export-stats=benchmark_results.csv

# Research mode with continuous learning
./nlm \
  --scenario=reproduction_study \
  --mode=educational \
  --learning-curve \
  --enable-feature meta_learning \
  --enable-feature structural_plasticity \
  --enable-feature curiosity \
  --enable-feature reward_modulation \
  --plot-mode \
  --export-stats=research_data.csv
```

## Advanced Python API Examples

### Enhanced Brain Configuration

```python
import pynlm

# Create and configure advanced brain
config = pynlm.createDefaultConfig()

# Advanced neural architecture
config.set("brain.neuron_count", 3000)
config.set("brain.region_count", 3)
config.set("brain.synapse_density", 0.2)
config.set("brain.connection_probability", 0.15)

# Enhanced plasticity configuration
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.025)
config.set("plasticity.stdp.tau_plus", 25.0)
config.set("plasticity.stdp.tau_minus", 25.0)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.02)
config.set("plasticity.structural.enable", True)

# Neuromodulation configuration
config.set("neuromod.dopamine.scale", 2.0)
config.set("neuromod.dopamine.baseline", 0.3)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.curiosity.threshold", 0.15)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.novelty.decay_rate", 0.985)

# Developmental configuration
config.set("development.synaptogenesis_rate", 0.002)
config.set("development.pruning_rate", 0.0005)
config.set("development.maturity_threshold", 500.0)

# Advanced reward configuration
config.set("reward.discount_factor", 0.995)
config.set("reward.discount_window", 100.0)
config.set("reward.target_error", 0.01)

# Environment configuration
config.set("environment.vision_width", 16)
config.set("environment.vision_height", 16)
config.set("environment.max_energy", 500.0)
config.set("environment.energy_decay_rate", 0.9995)

# Performance optimization
config.set("performance.enable_profiling", True)
config.set("performance.max_memory_usage", 2048)
config.set("performance.checkpoint_interval", 300.0)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created advanced brain with {brain.getTotalNeuronCount()} neurons")
print(f"Expected performance: {brain.getAverageFiringRate():.2f} Hz firing rate")
```

### Advanced Agent System Configuration

```python
import pynlm
import numpy as np

def create_advanced_agent(config_path=None):
    """Create an advanced agent with sophisticated capabilities."""
    
    # Create configuration
    if config_path:
        config = pynlm.createConfig()
        config.loadFromFile(config_path)
    else:
        config = pynlm.createDefaultConfig()
        
        # Configure for advanced agent behavior
        config.set("brain.neuron_count", 2000)
        config.set("brain.synapse_density", 0.15)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.dopamine.scale", 1.5)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world with advanced settings
    world = pynlm.createSimpleWorld()
    world.configure(
        width=30.0,
        height=30.0,
        visionWidth=12,
        visionHeight=12
    )
    world.reset()
    world.setRandomSeed(12345)
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Configure advanced agent behavior
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    # Set agent mode for advanced behavior
    # Note: This would be handled via configuration in the enhanced version
    
    return brain, agent, world

def run_advanced_agent_training(brain, agent, world, steps=5000):
    """Run advanced agent training with comprehensive monitoring."""
    
    training_metrics = {
        'steps_completed': 0,
        'total_rewards': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'prediction_errors': [],
        'behavioral_patterns': [],
        'learning_rates': []
    }
    
    print("Starting advanced agent training...")
    print(f"Training for {steps} steps with {brain.getTotalNeuronCount()} neurons")
    
    for step in range(steps):
        # Update world with advanced physics
        world.update(0.1)
        
        # Get sensory input with noise for realistic perception
        percept = world.getSensoryPercept()
        
        # Add subtle noise to simulate realistic sensory processing
        noise_level = 0.01
        for i in range(len(percept.getVision())):
            percept.setVision(i, percept.getVision()[i] + 
                            (np.random.random() - 0.5) * noise_level)
        
        # Process sensory input with enhanced agent
        agent.processSensoryInput(percept)
        
        # Run brain step with adaptive time stepping
        brain_step = step if step < 100 else 100 + ((step - 100) % 900)
        brain.step(brain_step)
        
        # Decode motor command with advanced decision making
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command with advanced physics
        result = world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Calculate prediction error for learning
        predicted_reward = agent.getPredictionError() * 0.5 + 0.1
        agent.applyRewardModulation(result.reward, predicted_reward)
        
        # Update development with age-dependent plasticity
        agent.updateDevelopment(0.1)
        
        # Collect comprehensive metrics
        training_metrics['steps_completed'] = step + 1
        training_metrics['total_rewards'].append(result.reward)
        training_metrics['curiosity_levels'].append(agent.getCuriosityLevel())
        training_metrics['novelty_levels'].append(agent.getNoveltyLevel())
        training_metrics['prediction_errors'].append(agent.getPredictionError())
        training_metrics['behavioral_patterns'].append(int(motor_cmd))
        training_metrics['learning_rates'].append(agent.getPredictionError())
        
        # Log progress with comprehensive analysis
        if step % 100 == 0:
            print(f"\nStep {step}:")
            print(f"  Reward: {result.reward:.3f} (pred: {predicted_reward:.3f})")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction Error: {agent.getPredictionError():.3f}")
            print(f"  Development Stage: {brain.getDevelopmentalStage()}")
            print(f"  Firing Rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"  Agent Action: {motor_cmd}")
            
            # Analyze behavioral patterns
            recent_actions = training_metrics['behavioral_patterns'][-10:]
            action_variance = np.var(recent_actions) if len(recent_actions) > 1 else 0
            print(f"  Action Variance (last 10 steps): {action_variance:.2f}")
    
    return training_metrics

# Example usage
if __name__ == "__main__":
    # Create advanced agent
    brain, agent, world = create_advanced_agent()
    
    # Run advanced training
    metrics = run_advanced_agent_training(brain, agent, world, steps=1000)
    
    # Analyze results
    print("\n" + "="*60)
    print("ADVANCED AGENT TRAINING COMPLETE")
    print("="*60)
    
    total_reward = np.sum(metrics['total_rewards'])
    avg_curiosity = np.mean(metrics['curiosity_levels'])
    avg_novelty = np.mean(metrics['novelty_levels'])
    learning_effectiveness = 1.0 - np.mean(np.abs(metrics['prediction_errors']))
    
    print(f"Total Training Reward: {total_reward:.2f}")
    print(f"Average Curiosity Level: {avg_curiosity:.3f}")
    print(f"Average Novelty Level: {avg_novelty:.3f}")
    print(f"Learning Effectiveness: {learning_effectiveness:.3f}")
    print(f"Final Developmental Stage: {brain.getDevelopmentalStage()}")
    
    # Save metrics for analysis
    import json
    with open('advanced_training_metrics.json', 'w') as f:
        json.dump(metrics, f, indent=2)
    
    print("\nTraining metrics saved to advanced_training_metrics.json")
```

## Performance Optimization Guide

### System Requirements

#### Minimum Requirements
- CPU: 4 cores or equivalent
- RAM: 8 GB minimum
- Storage: 1 GB free space
- Compilation: C++20 compatible compiler

#### Recommended for Advanced Use
- CPU: 8+ cores
- RAM: 16+ GB
- Storage: 10+ GB free space
- GPU: Optional for visualization acceleration

### Optimization Techniques

#### 1. Memory Management
```bash
# Enable memory pools and optimization flags
./nlm --mode=debug --memory-analysis

# Use memory-efficient configuration
./nlm --config=memory_efficient.cfg

# Enable checkpointing for long runs
./nlm --save-checkpoint=checkpoint.bin
```

#### 2. Performance Tuning
```bash
# High-performance mode
./nlm --mode=debug --benchmark --neurons=5000

# Precision mode
./nlm --mode=debug --high-precision

# Parallel processing
./nlm --enable-feature SIMD --enable-feature parallel
```

#### 3. Monitoring and Analysis
```bash
# Comprehensive performance monitoring
./nlm --performance-analysis --memory-analysis --neural-activity

# Export metrics for analysis
./nlm --export-stats=performance.csv --plot-mode

# Generate reports
./nlm --scenario=production_analysis --export-stats=report.json
```

## Troubleshooting Guide

### Common Issues and Solutions

#### 1. Memory Issues
```bash
# Problem: Excessive memory usage

# Solution 1: Reduce network size
./nlm --neurons=1000

# Solution 2: Enable memory-efficient mode
./nlm --config=memory_efficient.cfg

# Solution 3: Use checkpointing
./nlm --save-checkpoint=checkpoint.bin
```

#### 2. Performance Issues
```bash
# Problem: Slow execution

# Solution 1: Increase timestep
./nlm --set simulation_timestep=0.01

# Solution 2: Reduce network size
./nlm --neurons=500

# Solution 3: Enable optimization flags
./nlm --enable-feature optimization
```

#### 3. Configuration Issues
```bash
# Problem: Configuration file errors

# Solution 1: Use default configuration
./nlm --config=default.cfg

# Solution 2: Validate configuration
./nlm --config=your_config.cfg --validate

# Solution 3: Fix syntax errors
# Check for duplicate entries, missing values, and format errors
```

## Quick Reference

### Configuration Commands
```bash
# Quick configuration shortcuts
-fast                      # Debug + benchmark (100 steps)
-agent-only                # Agent-only mode (5000 steps)
-perf                      # Performance analysis
-plot                      # Plotting mode
-mem-debug                 # Memory debugging
-high-precision            # High precision simulation
-cloud-deploy              # Production deployment
-dev-watch                 # Development with auto-restart
-research-mode             # Research with continuous learning
-prod-validate             # Production validation
-quick-test                # Quick validation test
```

### Python API Examples
```python
# Advanced configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)

# Create advanced brain
brain = pynlm.createBrain(config)

# Create advanced agent
agent = pynlm.createAgentBrain(brain)

# Run simulation with monitoring
metrics = run_advanced_training(brain, agent, world)
```

### Error Codes
- Exit code 0: Success
- Exit code 1: Initialization failed
- Exit code 2: Configuration error
- Exit code 3: Memory allocation failed
- Exit code 4: Simulation error
- Exit code 5: Export error

## Conclusion

This enhanced version of NLM provides comprehensive tools for advanced users, researchers, and production deployments. The power-user shortcuts, advanced configuration options, and extensive monitoring capabilities make it suitable for a wide range of applications from research to production environments.

For the most up-to-date documentation and examples, visit the NLM documentation website or check the release notes in the repository.